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
	static int i=0;	 //ѭ���������
	static int j=0;  //�ܵı������
	if(inform=="��λ")   //����Ǹ�λ������ӵ���һ���У���������µ��С�
	{
		i=0;	
		CString no1;
		no1.Format("%d",j+1);
		
		CTime	cur1=CTime::GetCurrentTime();
		CString str1=cur1.Format("%Y-%m-%d��%H:%M:%S");	//��ʾ������
		m_showAlarmControl.SetItemText(0,0,no1); 
		m_showAlarmControl.SetItemText(0,1,str1); 
		m_showAlarmControl.SetItemText(0,2,"��λ"); 
		
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
	CString str=cur.Format("%Y-%m-%d��%H:%M:%S");	//��ʾ������
	m_showAlarmControl.SetItemText(i,0,no);   //i ��.��j��, i��0��ʼ,J��1��ʼ,��ΪJ��0�б����ռ��
	m_showAlarmControl.SetItemText(i,1,str);   //i ��.��j��, i��0��ʼ,J��1��ʼ,��ΪJ��0�б����ռ��	
	m_showAlarmControl.SetItemText(i,2,inform);   //i ��.��j��, i��0��ʼ,J��1��ʼ,��ΪJ��0�б����ռ��
	
	i++;
	j++;
	if(i>=23)
		i=2;
	m_showAlarmControl.SetItemText(i,0,"");   //i ��.��j��, i��0��ʼ,J��1��ʼ,��ΪJ��0�б����ռ��
	m_showAlarmControl.SetItemText(i,1,"");   //i ��.��j��, i��0��ʼ,J��1��ʼ,��ΪJ��0�б����ռ��	
	m_showAlarmControl.SetItemText(i,2,"");   //i ��.��j��, i��0��ʼ,J��1��ʼ,��ΪJ��0�б����ռ��

}

void CAlarmInform::ini_listAlarm()
{
	HWND hWnd=m_showAlarmControl.m_hWnd;	         //��ȡ���
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//��ȡ��ʾ��ʽ
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//������ʾ��ʽ
	m_showAlarmControl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//��������,ȫѡһ�� 
	int nColumnCount =12;   // m_showAlarmControl.GetHeaderCtrl()()->GetItemCount();
	//	int MAX_CHANNEL_PLUSE1=29;	
	
	for (int i=0;i < nColumnCount;i++)
	{
		m_showAlarmControl.DeleteColumn(i);//?����?0->i?		
	}	
	m_showAlarmControl.DeleteAllItems();
	
	m_showAlarmControl.InsertColumn(0,"���",LVCFMT_CENTER,50);
	m_showAlarmControl.InsertColumn(1,"ʱ��",LVCFMT_LEFT,160);
	m_showAlarmControl.InsertColumn(2,"��Ϣ",LVCFMT_LEFT,500);	
	
	CString	temp;
	for(int i=0;i<23;i++) 
	{
		temp.Format("%d",i+1);
		m_showAlarmControl.InsertItem(i+1,temp);  //������	
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
