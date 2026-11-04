// Jangki.h : main header file for the JANGKI application
//

#if !defined(AFX_JANGKI_H__530CA6E5_6E30_11D3_B509_BEB3B0969997__INCLUDED_)
#define AFX_JANGKI_H__530CA6E5_6E30_11D3_B509_BEB3B0969997__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CJangkiApp:
// See Jangki.cpp for the implementation of this class
//

class CJangkiApp : public CWinApp
{
public:
	CJangkiApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJangkiApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CJangkiApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JANGKI_H__530CA6E5_6E30_11D3_B509_BEB3B0969997__INCLUDED_)
