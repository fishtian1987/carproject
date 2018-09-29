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
    ULONG Version;                    // 版本
    ULONG Size;                       // 结构大小
    UCHAR DeviceType;                 // 设备类型
    UCHAR DeviceTypeModifier;         // SCSI-2额外的设备类型
    BOOLEAN RemovableMedia;           // 是否可移动
    BOOLEAN CommandQueueing;          // 是否支持命令队列
    ULONG VendorIdOffset;             // 厂家设定值的偏移
    ULONG ProductIdOffset;            // 产品ID的偏移
    ULONG ProductRevisionOffset;      // 产品版本的偏移
    ULONG SerialNumberOffset;         // 序列号的偏移
    STORAGE_BUS_TYPE BusType;         // 总线类型
    ULONG RawPropertiesLength;        // 额外的属性数据长度
    UCHAR RawDeviceProperties[1];     // 额外的属性数据(仅定义了象征性的1个字节)
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
	  函数详细
	*/
	void ThreadWorkStart();

	BOOL EnumNew();

	BOOL GetUSBDiskID(char cDriver, char *szDeviceID);
private:	
	/*!
	  \fn static DWORD WINAPI ThreadWork(LPVOID p);
	  \brief 工作线程
	  \param 
	  \param 
	  \return
	   工作线程
	*/
	static DWORD WINAPI ThreadWork(LPVOID p);

	//通过驱动获取盘符对应存储设备的厂商信息，型号信息，序列号
	BOOL GetDriveProperty(char cDriver, char *szVendorId, char *szProductId, char *szDeviceNo);

	//通过注册表获取U盘类设备ID
	BOOL GetUSBDiskIDbyReg(char cDriver, char *szDeviceNo);

	//获取卷标号
	BOOL GetCDriveSerialNumber(char cDriver, char *szSerialNumber);

	//获取总大小
	BOOL GetTotalNumber(char cDriver, char *szTotalNum);

	//另一种获取磁盘序列号的方式
	BOOL GetDiskIDbyOther(char cDriver, char *szVendorId, char *szProductId, char *szDeviceNo);

	//获取存储设备友好名称列表
	int GetFriendlyName();

	//获取设备类型
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
