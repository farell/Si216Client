// Strain.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "Strain.h"
#include "Glbs.h"
#include "Si216ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStrain dialog


CStrain::CStrain(CWnd* pParent /*=NULL*/)
	: CDialog(CStrain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStrain)
	m_calMidd = 0;
	//}}AFX_DATA_INIT
}


void CStrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStrain)
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Text(pDX, IDC_calMidd, m_calMidd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStrain, CDialog)
	//{{AFX_MSG_MAP(CStrain)
	ON_BN_CLICKED(IDC_reset, Onreset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStrain message handlers


void CStrain::init()
{
	HWND hWnd=m_list1.m_hWnd;      //��ȡ���
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//��ȡ��ʾ��ʽ
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//������ʾ��ʽ
	m_list1.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//��������,ȫѡһ�� 	
	int nColumnCount =m_list1.GetHeaderCtrl()->GetItemCount();
	
	m_list1.DeleteAllItems();	
	m_list1.InsertColumn(0,"���",LVCFMT_LEFT,40);
	/////////////////////////////////////
	
	m_list1.InsertColumn(1,"ͨ��1",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(2,"ͨ��2",LVCFMT_LEFT,72);  
	m_list1.InsertColumn(3,"ͨ��3",LVCFMT_LEFT,72);
	m_list1.InsertColumn(4,"ͨ��4",LVCFMT_LEFT,72);
	m_list1.InsertColumn(5,"ͨ��5",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(6,"ͨ��6",LVCFMT_LEFT,72);
	m_list1.InsertColumn(7,"ͨ��7.",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(8,"ͨ��8",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(10,"ͨ��9",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(11,"ͨ��10",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(11,"ͨ��11",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(12,"ͨ��12",LVCFMT_LEFT,72);  
	m_list1.InsertColumn(13,"ͨ��13",LVCFMT_LEFT,72);
	m_list1.InsertColumn(14,"ͨ��14",LVCFMT_LEFT,72);
	m_list1.InsertColumn(15,"ͨ��15",LVCFMT_LEFT,72); 
	m_list1.InsertColumn(16,"ͨ��16",LVCFMT_LEFT,72);

	CString	temp;
	int i;
	for(i=0;i<20;i++)
	{
		temp.Format("%d",i+1);
		m_list1.InsertItem(i,temp);
	}
}

BOOL CStrain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	init();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStrain::Onreset() 
{


}
//RGB(0,255,255);��  RGB(255,0,255);�� RGB(255,255,0);//�� 
//RGB(255,255,255);�ס�RGB(0,0,0)��
void CStrain::refresh_StrainList()
{	
	COLORREF crBackGroud,crText=RGB(0,0,0);
	CString temp;
	for(int ch=0;ch<16;ch++) 
	{
		for(int j=0;j<20;j++)	     //�����
		{
			m_list1.SetItemText(j, ch+1, "",RGB(0,0,0),RGB(255,255,255));
		}

		for( int j=0;j<maxSensor[ch];j++)  
		{
// 			if (ch==1 && j==3)
// 			{
// 				int ttt=1;
//  			}
			//switch(m_fbgPar[ch][j].alarmStatus)//�л�������ɫ
			switch(m_fbgPar[ch][j].showAlarmStatus)//�л�������ɫ
			{
			case NORMAL:
				crBackGroud=RGB(255,255,255);break;  //��
			case FAULT1:
				crBackGroud=RGB(255,255,0);break;    //��
			case FAULT2:
				crBackGroud=RGB(255,255,200);break;  //ǳ��
			case ALARM:
				crBackGroud=RGB(255,0,255);break;    //��
			default:
				crBackGroud=RGB(255,255,255);break;  //�ס�
			}
			if (m_fbgPar[ch][j].shield==true)
				crBackGroud=RGB(255,255,255);
			/*
			if (m_fbgPar[ch][j].sensorType=="�¶ȼ�" || m_fbgPar[ch][j].sensorType=="T")
			{
				temp.Format("%9.1f ��",m_fbgPar[ch][j].temperature);
				//temp+=m_fbgPar[ch][j].unit;
			}//memcpy(&m_fbgPar[ch][sn].fft_result.maxfre,&fft1.maxfre[0],sizeof(frequncy)*4);
			else if (m_fbgPar[ch][j].sensorType=="���ٶȼ�" || m_fbgPar[ch][j].sensorType=="ACCE")
			{
				temp.Format("%9.4f",m_fbgPar[ch][j].fft_result.maxfre[0].f);
			}
			else
			{
				temp.Format("%9.1f ",m_fbgPar[ch][j].strain);
				temp+=m_fbgPar[ch][j].unit;
			}
			*/			
			if (m_fbgPar[ch][j].sensorType=="���ٶȼ�" || m_fbgPar[ch][j].sensorType=="ACCE")//���ٶȼ�
			{		
				float tempF;
				if (m_fbgPar[ch][j].fft_result.maxfre[0].amplitude>=0.004)//����ʾ��ʱ�򣬴���һ����ֵ����ʾ��������ʾ0��
					tempF=m_fbgPar[ch][j].fft_result.maxfre[0].f;					
				else
					tempF=0.0;
				temp.Format("%9.1f",tempF);
				temp+=m_fbgPar[ch][j].unit;				
				/*	temp.Format("%9.5f",m_fbgPar[ch][j].fft_result.maxfre[0].f);
				CString sss;
				sss.Format("\n %f %f    %f %f    %f %f    %f %f ",
				m_fbgPar[ch][j].fft_result.maxfre[0].f,m_fbgPar[ch][j].fft_result.maxfre[0].amplitude,
				m_fbgPar[ch][j].fft_result.maxfre[1].f,m_fbgPar[ch][j].fft_result.maxfre[1].amplitude,
				m_fbgPar[ch][j].fft_result.maxfre[2].f,m_fbgPar[ch][j].fft_result.maxfre[2].amplitude,
				m_fbgPar[ch][j].fft_result.maxfre[3].f,m_fbgPar[ch][j].fft_result.maxfre[3].amplitude
				); 	ReportErr(&sss);
				*/
			}
			else //��ͨ������
			{				
				temp.Format("%9.1f",m_fbgPar[ch][j].strain);
				temp+=m_fbgPar[ch][j].unit;			
			}
			if (m_fbgPar[ch][j].alarmStatus==FAULT1 || m_fbgPar[ch][j].alarmStatus==FAULT2)			
				temp="      #";
			m_list1.SetItemText(j, ch+1, temp,crText,crBackGroud);			
		}
	}
	m_calMidd=m_udpCount.calMiddNum;
	//CString sss;
	//sss.Format("  count1= %d   calMiddNum= %d  calCount= %d ",m_udpCount.UdpCount1,m_udpCount.calMiddNum,m_udpCount.CalCount); ReportErr(&sss);
	UpdateData(false);	
}
