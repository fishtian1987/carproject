// PttApp.h : main header file for the PTTAPP application
//

#if !defined(AFX_PTTAPP_H__8EBFE157_D06D_4509_91AD_928D5D16F83A__INCLUDED_)
#define AFX_PTTAPP_H__8EBFE157_D06D_4509_91AD_928D5D16F83A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPttAppApp:
// See PttApp.cpp for the implementation of this class
//

class CPttAppApp : public CWinApp
{
public:
	CPttAppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPttAppApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPttAppApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTTAPP_H__8EBFE157_D06D_4509_91AD_928D5D16F83A__INCLUDED_)
