#if !defined(AFX_ADMINISTRATOR_H__F224EE0E_DC30_4750_85A8_B5F39411F6AE__INCLUDED_)
#define AFX_ADMINISTRATOR_H__F224EE0E_DC30_4750_85A8_B5F39411F6AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Administrator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdministrator dialog

class CAdministrator : public CDialog
{
	// Construction
public:		
	void saveClick();
	
	CWinThread* m_saveResultThread;  
	CFile Datafile[16];  //16个波长文件的数组，用于CHAR型保存。
	CString save16ChFileName[16];//16个文件的名字
	void init();
	byte a[4];
	CAdministrator(CWnd* pParent = NULL);   // standard constructor
	void creatCh16FileName(); //创建16个通道的文件名。
	void closeCh16File();//关闭这16个文件。
	
	// Dialog Data
	//{{AFX_DATA(CAdministrator)
	enum { IDD = IDD_ADMINISTRATOR };
	CIPAddressCtrl	m_ipAddress;
	BOOL	m_autoSend;
	BOOL	m_saveResult;
	int		m_saveSample;
	BOOL	m_sampleCheck;
	int		m_udpSendFrame;
	BOOL	m_udpSendChoice;
	BYTE	m_pcID;
	BOOL	m_sendFftBuffer;
	UINT	m_addressPort;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdministrator)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CAdministrator)
	virtual BOOL OnInitDialog();
	afx_msg void OnmodifyIP();
	afx_msg void OnautoSend();
	afx_msg void OnsaveResult();
	afx_msg void Onreset();
	afx_msg void OnsampleCheck();
	afx_msg void Onset();
	afx_msg void OnUdpSendChoice();
	afx_msg void OnsendFFTBuffer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADMINISTRATOR_H__F224EE0E_DC30_4750_85A8_B5F39411F6AE__INCLUDED_)
