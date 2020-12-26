// ColorListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColorListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct ItemData
{
public:
	ItemData(): dwData( NULL ), arrpsz( NULL ), crText ( NULL ), crBk (NULL) {}
	DWORD dwData;
	LPTSTR* arrpsz;
	//color
	COLORREF* crText;
	COLORREF* crBk;
	
private:
	// ban copying.
	ItemData( const ItemData& );
	ItemData& operator=( const ItemData& );
};

/////////////////////////////////////////////////////////////////////////////
// CColorListCtrl

CColorListCtrl::CColorListCtrl()
{
	crWindow        = ::GetSysColor(COLOR_WINDOW);
	crWindowText    = ::GetSysColor(COLOR_WINDOWFRAME);
	crHighLight     = ::GetSysColor(COLOR_HIGHLIGHT);
	crHighLightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
}

CColorListCtrl::~CColorListCtrl()
{

}


BEGIN_MESSAGE_MAP(CColorListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CColorListCtrl)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorListCtrl message handlers

// void CColorListCtrl::SetItemColor(int nItem, int nSubItem, COLORREF clrText, COLORREF clrBkgnd)
// {
// 	ASSERT(nItem >= 0);
// 	ASSERT(nItem < GetItemCount());
// 	if ((nItem < 0) || nItem >= GetItemCount())
// 		return ;
// 	ASSERT(nSubItem >= 0);
// 	ASSERT(nSubItem < GetColumnCount());
// 	if ((nSubItem < 0) || nSubItem >= GetColumnCount())
// 		return ;
// 	
// 	ItemData *pid = (ItemData *) CListCtrl::GetItemData(nItem);
// 	if (pid)
// 	{
// 		(pid->crText)[nSubItem]	= (clrText == -1) ? crWindowText : clrText;
// 		(pid->crBak)[nSubItem] = (clrBkgnd == -1) ? crWindow : clrBkgnd;
// 	}
// }

// void CColorListCtrl::UpdateSubItem(int nItem, int nSubItem)
// {
// 	ASSERT(nItem >= 0);
// 	ASSERT(nItem < GetItemCount());
// 	if ((nItem < 0) || nItem >= GetItemCount())
// 		return;
// 	ASSERT(nSubItem >= 0);
// 	ASSERT(nSubItem < GetColumnCount());
// 	if ((nSubItem < 0) || nSubItem >= GetColumnCount())
// 		return;
// 	
// 	CRect rect;
// 	if (nSubItem == -1)
// 	{
// 		GetItemRect(nItem, &rect, LVIR_BOUNDS);
// 	}
// 	else
// 	{
// 		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
// 	}
// }

int CColorListCtrl::GetColumnCount()
{
	return ((CHeaderCtrl*)CListCtrl::GetHeaderCtrl())->GetItemCount();
}

int CColorListCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	int m_iColumns = GetColumnCount();
	ItemData* pid = new ItemData();
	pid->arrpsz = new LPTSTR[ m_iColumns ];
	pid->crText = new COLORREF[ m_iColumns ];
	pid->crBk = new COLORREF[ m_iColumns ];
	
	pid->arrpsz[0] = new TCHAR[ lstrlen( lpszItem ) + 1 ];
	for (int i=0; i<m_iColumns; i++)
	{
		if(i)
			pid->arrpsz[i] = NULL;
		pid->crText[i] = crWindowText;
		pid->crBk[i] = crWindow;
	}
	CListCtrl::InsertItem(nItem, lpszItem);
	CListCtrl::SetItemData(nItem, (DWORD)pid);
	
	return nItem;
}

BOOL CColorListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText, COLORREF clrText, COLORREF clrBk)
{
	ItemData* pid = (ItemData*) CListCtrl::GetItemData(nItem);
	LPTSTR pszText = NULL;
	if(pid->arrpsz[nSubItem] != NULL)
	{
		pszText = pid->arrpsz[nSubItem];
		delete pszText;
		pszText = NULL;
	}
	pszText = new TCHAR[ lstrlen( lpszText ) + 1 ];
	lstrcpy( pszText, lpszText );
	pid->arrpsz[nSubItem] = pszText;
	pid->crText[nSubItem] = (clrText < 0) ? crWindowText : clrText;
	pid->crBk[nSubItem] = (clrBk < 0) ? crWindow : clrBk;

	if( !CListCtrl::SetItemText( nItem, nSubItem, lpszText ) )
		return FALSE;
	
	return TRUE;
}

void CColorListCtrl::OnSysColorChange() 
{
	CListCtrl::OnSysColorChange();
	// TODO: Add your message handler code here
	crWindow        = GetSysColor(COLOR_WINDOW);
	crWindowText    = GetSysColor(COLOR_WINDOWTEXT);
	crHighLight     = GetSysColor(COLOR_HIGHLIGHT);
	crHighLightText = GetSysColor(COLOR_HIGHLIGHTTEXT);	
}

void CColorListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	//draw each item.set txt color,bkcolor....
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	
	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;
	
	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	
	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.
		
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.
		
		int nItem = static_cast<int> (pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;
		
		ItemData *pXLCD = (ItemData *) pLVCD->nmcd.lItemlParam;
		ASSERT(pXLCD);
		
		COLORREF crText  = crWindowText;
		COLORREF crBkgnd = crWindow;
		
		if (pXLCD)
		{
			crText  = (pXLCD->crText)[nSubItem];
			crBkgnd = (pXLCD->crBk)[nSubItem];
		}
		// store the colors back in the NMLVCUSTOMDRAW struct
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;
		
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		CRect rect;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
		COLORREF crTextReverse = RGB(255-GetRValue(crText),255-GetGValue(crText),255-GetBValue(crText));
		if (GetItemState(nItem, LVIS_SELECTED))
			DrawText(nItem, nSubItem, pDC, crBkgnd, crHighLight, rect);
		else
			DrawText(nItem, nSubItem, pDC, crText, crBkgnd, rect);
		
		*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
	}
}

void CColorListCtrl::DrawText(int nItem, int nSubItem, CDC *pDC, COLORREF crText, COLORREF crBkgnd, CRect &rect)
{
	ASSERT(pDC);
	
	pDC->FillSolidRect(&rect, crBkgnd);
	
	CString str;
	str = GetItemText(nItem, nSubItem);
	
	if (!str.IsEmpty())
	{
		// get text justification
		HDITEM hditem;
		hditem.mask = HDI_FORMAT;
		((CHeaderCtrl*)CListCtrl::GetHeaderCtrl())->GetItem(nSubItem, &hditem);
		int nFmt = hditem.fmt & HDF_JUSTIFYMASK;
		UINT nFormat = DT_VCENTER | DT_SINGLELINE;
		if (nFmt == HDF_CENTER)
			nFormat |= DT_CENTER;
		else if (nFmt == HDF_LEFT)
			nFormat |= DT_LEFT;
		else
			nFormat |= DT_RIGHT;
		
		pDC->SetTextColor(crText);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(crBkgnd);
		pDC->DrawText(str, &rect, nFormat);
	}
}

DWORD CColorListCtrl::GetItemData( int nItem ) const
{
	ASSERT( nItem < GetItemCount() );
	
	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( nItem ) );
	ASSERT( pid );
	return pid->dwData;
}

BOOL CColorListCtrl::GetSubItemRect(int nItem, int nSubItem, int nArea, CRect &rect)
{
	
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumnCount());
	if ((nSubItem < 0) || nSubItem >= GetColumnCount())
		return FALSE;
	
	BOOL bRC = CListCtrl::GetSubItemRect(nItem, nSubItem, nArea, rect);
	
	// if nSubItem == 0, the rect returned by CListCtrl::GetSubItemRect
	// is the entire row, so use left edge of second subitem
	
	if (nSubItem == 0)
	{
		if (GetColumnCount() > 1)
		{
			CRect rect1;
			bRC = CListCtrl::GetSubItemRect(nItem, 1, LVIR_BOUNDS, rect1);
			rect.right = rect1.left;
		}
	}
	
	return bRC;
}

void CColorListCtrl::OnPaint() 
{
	Default();
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CListCtrl::OnPaint() for painting messages
}

void CColorListCtrl::FreeItemMemory( const int iItem )
{
	int m_iColumns = GetColumnCount();
	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( iItem ) );
	LPTSTR* arrpsz = pid->arrpsz;
	for( int i = 0; i < m_iColumns; i++ )
	{
		delete[] arrpsz[i];	
	}
	delete[] pid->crText;
	delete[] pid->crBk;
	delete[] arrpsz;
	delete pid;
	
	VERIFY( CListCtrl::SetItemData( iItem, NULL ) );
}

BOOL CColorListCtrl::DeleteAllItems()
{
	for( int iItem = 0; iItem < CListCtrl::GetItemCount(); iItem ++ )
	{
		FreeItemMemory( iItem );
	}
	
	return CListCtrl::DeleteAllItems();
}
