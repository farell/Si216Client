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
{	/////////////////下拉框///////////////////////	//m_SelSpectChn.AddString("ETALON");
	int num,ch;
	for(int chn=0;chn<16;chn++)
	{
		CString s1;
		s1.Format("%d",chn+1);
		s1="通道"+s1;
		m_SelSpectChn.AddString(s1);
	}
	m_SelSpectChn.SetCurSel(0);
	channel=m_SelSpectChn.GetCurSel();//
	line=0;      //在参数表中选中的行号。0,1,2,3,4,

	////////////列表//////////////////////
	HWND hWnd=m_paraList.m_hWnd;      //获取句柄
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//获取显示格式
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//设置显示格式
	//m_paraList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//加坐标线,全选一行 	
	m_paraList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_INFOTIP | LVS_EX_GRIDLINES);
	
	int nColumnCount =m_paraList.GetHeaderCtrl()->GetItemCount();
	
	m_paraList.DeleteAllItems();

	CString temp;
	
	m_paraList.InsertColumn(0,"序号",LVCFMT_CENTER,50);
	m_paraList.InsertColumn(1,"ID",LVCFMT_LEFT,50);
	m_paraList.InsertColumn(2,"传感器类型",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(3,"当前值",LVCFMT_LEFT,60);
	m_paraList.InsertColumn(4,"单位",LVCFMT_LEFT,25);	
	
	m_paraList.InsertColumn(5,"原始波长",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(6,"波长下限",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(7,"波长上限",LVCFMT_LEFT,72);
	/*
	m_paraList.InsertColumn(8,"初始温度",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(9,"温度系数",LVCFMT_LEFT,72);	
	m_paraList.InsertColumn(10,"传感系数",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(11,"温补ID",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(12,"温补系数",LVCFMT_LEFT,72);	
	
	m_paraList.InsertColumn(13,"下限报警",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(14,"上限报警",LVCFMT_LEFT,72);	
	*/
	m_paraList.InsertColumn(8,"a",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(9,"b",LVCFMT_LEFT,88);	
	m_paraList.InsertColumn(10,"c",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(11,"k",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(12,"d",LVCFMT_LEFT,88);		
	m_paraList.InsertColumn(13,"e",LVCFMT_LEFT,88);
	m_paraList.InsertColumn(14,"f",LVCFMT_LEFT,88);	
	m_paraList.InsertColumn(15,"温补ID",LVCFMT_LEFT,72);
	///////////////////////////////////////////////////////
	m_paraList.InsertColumn(16,"报警下限",LVCFMT_LEFT,72);
	m_paraList.InsertColumn(17,"报警上限",LVCFMT_LEFT,72);	
	m_paraList.InsertColumn(18,"地理位置",LVCFMT_LEFT,90);
	m_paraList.InsertColumn(19,"屏蔽",LVCFMT_LEFT,40);
	m_paraList.InsertColumn(20,"备注",LVCFMT_LEFT,100);
	m_paraList.InsertColumn(21,"",LVCFMT_LEFT,20);
	
	for(int i=0; i<21; i++)
	{
		temp.Format("%d",i+1);
		m_paraList.InsertItem(i,temp);
	}
//////////////////显示参数表的内容///////////////////////////
	//showParaList(channel);
}
/*
void ParaSet::showParaList(int ch)//将读出来的 m_fbgPara 数据在列表中进行显示。
{	
	CString temp;
	int num;
	///////////////////先清空////////////////////
	for (int line1=0;line1<20;line1++)
	{	
		for (num=0;num<17;num++)
		{
			temp.Format("%d %d",line1,num);
			m_paraList.SetItemText(line1,num+1,"");
		}
	}
	//将 m_fbgPar中的值赋到表格中去，
	for (num=0;num<maxSensor[ch];num++)   //num是行号，每个传感器占一行。
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
			temp="屏蔽";
		else
			temp="";
		m_paraList.SetItemText(num,16,temp);	

		temp=m_fbgPar[ch][num].note;
		m_paraList.SetItemText(num,17,temp);			
	}		
	
}   */

void ParaSet::showParaList(int ch)//将读出来的 m_fbgPara 数据在列表中进行显示。
{	
	CString temp;
	int num;
	///////////////////先清空////////////////////
	for (int line1=0;line1<20;line1++)
	{	
		for (num=0;num<20;num++)
		{
			temp.Format("%d %d",line1,num);
			m_paraList.SetItemText(line1,num+1,"");
		}
	}
	//将 m_fbgPar中的值赋到表格中去，
	for (num=0;num<maxSensor[ch];num++)   //num是行号，每个传感器占一行。
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
			temp="屏蔽";
		else
			temp="";
		m_paraList.SetItemText(num,19,temp);

		temp=m_fbgPar[ch][num].note;
		m_paraList.SetItemText(num,20,temp);
		//////////////////////////////////
		
		if (m_fbgPar[channel][num].matchFbgCh==-1 ||m_fbgPar[channel][num].matchFbgNum==-1)	//配对光栅号不正确，找不到。	
			temp="";
		else
			temp.Format("CH%d-%d",m_fbgPar[channel][num].matchFbgCh+1,m_fbgPar[channel][num].matchFbgNum+1);
		m_paraList.SetItemText(num,17,temp);			
	}		
	/**/
}

void ParaSet::OnSelchangeCombo1() 
{
	channel=m_SelSpectChn.GetCurSel();  //改变下拉框中的通道号，
	showParaList(channel);              //显示该通道中的传感器参数
}

void ParaSet::OnClickList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	line=m_paraList.GetSelectionMark();//得到选中行的序号	
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
	
	CString s1;///小数显示不全，重新作一下。
	s1.Format("%9.3f",m_orignalWavelength); GetDlgItem(IDC_orignWave)->SetWindowText(s1);
	s1.Format("%9.3f",m_waveLengthLeftLimited); GetDlgItem(IDC_leftWave)->SetWindowText(s1);	
	s1.Format("%9.3f",m_waveLengthRightLimited); GetDlgItem(IDC_rightWave)->SetWindowText(s1);
	s1.Format("%9.3f",m_waveLengthLeftLimited); GetDlgItem(IDC_leftWave)->SetWindowText(s1);
}

void ParaSet::Onaccept() //选中一个传感器，并改变它的参数，在文件中和列表中进行刷新。
{	
	UpdateData(TRUE);
	line=m_paraList.GetSelectionMark();//得到选中行的序号、即传感器序号。	

	if(line<0  || line>=maxSensor[channel]) 
	{
		AfxMessageBox("未选中传感器，请点击选中");
		return;
	}
	if (m_fbgPar[channel][line].sensorType=="加速度计" ||m_fbgPar[channel][line].sensorType=="ACCE" )
	{
		m_a=0;	m_b=0;	m_c=0;	m_d=0;	m_e=0;	m_f=0;	m_bhk=0;
		m_alarmMax=0;
		m_alarmMin=0;
		m_matchFbgID="";
		memset(&m_fbgPar[channel][line].bh,0,sizeof(double)*7);
		//m_fbgPar[channel][line].matchFbgID="";
	}
	////////////////修改后的数据赋给参数表中相应的参数////////////////		
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

void ParaSet::modifyParameter(int num)//修改参数表中一条参数。
{
	_variant_t	var;  
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;	 //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr	m_pRecordset;    //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try    					//  打开本地Access库FbgParameter.mdb             
	{
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("数据库连接失败，确认数据库FbgParameter.mdb是否正确!");
		return;
	} 
	///////////////////////////////////////////////////////////////	
	m_pRecordset.CreateInstance(__uuidof(Recordset));	
	//////////////////////////////////////////////////////////////
	
	CString temp;	
	temp.Format("%d",channel+1);
	try                       //打开表CHN,看是否成功.
	{
		CString mm="SELECT * FROM CH"+temp+" ORDER BY 原始波长 ASC";// 	
		m_pRecordset->Open(_bstr_t(mm),          // 查询文件CHN表中所有字段	
			m_pConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
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
	////////////—读取表内数据。文件尾m_pRecordset->BOF=-1,!m_pRecordset->BOF=0, 非文件尾为0, !()=1;
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
				AfxMessageBox("表内数据为空");
				return ;
			}
			CString temp1;
			// 读入库中各字段并加入列表框中
			
			///////////////////////  将界面上的报警温度等输入参数表文件  ////////////////////////////
			try
			{
				m_pRecordset->PutCollect("ID", _variant_t(m_fbgPar[channel][num].ID));			
				m_pRecordset->PutCollect("传感器类型", _variant_t(m_fbgPar[channel][num].sensorType));		
				m_pRecordset->PutCollect("原始波长", _variant_t(m_fbgPar[channel][num].orignalWavelengh));		
				m_pRecordset->PutCollect("波长下限", _variant_t(m_fbgPar[channel][num].waveLengthLeftLimited));		
				m_pRecordset->PutCollect("波长上限", _variant_t(m_fbgPar[channel][num].waveLengthRightLimited));		
				
				m_pRecordset->PutCollect("初始温度", _variant_t(m_fbgPar[channel][num].orignalTemperature));			
				m_pRecordset->PutCollect("温度系数", _variant_t(m_fbgPar[channel][num].temperatureCoefficnet));		
				m_pRecordset->PutCollect("下限报警", _variant_t(m_fbgPar[channel][num].alarmMin));		
				m_pRecordset->PutCollect("上限报警", _variant_t(m_fbgPar[channel][num].alarmMax));		
				m_pRecordset->PutCollect("传感系数", _variant_t(m_fbgPar[channel][num].k));	
				
				m_pRecordset->PutCollect("温补系数", _variant_t(m_fbgPar[channel][num].tCompezation));			
				m_pRecordset->PutCollect("温补ID", _variant_t(m_fbgPar[channel][num].matchFbgID));		
				m_pRecordset->PutCollect("单位", _variant_t(m_fbgPar[channel][num].unit));		
				m_pRecordset->PutCollect("地理位置", _variant_t(m_fbgPar[channel][num].position));		
				m_pRecordset->PutCollect("屏蔽", _variant_t(m_fbgPar[channel][num].shield));	
				m_pRecordset->PutCollect("备注", _variant_t(m_fbgPar[channel][num].note));				

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
	else		//点击的序号超了，不动作。
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

void ParaSet::OnaddPara()  //在参数表文件中加入一个参数，重新读一遍文件，并重新刷新显示。
{
	UpdateData(TRUE);
	line=maxSensor[channel];	//行号=最大传感器数量。

	_variant_t	var;  
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;	 //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr	m_pRecordset;    //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try    	  //  打开本地Access库FbgParameter.mdb             
	{
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("数据库连接失败，确认数据库FbgParameter.mdb是否正确!");
		return;
	} 
	///////////////////////////////////////////////////////////////	
	m_pRecordset.CreateInstance(__uuidof(Recordset));	
	//////////////////////////////////////////////////////////////
	
	CString temp;	
	temp.Format("%d",channel+1);
	try                       //打开表CHN,看是否成功.
	{
		CString mm="SELECT * FROM CH"+temp+" ORDER BY 原始波长 ASC";// 	
		m_pRecordset->Open(_bstr_t(mm),          // 查询文件CHN表中所有字段	
			m_pConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
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
	////////////—读取表内数据。文件尾m_pRecordset->BOF=-1,!m_pRecordset->BOF=0, 非文件尾为0, !()=1;
	////////////////将表格中的数据赋给参数表中相应的参数////////////////		
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
		m_pRecordset->PutCollect("传感器类型", _variant_t(m_fbgPar[channel][num].sensorType));		
		m_pRecordset->PutCollect("原始波长", _variant_t(m_fbgPar[channel][num].orignalWavelengh));		
		m_pRecordset->PutCollect("波长下限", _variant_t(m_fbgPar[channel][num].waveLengthLeftLimited));		
		m_pRecordset->PutCollect("波长上限", _variant_t(m_fbgPar[channel][num].waveLengthRightLimited));		
		
		m_pRecordset->PutCollect("初始温度", _variant_t(m_fbgPar[channel][num].orignalTemperature));			
		m_pRecordset->PutCollect("温度系数", _variant_t(m_fbgPar[channel][num].temperatureCoefficnet));		
		m_pRecordset->PutCollect("下限报警", _variant_t(m_fbgPar[channel][num].alarmMin));		
		m_pRecordset->PutCollect("上限报警", _variant_t(m_fbgPar[channel][num].alarmMax));		
		m_pRecordset->PutCollect("传感系数", _variant_t(m_fbgPar[channel][num].k));	
		
		m_pRecordset->PutCollect("温补系数", _variant_t(m_fbgPar[channel][num].tCompezation));			
		m_pRecordset->PutCollect("温补ID", _variant_t(m_fbgPar[channel][num].matchFbgID));		
		m_pRecordset->PutCollect("单位", _variant_t(m_fbgPar[channel][num].unit));		
		m_pRecordset->PutCollect("地理位置", _variant_t(m_fbgPar[channel][num].position));		
		m_pRecordset->PutCollect("屏蔽", _variant_t(m_fbgPar[channel][num].shield));	
		m_pRecordset->PutCollect("备注", _variant_t(m_fbgPar[channel][num].note));		

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
	
//////////////////////重新读出参数，并刷新一下。要重新读，可以按照波长重新排序一下。
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->m_access.ReadAllFbgParameter();
	showParaList(channel);
	findID();

}

void ParaSet::Ondelete() 
{
	line=m_paraList.GetSelectionMark();//得到选中行的序号、即传感器序号。
	if (line<0 || line>=maxSensor[channel])
	{
		AfxMessageBox("未选中参数");
		return;
	}
	if(MessageBox(NULL,"确定删除吗?",MB_YESNO+MB_DEFBUTTON2)==IDNO)
		return;

	_variant_t	var;  
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;	 //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr	m_pRecordset;    //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try    	  //  打开本地Access库FbgParameter.mdb             
	{
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("数据库连接失败，确认数据库FbgParameter.mdb是否正确!");
		return;
	} 
	///////////////////////////////////////////////////////////////	
	m_pRecordset.CreateInstance(__uuidof(Recordset));	
	//////////////////////////////////////////////////////////////
	
	CString temp;	
	temp.Format("%d",channel+1);
	try                       //打开表CHN,看是否成功.
	{
		//CString mm="DELETE FROM CH"+temp+" ORDER BY 原始波长 ASC";// 
		CString mm;
		mm.Format("DELETE FROM CH%d  WHERE 编号 = %d",channel+1,m_fbgPar[channel][line].sn);
		m_pRecordset->Open(_bstr_t(mm),          // 查询文件CHN表中所有字段	
			m_pConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
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
	findID();//临时测试用。
	showParaList(channel);
}

void ParaSet::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	showParaList(channel);
	
	// Do not call CDialog::OnPaint() for painting messages
}

void ParaSet::findID()//根据传感器的温补配对ID找到相对的通道号及编号。//如果温补ID是空格组成，则不找。
{
	CString temp="",temp1;
	for (int channel=0;channel<16;channel++)
	{
		for (int num=0;num<maxSensor[channel];num++)////////////////找配对传感器通道号及
		{		
			strncpy(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorType,(LPCTSTR)m_fbgPar[channel][num].sensorType,
				sizeof(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorType));
			strncpy(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorID,(LPCTSTR)m_fbgPar[channel][num].ID,
				sizeof(m_fbgResult_1.result_ch[channel].fbgResult[num].sensorID));
			strncpy(m_fbgResult_1.result_ch[channel].fbgResult[num].unit,(LPCTSTR)m_fbgPar[channel][num].unit,
				sizeof(m_fbgResult_1.result_ch[channel].fbgResult[num].unit));

			m_fbgPar[channel][num].matchFbgCh=-1;//先赋初值，没有找到温补配对光栅，配对光栅号不正确
			m_fbgPar[channel][num].matchFbgNum=-1;			

			temp=m_fbgPar[channel][num].matchFbgID;

			//if(temp=="" || m_fbgPar[channel][num].sensorType=="温度计" ||temp==" " ||temp=="  "||temp=="   " )
			if(temp=="" || temp==" " ||temp=="  "||temp=="   " )
			{
				m_fbgPar[channel][num].matchFbgCh=-2;//没有配对光栅，不需要配对光栅。
				m_fbgPar[channel][num].matchFbgNum=-2;			
				continue;
			}
			
			for (int cc=0;cc<16;cc++) //找配对传感器，要遍历
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