// WaveSheet1.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "WaveSheet1.h"
#include "Glbs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveSheet1 dialog


CWaveSheet1::CWaveSheet1(CWnd* pParent /*=NULL*/)
	: CDialog(CWaveSheet1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaveSheet1)
	m_count1 = 0;
	m_maxCycle = 0;
	//}}AFX_DATA_INIT
}


void CWaveSheet1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaveSheet1)
	DDX_Control(pDX, IDC_LIST1, m_ListChannelRes);
	DDX_Text(pDX, IDC_Count1, m_count1);
	DDX_Text(pDX, IDC_maxCycle, m_maxCycle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaveSheet1, CDialog)
	//{{AFX_MSG_MAP(CWaveSheet1)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveSheet1 message handlers

void CWaveSheet1::init()
{
	HWND hWnd=m_ListChannelRes.m_hWnd;	         //��ȡ���
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//��ȡ��ʾ��ʽ
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//������ʾ��ʽ
	m_ListChannelRes.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//��������,ȫѡһ�� 
	//m_ListChannelRes.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_TYPEMASK|LVS_SHOWSELALWAYS);//��������
	int nColumnCount =m_ListChannelRes.GetHeaderCtrl()->GetItemCount();
	m_ListChannelRes.DeleteAllItems();
	m_ListChannelRes.InsertColumn(0,"���",LVCFMT_CENTER,50);
	m_ListChannelRes.InsertColumn(1,"ͨ��1����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(2,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(3,"ͨ��2����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(4,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(5,"ͨ��3����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(6,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(7,"ͨ��4����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(8,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(9,"ͨ��5����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(10,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(11,"ͨ��6����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(12,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(13,"ͨ��7����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(14,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(15,"ͨ��8����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(16,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(17,"ͨ��9����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(18,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(19,"ͨ��10����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(20,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(21,"ͨ��11����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(22,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(23,"ͨ��12����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(24,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(25,"ͨ��13����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(26,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(27,"ͨ��14����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(28,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(29,"ͨ��15����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(50,"����",LVCFMT_LEFT,50);
	m_ListChannelRes.InsertColumn(31,"ͨ��16����",LVCFMT_LEFT,72);
	m_ListChannelRes.InsertColumn(32,"����",LVCFMT_LEFT,50);
	CString	temp;
	int i;
	for(i=0;i<64;i++) {
		temp.Format("%d",i+1);
		m_ListChannelRes.InsertItem(i+1,temp);
	}
}

BOOL CWaveSheet1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	init();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaveSheet1::refresh_list()
{
	CString	temp;
	int i,j,num;
	m_count1=recvFrNum;
	m_maxCycle=g_temp;


	UpdateData(FALSE);
	for(i=0;i<16;i++)
	{
		num=g_UdpBuffer[m_udpCount.UdpCount_old].mChnRes[i].num;//num = mRecvData.mChnRes[i].num;
		for(j=0;j<64;j++){
			m_ListChannelRes.SetItemText(j, 2*i+1, "");
			m_ListChannelRes.SetItemText(j, 2*i+2, "");
		}		
		for(j=0;j<num;j++){
			//temp.Format("%9.4f",mRecvData.mChnRes[i].mWavelg[j]);
			temp.Format("%9.4f",g_UdpBuffer[m_udpCount.UdpCount_old].mChnRes[i].mWavelg[j]);
			m_ListChannelRes.SetItemText(j, 2*i+1, temp);
			temp.Format("%6.2f",g_UdpBuffer[m_udpCount.UdpCount_old].mChnRes[i].mPower[j]);
			m_ListChannelRes.SetItemText(j, 2*i+2, temp);		
		}
	}
}



