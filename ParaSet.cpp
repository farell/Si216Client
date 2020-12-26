// ParaSet.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "ParaSet.h"
#include "Glbs.h"
#include "Si216ClientDlg.h"
#include "ColorListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ParaSet dialog


ParaSet::ParaSet(CWnd* pParent /*=NULL*/)
	: CDialog(ParaSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(ParaSet)
	m_id = _T("");
	m_unit = _T("");
	m_type = _T("");
	m_position = _T("");
	m_orignalWavelength = 0.0f;
	m_note = _T("");
	m_alarmMax = 0.0f;
	m_alarmMin = 0.0f;
	m_waveLengthLeftLimited = 0.0f;
	m_waveLengthRightLimited = 0.0f;
	m_orignalTemperature = 0.0f;
	m_shield = FALSE;
	m_temperatureCoefficnet = 0.0f;
	m_tCompezation = 0.0f;
	m_matchFbgID = _T("");
	m_k = 0.0f;
	m_a = 0.0;
	m_b = 0.0;
	m_c = 0.0;
	m_d = 0.0;
	m_e = 0.0;
	m_f = 0.0;
	m_bhk = 0.0;
	//}}AFX_DATA_INIT
}


void ParaSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ParaSet)
	DDX_Control(pDX, IDC_COMBO1, m_SelSpectChn);
	DDX_Control(pDX, IDC_LIST2, m_paraList);
	DDX_Text(pDX, IDC_id, m_id);
	DDX_Text(pDX, IDC_unit, m_unit);
	DDX_Text(pDX, IDC_type, m_type);
	DDX_Text(pDX, IDC_position, m_position);
	DDX_Text(pDX, IDC_orignWave, m_orignalWavelength);
	DDX_Text(pDX, IDC_note, m_note);
	DDX_Text(pDX, IDC_alarmMax, m_alarmMax);
	DDX_Text(pDX, IDC_alarmMin, m_alarmMin);
	DDX_Text(pDX, IDC_leftWave, m_waveLengthLeftLimited);
	DDX_Text(pDX, IDC_rightWave, m_waveLengthRightLimited);
	DDX_Text(pDX, IDC_orignTemp, m_orignalTemperature);
	DDX_Check(pDX, IDC_shield, m_shield);
	DDX_Text(pDX, IDC_tempCoeff, m_temperatureCoefficnet);
	DDX_Text(pDX, IDC_tempCompCoeff, m_tCompezation);
	DDX_Text(pDX, IDC_tempCompID, m_matchFbgID);
	DDX_Text(pDX, IDC_k, m_k);
	DDX_Text(pDX, IDC_a, m_a);
	DDX_Text(pDX, IDC_b, m_b);
	DDX_Text(pDX, IDC_c, m_c);
	DDX_Text(pDX, IDC_d, m_d);
	DDX_Text(pDX, IDC_e, m_e);
	DDX_Text(pDX, IDC_f, m_f);
	DDX_Text(pDX, IDC_bhk, m_bhk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ParaSet, CDialog)
	//{{AFX_MSG_MAP(ParaSet)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickList2)
	ON_BN_CLICKED(IDC_accept, Onaccept)
	ON_BN_CLICKED(IDC_ReadParameter, OnReadParameter)
	ON_BN_CLICKED(IDC_addPara, OnaddPara)
	ON_BN_CLICKED(IDC_delete, Ondelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkList2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ParaSet message handlers

BOOL ParaSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	init();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ParaSet::init()
{	/////////////////������///////////////////////	//m_SelSpectChn.AddString("ETALON");
	int num,ch;
	for(int chn=0;chn<16;chn++)
	{
		CString s1;
		s1.Format("%d",chn+1);
		s1="ͨ��"+s1;
		m_SelSpectChn.AddString(s1);
	}
	m_SelSpectChn.SetCurSel(0);
	channel=m_SelSpectChn.GetCurSel();//
	line=0;      //�ڲ�������ѡ�е��кš�0,1,2,3,4,

	////////////�б�//////////////////////
	HWND hWnd=m_paraList.m_hWnd;      //��ȡ���
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//��ȡ��ʾ��ʽ
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//������ʾ��ʽ
	//m_paraList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//��������,ȫѡһ�� 	
	m_paraList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
	
	int nColumnCount =m_paraList.GetHeaderCtrl()->GetItemCount();
	
	m_paraList.DeleteAllItems();

	CString temp;
	
	m_paraList.InsertColumn(0,"���",LVCFMT_CENTER,50);
	m_paraList.InsertColumn(1,"ID",LVCFMT_LEFT,50);
	m_paraList.InsertColumn(2,"����������",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(3,"��ǰֵ",LVCFMT_LEFT,60);
	m_paraList.InsertColumn(4,"��λ",LVCFMT_LEFT,25);	
	
	m_paraList.InsertColumn(5,"ԭʼ����",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(6,"��������",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(7,"��������",LVCFMT_LEFT,72);
	/*
	m_paraList.InsertColumn(8,"��ʼ�¶�",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(9,"�¶�ϵ��",LVCFMT_LEFT,72);	
	m_paraList.InsertColumn(10,"����ϵ��",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(11,"�²�ID",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(12,"�²�ϵ��",LVCFMT_LEFT,72);	
	
	m_paraList.InsertColumn(13,"���ޱ���",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(14,"���ޱ���",LVCFMT_LEFT,72);	
	*/
	m_paraList.InsertColumn(8,"a",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(9,"b",LVCFMT_LEFT,88);	
	m_paraList.InsertColumn(10,"c",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(11,"k",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(12,"d",LVCFMT_LEFT,88);		
	m_paraList.InsertColumn(13,"e",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(14,"f",LVCFMT_LEFT,88);	
	m_paraList.InsertColumn(15,"�²�ID",LVCFMT_LEFT,72);
	///////////////////////////////////////////////////////
	m_paraList.InsertColumn(16,"��������",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(17,"��������",LVCFMT_LEFT,72);	
	m_paraList.InsertColumn(18,"����λ��",LVCFMT_LEFT,90);
	m_paraList.InsertColumn(19,"����",LVCFMT_LEFT,40);
	m_paraList.InsertColumn(20,"��ע",LVCFMT_LEFT,100);
	m_paraList.InsertColumn(21,"",LVCFMT_LEFT,20);
	
	for(int i=0; i<21; i++)
	{
		temp.Format("%d",i+1);
		m_paraList.InsertItem(i,temp);
	}
//////////////////��ʾ�����������///////////////////////////
	//showParaList(channel);
}
/*
void ParaSet::showParaList(int ch)//���������� m_fbgPara �������б��н�����ʾ��
{	
	CString temp;
	int num;
	///////////////////�����////////////////////
	for (int line1=0;line1<20;line1++)
	{	
		for (num=0;num<17;num++)
		{
			temp.Format("%d %d",line1,num);
			m_paraList.SetItemText(line1,num+1,"");
		}
	}
	//�� m_fbgPar�е�ֵ���������ȥ��
	for (num=0;num<maxSensor[ch];num++)   //num���кţ�ÿ��������ռһ�С�
	{
		temp=m_fbgPar[ch][num].ID;
		m_paraList.SetItemText(num,1,temp);

		temp=m_fbgPar[ch][num].sensorType;
		m_paraList.SetItemText(num,2,temp);		

		m_paraList.SetItemText(num,3,"");
		temp=m_fbgPar[ch][num].unit;
		m_paraList.SetItemText(num,4,temp);	
		
		temp.Format("%9.3f",m_fbgPar[ch][num].orignalWavelengh);
		m_paraList.SetItemText(num,5,temp);	

		temp.Format("%9.3f",m_fbgPar[ch][num].waveLengthLeftLimited);
		m_paraList.SetItemText(num,6,temp);	
		temp.Format("%9.3f",m_fbgPar[ch][num].waveLengthRightLimited);
		m_paraList.SetItemText(num,7,temp);	

		temp.Format("%4.1f",m_fbgPar[ch][num].orignalTemperature);
		m_paraList.SetItemText(num,8,temp);	

		temp.Format("%4.1f",m_fbgPar[ch][num].temperatureCoefficnet);
		m_paraList.SetItemText(num,9,temp);	

		temp.Format("%7.1f",m_fbgPar[ch][num].k);
		m_paraList.SetItemText(num,10,temp);	

		temp=m_fbgPar[ch][num].matchFbgID;
		m_paraList.SetItemText(num,11,temp);

		temp.Format("%9.1f",m_fbgPar[ch][num].tCompezation);
		m_paraList.SetItemText(num,12,temp);		

		temp.Format("%9.1f",m_fbgPar[ch][num].alarmMin);
		m_paraList.SetItemText(num,13,temp);	
		temp.Format("%9.1f",m_fbgPar[ch][num].alarmMax);
		m_paraList.SetItemText(num,14,temp);

		temp=m_fbgPar[ch][num].position;
		m_paraList.SetItemText(num,15,temp);	

		if (m_fbgPar[ch][num].shield==true)
			temp="����";
		else
			temp="";
		m_paraList.SetItemText(num,16,temp);	

		temp=m_fbgPar[ch][num].note;
		m_paraList.SetItemText(num,17,temp);			
	}		
	
}   */

void ParaSet::showParaList(int ch)//���������� m_fbgPara �������б��н�����ʾ��
{	
	CString temp;
	int num;
	///////////////////�����////////////////////
	for (int line1=0;line1<20;line1++)
	{	
		for (num=0;num<20;num++)
		{
			temp.Format("%d %d",line1,num);
			m_paraList.SetItemText(line1,num+1,"");
		}
	}
	//�� m_fbgPar�е�ֵ���������ȥ��
	for (num=0;num<maxSensor[ch];num++)   //num���кţ�ÿ��������ռһ�С�
	{
		temp=m_fbgPar[ch][num].ID;
		m_paraList.SetItemText(num,1,temp);
		
		temp=m_fbgPar[ch][num].sensorType;
		m_paraList.SetItemText(num,2,temp);		
		
		m_paraList.SetItemText(num,3,"");
		temp=m_fbgPar[ch][num].unit;
		m_paraList.SetItemText(num,4,temp);
		
		temp.Format("%9.4f",m_fbgPar[ch][num].orignalWavelengh);
		m_paraList.SetItemText(num,5,temp);
		
		temp.Format("%9.4f",m_fbgPar[ch][num].waveLengthLeftLimited);
		m_paraList.SetItemText(num,6,temp);	
		temp.Format("%9.4f",m_fbgPar[ch][num].waveLengthRightLimited);
		m_paraList.SetItemText(num,7,temp);	

		////////////////////////////////////////////////
		temp.Format("%7.10f",m_fbgPar[ch][num].bh.a);
		m_paraList.SetItemText(num,8,temp);	
		
		temp.Format("%7.10f",m_fbgPar[ch][num].bh.b);
		m_paraList.SetItemText(num,9,temp);	
		
		temp.Format("%7.10f",m_fbgPar[ch][num].bh.c);
		m_paraList.SetItemText(num,10,temp);	

		temp.Format("%7.10f",m_fbgPar[ch][num].bh.k);
		m_paraList.SetItemText(num,11,temp);	
		
		temp.Format("%7.10f",m_fbgPar[ch][num].bh.d);
		m_paraList.SetItemText(num,12,temp);	

		temp.Format("%7.10f",m_fbgPar[ch][num].bh.e);
		m_paraList.SetItemText(num,13,temp);	

		temp.Format("%7.10f",m_fbgPar[ch][num].bh.f);
		m_paraList.SetItemText(num,14,temp);	
		
		////////////////////////////////////////////////
		temp=m_fbgPar[ch][num].matchFbgID;
		m_paraList.SetItemText(num,15,temp);		
		
		temp.Format("%9.1f",m_fbgPar[ch][num].alarmMin);
		m_paraList.SetItemText(num,16,temp);	
		temp.Format("%9.1f",m_fbgPar[ch][num].alarmMax);
		m_paraList.SetItemText(num,17,temp);
		
		temp=m_fbgPar[ch][num].position;
		m_paraList.SetItemText(num,18,temp);	
		
		if (m_fbgPar[ch][num].shield==true)
			temp="����";
		else
			temp="";
		m_paraList.SetItemText(num,19,temp);

		temp=m_fbgPar[ch][num].note;
		m_paraList.SetItemText(num,20,temp);
		//////////////////////////////////
		
		if (m_fbgPar[channel][num].matchFbgCh==-1 ||m_fbgPar[channel][num].matchFbgNum==-1)	//��Թ�դ�Ų���ȷ���Ҳ�����	
			temp="";
		else
			temp.Format("CH%d-%d",m_fbgPar[channel][num].matchFbgCh+1,m_fbgPar[channel][num].matchFbgNum+1);
		m_paraList.SetItemText(num,17,temp);			
	}		
	/**/
}

void ParaSet::OnSelchangeCombo1() 
{
	channel=m_SelSpectChn.GetCurSel();  //�ı��������е�ͨ���ţ�
	showParaList(channel);              //��ʾ��ͨ���еĴ���������
}

void ParaSet::OnClickList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	line=m_paraList.GetSelectionMark();//�õ�ѡ���е����	
	*pResult = 0;
	m_id=m_fbgPar[channel][line].ID;
	m_unit=m_fbgPar[channel][line].unit;
	m_type=m_fbgPar[channel][line].sensorType;
	m_orignalWavelength=m_fbgPar[channel][line].orignalWavelengh;
	m_waveLengthLeftLimited=m_fbgPar[channel][line].waveLengthLeftLimited;
	m_waveLengthRightLimited=m_fbgPar[channel][line].waveLengthRightLimited;
	m_orignalTemperature=m_fbgPar[channel][line].orignalTemperature;
	m_temperatureCoefficnet=m_fbgPar[channel][line].temperatureCoefficnet;
	m_alarmMin=m_fbgPar[channel][line].alarmMin;
	m_alarmMax=m_fbgPar[channel][line].alarmMax;
	m_matchFbgID=m_fbgPar[channel][line].matchFbgID;
	m_temperatureCoefficnet=m_fbgPar[channel][line].temperatureCoefficnet;
	m_tCompezation=m_fbgPar[channel][line].tCompezation;
	m_shield=m_fbgPar[channel][line].shield;
	m_position=m_fbgPar[channel][line].position;
	m_note=m_fbgPar[channel][line].note;
	m_k=m_fbgPar[channel][line].k;//
	////////////////////////////////////////////
	m_a=m_fbgPar[channel][line].bh.a;
	m_b=m_fbgPar[channel][line].bh.b;
	m_c=m_fbgPar[channel][line].bh.c;
	m_d=m_fbgPar[channel][line].bh.d;
	m_e=m_fbgPar[channel][line].bh.e;
	m_f=m_fbgPar[channel][line].bh.f;
	m_bhk=m_fbgPar[channel][line].bh.k;

	///////////////////////////////////////////
	UpdateData(FALSE);
	
	CString s1;///С����ʾ��ȫ��������һ�¡�
	s1.Format("%9.3f",m_orignalWavelength); GetDlgItem(IDC_orignWave)->SetWindowText(s1);
	s1.Format("%9.3f",m_waveLengthLeftLimited); GetDlgItem(IDC_leftWave)->SetWindowText(s1);	
	s1.Format("%9.3f",m_waveLengthRightLimited); GetDlgItem(IDC_rightWave)->SetWindowText(s1);
	s1.Format("%9.3f",m_waveLengthLeftLimited); GetDlgItem(IDC_leftWave)->SetWindowText(s1);
}

void ParaSet::Onaccept() //ѡ��һ�������������ı����Ĳ��������ļ��к��б��н���ˢ�¡�
{	
	UpdateData(TRUE);
	line=m_paraList.GetSelectionMark();//�õ�ѡ���е���š�����������š�	

	if(line<0  || line>=maxSensor[channel]) 
	{
		AfxMessageBox("δѡ�д�����������ѡ��");
		return;
	}
	if (m_fbgPar[channel][line].sensorType=="���ٶȼ�" ||m_fbgPar[channel][line].sensorType=="ACCE" )
	{
		m_a=0;	m_b=0;	m_c=0;	m_d=0;	m_e=0;	m_f=0;	m_bhk=0;
		m_alarmMax=0;
		m_alarmMin=0;
		m_matchFbgID="";
		memset(&m_fbgPar[channel][line].bh,0,sizeof(double)*7);
		//m_fbgPar[channel][line].matchFbgID="";
	}
	////////////////�޸ĺ�����ݸ�������������Ӧ�Ĳ���////////////////		
	m_fbgPar[channel][line].ID=m_id;
	m_fbgPar[channel][line].unit=m_unit;
	m_fbgPar[channel][line].sensorType=m_type;
	m_fbgPar[channel][line].orignalWavelengh=m_orignalWavelength;
	m_fbgPar[channel][line].waveLengthLeftLimited=m_waveLengthLeftLimited;
	m_fbgPar[channel][line].waveLengthRightLimited=m_waveLengthRightLimited;
	m_fbgPar[channel][line].orignalTemperature=m_orignalTemperature;
	m_fbgPar[channel][line].temperatureCoefficnet=m_temperatureCoefficnet;
	m_fbgPar[channel][line].alarmMin=m_alarmMin;
	m_fbgPar[channel][line].alarmMax=m_alarmMax;
	m_fbgPar[channel][line].matchFbgID=m_matchFbgID;
	m_fbgPar[channel][line].temperatureCoefficnet=m_temperatureCoefficnet;
	m_fbgPar[channel][line].tCompezation=m_tCompezation;
	m_fbgPar[channel][line].shield=m_shield;
	m_fbgPar[channel][line].position=m_position;
	m_fbgPar[channel][line].note=m_note;
	m_fbgPar[channel][line].k=m_k;
	//////////////////////////////////////////////////////
	m_fbgPar[channel][line].bh.a=m_a;
	m_fbgPar[channel][line].bh.b=m_b;
	m_fbgPar[channel][line].bh.c=m_c;
	m_fbgPar[channel][line].bh.d=m_d;
	m_fbgPar[channel][line].bh.e=m_e;
	m_fbgPar[channel][line].bh.f=m_f;
	m_fbgPar[channel][line].bh.k=m_bhk;
	//////////////////////////////////////////////////////
	
	
	modifyParameter(line);		
	OnReadParameter();
	findID();	
	//m_note=m_fbgPar[channel][line].note;
	UpdateData(FALSE);
	showParaList(channel);
}

void ParaSet::modifyParameter(int num)//�޸Ĳ�������һ��������
{
	_variant_t	var;  
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;	 //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr	m_pRecordset;    //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try    					//  �򿪱���Access��FbgParameter.mdb             
	{
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("���ݿ�����ʧ�ܣ�ȷ�����ݿ�FbgParameter.mdb�Ƿ���ȷ!");
		return;
	} 
	///////////////////////////////////////////////////////////////	
	m_pRecordset.CreateInstance(__uuidof(Recordset));	
	//////////////////////////////////////////////////////////////
	
	CString temp;	
	temp.Format("%d",channel+1);
	try                       //�򿪱�CHN,���Ƿ�ɹ�.
	{
		CString mm="SELECT * FROM CH"+temp+" ORDER BY ԭʼ���� ASC";// 	
		m_pRecordset->Open(_bstr_t(mm),          // ��ѯ�ļ�CHN���������ֶ�	
			m_pConnection.GetInterfacePtr(),	 // ��ȡ��ӿ��IDispatchָ��
			adOpenDynamic,
			adLockOptimistic,
			adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return;
	}
	////////////����ȡ�������ݡ��ļ�βm_pRecordset->BOF=-1,!m_pRecordset->BOF=0, ���ļ�βΪ0, !()=1;
	if (num<maxSensor[channel])			
	{
		try
		{
			if(!m_pRecordset->BOF)
			{
				m_pRecordset->MoveFirst();
				m_pRecordset->Move(num);
			}
			else
			{
				AfxMessageBox("��������Ϊ��");
				return ;
			}
			CString temp1;
			// ������и��ֶβ������б����
			
			///////////////////////  �������ϵı����¶ȵ�����������ļ�  ////////////////////////////
			try
			{
				m_pRecordset->PutCollect("ID", _variant_t(m_fbgPar[channel][num].ID));			
				m_pRecordset->PutCollect("����������", _variant_t(m_fbgPar[channel][num].sensorType));		
				m_pRecordset->PutCollect("ԭʼ����", _variant_t(m_fbgPar[channel][num].orignalWavelengh));		
				m_pRecordset->PutCollect("��������", _variant_t(m_fbgPar[channel][num].waveLengthLeftLimited));		
				m_pRecordset->PutCollect("��������", _variant_t(m_fbgPar[channel][num].waveLengthRightLimited));		
				
				m_pRecordset->PutCollect("��ʼ�¶�", _variant_t(m_fbgPar[channel][num].orignalTemperature));			
				m_pRecordset->PutCollect("�¶�ϵ��", _variant_t(m_fbgPar[channel][num].temperatureCoefficnet));		
				m_pRecordset->PutCollect("���ޱ���", _variant_t(m_fbgPar[channel][num].alarmMin));		
				m_pRecordset->PutCollect("���ޱ���", _variant_t(m_fbgPar[channel][num].alarmMax));		
				m_pRecordset->PutCollect("����ϵ��", _variant_t(m_fbgPar[channel][num].k));	
				
				m_pRecordset->PutCollect("�²�ϵ��", _variant_t(m_fbgPar[channel][num].tCompezation));			
				m_pRecordset->PutCollect("�²�ID", _variant_t(m_fbgPar[channel][num].matchFbgID));		
				m_pRecordset->PutCollect("��λ", _variant_t(m_fbgPar[channel][num].unit));		
				m_pRecordset->PutCollect("����λ��", _variant_t(m_fbgPar[channel][num].position));		
				m_pRecordset->PutCollect("����", _variant_t(m_fbgPar[channel][num].shield));	
				m_pRecordset->PutCollect("��ע", _variant_t(m_fbgPar[channel][num].note));				

				////////////////////////////////////////////////////////////////////////
				m_pRecordset->PutCollect("a", _variant_t(m_fbgPar[channel][num].bh.a));	
				m_pRecordset->PutCollect("b", _variant_t(m_fbgPar[channel][num].bh.b));	
				m_pRecordset->PutCollect("c", _variant_t(m_fbgPar[channel][num].bh.c));	
				m_pRecordset->PutCollect("d", _variant_t(m_fbgPar[channel][num].bh.d));	
				m_pRecordset->PutCollect("e", _variant_t(m_fbgPar[channel][num].bh.e));	
				m_pRecordset->PutCollect("f", _variant_t(m_fbgPar[channel][num].bh.f));	
				m_pRecordset->PutCollect("k", _variant_t(m_fbgPar[channel][num].bh.k));	
				////////////////////////////////////////////////////////////////////////
				
				m_pRecordset->Update();	
			}
			catch(_com_error *e)
			{
				AfxMessageBox(e->ErrorMessage());
				m_pConnection->Close();	
				return;		
			}
		}
		catch(_com_error *e)
		{
			AfxMessageBox(e->ErrorMessage());
			m_pConnection->Close();	
			return;		
		}		
	}
	
	///////////////////////////////////////////////////
	else		//�������ų��ˣ���������
	{
	}	
	m_pConnection->Close();	
}

void ParaSet::OnReadParameter() //
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->m_access.ReadAllFbgParameter();
	showParaList(channel);
}

void ParaSet::OnaddPara()  //�ڲ������ļ��м���һ�����������¶�һ���ļ���������ˢ����ʾ��
{
	UpdateData(TRUE);
	line=maxSensor[channel];	//�к�=��󴫸���������

	_variant_t	var;  
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;	 //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr	m_pRecordset;    //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try    	  //  �򿪱���Access��FbgParameter.mdb             
	{
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("���ݿ�����ʧ�ܣ�ȷ�����ݿ�FbgParameter.mdb�Ƿ���ȷ!");
		return;
	} 
	///////////////////////////////////////////////////////////////	
	m_pRecordset.CreateInstance(__uuidof(Recordset));	
	//////////////////////////////////////////////////////////////
	
	CString temp;	
	temp.Format("%d",channel+1);
	try                       //�򿪱�CHN,���Ƿ�ɹ�.
	{
		CString mm="SELECT * FROM CH"+temp+" ORDER BY ԭʼ���� ASC";// 	
		m_pRecordset->Open(_bstr_t(mm),          // ��ѯ�ļ�CHN���������ֶ�	
			m_pConnection.GetInterfacePtr(),	 // ��ȡ��ӿ��IDispatchָ��
			adOpenDynamic,
			adLockOptimistic,
			adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return;
	}
	////////////����ȡ�������ݡ��ļ�βm_pRecordset->BOF=-1,!m_pRecordset->BOF=0, ���ļ�βΪ0, !()=1;
	////////////////������е����ݸ�������������Ӧ�Ĳ���////////////////		
	m_fbgPar[channel][line].ID=m_id;
	m_fbgPar[channel][line].unit=m_unit;
	m_fbgPar[channel][line].sensorType=m_type;
	m_fbgPar[channel][line].orignalWavelengh=m_orignalWavelength;
	m_fbgPar[channel][line].waveLengthLeftLimited=m_waveLengthLeftLimited;
	m_fbgPar[channel][line].waveLengthRightLimited=m_waveLengthRightLimited;
	m_fbgPar[channel][line].orignalTemperature=m_orignalTemperature;
	m_fbgPar[channel][line].temperatureCoefficnet=m_temperatureCoefficnet;
	m_fbgPar[channel][line].alarmMin=m_alarmMin;
	m_fbgPar[channel][line].alarmMax=m_alarmMax;
	m_fbgPar[channel][line].matchFbgID=m_matchFbgID;
	m_fbgPar[channel][line].temperatureCoefficnet=m_temperatureCoefficnet;
	m_fbgPar[channel][line].tCompezation=m_tCompezation;
	m_fbgPar[channel][line].shield=m_shield;
	m_fbgPar[channel][line].position=m_position;
	m_fbgPar[channel][line].note=m_note;
	m_fbgPar[channel][line].k=m_k;
	//////////////////////////////////////////////////////
	m_fbgPar[channel][line].bh.a=m_a;
	m_fbgPar[channel][line].bh.b=m_b;
	m_fbgPar[channel][line].bh.c=m_c;
	m_fbgPar[channel][line].bh.d=m_d;
	m_fbgPar[channel][line].bh.e=m_e;
	m_fbgPar[channel][line].bh.f=m_f;
	m_fbgPar[channel][line].bh.k=m_bhk;
	//////////////////////////////////////////////////////
	
	int num=line;
	try
	{
		m_pRecordset->AddNew();	
		m_pRecordset->PutCollect("ID", _variant_t(m_fbgPar[channel][num].ID));			
		m_pRecordset->PutCollect("����������", _variant_t(m_fbgPar[channel][num].sensorType));		
		m_pRecordset->PutCollect("ԭʼ����", _variant_t(m_fbgPar[channel][num].orignalWavelengh));		
		m_pRecordset->PutCollect("��������", _variant_t(m_fbgPar[channel][num].waveLengthLeftLimited));		
		m_pRecordset->PutCollect("��������", _variant_t(m_fbgPar[channel][num].waveLengthRightLimited));		
		
		m_pRecordset->PutCollect("��ʼ�¶�", _variant_t(m_fbgPar[channel][num].orignalTemperature));			
		m_pRecordset->PutCollect("�¶�ϵ��", _variant_t(m_fbgPar[channel][num].temperatureCoefficnet));		
		m_pRecordset->PutCollect("���ޱ���", _variant_t(m_fbgPar[channel][num].alarmMin));		
		m_pRecordset->PutCollect("���ޱ���", _variant_t(m_fbgPar[channel][num].alarmMax));		
		m_pRecordset->PutCollect("����ϵ��", _variant_t(m_fbgPar[channel][num].k));	
		
		m_pRecordset->PutCollect("�²�ϵ��", _variant_t(m_fbgPar[channel][num].tCompezation));			
		m_pRecordset->PutCollect("�²�ID", _variant_t(m_fbgPar[channel][num].matchFbgID));		
		m_pRecordset->PutCollect("��λ", _variant_t(m_fbgPar[channel][num].unit));		
		m_pRecordset->PutCollect("����λ��", _variant_t(m_fbgPar[channel][num].position));		
		m_pRecordset->PutCollect("����", _variant_t(m_fbgPar[channel][num].shield));	
		m_pRecordset->PutCollect("��ע", _variant_t(m_fbgPar[channel][num].note));		

		////////////////////////////////////////////////////////////////////////
		m_pRecordset->PutCollect("a", _variant_t(m_fbgPar[channel][num].bh.a));	
		m_pRecordset->PutCollect("b", _variant_t(m_fbgPar[channel][num].bh.b));	
		m_pRecordset->PutCollect("c", _variant_t(m_fbgPar[channel][num].bh.c));	
		m_pRecordset->PutCollect("d", _variant_t(m_fbgPar[channel][num].bh.d));	
		m_pRecordset->PutCollect("e", _variant_t(m_fbgPar[channel][num].bh.e));	
		m_pRecordset->PutCollect("f", _variant_t(m_fbgPar[channel][num].bh.f));	
		m_pRecordset->PutCollect("k", _variant_t(m_fbgPar[channel][num].bh.k));	
		
		////////////////////////////////////////////////////////////////////////
		m_pRecordset->Update();	
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	} 	
	maxSensor[channel]++;	
	m_pConnection->Close();	
	
//////////////////////���¶�����������ˢ��һ�¡�Ҫ���¶������԰��ղ�����������һ�¡�
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->m_access.ReadAllFbgParameter();
	showParaList(channel);
	findID();

}

void ParaSet::Ondelete() 
{
	line=m_paraList.GetSelectionMark();//�õ�ѡ���е���š�����������š�
	if (line<0 || line>=maxSensor[channel])
	{
		AfxMessageBox("δѡ�в���");
		return;
	}
	if(MessageBox(NULL,"ȷ��ɾ����?",MB_YESNO+MB_DEFBUTTON2)==IDNO)
		return;

	_variant_t	var;  
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;	 //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr	m_pRecordset;    //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try    	  //  �򿪱���Access��FbgParameter.mdb             
	{
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("���ݿ�����ʧ�ܣ�ȷ�����ݿ�FbgParameter.mdb�Ƿ���ȷ!");
		return;
	} 
	///////////////////////////////////////////////////////////////	
	m_pRecordset.CreateInstance(__uuidof(Recordset));	
	//////////////////////////////////////////////////////////////
	
	CString temp;	
	temp.Format("%d",channel+1);
	try                       //�򿪱�CHN,���Ƿ�ɹ�.
	{
		//CString mm="DELETE FROM CH"+temp+" ORDER BY ԭʼ���� ASC";// 
		CString mm;
		mm.Format("DELETE FROM CH%d  WHERE ��� = %d",channel+1,m_fbgPar[channel][line].sn);
		m_pRecordset->Open(_bstr_t(mm),          // ��ѯ�ļ�CHN���������ֶ�	
			m_pConnection.GetInterfacePtr(),	 // ��ȡ��ӿ��IDispatchָ��
			adOpenDynamic,
			adLockOptimistic,
			adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return;
	}
	//m_pRecordset->Update();	
	m_pConnection->Close();	
	///////////////////////
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->m_access.ReadAllFbgParameter();
	showParaList(channel);
	findID();
}

void ParaSet::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//Ondelete();
	*pResult = 0;
}

void ParaSet::OnButton1() 
{
	findID();//��ʱ�����á�
	showParaList(channel);
}

void ParaSet::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	showParaList(channel);
	
	// Do not call CDialog::OnPaint() for painting messages
}

void ParaSet::findID()//���ݴ��������²����ID�ҵ���Ե�ͨ���ż���š�//����²�ID�ǿո���ɣ����ҡ�
{
	CString temp="",temp1;
	for (int channel=0;channel<16;channel++)
	{
		for (int num=0;num<maxSensor[channel];num++)////////////////����Դ�����ͨ���ż�
		{		
			strncpy(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorType,(LPCTSTR)m_fbgPar[channel][num].sensorType,
				sizeof(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorType));
			strncpy(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorID,(LPCTSTR)m_fbgPar[channel][num].ID,
				sizeof(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorID));
			strncpy(m_fbgResult_1.result_ch[channel].fbgResult[num].unit,(LPCTSTR)m_fbgPar[channel][num].unit,
				sizeof(m_fbgResult_1.result_ch[channel].fbgResult[num].unit));

			m_fbgPar[channel][num].matchFbgCh=-1;//�ȸ���ֵ��û���ҵ��²���Թ�դ����Թ�դ�Ų���ȷ
			m_fbgPar[channel][num].matchFbgNum=-1;			

			temp=m_fbgPar[channel][num].matchFbgID;

			//if(temp=="" || m_fbgPar[channel][num].sensorType=="�¶ȼ�" ||temp==" " ||temp=="  "||temp=="   " )
			if(temp=="" || temp==" " ||temp=="  "||temp=="   " )
			{
				m_fbgPar[channel][num].matchFbgCh=-2;//û����Թ�դ������Ҫ��Թ�դ��
				m_fbgPar[channel][num].matchFbgNum=-2;			
				continue;
			}
			
			for (int cc=0;cc<16;cc++) //����Դ�������Ҫ����
			{
				bool findTarget=false;
				for (int sn=0;sn<maxSensor[cc];sn++)
				{
					if (temp==m_fbgPar[cc][sn].ID)
					{
						m_fbgPar[channel][num].matchFbgCh=cc;
						m_fbgPar[channel][num].matchFbgNum=sn;
						temp1.Format("CH%d-%d",cc+1,sn+1);
						m_fbgPar[channel][num].note=temp1;
						findTarget=true;
						break;
					}//////////////////////////////
				}
				if (findTarget==true) break;
			}//////////////////////////////			
		}//////////////////////////////
	}//////////////////////////////
}