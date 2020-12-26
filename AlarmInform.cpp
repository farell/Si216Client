// AlarmInform.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "AlarmInform.h"
#include "CheckHistoryAlarm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmInform dialog


CAlarmInform::CAlarmInform(CWnd* pParent /*=NULL*/)
	: CDialog(CAlarmInform::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlarmInform)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAlarmInform::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmInform)
	DDX_Control(pDX, IDC_LIST3, m_showAlarmControl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlarmInform, CDialog)
	//{{AFX_MSG_MAP(CAlarmInform)
	ON_BN_CLICKED(IDC_CHECKALARM, OnCheckalarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmInform message handlers

void CAlarmInform::addAlarmInform(CString inform)
{
	static int i=0;	 //循环报警序号
	static int j=0;  //总的报警序号
	if(inform=="复位")   //如果是复位，则添加到第一条中，并清空以下的行。
	{
		i=0;	
		CString no1;
		no1.Format("%d",j+1);
		
		CTime	cur1=CTime::GetCurrentTime();
		CString str1=cur1.Format("%Y-%m-%d号%H:%M:%S");	//显示年月日
		m_showAlarmControl.SetItemText(0,0,no1); 
		m_showAlarmControl.SetItemText(0,1,str1); 
		m_showAlarmControl.SetItemText(0,2,"复位"); 
		
		for(int k=1;k<23;k++)
		{	
			m_showAlarmControl.SetItemText(k,0,"");
			m_showAlarmControl.SetItemText(k,1,"");
			m_showAlarmControl.SetItemText(k,2,"");			
		}
		i++;
		j++;
		return;
	}	
	CString no;
	no.Format("%d",j+1);
	CTime	cur=CTime::GetCurrentTime();
	CString str=cur.Format("%Y-%m-%d号%H:%M:%S");	//显示年月日
	m_showAlarmControl.SetItemText(i,0,no);   //i 行.第j列, i从0开始,J从1开始,因为J的0列被序号占了
	m_showAlarmControl.SetItemText(i,1,str);   //i 行.第j列, i从0开始,J从1开始,因为J的0列被序号占了	
	m_showAlarmControl.SetItemText(i,2,inform);   //i 行.第j列, i从0开始,J从1开始,因为J的0列被序号占了
	
	i++;
	j++;
	if(i>=23)
		i=2;
	m_showAlarmControl.SetItemText(i,0,"");   //i 行.第j列, i从0开始,J从1开始,因为J的0列被序号占了
	m_showAlarmControl.SetItemText(i,1,"");   //i 行.第j列, i从0开始,J从1开始,因为J的0列被序号占了	
	m_showAlarmControl.SetItemText(i,2,"");   //i 行.第j列, i从0开始,J从1开始,因为J的0列被序号占了

}

void CAlarmInform::ini_listAlarm()
{
	HWND hWnd=m_showAlarmControl.m_hWnd;	         //获取句柄
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//获取显示格式
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//设置显示格式
	m_showAlarmControl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//加坐标线,全选一行 
	int nColumnCount =12;   // m_showAlarmControl.GetHeaderCtrl()()->GetItemCount();
	//	int MAX_CHANNEL_PLUSE1=29;	
	
	for (int i=0;i < nColumnCount;i++)
	{
		m_showAlarmControl.DeleteColumn(i);//?对吗?0->i?		
	}	
	m_showAlarmControl.DeleteAllItems();
	
	m_showAlarmControl.InsertColumn(0,"序号",LVCFMT_CENTER,50);
	m_showAlarmControl.InsertColumn(1,"时间",LVCFMT_LEFT,160);
	m_showAlarmControl.InsertColumn(2,"信息",LVCFMT_LEFT,500);	
	
	CString	temp;
	for(int i=0;i<23;i++) 
	{
		temp.Format("%d",i+1);
		m_showAlarmControl.InsertItem(i+1,temp);  //插入行	
	}

}

BOOL CAlarmInform::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ini_listAlarm();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAlarmInform::OnCheckalarm() 
{
	CCheckHistoryAlarm checkHistoryAlarmDlg;
	checkHistoryAlarmDlg.DoModal();
}
