// getprocessinfo.h: interface for the getprocessinfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETPROCESSINFO_H__D1736DAB_08F6_42F8_8F56_99CFF9C5B910__INCLUDED_)
#define AFX_GETPROCESSINFO_H__D1736DAB_08F6_42F8_8F56_99CFF9C5B910__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
	ProcessWow64Information = 26
} PROCESSINFOCLASS;

typedef struct
{
    DWORD Filler[4];
    DWORD ProcessParameters;
} __PEB;

typedef struct
{
    UINT64 Filler[4];
    UINT64 ProcessParameters;
} __PEB64;

typedef struct _UNICODE_STRING64 {
    SHORT Length;
    SHORT MaximumLength;
    DWORD Fill;
    UINT64  Buffer;
} UNICODE_STRING64;

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

typedef struct 
{
    DWORD MaximumLength;
    DWORD Length;
    DWORD Flags;
    DWORD DebugFlags;
    PVOID ConsoleHandle;
    DWORD ConsoleFlags;
    PVOID StandardInput;
    PVOID StandardOutput;
    PVOID StandardError;
    //////////////////////////
    UNICODE_STRING DosPath;    //CurrentDirectory
    HANDLE Handle;
    //////////////////////////
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CmdLine;
    //бнбн
}_RTL_USER_PROCESS_PARAMETERS;

typedef struct 
{
    DWORD MaximumLength;
    DWORD Length;
    DWORD Flags;
    DWORD DebugFlags;
    UINT64 ConsoleHandle;
    DWORD ConsoleFlags;
    UINT64 StandardInput;
    UINT64 StandardOutput;
    UINT64 StandardError;
    //////////////////////////
    UNICODE_STRING64 DosPath;//CurrentDirectory
    HANDLE Handle;
    //////////////////////////
    UNICODE_STRING64 DllPath;
    UNICODE_STRING64 ImagePathName;
    UNICODE_STRING64 CmdLine;
    //бнбн
}_RTL_USER_PROCESS_PARAMETERS64;

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

// end_ntddk end_ntifs 
typedef struct _PROCESS_BASIC_INFORMATION64 { 
    UINT64 Reserved1;
    UINT64 PebBaseAddress;
    UINT64 Reserved2[2];
    UINT64 UniqueProcessId;
    UINT64 Reserved3;
} PROCESS_BASIC_INFORMATION64,*PPROCESS_BASIC_INFORMATION64; 

int EnumProcess(char* buf);

DWORD ThreadGetProcessInfo(char * pPara);

#endif // !defined(AFX_GETPROCESSINFO_H__D1736DAB_08F6_42F8_8F56_99CFF9C5B910__INCLUDED_)
