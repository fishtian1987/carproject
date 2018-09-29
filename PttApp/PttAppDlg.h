// PttAppDlg.h : header file
//

#if !defined(AFX_PTTAPPDLG_H__27480E32_0951_46D9_AFF1_CAE3B56E1856__INCLUDED_)
#define AFX_PTTAPPDLG_H__27480E32_0951_46D9_AFF1_CAE3B56E1856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPttAppDlg dialog

class CPttAppDlg : public CDialog
{
// Construction
public:
	CPttAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPttAppDlg)
	enum { IDD = IDD_PTTAPP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPttAppDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPttAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

bool RunShadow();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTTAPPDLG_H__27480E32_0951_46D9_AFF1_CAE3B56E1856__INCLUDED_)
