// Si216Client.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Si216Client.h"
#include "Si216ClientDlg.h"
#include "Glbs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSi216ClientApp

BEGIN_MESSAGE_MAP(CSi216ClientApp, CWinApp)
	//{{AFX_MSG_MAP(CSi216ClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSi216ClientApp construction

CSi216ClientApp::CSi216ClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSi216ClientApp object

CSi216ClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSi216ClientApp initialization

BOOL CSi216ClientApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}	
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CSi216ClientDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	stopWork();
	return FALSE;
}

void CSi216ClientApp::stopWork()
{
	mUdpSocket.Close();
	udpSendPrg.Close();
	if(errReportFile.m_hFile!=CFile::hFileNull)
		errReportFile.Close();
}
