#if !defined(AFX_CHECKHISTORYALARM_H__F3A9873B_0284_416B_A962_1BA6139D0772__INCLUDED_)
#define AFX_CHECKHISTORYALARM_H__F3A9873B_0284_416B_A962_1BA6139D0772__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckHistoryAlarm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCheckHistoryAlarm dialog

class CCheckHistoryAlarm : public CDialog
{
	// Construction
public:
	int everySheet;     //每页显示的数量
	void GetTotalCount();
	int nCountAcess;             //报警记录中有多少条记录。
	int sheetStartCount;               //查询的时候的每一页的起始点。	
	void ReadAlarmInfoFile();
	void ini_list();
	CCheckHistoryAlarm(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CCheckHistoryAlarm)
	enum { IDD = IDD_CHECKHISTORYALARM };
	CListCtrl	m_historyALarm;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckHistoryAlarm)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CCheckHistoryAlarm)
	virtual BOOL OnInitDialog();
	afx_msg void OnExit();
	afx_msg void Onbefore();
	afx_msg void Onnext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKHISTORYALARM_H__C3F4F3D3_301A_499A_B64E_02789EA599AE__INCLUDED_)
