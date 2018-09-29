// WinApi.h: interface for the WinApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINAPI_H__B4B48599_8EA7_4DE2_BBF2_0656874A77BD__INCLUDED_)
#define AFX_WINAPI_H__B4B48599_8EA7_4DE2_BBF2_0656874A77BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <time.h>
#include <list>
#include <lmcons.h>
#include <IPTypes.h>
#include <Nb30.h>
#include <ras.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
#include <setupapi.h>
#include <Wtsapi32.h>
#include <Userenv.h>
#include "wininet.h"
#include <ipexport.h>

//////////////////////////////////////////////////////////////////////////

//Advapi32.dll
typedef WINADVAPI BOOL (CALLBACK * pAdjustTokenPrivileges)	(HANDLE TokenHandle,BOOL DisableAllPrivileges,PTOKEN_PRIVILEGES NewState,DWORD BufferLength,PTOKEN_PRIVILEGES PreviousState,PDWORD ReturnLength);
typedef WINADVAPI BOOL (CALLBACK * pLookupPrivilegeValueA)	(LPCSTR lpSystemName, LPCSTR lpName, PLUID lpLuid);
typedef WINADVAPI BOOL (CALLBACK * pOpenProcessToken)		(HANDLE ProcessHandle,DWORD DesiredAccess,PHANDLE TokenHandle);
typedef WINADVAPI BOOL (CALLBACK * pOpenThreadToken)		(HANDLE ThreadHandle,DWORD DesiredAccess,BOOL OpenAsSelf,PHANDLE TokenHandle);
typedef WINADVAPI LONG (APIENTRY * pRegCloseKey) (HKEY hKey);
typedef WINADVAPI LONG (APIENTRY * pRegQueryValueExA) (HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef WINADVAPI LONG (APIENTRY * pRegQueryValueExW) (HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef WINADVAPI LONG (APIENTRY * pRegOpenKeyExA) (HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
typedef WINADVAPI LONG (APIENTRY * pRegOpenKeyExW) (HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
typedef WINADVAPI LONG (APIENTRY * pRegSetValueExA) (HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, WORD cbData);
typedef WINADVAPI LONG (APIENTRY * pRegEnumValueA) (HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcbValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef WINADVAPI LONG (APIENTRY * pRegEnumValueW) (HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, LPDWORD lpcbValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef WINADVAPI LONG (APIENTRY * pRegDeleteValueA) (HKEY hKey, LPCSTR lpValueName);
typedef WINADVAPI LONG (APIENTRY * pRegDeleteValueW) (HKEY hKey, LPCWSTR lpValueName);
typedef WINADVAPI LONG (APIENTRY * pRegCreateKeyExA) (HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
typedef WINADVAPI LONG (APIENTRY * pRegEnumKeyExA) (HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcbName, LPDWORD lpReserved, LPSTR lpClass, LPDWORD lpcbClass, PFILETIME lpftLastWriteTime);
typedef WINADVAPI LONG (APIENTRY * pRegQueryInfoKeyA) (HKEY hKey, LPSTR lpClass, LPDWORD lpcbClass, LPDWORD lpReserved, LPDWORD lpcSubKeys, LPDWORD lpcbMaxSubKeyLen, LPDWORD lpcbMaxClassLen, LPDWORD lpcValues, LPDWORD lpcbMaxValueNameLen, LPDWORD lpcbMaxValueLen, LPDWORD lpcbSecurityDescriptor, PFILETIME lpftLastWriteTime);
typedef WINADVAPI LONG (APIENTRY * pRegEnumKeyA) (HKEY hKey, DWORD dwIndex, LPSTR lpName, DWORD cbName);
typedef WINADVAPI LONG (APIENTRY * pRegOpenKeyA) (HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult);
typedef WINADVAPI LONG (APIENTRY * pRegConnectRegistryA) (LPCSTR lpMachineName, HKEY hKey, PHKEY phkResult);
typedef WINADVAPI BOOL (WINAPI * pCreateProcessAsUserW) (IN HANDLE hToken, IN LPCWSTR lpApplicationName,
					  IN LPWSTR lpCommandLine, IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
					  IN LPSECURITY_ATTRIBUTES lpThreadAttributes, IN BOOL bInheritHandles,
					  IN DWORD dwCreationFlags, IN LPVOID lpEnvironment, IN LPCWSTR lpCurrentDirectory,
					  IN LPSTARTUPINFOW lpStartupInfo, OUT LPPROCESS_INFORMATION lpProcessInformation
    );
typedef WINADVAPI BOOL (WINAPI * pDuplicateTokenEx) (IN HANDLE hExistingToken, IN DWORD dwDesiredAccess,
				 IN LPSECURITY_ATTRIBUTES lpTokenAttributes, IN SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
				 IN TOKEN_TYPE TokenType, OUT PHANDLE phNewToken);
typedef WINADVAPI BOOL (WINAPI * pGetTokenInformation) (IN HANDLE TokenHandle, IN TOKEN_INFORMATION_CLASS TokenInformationClass,
					 OUT LPVOID TokenInformation, IN DWORD TokenInformationLength, OUT PDWORD ReturnLength);

extern pAdjustTokenPrivileges		PAdjustTokenPrivileges ;
extern pLookupPrivilegeValueA		PLookupPrivilegeValueA ;
extern pOpenProcessToken			POpenProcessToken;
extern pOpenThreadToken				POpenThreadToken;
extern pRegCloseKey					PRegCloseKey;
extern pRegQueryValueExA			PRegQueryValueExA;
extern pRegQueryValueExW			PRegQueryValueExW;
extern pRegOpenKeyExA				PRegOpenKeyExA;
extern pRegOpenKeyExW				PRegOpenKeyExW;
extern pRegSetValueExA				PRegSetValueExA;
extern pRegEnumValueA				PRegEnumValueA;
extern pRegEnumValueW				PRegEnumValueW;
extern pRegDeleteValueA				PRegDeleteValueA;
extern pRegDeleteValueW				PRegDeleteValueW;
extern pRegCreateKeyExA				PRegCreateKeyExA;
extern pRegEnumKeyExA				PRegEnumKeyExA;
extern pRegQueryInfoKeyA			PRegQueryInfoKeyA;
extern pRegEnumKeyA					PRegEnumKeyA;
extern pRegOpenKeyA					PRegOpenKeyA;
extern pRegConnectRegistryA			PRegConnectRegistryA;
extern pCreateProcessAsUserW		PCreateProcessAsUserW;
extern pDuplicateTokenEx			PDuplicateTokenEx;
extern pGetTokenInformation			PGetTokenInformation;

//Netapi32.dll
typedef NET_API_STATUS (CALLBACK * pNetUserEnum)		(LPCWSTR servername,DWORD level,DWORD filter,LPBYTE *bufptr,DWORD prefmaxlen,LPDWORD entriesread,LPDWORD totalentries,LPDWORD resume_handle);
typedef NET_API_STATUS (CALLBACK * pNetApiBufferFree)	(LPVOID Buffer);

extern pNetApiBufferFree PNetApiBufferFree;
extern pNetUserEnum PNetUserEnum;

//IPHLPAPI.DLL
typedef DWORD (WINAPI * pGetAdaptersInfo) (PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
typedef DWORD (WINAPI * pSendARP) (IPAddr DestIP, IPAddr SrcIP, PULONG pMacAddr, PULONG  PhyAddrLen);

extern pGetAdaptersInfo			PGetAdaptersInfo;
extern pSendARP PSendARP;

//NETAPI32.DLL
typedef UCHAR (APIENTRY * pNetbios)						(PNCB pncb);
extern pNetbios				PNetbios;

//RASAPI32.DLL
typedef DWORD (APIENTRY * pRasEnumConnectionsA)			(LPRASCONN lprasconn, LPDWORD lpcb, LPDWORD lpcConnections);
typedef DWORD (APIENTRY * pRasEnumEntriesA)				(LPCSTR reserved, LPCSTR lpszPhonebook, LPRASENTRYNAME lprasentryname, LPDWORD lpcb, LPDWORD lpcEntries);
typedef DWORD (APIENTRY * pRasGetEntryDialParamsA)		(LPCSTR, LPRASDIALPARAMSA, LPBOOL );

extern pRasEnumConnectionsA		PRasEnumConnectionsA;
extern pRasEnumEntriesA			PRasEnumEntriesA;
extern pRasGetEntryDialParamsA	PRasGetEntryDialParamsA;

//SHLWAPI.DLL
typedef DWORD (STDAPICALLTYPE * pSHDeleteValueA)		(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszValue);
typedef DWORD (STDAPICALLTYPE * pSHGetValueA)			(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszValue, DWORD *pdwType, void *pvData, DWORD *pcbData);
typedef DWORD (STDAPICALLTYPE * pSHSetValueA)			(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszValue, DWORD dwType, LPCVOID pvData, DWORD cbData);

extern pSHDeleteValueA		PSHDeleteValueA;
extern pSHGetValueA			PSHGetValueA;
extern pSHSetValueA			PSHSetValueA;

//USER32.dll
typedef WINUSERAPI BOOL		(WINAPI * pChangeClipboardChain)	( IN HWND hWndRemove, IN HWND hWndNewNext);
typedef WINUSERAPI BOOL		(WINAPI * pCloseClipboard)			( VOID);
typedef WINUSERAPI HANDLE	(WINAPI * pGetClipboardData)		( IN UINT uFormat);
typedef WINUSERAPI HWND		(WINAPI * pGetForegroundWindow)		( VOID);
typedef			   HWND		(WINAPI * pCreateWindowExA)			( DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
typedef			   ATOM		(WINAPI * pRegisterClassExA)		( CONST WNDCLASSEXA *lpwcx);
typedef WINUSERAPI HWND		(WINAPI * pSetClipboardViewer)		( IN HWND hWndNewViewer);
typedef WINUSERAPI LRESULT	(WINAPI * pSendMessageA)			( IN HWND hWnd, IN UINT Msg, IN WPARAM wParam, IN LPARAM lParam);
typedef WINUSERAPI int		(WINAPI * pGetPriorityClipboardFormat)( OUT UINT *paFormatPriorityList, IN int cFormats);
typedef WINUSERAPI int		(WINAPI * pGetWindowTextW)			( IN HWND hWnd, OUT LPWSTR lpString, IN int nMaxCount);
typedef WINUSERAPI BOOL		(WINAPI * pCloseDesktop)			( IN HDESK hDesktop);
typedef WINUSERAPI BOOL		(WINAPI * pCloseWindowStation)		( IN HWINSTA hWinSta);
typedef WINUSERAPI UINT_PTR (WINAPI * pSetTimer)				( IN HWND hWnd, IN UINT_PTR nIDEvent, IN UINT uElapse, IN TIMERPROC lpTimerFunc);
typedef WINUSERAPI LRESULT	(WINAPI * pDispatchMessageA)		( IN CONST MSG *lpMsg);
typedef WINUSERAPI LRESULT	(WINAPI * pDefWindowProcA)			( IN HWND hWnd, IN UINT Msg, IN WPARAM wParam, IN LPARAM lParam);
typedef WINUSERAPI BOOL		(WINAPI * pTranslateMessage)		( IN CONST MSG *lpMsg);
typedef WINUSERAPI VOID		(WINAPI * pPostQuitMessage)			( IN int nExitCode);
typedef WINUSERAPI BOOL		(WINAPI * pGetMessageA)				( OUT LPMSG lpMsg, IN HWND hWnd, IN UINT wMsgFilterMin, IN UINT wMsgFilterMax);
typedef WINUSERAPI BOOL		(WINAPI * pIsCharAlphaNumericA)		( IN CHAR ch);
typedef WINUSERAPI BOOL     (WINAPI * pOpenClipboard)			( HWND hWndNewOwner);
typedef WINUSERAPI HDESK	(WINAPI * pGetThreadDesktop)		( DWORD dwThreadId);
typedef WINUSERAPI int		(WINAPI * pReleaseDC)				( IN HWND hWnd, IN HDC hDC);
typedef WINUSERAPI HDC		(WINAPI * pGetDC)					( HWND hWnd);
typedef WINUSERAPI HDESK	(WINAPI * pOpenDesktopA)			( IN LPCSTR lpszDesktop, IN DWORD dwFlags, IN BOOL fInherit, IN ACCESS_MASK dwDesiredAccess);
typedef WINUSERAPI BOOL		(WINAPI * pSetThreadDesktop)		( IN HDESK hDesktop);
typedef WINUSERAPI BOOL		(WINAPI * pSetProcessWindowStation)	( IN HWINSTA hWinSta);
typedef WINUSERAPI HWINSTA	(WINAPI * pGetProcessWindowStation)	( VOID);
typedef WINUSERAPI HWINSTA	(WINAPI * pOpenWindowStationA)		( IN LPCSTR lpszWinSta, IN BOOL fInherit, IN ACCESS_MASK dwDesiredAccess);

extern pChangeClipboardChain		PChangeClipboardChain;
extern pCloseClipboard				PCloseClipboard;
extern pGetClipboardData			PGetClipboardData;
extern pGetForegroundWindow			PGetForegroundWindow;
extern pCreateWindowExA				PCreateWindowExA;
extern pRegisterClassExA			PRegisterClassExA;
extern pSetClipboardViewer			PSetClipboardViewer;
extern pSendMessageA				PSendMessageA;
extern pGetPriorityClipboardFormat	PGetPriorityClipboardFormat;
extern pGetWindowTextW				PGetWindowTextW;
extern pCloseDesktop				PCloseDesktop;
extern pCloseWindowStation			PCloseWindowStation;
extern pSetTimer					PSetTimer;
extern pDispatchMessageA			PDispatchMessageA;
extern pDefWindowProcA				PDefWindowProcA;
extern pTranslateMessage			PTranslateMessage;
extern pPostQuitMessage				PPostQuitMessage;
extern pGetMessageA					PGetMessageA;
extern pIsCharAlphaNumericA			PIsCharAlphaNumericA;
extern pOpenClipboard				POpenClipboard;
extern pGetThreadDesktop			PGetThreadDesktop;
extern pReleaseDC					PReleaseDC;
extern pGetDC						PGetDC;
extern pOpenDesktopA				POpenDesktopA;
extern pSetThreadDesktop			PSetThreadDesktop;
extern pSetProcessWindowStation		PSetProcessWindowStation;
extern pGetProcessWindowStation		PGetProcessWindowStation;
extern pOpenWindowStationA			POpenWindowStationA;

//KERNEL32.DLL
typedef WINBASEAPI BOOL		(WINAPI * pWritePrivateProfileStringA)	( IN LPCSTR lpAppName, IN LPCSTR lpKeyName, IN LPCSTR lpString, IN LPCSTR lpFileName );
typedef WINBASEAPI DWORD	(WINAPI * pGetPrivateProfileStringA)	( IN LPCSTR lpAppName, IN LPCSTR lpKeyName, IN LPCSTR lpDefault, OUT LPSTR lpReturnedString, IN DWORD nSize, IN LPCSTR lpFileName );
typedef WINBASEAPI UINT		(WINAPI * pGetPrivateProfileIntA)		( IN LPCSTR lpAppName, IN LPCSTR lpKeyName, IN INT nDefault, IN LPCSTR lpFileName );
typedef WINBASEAPI DWORD	(WINAPI * pGetEnvironmentVariableA)		( IN LPCSTR lpName, OUT LPSTR lpBuffer, IN DWORD nSize );
typedef WINBASEAPI LCID		(WINAPI * pGetSystemDefaultLCID)		( void);
typedef WINBASEAPI HANDLE	(WINAPI * pFindFirstFileW)				( IN LPCWSTR lpFileName, OUT LPWIN32_FIND_DATAW lpFindFileData );
typedef WINBASEAPI HANDLE	(WINAPI * pFindFirstFileA)				( IN LPCSTR lpFileName, OUT LPWIN32_FIND_DATAA lpFindFileData );
typedef WINBASEAPI BOOL		(WINAPI * pFindNextFileW)				( IN HANDLE hFindFile, OUT LPWIN32_FIND_DATAW lpFindFileData );
typedef WINBASEAPI BOOL		(WINAPI * pFindNextFileA)				( IN HANDLE hFindFile, OUT LPWIN32_FIND_DATAA lpFindFileData );
typedef WINBASEAPI BOOL		(WINAPI * pFindClose)					( IN OUT HANDLE hFindFile );
typedef			   BOOL		(WINAPI * pProcess32First)				( HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef			   BOOL		(WINAPI * pProcess32Next)				( HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef			   HANDLE	(WINAPI * pCreateToolhelp32Snapshot)	( DWORD dwFlags, DWORD th32ProcessID );
typedef WINBASEAPI HANDLE	(WINAPI * pGetCurrentProcess)			( VOID );
typedef WINBASEAPI DWORD	(WINAPI * pGetCurrentProcessId)			( VOID );
typedef WINBASEAPI HANDLE	(WINAPI * pGetCurrentThread)			( VOID );
typedef WINBASEAPI DWORD	(WINAPI * pGetCurrentThreadId)			( VOID );
typedef WINBASEAPI BOOL		(WINAPI * pFileTimeToLocalFileTime)		( IN CONST FILETIME *lpFileTime, OUT LPFILETIME lpLocalFileTime );
typedef WINBASEAPI BOOL		(WINAPI * pFileTimeToSystemTime)		( IN CONST FILETIME *lpFileTime, OUT LPSYSTEMTIME lpSystemTime );
typedef WINBASEAPI UINT		(WINAPI * pGetDriveTypeA)				( IN LPCSTR lpRootPathName );
typedef WINBASEAPI DWORD	(WINAPI * pGetLogicalDriveStringsA)		( IN DWORD nBufferLength, OUT LPSTR lpBuffer );
typedef WINBASEAPI BOOL		(WINAPI * pGetDiskFreeSpaceExA)			( IN LPCSTR lpDirectoryName, OUT PULARGE_INTEGER lpFreeBytesAvailableToCaller, OUT PULARGE_INTEGER lpTotalNumberOfBytes, OUT PULARGE_INTEGER lpTotalNumberOfFreeBytes );
typedef WINBASEAPI BOOL		(WINAPI * pGetVolumeInformationA)		( LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer,DWORD nVolumeNameSize,LPDWORD lpVolumeSerialNumber,LPDWORD lpMaximumComponentLength,LPDWORD lpFileSystemFlags,LPSTR lpFileSystemNameBuffer,DWORD nFileSystemNameSize);
typedef WINBASEAPI DWORD	(WINAPI * pGetLogicalDrives)			( VOID );
typedef WINBASEAPI BOOL		(WINAPI * pGetExitCodeThread)			( IN HANDLE hThread, OUT LPDWORD lpExitCode );
typedef WINBASEAPI BOOL		(WINAPI * pCreateProcessA)				( LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
typedef WINBASEAPI int		(WINAPI * pWideCharToMultiByte)			( IN UINT CodePage, IN DWORD dwFlags, IN LPCWSTR lpWideCharStr, IN int cchWideChar, OUT LPSTR lpMultiByteStr, IN int cbMultiByte, IN LPCSTR lpDefaultChar, OUT LPBOOL lpUsedDefaultChar);
typedef WINBASEAPI BOOL		(WINAPI * pTerminateProcess)			( IN HANDLE hProcess, IN UINT uExitCode );
typedef WINBASEAPI BOOL		(WINAPI * pTerminateThread)				( IN OUT HANDLE hThread, IN DWORD dwExitCode );
typedef WINBASEAPI HANDLE	(WINAPI * pOpenProcess)					( IN DWORD dwDesiredAccess, IN BOOL bInheritHandle, IN DWORD dwProcessId );
typedef WINBASEAPI BOOL		(WINAPI * pDeviceIoControl)				( IN HANDLE hDevice, IN DWORD dwIoControlCode, IN LPVOID lpInBuffer, IN DWORD nInBufferSize, OUT LPVOID lpOutBuffer, IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned, IN LPOVERLAPPED lpOverlapped );
typedef WINBASEAPI LONG		(WINAPI * pInterlockedCompareExchange)	( IN OUT LONG volatile *Destination, IN LONG Exchange, IN LONG Comperand );
typedef WINBASEAPI LPSTR	(WINAPI * plstrcpyA)					( OUT LPSTR lpString1, IN LPCSTR lpString2 );
typedef WINBASEAPI LPSTR	(WINAPI * plstrcatA)					( IN OUT LPSTR lpString1, IN LPCSTR lpString2 );
typedef WINBASEAPI BOOL		(WINAPI * pMoveFileExA)					( IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN DWORD dwFlags );
typedef WINBASEAPI BOOL		(WINAPI * pMoveFileExW)					( IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN DWORD dwFlags );
typedef	WINBASEAPI UINT		(WINAPI * pGetWindowsDirectoryA)		( OUT LPSTR lpBuffer, IN UINT uSize );
typedef WINBASEAPI HANDLE	(WINAPI * pCreateMutexA)				( IN LPSECURITY_ATTRIBUTES lpMutexAttributes, IN BOOL bInitialOwner, IN LPCSTR lpName );
typedef WINBASEAPI BOOL		(WINAPI * pSetFileTime)					( IN HANDLE hFile, IN CONST FILETIME *lpCreationTime, IN CONST FILETIME *lpLastAccessTime, IN CONST FILETIME *lpLastWriteTime );
typedef WINBASEAPI BOOL		(WINAPI * pLocalFileTimeToFileTime)		( IN CONST FILETIME *lpLocalFileTime, OUT LPFILETIME lpFileTime );
typedef WINBASEAPI BOOL		(WINAPI * pGetFileTime)					( IN HANDLE hFile, OUT LPFILETIME lpCreationTime, OUT LPFILETIME lpLastAccessTime, OUT LPFILETIME lpLastWriteTime );
typedef WINBASEAPI DWORD	(WINAPI * pGetFileSize)					( IN HANDLE hFile, LPDWORD lpFileSizeHigh);
typedef WINBASEAPI BOOL		(WINAPI * pWriteProcessMemory)			( IN HANDLE hProcess, IN LPVOID lpBaseAddress, IN LPCVOID lpBuffer, IN SIZE_T nSize, OUT SIZE_T * lpNumberOfBytesWritten );
typedef WINBASEAPI BOOL		(WINAPI * pReadProcessMemory)			( IN HANDLE hProcess, IN LPCVOID lpBaseAddress, OUT LPVOID lpBuffer, IN SIZE_T nSize, OUT SIZE_T * lpNumberOfBytesRead );
typedef WINBASEAPI BOOL		(WINAPI * pGlobalUnlock)				( IN HGLOBAL hMem );
typedef WINBASEAPI int		(WINAPI * pMultiByteToWideChar)			( IN UINT CodePage, IN DWORD dwFlags, IN LPCSTR lpMultiByteStr, IN int cbMultiByte, OUT LPWSTR lpWideCharStr, IN int cchWideChar);
typedef WINBASEAPI LPVOID	(WINAPI * pGlobalLock)					( IN HGLOBAL hMem );
typedef WINBASEAPI UINT		(WINAPI * pGetSystemDirectoryA)			( OUT LPSTR lpBuffer, IN UINT uSize );
typedef	WINBASEAPI BOOL		(WINAPI * pGetVersionExA)				( IN OUT LPOSVERSIONINFOA lpVersionInformation );
typedef WINBASEAPI DWORD	(WINAPI * pGetFileAttributesA)			( IN LPCSTR lpFileName );
typedef WINBASEAPI BOOL		(WINAPI * pDeleteFileA)					( IN LPCSTR lpFileName );
typedef WINBASEAPI BOOL		(WINAPI * pDeleteFileW)					( IN LPCWSTR lpFileName );
typedef WINBASEAPI HGLOBAL	(WINAPI * pGlobalAlloc)					( IN UINT uFlags, IN SIZE_T dwBytes );
typedef WINBASEAPI HANDLE	(WINAPI * pCreateFileA)					( IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile );
typedef WINBASEAPI HANDLE	(WINAPI * pCreateFileW)					( IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile );
typedef WINBASEAPI HLOCAL	(WINAPI * pLocalAlloc)					( IN UINT uFlags, IN SIZE_T uBytes );
typedef WINBASEAPI BOOL		(WINAPI * pCreatePipe)					( OUT PHANDLE hReadPipe, OUT PHANDLE hWritePipe, IN LPSECURITY_ATTRIBUTES lpPipeAttributes, IN DWORD nSize );
typedef WINBASEAPI BOOL		(WINAPI * pFreeResource)				( IN HGLOBAL hResData );
typedef WINBASEAPI BOOL		(WINAPI * pCopyFileA)					( IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN BOOL bFailIfExists );
typedef WINBASEAPI BOOL		(WINAPI * pCopyFileW)					( IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN BOOL bFailIfExists );
typedef WINBASEAPI HGLOBAL	(WINAPI * pLoadResource)				( IN HMODULE hModule, IN HRSRC hResInfo );
typedef UINT (WINAPI *pWindExec)( LPCSTR lpCmdLine, UINT uCmdShow );
typedef WINBASEAPI BOOL (WINAPI * pCreateProcessW) ( IN LPCWSTR lpApplicationName, IN LPWSTR lpCommandLine,
			   IN LPSECURITY_ATTRIBUTES lpProcessAttributes, IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
			   IN BOOL bInheritHandles, IN DWORD dwCreationFlags, IN LPVOID lpEnvironment,
			   IN LPCWSTR lpCurrentDirectory, IN LPSTARTUPINFOW lpStartupInfo,
			   OUT LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL (WINAPI * pProcess32FirstW) ( HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
typedef BOOL (WINAPI * pProcess32NextW) ( HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
typedef WINBASEAPI DWORD (WINAPI * pGetTempPathA)( IN DWORD nBufferLength, OUT LPSTR lpBuffer);
typedef WINBASEAPI DWORD (WINAPI * pGetTempPathW)( IN DWORD nBufferLength, OUT LPWSTR lpBuffer);
typedef WINBASEAPI VOID (WINAPI * pGetLocalTime)(OUT LPSYSTEMTIME lpSystemTime);
typedef VOID (WINAPI *pGetStartupInfoA)( OUT LPSTARTUPINFOA lpStartupInfo );
typedef WINBASEAPI BOOL (WINAPI *pGetFileSizeEx) (HANDLE hFile, PLARGE_INTEGER lpFileSize);
typedef BOOL (WINAPI *pIsWow64Process) (HANDLE, PBOOL);
typedef WINBASEAPI DWORD	(WINAPI * pGetModuleFileNameA) (IN HMODULE hModule, OUT LPSTR lpFilename, IN DWORD nSize);
typedef WINBASEAPI DWORD	(WINAPI * pGetModuleFileNameW) (IN HMODULE hModule, OUT LPWSTR lpFilename, IN DWORD nSize);
typedef WINBASEAPI BOOL		(WINAPI * pSetFileAttributesA) (IN LPCSTR lpFileName, IN DWORD dwFileAttributes);
typedef WINBASEAPI BOOL		(WINAPI * pSetFileAttributesW) (IN LPCWSTR lpFileName, IN DWORD dwFileAttributes);
typedef WINBASEAPI int		(WINAPI * plstrlenA) ( IN LPCSTR lpString );
typedef WINBASEAPI BOOL		(WINAPI * pWriteFile)( IN HANDLE hFile, IN LPCVOID lpBuffer, IN DWORD nNumberOfBytesToWrite, OUT LPDWORD lpNumberOfBytesWritten, IN LPOVERLAPPED lpOverlapped);
typedef WINBASEAPI BOOL		(WINAPI * pReadFile) ( IN HANDLE hFile, OUT LPVOID lpBuffer, IN DWORD nNumberOfBytesToRead, OUT LPDWORD lpNumberOfBytesRead, IN LPOVERLAPPED lpOverlapped);
typedef WINBASEAPI BOOL		(WINAPI * pSystemTimeToFileTime) ( IN CONST SYSTEMTIME *lpSystemTime, OUT LPFILETIME lpFileTime);
typedef WINBASEAPI BOOL		(WINAPI * pProcessIdToSessionId) ( IN  DWORD dwProcessId, OUT DWORD *pSessionId);
typedef WINBASEAPI LONG		(WINAPI * pInterlockedExchange) ( IN OUT LONG volatile *Target, IN LONG Value);
typedef WINBASEAPI LONG		(WINAPI * pInterlockedDecrement) ( IN OUT LONG volatile *lpAddend);
typedef WINBASEAPI BOOL		(WINAPI * pGetComputerNameW) ( OUT LPWSTR lpBuffer, IN OUT LPDWORD nSize);
typedef WINBASEAPI LONG		(WINAPI * pInterlockedIncrement) (IN OUT LONG volatile *lpAddend);
typedef WINBASEAPI BOOL		(WINAPI * pSetFilePointerEx) ( HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
typedef WINBASEAPI DWORD	(WINAPI * pGetTickCount) ( VOID);
typedef WINBASEAPI VOID		(WINAPI * pSleep) ( IN DWORD dwMilliseconds);
typedef WINBASEAPI HLOCAL	(WINAPI * pLocalFree) ( IN HLOCAL hMem);
typedef WINBASEAPI HGLOBAL	(WINAPI * pGlobalFree) ( IN HGLOBAL hMem);
typedef WINBASEAPI DWORD	(WINAPI * pGetLastError) ( VOID);
typedef WINBASEAPI VOID		(WINAPI * pOutputDebugStringA) ( IN LPCSTR lpOutputString);

extern pWritePrivateProfileStringA	PWritePrivateProfileStringA;
extern pGetPrivateProfileStringA	PGetPrivateProfileStringA;
extern pGetPrivateProfileIntA		PGetPrivateProfileIntA;
extern pGetEnvironmentVariableA		PGetEnvironmentVariableA;
extern pGetSystemDefaultLCID		PGetSystemDefaultLCID;
extern pFindFirstFileW				PFindFirstFileW;
extern pFindFirstFileA				PFindFirstFileA;
extern pFindNextFileW				PFindNextFileW;
extern pFindNextFileA				PFindNextFileA;			
extern pFindClose					PFindClose;
extern pProcess32First				PProcess32First;
extern pProcess32Next				PProcess32Next;
extern pCreateToolhelp32Snapshot	PCreateToolhelp32Snapshot;
extern pGetCurrentProcess			PGetCurrentProcess;
extern pGetCurrentProcessId			PGetCurrentProcessId;
extern pGetCurrentThread			PGetCurrentThread;
extern pGetCurrentThreadId			PGetCurrentThreadId;
extern pFileTimeToLocalFileTime		PFileTimeToLocalFileTime;
extern pFileTimeToSystemTime		PFileTimeToSystemTime;
extern pGetDriveTypeA				PGetDriveTypeA;
extern pGetLogicalDriveStringsA		PGetLogicalDriveStringsA;
extern pGetDiskFreeSpaceExA			PGetDiskFreeSpaceExA;
extern pGetVolumeInformationA		PGetVolumeInformationA;
extern pGetLogicalDrives			PGetLogicalDrives;
extern pGetExitCodeThread			PGetExitCodeThread;
extern pCreateProcessA				PCreateProcessA;
extern pWideCharToMultiByte			PWideCharToMultiByte;
extern pTerminateProcess			PTerminateProcess;
extern pTerminateThread				PTerminateThread;
extern pOpenProcess					POpenProcess;
extern pDeviceIoControl				PDeviceIoControl;
extern pInterlockedCompareExchange	PInterlockedCompareExchange;
extern plstrcpyA					PlstrcpyA;
extern plstrcatA					PlstrcatA;
extern pMoveFileExA					PMoveFileExA;
extern pMoveFileExW					PMoveFileExW;
extern pGetWindowsDirectoryA		PGetWindowsDirectoryA;
extern pCreateMutexA				PCreateMutexA;
extern pSetFileTime					PSetFileTime;
extern pLocalFileTimeToFileTime		PLocalFileTimeToFileTime;
extern pGetFileTime					PGetFileTime;
extern pGetFileSize					PGetFileSize;
extern pWriteProcessMemory			PWriteProcessMemory;
extern pReadProcessMemory			PReadProcessMemory;
extern pGlobalUnlock				PGlobalUnlock;
extern pMultiByteToWideChar			PMultiByteToWideChar;
extern pGlobalLock					PGlobalLock;
extern pGetSystemDirectoryA			PGetSystemDirectoryA;
extern pGetVersionExA				PGetVersionExA;
extern pGetFileAttributesA			PGetFileAttributesA;
extern pDeleteFileA					PDeleteFileA;
extern pDeleteFileW					PDeleteFileW;
extern pGlobalAlloc					PGlobalAlloc;
extern pCreateFileA					PCreateFileA;
extern pCreateFileW					PCreateFileW;
extern pLocalAlloc					PLocalAlloc;
extern pCreatePipe					PCreatePipe;
extern pFreeResource				PFreeResource;
extern pCopyFileA					PCopyFileA;
extern pCopyFileW					PCopyFileW;
extern pLoadResource				PLoadResource;
extern pWindExec					PWindExec;
extern pCreateProcessW				PCreateProcessW;
extern pProcess32FirstW				PProcess32FirstW;
extern pProcess32NextW				PProcess32NextW;
extern pGetTempPathA				PGetTempPathA;
extern pGetTempPathW				PGetTempPathW;
extern pGetLocalTime				PGetLocalTime;
extern pGetStartupInfoA				PGetStartupInfoA;
extern pGetFileSizeEx				PGetFileSizeEx;
extern pIsWow64Process				PIsWow64Process;
extern pGetModuleFileNameA			PGetModuleFileNameA;
extern pGetModuleFileNameW			PGetModuleFileNameW;
extern pSetFileAttributesA			PSetFileAttributesA;
extern pSetFileAttributesW			PSetFileAttributesW;
extern plstrlenA					PlstrlenA;
extern pWriteFile					PWriteFile;
extern pReadFile					PReadFile;
extern pSystemTimeToFileTime		PSystemTimeToFileTime;
extern pProcessIdToSessionId		PProcessIdToSessionId;
extern pInterlockedExchange			PInterlockedExchange;
extern pInterlockedDecrement		PInterlockedDecrement;
extern pGetComputerNameW			PGetComputerNameW;
extern pInterlockedIncrement		PInterlockedIncrement;
extern pSetFilePointerEx			PSetFilePointerEx;
extern pGetTickCount				PGetTickCount;
extern pSleep						PSleep;
extern pLocalFree					PLocalFree;
extern pGlobalFree					PGlobalFree;
extern pGetLastError				PGetLastError;
extern pOutputDebugStringA			POutputDebugStringA;

//GDI32.DLL
typedef WINGDIAPI COLORREF	(WINAPI * pGetPixel)			( IN HDC, IN int, IN int);
typedef WINGDIAPI BOOL		(WINAPI * pDeleteDC)			( IN HDC);
typedef WINGDIAPI BOOL		(WINAPI * pDeleteObject)		( IN HGDIOBJ);
typedef WINGDIAPI int		(WINAPI * pGetDIBits)			( IN HDC, IN HBITMAP, IN UINT, IN UINT, OUT LPVOID, IN OUT LPBITMAPINFO, IN UINT);
typedef WINGDIAPI UINT		(WINAPI * pRealizePalette)		( IN HDC);
typedef WINGDIAPI HPALETTE	(WINAPI * pSelectPalette)		( IN HDC, IN HPALETTE, IN BOOL);
typedef WINGDIAPI HGDIOBJ	(WINAPI * pGetStockObject)		( IN int);
typedef WINGDIAPI int		(WINAPI * pGetDeviceCaps)		( HDC, int);
typedef WINGDIAPI BOOL		(WINAPI * pBitBlt)				( HDC, int, int, int, int, HDC, int, int, DWORD);
typedef WINGDIAPI HGDIOBJ	(WINAPI * pSelectObject)		( IN HDC, IN HGDIOBJ);
typedef WINGDIAPI HDC		(WINAPI * pCreateDCA)			( IN LPCSTR, IN LPCSTR, IN LPCSTR, IN CONST DEVMODEA *);
typedef WINGDIAPI HDC		(WINAPI * pCreateCompatibleDC)	( HDC);
typedef WINGDIAPI HBITMAP	(WINAPI * pCreateCompatibleBitmap) ( HDC, int, int);
typedef WINGDIAPI int		(WINAPI * pGetObjectA)			( IN HGDIOBJ, IN int, OUT LPVOID);
typedef WINGDIAPI BOOL		(WINAPI * pStretchBlt)			( IN HDC, IN int, IN int, IN int, IN int, IN HDC, IN int, IN int, IN int, IN int, IN DWORD);


extern pGetPixel			PGetPixel;
extern pDeleteDC			PDeleteDC;
extern pDeleteObject		PDeleteObject;
extern pGetDIBits			PGetDIBits;
extern pRealizePalette		PRealizePalette;
extern pSelectPalette		PSelectPalette;
extern pGetStockObject		PGetStockObject;
extern pGetDeviceCaps		PGetDeviceCaps;
extern pBitBlt				PBitBlt;
extern pSelectObject		PSelectObject;
extern pCreateDCA			PCreateDCA;
extern pCreateCompatibleDC	PCreateCompatibleDC;
extern pCreateCompatibleBitmap	PCreateCompatibleBitmap;
extern pGetObjectA			PGetObjectA;
extern pStretchBlt			PStretchBlt;

//SHELL32.DLL
typedef BOOL		(STDAPICALLTYPE * pSHGetPathFromIDListW)		(LPCITEMIDLIST pidl, LPWSTR pszPath);
typedef	HRESULT		(STDAPICALLTYPE * pSHGetSpecialFolderLocation)	(HWND hwnd, int csidl, LPITEMIDLIST *ppidl);
typedef	BOOL		(STDAPICALLTYPE * pSHGetSpecialFolderPathA)		(HWND hwnd, LPSTR pszPath, int csidl, BOOL fCreate);
typedef	BOOL		(STDAPICALLTYPE * pSHGetSpecialFolderPathW)		(HWND hwnd, LPWSTR pszPath, int csidl, BOOL fCreate);
typedef HINSTANCE	(STDAPICALLTYPE * pShellExecuteA) (HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
typedef HRESULT		(STDAPICALLTYPE * pSHGetMalloc) (LPMALLOC * ppMalloc);
typedef HRESULT		(STDAPICALLTYPE * pSHGetDesktopFolder) (IShellFolder **ppshf);

extern pSHGetPathFromIDListW		PSHGetPathFromIDListW;
extern pSHGetSpecialFolderLocation	PSHGetSpecialFolderLocation;
extern pSHGetSpecialFolderPathA		PSHGetSpecialFolderPathA;
extern pSHGetSpecialFolderPathW		PSHGetSpecialFolderPathW;
extern pShellExecuteA				PShellExecuteA;
extern pSHGetMalloc					PSHGetMalloc;
extern pSHGetDesktopFolder			PSHGetDesktopFolder;

//OLE32.dll
typedef HRESULT		(STDAPICALLTYPE * pCoCreateInstance)	(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv);
typedef void		(STDAPICALLTYPE * pCoUninitialize)		(void);
typedef HRESULT		(STDAPICALLTYPE * pCoInitialize)		(IN LPVOID pvReserved);
typedef HRESULT		(STDAPICALLTYPE * pCLSIDFromProgID)		(IN LPCOLESTR lpszProgID, OUT LPCLSID lpclsid);

extern pCoCreateInstance			PCoCreateInstance;
extern pCoUninitialize				PCoUninitialize;
extern pCoInitialize				PCoInitialize;
extern pCLSIDFromProgID				PCLSIDFromProgID;

//SETUPAPI.dll
typedef BOOL		(WINAPI * pSetupDiCallClassInstaller)	( DI_FUNCTION InstallFunction, HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData OPTIONAL );
typedef BOOL		(WINAPI * pSetupDiDestroyDeviceInfoList)( HDEVINFO DeviceInfoSet );
typedef BOOL		(WINAPI * pSetupDiEnumDeviceInfo)		( HDEVINFO DeviceInfoSet, DWORD MemberIndex, PSP_DEVINFO_DATA DeviceInfoData );
typedef HDEVINFO	(WINAPI * pSetupDiGetClassDevsA)		( CONST GUID *ClassGuid, OPTIONAL IN PCSTR Enumerator, OPTIONAL IN HWND hwndParent, OPTIONAL IN DWORD Flags );
typedef BOOL		(WINAPI * pSetupDiGetDeviceRegistryPropertyA)(HDEVINFO DeviceInfoSet,PSP_DEVINFO_DATA DeviceInfoData,DWORD Property,PDWORD PropertyRegDataType, OPTIONAL OUT PBYTE PropertyBuffer,DWORD PropertyBufferSize,PDWORD RequiredSize OPTIONAL);
typedef BOOL		(WINAPI * pSetupDiSetClassInstallParamsA)( IN HDEVINFO DeviceInfoSet, IN PSP_DEVINFO_DATA DeviceInfoData, OPTIONAL IN PSP_CLASSINSTALL_HEADER ClassInstallParams, OPTIONAL IN DWORD ClassInstallParamsSize );
typedef BOOL		(WINAPI * pSetupDiEnumDeviceInterfaces)(
							IN  HDEVINFO                   DeviceInfoSet,
							IN  PSP_DEVINFO_DATA           DeviceInfoData,     OPTIONAL
							IN  CONST GUID                *InterfaceClassGuid,
							IN  DWORD                      MemberIndex,
							OUT PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData
    );
typedef BOOL		(WINAPI * pSetupDiGetDeviceInterfaceDetailA)(
								 IN  HDEVINFO                           DeviceInfoSet,
								 IN  PSP_DEVICE_INTERFACE_DATA          DeviceInterfaceData,
								 OUT PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData,     OPTIONAL
								 IN  DWORD                              DeviceInterfaceDetailDataSize,
								 OUT PDWORD                             RequiredSize,                  OPTIONAL
								 OUT PSP_DEVINFO_DATA                   DeviceInfoData                 OPTIONAL
    );

extern pSetupDiCallClassInstaller	PSetupDiCallClassInstaller;
extern pSetupDiDestroyDeviceInfoList PSetupDiDestroyDeviceInfoList;
extern pSetupDiEnumDeviceInfo		PSetupDiEnumDeviceInfo;
extern pSetupDiGetClassDevsA		PSetupDiGetClassDevsA;
extern pSetupDiGetDeviceRegistryPropertyA	PSetupDiGetDeviceRegistryPropertyA;
extern pSetupDiSetClassInstallParamsA PSetupDiSetClassInstallParamsA;
extern pSetupDiEnumDeviceInterfaces PSetupDiEnumDeviceInterfaces;
extern pSetupDiGetDeviceInterfaceDetailA PSetupDiGetDeviceInterfaceDetailA;

//Ws2_32.dll
typedef int			(WSAAPI * pSendTo)					( SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen);
typedef struct hostent FAR * (WSAAPI *pgethostbyname)	( const char FAR * name );
typedef SOCKET		(WSAAPI * psocket)					( int af, int type, int protocol );
typedef int			(WSAAPI * precvfrom)				( SOCKET s, char FAR * buf, int len, int flags, struct sockaddr FAR * from, int FAR * fromlen );
typedef unsigned long (WSAAPI * pinet_addr)				( const char FAR * cp );
typedef int (WSAAPI * pWSAStartup)	( IN WORD wVersionRequested, OUT LPWSADATA lpWSAData );
typedef u_long (WSAAPI * pntohl)	( IN u_long netlong );
typedef u_long (WSAAPI * phtonl)	( IN u_long hostlong );
typedef u_short (WSAAPI * phtons)	( IN u_short hostshort );
typedef int (WSAAPI * precv)		( IN SOCKET s, OUT char FAR * buf, IN int len, IN int flags );
typedef int (WSAAPI * psend)		( IN SOCKET s, IN const char FAR * buf, IN int len, IN int flags );
typedef int (WSAAPI * pconnect)		( IN SOCKET s, IN const struct sockaddr FAR * name, IN int namelen );
typedef char FAR * (WSAAPI *pinet_ntoa)	( IN struct in_addr in );
typedef struct hostent FAR * (WSAAPI *pgethostbyaddr)	( IN const char FAR * addr, IN int len, IN int type );
typedef struct protoent FAR * (WSAAPI *pgetprotobyname)	( IN const char FAR * name );
typedef int (WSAAPI *psetsockopt)	( IN SOCKET s, IN int level, IN int optname, IN const char FAR * optval, IN int optlen );
typedef int (WSAAPI *pWSAGetLastError) ( void );

extern pSendTo	PSendTo;
extern pgethostbyname Pgethostbyname;
extern psocket Psocket;
extern precvfrom Precvfrom;
extern pinet_addr Pinet_addr;
extern pWSAStartup PWSAStartup;
extern pntohl Pntohl;
extern phtonl Phtonl;
extern phtons Phtons;
extern precv Precv;
extern psend Psend;
extern pconnect Pconnect;
extern pinet_ntoa Pinet_ntoa;
extern pgethostbyaddr Pgethostbyaddr;
extern pgetprotobyname Pgetprotobyname;
extern psetsockopt Psetsockopt;
extern pWSAGetLastError PWSAGetLastError;

//wininet.dll
typedef HINTERNET (WINAPI * pInternetOpenA) (IN LPCSTR lpszAgent, IN DWORD dwAccessType, IN LPCSTR lpszProxy OPTIONAL, IN LPCSTR lpszProxyBypass OPTIONAL, IN DWORD dwFlags);
typedef BOOL (WINAPI * pInternetReadFile) (IN HINTERNET hFile, IN LPVOID lpBuffer, IN DWORD dwNumberOfBytesToRead, OUT LPDWORD lpdwNumberOfBytesRead);
typedef BOOL (WINAPI * pInternetCloseHandle) (IN HINTERNET hInternet);
typedef HINTERNET (WINAPI * pInternetOpenUrlA) (IN HINTERNET hInternet, IN LPCSTR lpszUrl, IN LPCSTR lpszHeaders OPTIONAL, IN DWORD dwHeadersLength, IN DWORD dwFlags, IN DWORD dwContext);

extern pInternetOpenA PInternetOpenA;
extern pInternetReadFile PInternetReadFile;
extern pInternetCloseHandle PInternetCloseHandle;
extern pInternetOpenUrlA PInternetOpenUrlA;

//Wtsapi32.dll
typedef BOOL (WINAPI * pWTSEnumerateSessionsA) (IN HANDLE hServer, IN DWORD Reserved, IN DWORD Version, OUT PWTS_SESSION_INFOA * ppSessionInfo, OUT DWORD * pCount);
typedef BOOL (WINAPI * pWTSQueryUserToken) (ULONG SessionId, PHANDLE phToken);
typedef VOID (WINAPI * pWTSFreeMemory) (IN PVOID pMemory);

extern pWTSEnumerateSessionsA PWTSEnumerateSessionsA;
extern pWTSQueryUserToken PWTSQueryUserToken;
extern pWTSFreeMemory PWTSFreeMemory;

//Userenv.dll
typedef USERENVAPI BOOL (WINAPI * pCreateEnvironmentBlock) (OUT LPVOID *lpEnvironment, IN HANDLE hToken, IN BOOL bInherit);
typedef USERENVAPI BOOL (WINAPI * pDestroyEnvironmentBlock) (IN LPVOID  lpEnvironment);

extern pCreateEnvironmentBlock PCreateEnvironmentBlock;
extern pDestroyEnvironmentBlock PDestroyEnvironmentBlock;

//ntdll.dll
typedef LONG (WINAPI *pNtQueryInformationProcess)(HANDLE, UINT32, PVOID, UINT32, UINT32*);
typedef LONG (WINAPI *pNtWow64QueryInformationProcess64)(HANDLE, UINT32, PVOID, UINT32, UINT32*);
typedef LONG (WINAPI *pNtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded);
typedef LONG (WINAPI *pNtWow64ReadVirtualMemory64)(HANDLE ProcessHandle, UINT64 BaseAddress, PVOID Buffer, UINT64 NumberOfBytesToRead, PUINT64 NumberOfBytesReaded);

extern pNtQueryInformationProcess PNtQueryInformationProcess;
extern pNtWow64QueryInformationProcess64 PNtWow64QueryInformationProcess64;
extern pNtReadVirtualMemory PNtReadVirtualMemory;
extern pNtWow64ReadVirtualMemory64 PNtWow64ReadVirtualMemory64;

//version.dll
typedef BOOL (APIENTRY * pGetFileVersionInfoA) (LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
typedef DWORD (APIENTRY * pGetFileVersionInfoSizeA) (LPCSTR lptstrFilename, LPDWORD lpdwHandle);
typedef BOOL (APIENTRY * pVerQueryValueA) (const LPVOID pBlock, LPSTR lpSubBlock, LPVOID * lplpBuffer, PUINT puLen);

extern pGetFileVersionInfoA PGetFileVersionInfoA;
extern pGetFileVersionInfoSizeA PGetFileVersionInfoSizeA;
extern pVerQueryValueA PVerQueryValueA;

//oleaut32.dll
typedef BSTR (STDAPICALLTYPE * pSysAllocString) (const OLECHAR *);
typedef void (STDAPICALLTYPE * pSysFreeString) (BSTR);

extern pSysAllocString PSysAllocString;
extern pSysFreeString PSysFreeString;

void	GetNTProcesspFun();	

HMODULE myLoadLibrary(LPCSTR lpLibFileName);

DWORD myGetProcAddress(HMODULE hModule, LPCSTR lpProcName);

void GetLoadFun(LPVOID);

#endif // !defined(AFX_WINAPI_H__B4B48599_8EA7_4DE2_BBF2_0656874A77BD__INCLUDED_)
