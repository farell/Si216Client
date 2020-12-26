#if !defined(AFX_WAVESHEET1_H__16DCE9CC_F48D_4436_BAAB_015645222DD1__INCLUDED_)
#define AFX_WAVESHEET1_H__16DCE9CC_F48D_4436_BAAB_015645222DD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaveSheet1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaveSheet1 dialog

class CWaveSheet1 : public CDialog
{
// Construction
public:
	void refresh_list();
	void init();
	CWaveSheet1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaveSheet1)
	enum { IDD = IDD_WaveSheet };
	CListCtrl	m_ListChannelRes;
	int		m_count1;
	int		m_maxCycle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveSheet1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaveSheet1)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVESHEET1_H__16DCE9CC_F48D_4436_BAAB_015645222DD1__INCLUDED_)
