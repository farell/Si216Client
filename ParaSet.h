#if !defined(AFX_PARASET_H__406A4C7A_AE4B_4BFB_8D7D_88C1D76893AA__INCLUDED_)
#define AFX_PARASET_H__406A4C7A_AE4B_4BFB_8D7D_88C1D76893AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParaSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ParaSet dialog

class ParaSet : public CDialog
{
// Construction
public:	
	void findID();
	void modifyParameter(int num);
	void showParaList(int ch);
	int channel;//下拉框选中的通道号,0,1,2,3,4
	int line;   //在参数表中选中的行号。
	
	void init();
	ParaSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ParaSet)
	enum { IDD = IDD_ParaSet };
	CComboBox	m_SelSpectChn;
	CListCtrl	m_paraList;
	CString	m_id;
	CString	m_unit;
	CString	m_type;
	CString	m_position;
	float	m_orignalWavelength;
	CString	m_note;
	float	m_alarmMax;
	float	m_alarmMin;
	float	m_waveLengthLeftLimited;
	float	m_waveLengthRightLimited;
	float	m_orignalTemperature;
	BOOL	m_shield;
	float	m_temperatureCoefficnet;//传感器温度系数；
	float	m_tCompezation;         //传感器温补系数，相当于B
	CString	m_matchFbgID;
	float	m_k;
	double	m_a;
	double	m_b;
	double	m_c;
	double	m_d;
	double	m_e;
	double	m_f;
	double	m_bhk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ParaSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ParaSet)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void Onaccept();
	afx_msg void OnReadParameter();
	afx_msg void OnaddPara();
	afx_msg void Ondelete();
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton1();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARASET_H__406A4C7A_AE4B_4BFB_8D7D_88C1D76893AA__INCLUDED_)
