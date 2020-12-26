#if !defined(AFX_ALARMINFORM_H__AEBEABD6_F158_412B_8182_D2D86162F126__INCLUDED_)
#define AFX_ALARMINFORM_H__AEBEABD6_F158_412B_8182_D2D86162F126__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlarmInform.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlarmInform dialog

class CAlarmInform : public CDialog  //显示报警记录的页面。
{
// Construction
public:
	void ini_listAlarm();
	void addAlarmInform(CString inform);  //直接往里输入，此函数会刷新列表，m_showAlarmControl，
	CAlarmInform(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlarmInform)
	enum { IDD = IDD_ALARMINFORM };
	CListCtrl	m_showAlarmControl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmInform)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlarmInform)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckalarm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMINFORM_H__AEBEABD6_F158_412B_8182_D2D86162F126__INCLUDED_)
