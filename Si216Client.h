// Si216Client.h : main header file for the SI216CLIENT application
//

#if !defined(AFX_SI216CLIENT_H__D56C3C71_6924_4B31_9112_4C5C41074F51__INCLUDED_)
#define AFX_SI216CLIENT_H__D56C3C71_6924_4B31_9112_4C5C41074F51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSi216ClientApp:
// See Si216Client.cpp for the implementation of this class
//

class CSi216ClientApp : public CWinApp
{
public:
	void stopWork();
	CSi216ClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSi216ClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSi216ClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SI216CLIENT_H__D56C3C71_6924_4B31_9112_4C5C41074F51__INCLUDED_)
