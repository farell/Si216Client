// Si216ClientDlg.h : header file
//

#if !defined(AFX_SI216CLIENTDLG_H__0ED9DA25_5D0A_4ED2_B417_73DED36A7A16__INCLUDED_)
#define AFX_SI216CLIENTDLG_H__0ED9DA25_5D0A_4ED2_B417_73DED36A7A16__INCLUDED_

#include "RWAcess.h"	// Added by ClassView
#include "WaveSheet1.h"	// Added by ClassView
#include "Strain.h"	// Added by ClassView
#include "Udp.h"
#include "ParaSet.h"	// Added by ClassView
#include "Administrator.h"
#include "AlarmInform.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSi216ClientDlg dialog

class CSi216ClientDlg : public CDialog
{
// Construction
public:
	void alarmLight(int color1);
	int m_countUdpErr;
	bool flag_udpReceived;  // ’µΩUDP¡À°£
	void showLight(int color1);
	CAlarmInform alarmInfo;
	CWinThread* m_fbgCalThread;
	RWAcess m_access;
	ParaSet m_paraSet;
	CAdministrator m_admin;

	void stopSoft();
	void init();
	CStrain m_strain;
	CWaveSheet1 m_wavesheet1;
	
	void table_init();
	CSi216ClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSi216ClientDlg)
	enum { IDD = IDD_SI216CLIENT_DIALOG };
	CTabCtrl	m_CtrlTab;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSi216ClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSi216ClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SI216CLIENTDLG_H__0ED9DA25_5D0A_4ED2_B417_73DED36A7A16__INCLUDED_)
