// CheckHistoryAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "CheckHistoryAlarm.h"
#include "Glbs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckHistoryAlarm dialog



CCheckHistoryAlarm::CCheckHistoryAlarm(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckHistoryAlarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckHistoryAlarm)
		// NOTE: the ClassWizard will add member initialization here
	everySheet=100;
	//}}AFX_DATA_INIT
}


void CCheckHistoryAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckHistoryAlarm)
	DDX_Control(pDX, IDC_LIST1, m_historyALarm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckHistoryAlarm, CDialog)
	//{{AFX_MSG_MAP(CCheckHistoryAlarm)
	ON_BN_CLICKED(IDC_Exit, OnExit)
	ON_BN_CLICKED(IDC_before, Onbefore)
	ON_BN_CLICKED(IDC_next, Onnext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckHistoryAlarm message handlers


BOOL CCheckHistoryAlarm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ini_list();
	GetTotalCount();
	if(nCountAcess<everySheet)
		GetDlgItem(IDC_before)->EnableWindow(FALSE);

	GetDlgItem(IDC_next)->EnableWindow(FALSE);
	Sleep(100);
	ReadAlarmInfoFile();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCheckHistoryAlarm::ini_list()
{
	HWND hWnd=m_historyALarm.m_hWnd;	         //��ȡ���
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//��ȡ��ʾ��ʽ
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//������ʾ��ʽ
	m_historyALarm.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//��������,ȫѡһ�� 
	int nColumnCount =12;   // m_showAlarmControl.GetHeaderCtrl()()->GetItemCount();
	int MAX_CHANNEL_PLUSE1=29;	
	int i;
/*	for (i=0;i < nColumnCount;i++)
	{
		m_historyALarm.DeleteColumn(i);//?����?0->i?		
	}	
*/
	m_historyALarm.DeleteAllItems();
	
	m_historyALarm.InsertColumn(0,"���",LVCFMT_CENTER,50);
	m_historyALarm.InsertColumn(1,"���б��",LVCFMT_CENTER,80);
	m_historyALarm.InsertColumn(2,"ʱ��",LVCFMT_LEFT,155);
	m_historyALarm.InsertColumn(3,"����������",LVCFMT_LEFT,145);	
	m_historyALarm.InsertColumn(4,"������Ϣ",LVCFMT_LEFT,280);	
	
	CString	temp;
/**/
	for(i=0;i<100;i++) 
	{
		temp.Format("%d",i+1);
		m_historyALarm.InsertItem(i+1,"");  //������	
	}

}

void CCheckHistoryAlarm::OnExit() 
{
	CDialog::OnCancel();
}

void CCheckHistoryAlarm::ReadAlarmInfoFile()  //�ӱ�����¼�ļ��ж�ȡ��Ϣ������ʾ�ڱ����
{
	int fromNo=sheetStartCount;        //�ӱ��п�ʼ�����
	
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\��ʷ��¼\\������¼.mdb";
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// �򿪱���Access��FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("���ݿ�����ʧ��  ��鿴'������¼.mdb'  �Ƿ���ȷ  3!");
		return;
	} 
	//////////////////   ����ʷ��¼���ݿ�  /////////////////////////////////////////////		
	
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //��ʼ��	
	try                       //�򿪱�CHN,���Ƿ�ɹ�.
	{
		CString mm="SELECT * FROM ��ʷ��¼";
		m_pRecordset->Open(_bstr_t(mm),          // ��ѯ�ļ�   CHN��	
			m_pConnection.GetInterfacePtr(),	 // ��ȡ��ӿ��IDispatchָ��
			adOpenStatic,
			//				adOpenDynamic,
			adLockOptimistic,
			adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return;
	}
	//////////////////////	������ȡCHN �������ݡ�
	try
	{
		if(!m_pRecordset->BOF)    //���CHN��������,���������һ��.
		{
			m_pRecordset->MoveLast();
			if(nCountAcess-sheetStartCount>=0)
			m_pRecordset->Move(sheetStartCount-nCountAcess);
		}
		else
		{
			AfxMessageBox("��������Ϊ��");   //���Ϊ��, Ӧ�ø�ֵΪ0,������һͨ��,�˳���ѭ��			
		}
		//////////////////////ԭʼ�����Ȳ���///////////////////////////////////////
		CString temp,str;
		int iii=0;
		while(iii<everySheet)  
		{		////////////����ȡ�������ݡ��ļ�βm_pRecordset->BOF=-1,!m_pRecordset->BOF=0;  ���ļ�βΪ0, !()=1;
			temp.Format("%d",fromNo);				// ����ת��Ϊ�ַ�����   			
			m_historyALarm.SetItemText(iii,0,temp); 

			_variant_t var1 = m_pRecordset->GetCollect(_variant_t("���"));//���"));
			if(var1.vt != VT_NULL)	
			{				
				int ab=var1.iVal;			
				temp.Format("%d",ab);			// ����ת��Ϊ�ַ�����     						
				m_historyALarm.SetItemText(iii,1,temp); 	
			}
			else
			{
				m_historyALarm.SetItemText(iii,1,""); 					
			}	
			////////////////////////////////////////////////////////////////////////////////
			var1 = m_pRecordset->GetCollect(_variant_t("ʱ��"));
			if(var1.vt != VT_NULL)
			{	
				COleDateTime t(var1.date);
				temp=t.Format("%Y-%m-%d %H:%M:%S");
				//s.ReleaseBuffer();		
				m_historyALarm.SetItemText(iii,2,temp); 
			}					
			else
			{
				m_historyALarm.SetItemText(iii,2,""); 					
			}	
			///////////////////////////////////////////////////////////////////////////////////
			var1 = m_pRecordset->GetCollect(_variant_t("����������"));
			if(var1.vt != VT_NULL)	
			{
				temp=var1.bstrVal;
				m_historyALarm.SetItemText(iii,3,temp); 					
			}
			else
			{
				m_historyALarm.SetItemText(iii,3,""); 					
			}	
			///////////////////////////////////////////////////////////////////////////////////
			var1 = m_pRecordset->GetCollect(_variant_t("��������"));
			if(var1.vt != VT_NULL)
			{
				temp=var1.bstrVal;
				m_historyALarm.SetItemText(iii,4,temp); 					
			}
			else
			{
				m_historyALarm.SetItemText(iii,4,""); 					
			}	
			/////////////////////////////////////////////////////////////////
			iii++;	
			fromNo--;
			if(fromNo<1)   //��������<1��ʾ�Ѿ����ļ�ͷ�ˡ�
				break;
			/////////////////////////////////////////////////////
			m_pRecordset->MovePrevious();		
		}		
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
		return;
	}  	
	m_pConnection->Close();	
}


void CCheckHistoryAlarm::GetTotalCount()
{
	static int totalNo=0;  //���е������
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\��ʷ��¼\\������¼.mdb";
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// �򿪱���Access��FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("���ݿ�����ʧ��  ��鿴'������¼.mdb'  �Ƿ���ȷ  1!");
		return;
	} 
	//////////////////   ����ʷ��¼���ݿ�  /////////////////////////////////////////////		
	
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //��ʼ��
	
	try                       //�򿪱�CHN,���Ƿ�ɹ�.
	{
		CString mm="SELECT * FROM ��ʷ��¼";
		m_pRecordset->Open(_bstr_t(mm),          // ��ѯ�ļ�   CHN��	
			m_pConnection.GetInterfacePtr(),	 // ��ȡ��ӿ��IDispatchָ��
			adOpenStatic,
			//				adOpenDynamic,
			adLockOptimistic,
			adCmdText);
		nCountAcess= m_pRecordset->GetRecordCount();//�õ��м�������
		sheetStartCount=nCountAcess;
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return;
	}
	m_pConnection->Close();	
}


void CCheckHistoryAlarm::Onbefore() 
{
	if(sheetStartCount>everySheet)
		sheetStartCount-=everySheet;
////---------------------------------------------------------
	if(sheetStartCount>nCountAcess-everySheet)  //
	{
		GetDlgItem(IDC_next)->EnableWindow(FALSE);			
	}
	else 
	{
		GetDlgItem(IDC_next)->EnableWindow(true);
	}//��ť

	if(sheetStartCount<everySheet)  //
	{
		GetDlgItem(IDC_before)->EnableWindow(FALSE);			
	}
	else 
	{
		GetDlgItem(IDC_before)->EnableWindow(true);
	}
///////////////////////////////////////////////////////
	for(int i=0;i<everySheet;i++)//����б�
	{
		m_historyALarm.SetItemText(i,0,""); 	
		m_historyALarm.SetItemText(i,1,""); 	
		m_historyALarm.SetItemText(i,2,""); 	
		m_historyALarm.SetItemText(i,3,""); 	
		m_historyALarm.SetItemText(i,4,""); 	
	}
	ReadAlarmInfoFile();	
}

void CCheckHistoryAlarm::Onnext() 
{
	if(sheetStartCount<=nCountAcess-everySheet)
		sheetStartCount+=everySheet;

	if(sheetStartCount>nCountAcess-everySheet)  //
	{
		GetDlgItem(IDC_next)->EnableWindow(FALSE);			
	}
	else 
	{
		GetDlgItem(IDC_next)->EnableWindow(true);
	}//��ť
	
	if(sheetStartCount<everySheet)  //
	{
		GetDlgItem(IDC_before)->EnableWindow(FALSE);			
	}
	else 
	{
		GetDlgItem(IDC_before)->EnableWindow(true);
	}

	for(int i=0;i<everySheet;i++)//����б�
	{
		m_historyALarm.SetItemText(i,0,""); 	
		m_historyALarm.SetItemText(i,1,""); 	
		m_historyALarm.SetItemText(i,2,""); 	
		m_historyALarm.SetItemText(i,3,""); 	
		m_historyALarm.SetItemText(i,4,""); 	
	}
	ReadAlarmInfoFile();	
}
