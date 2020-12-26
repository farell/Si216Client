#if !defined(AFX_COLORLISTCTRL_H__957265A6_52C8_48C5_B670_222EBA846DCF__INCLUDED_)
#define AFX_COLORLISTCTRL_H__957265A6_52C8_48C5_B670_222EBA846DCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorListCtrl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CColorListCtrl window

class CColorListCtrl : public CListCtrl
{
// Construction
public:
	CColorListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawText(int nItem, int nSubItem, CDC *pDC, COLORREF crText, COLORREF crBkgnd, CRect &rect);
	BOOL SetItemText( int nItem, int nSubItem, LPCTSTR lpszText, COLORREF clrText, COLORREF clrBkgnd);
	int InsertItem(int nItem, LPCTSTR lpszItem);
	int GetColumnCount();
//	void UpdateSubItem(int nItem, int nSubItem);
//	void SetItemColor(int nItem, int nSubItem, COLORREF& clrText, COLORREF& clrBk);
	DWORD GetItemData(int nItem) const;
	BOOL GetSubItemRect(int nItem, int nSubItem, int nArea, CRect& rect);
	BOOL DeleteAllItems();
	void FreeItemMemory( const int iItem );
	virtual ~CColorListCtrl();
private:
	COLORREF crWindowText,crWindow,crHighLight,crHighLightText;
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorListCtrl)
	afx_msg void OnPaint();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORLISTCTRL_H__957265A6_52C8_48C5_B670_222EBA846DCF__INCLUDED_)
