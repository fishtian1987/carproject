// GetUsbInfo.h: interface for the GetUsbInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETUSBINFO_H__DB6F6199_8755_41DC_AC13_9D6E4DF2C095__INCLUDED_)
#define AFX_GETUSBINFO_H__DB6F6199_8755_41DC_AC13_9D6E4DF2C095__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WinIoCtl.h"
#pragma warning(disable : 4786)
#include "map"
#include "string"
using namespace std;


#ifndef IOCTL_STORAGE_QUERY_PROPERTY
#define IOCTL_STORAGE_QUERY_PROPERTY \
CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

typedef struct _STORAGE_DEVICE_DESCRIPTOR {
    ULONG Version;                    // �汾
    ULONG Size;                       // �ṹ��С
    UCHAR DeviceType;                 // �豸����
    UCHAR DeviceTypeModifier;         // SCSI-2������豸����
    BOOLEAN RemovableMedia;           // �Ƿ���ƶ�
    BOOLEAN CommandQueueing;          // �Ƿ�֧���������
    ULONG VendorIdOffset;             // �����趨ֵ��ƫ��
    ULONG ProductIdOffset;            // ��ƷID��ƫ��
    ULONG ProductRevisionOffset;      // ��Ʒ�汾��ƫ��
    ULONG SerialNumberOffset;         // ���кŵ�ƫ��
    STORAGE_BUS_TYPE BusType;         // ��������
    ULONG RawPropertiesLength;        // ������������ݳ���
    UCHAR RawDeviceProperties[1];     // �������������(�������������Ե�1���ֽ�)
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

typedef enum _STORAGE_PROPERTY_ID { 
	StorageDeviceProperty                   = 0,
		StorageAdapterProperty,
		StorageDeviceIdProperty,
		StorageDeviceUniqueIdProperty,
		StorageDeviceWriteCacheProperty,
		StorageMiniportProperty,
		StorageAccessAlignmentProperty,
		StorageDeviceSeekPenaltyProperty,
		StorageDeviceTrimProperty,
		StorageDeviceWriteAggregationProperty,
		StorageDeviceDeviceTelemetryProperty,
		StorageDeviceLBProvisioningProperty,
		StorageDevicePowerProperty,
		StorageDeviceCopyOffloadProperty,
		StorageDeviceResiliencyProperty,
		StorageDeviceMediumProductType,
		StorageDeviceIoCapabilityProperty       = 48,
		StorageAdapterProtocolSpecificProperty,
		StorageDeviceProtocolSpecificProperty,
		StorageAdapterTemperatureProperty,
		StorageDeviceTemperatureProperty,
		StorageAdapterPhysicalTopologyProperty,
		StorageDevicePhysicalTopologyProperty,
		StorageDeviceAttributesProperty
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

typedef enum _STORAGE_QUERY_TYPE { 
	PropertyStandardQuery    = 0,
		PropertyExistsQuery      = 1,
		PropertyMaskQuery        = 2,
		PropertyQueryMaxDefined  = 3
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

typedef struct _STORAGE_PROPERTY_QUERY {
	STORAGE_PROPERTY_ID PropertyId;
	STORAGE_QUERY_TYPE  QueryType;
	BYTE                AdditionalParameters[1];
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

class GetDiskInfo  
{
public:
	GetDiskInfo();
	virtual ~GetDiskInfo();

public:
	static GetDiskInfo* GetInstance();
	static void DestroyInstance();
	/*!
	  \fn 	void ThreadWorkStart();	
	  \brief 
	  \param 
	  \param 
	  \return
	  ������ϸ
	*/
	void ThreadWorkStart();

	BOOL EnumNew();

	BOOL GetUSBDiskID(char cDriver, char *szDeviceID);
private:	
	/*!
	  \fn static DWORD WINAPI ThreadWork(LPVOID p);
	  \brief �����߳�
	  \param 
	  \param 
	  \return
	   �����߳�
	*/
	static DWORD WINAPI ThreadWork(LPVOID p);

	//ͨ��������ȡ�̷���Ӧ�洢�豸�ĳ�����Ϣ���ͺ���Ϣ�����к�
	BOOL GetDriveProperty(char cDriver, char *szVendorId, char *szProductId, char *szDeviceNo);

	//ͨ��ע����ȡU�����豸ID
	BOOL GetUSBDiskIDbyReg(char cDriver, char *szDeviceNo);

	//��ȡ����
	BOOL GetCDriveSerialNumber(char cDriver, char *szSerialNumber);

	//��ȡ�ܴ�С
	BOOL GetTotalNumber(char cDriver, char *szTotalNum);

	//��һ�ֻ�ȡ�������кŵķ�ʽ
	BOOL GetDiskIDbyOther(char cDriver, char *szVendorId, char *szProductId, char *szDeviceNo);

	//��ȡ�洢�豸�Ѻ������б�
	int GetFriendlyName();

	//��ȡ�豸����
	int myGetDeviceType(char cDriver);

	BOOL WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish = FALSE);

	static GetDiskInfo* m_pInstance;
	char * SendBuf;
	DWORD SendCount;
	int offset;
	unsigned int mask;
	map<string, string> m_FriendlyNameList;
};

BOOL GetUSBDiskIDbyReg(char cDriver, char *szDeviceNo);

extern DWORD WINAPI EnumNewRemovableDriver(LPVOID p);

//////////////////////////////////////////////////////////////////////////

//  Max number of drives assuming primary/secondary, master/slave topology
#define  MAX_IDE_DRIVES  4

//////////////////////////////////////////////////////////////////////////

#define IOCTL_DISK_GET_DRIVE_GEOMETRY_EX CTL_CODE(IOCTL_DISK_BASE, 0x0028, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _DISK_GEOMETRY_EX {
	DISK_GEOMETRY  Geometry;
	LARGE_INTEGER  DiskSize;
	UCHAR  Data[1];
} DISK_GEOMETRY_EX, *PDISK_GEOMETRY_EX;

int ReadPhysicalDriveInNTWithZeroRights (char *res);

#endif // !defined(AFX_GETUSBINFO_H__DB6F6199_8755_41DC_AC13_9D6E4DF2C095__INCLUDED_)
