// Administrator.cpp : implementation file
//
 
#include "stdafx.h"
#include "Si216Client.h"
#include "Si216ClientDlg.h"
#include "Administrator.h"
#include "Glbs.h"
#include "RWAcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdministrator dialog


CAdministrator::CAdministrator(CWnd* pParent /*=NULL*/)
	: CDialog(CAdministrator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdministrator)
	m_autoSend = FALSE;
	m_saveResult = FALSE;
	m_saveSample = 1;
	m_sampleCheck = FALSE;
	m_udpSendFrame = 1;
	m_udpSendChoice = FALSE;
	m_pcID = 0;
	m_sendFftBuffer = FALSE;
	m_addressPort = 0x8001;
	//}}AFX_DATA_INIT
}


void CAdministrator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdministrator)
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipAddress);
	DDX_Check(pDX, IDC_autoSend, m_autoSend);
	DDX_Check(pDX, IDC_saveResult, m_saveResult);
	DDX_Text(pDX, IDC_saveSample, m_saveSample);
	DDV_MinMaxInt(pDX, m_saveSample, 1, 6000);
	DDX_Check(pDX, IDC_sampleCheck, m_sampleCheck);
	DDX_Text(pDX, IDC_udpSendFrame, m_udpSendFrame);
	DDV_MinMaxInt(pDX, m_udpSendFrame, 1, 1000000);
	DDX_Check(pDX, IDC_UdpSendChoice, m_udpSendChoice);
	DDX_Text(pDX, IDC_pcID, m_pcID);
	DDX_Check(pDX, IDC_sendFFTBuffer, m_sendFftBuffer);
	DDX_Text(pDX, IDC_addressPort, m_addressPort);
	DDV_MinMaxUInt(pDX, m_addressPort, 5000, 50000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdministrator, CDialog)
	//{{AFX_MSG_MAP(CAdministrator)
	ON_BN_CLICKED(IDC_modifyIP, OnmodifyIP)
	ON_BN_CLICKED(IDC_autoSend, OnautoSend)
	ON_BN_CLICKED(IDC_saveResult, OnsaveResult)
	ON_BN_CLICKED(IDC_reset, Onreset)
	ON_BN_CLICKED(IDC_sampleCheck, OnsampleCheck)
	ON_BN_CLICKED(IDC_set, Onset)
	ON_BN_CLICKED(IDC_UdpSendChoice, OnUdpSendChoice)
	ON_BN_CLICKED(IDC_sendFFTBuffer, OnsendFFTBuffer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdministrator message handlers


BOOL CAdministrator::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	init();
	UpdateData(TRUE);
	g_autoSend=m_autoSend;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdministrator::init()
{	
	m_ipAddress.SetAddress(a[0],a[1],a[2],a[3]);
	sprintf(ID_ipStr,"%u.%u.%u.%u",a[0],a[1],a[2],a[3]);
	UpdateData(FALSE);
	if ((a[0]==127 && a[1]==0) || (a[0]==192 && a[1]==168))//if (a[0]==127 || a[0]==192)
		lan=true;
	else
		lan=false;	

	//////////////创建保存数据的线程//////////////////////
	m_saveResultThread=AfxBeginThread(SaveResultThread,NULL);//开始保存波长线程
	if (m_saveResult==false)	
		m_saveResultThread->SuspendThread();     //暂停保存
	else
	{
		creatCh16FileName();//创建16个通道的文件名称。	
	}
}

void CAdministrator::OnmodifyIP() 
{
	UpdateData(TRUE);
	g_udpSendFrame=m_udpSendFrame;
	g_udpSendChoice=m_udpSendChoice;
	g_sendFftBuffer=m_sendFftBuffer;
	g_pcID=m_pcID;
	for (int i=0;i<16;i++)	
		m_fbgResult_1.result_ch[i].pcID=g_pcID;		

	m_sensorfftBuffer.pcID=g_pcID;

	if (m_addressPort<5000) m_addressPort=0x8001;
	g_addressPort=m_addressPort;

	BYTE a1,a2,a3,a4;
	m_ipAddress.GetAddress(a1,a2,a3,a4);	
	if ((a1==127 && a2==0) || (a1==192 && a2==168))
		lan=true;
	else
		lan=false;

	memset(ID_ipStr,0,sizeof(ID_ipStr));
	sprintf(ID_ipStr,"%u.%u.%u.%u",a1,a2,a3,a4);//读出界面上的地址，并赋给全局ID_ipStr
	
	m_ipAddress.GetAddress(a[0],a[1],a[2],a[3]);   //将地址写入参数表中。
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
	m_pRecordset.CreateInstance(__uuidof(Recordset));	
	
	try                       //打开表CHN,看是否成功.
	{
		CString mm="SELECT * FROM setup ORDER BY 编号 ASC";// 	
		m_pRecordset->Open(_bstr_t(mm),          // 查询文件CHN表中所有字段		 // 获取库接库的IDispatch指针
			m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return;
	}
	////////////—读取表内数据。文件尾m_pRecordset->BOF=-1,!m_pRecordset->BOF=0, 非文件尾为0, !()=1;
	try
	{
		if(!m_pRecordset->BOF)	
			m_pRecordset->MoveFirst();			
		else
		{
			AfxMessageBox("表内数据为空");
			return ;
		}		
		///////////////////////  将界面上的报警温度等输入参数表文件  ////////////////////////////		
		try
		{
			bool aa=m_autoSend;
			m_pRecordset->PutCollect("IP发送", _variant_t(aa) );			

			aa=m_udpSendChoice;
			m_pRecordset->PutCollect("抽样发送", _variant_t(aa) );
			
			aa=m_sendFftBuffer;
			m_pRecordset->PutCollect("加速度波长发送", _variant_t(aa) );

			/////////2017-8-23 BW wanted
			aa=m_sampleCheck;
			m_pRecordset->PutCollect("间隔记录", _variant_t(aa) );

			aa=m_saveResult;
			m_pRecordset->PutCollect("保存结果", _variant_t(aa) );

			/////////////////////////////////////////////

			_variant_t a1;
			a1.ChangeType(VT_I4);
			a1.intVal=m_udpSendFrame;
			m_pRecordset->PutCollect("发送间隔", _variant_t(a1));	
			m_pRecordset->MoveNext();

			a1.ChangeType(VT_I4);
			a1.intVal=m_addressPort;
			m_pRecordset->PutCollect("发送间隔", _variant_t(a1));	//地址端口号

			m_pRecordset->MoveFirst();

			/////////2017-8-23 BW wanted
			a1.ChangeType(VT_I4);
			a1.intVal=m_saveSample;
			m_pRecordset->PutCollect("保存间隔", _variant_t(a1));			


			////////////////////////////
			a1.ChangeType(VT_I4);
			a1.intVal=m_pcID;
			m_pRecordset->PutCollect("本机ID", _variant_t(a1));
			
			m_pRecordset->PutCollect("IP", _variant_t(a[0]) );			
			m_pRecordset->MoveNext();			
			m_pRecordset->PutCollect("IP", _variant_t(a[1]) );
			m_pRecordset->MoveNext();			
			m_pRecordset->PutCollect("IP", _variant_t(a[2]) );
			m_pRecordset->MoveNext();
			m_pRecordset->PutCollect("IP", _variant_t(a[3]) );			
			
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
	m_pConnection->Close();		
}


void CAdministrator::OnautoSend() 
{
	UpdateData(TRUE);
	g_autoSend=m_autoSend;
}

void CAdministrator::OnsaveResult() 
{
	UpdateData(TRUE);
	if(m_saveResult)
	{
		creatCh16FileName(); //创建32个文件名字。并开始响应保存波长线程。不关文件。	
		g_saveResultFlag=TRUE;
		m_saveResultThread->ResumeThread();
	}
	else
	{		
		g_saveResultFlag=false;		
		m_saveResultThread->SuspendThread();
		closeCh16File();     //关闭保存文件，及停止响应线程。
	}
}

void CAdministrator::creatCh16FileName() //创建32个通道的文件名。
{
	CTime CursaveTime = CTime::GetCurrentTime();
	CString savetime = CursaveTime.Format("%Y-%m-%d.%H.%M.%S");	
	CString temp1;
	
	for(int ich=0;ich<16;ich++)
	{
		temp1.Format("%d",ich+1);
		save16ChFileName[ich]=sPath+"\\历史记录"+"\\CH"+temp1+"@"+savetime+".txt"   ;//CHx@时间.txt				
		Datafile[ich].Open(save16ChFileName[ich],CFile::modeCreate|CFile::modeReadWrite| CFile::shareDenyNone);
		//mChnSaveFile[i].Open(fname,CFile::modeCreate|CFile::modeReadWrite);
		//	Datafile[ich].Close();		
	}
}
void CAdministrator::closeCh16File() //关闭32个文件。
{
	for(int ich=0;ich<16;ich++)
	{		
		Datafile[ich].Close();		
	}
}

void CAdministrator::Onreset() 
{
	for (int ch=0;ch<16;ch++)        //初始化报警状态
	{
		for (int sn=0;sn<maxSensor[ch];sn++)
		{
			m_fbgPar[ch][sn].alarmStatus=NORMAL;
			m_fbgPar[ch][sn].oldAlarmStatus=NORMAL;
			m_fbgPar[ch][sn].showAlarmStatus=NORMAL;
			m_fbgPar[ch][sn].showAlarmStatusLast=NORMAL;
			m_fbgPar[ch][sn].conCountAlarm=0;
			m_fbgPar[ch][sn].conCountFault=0;
			
		}
	}
	okLight();                 //灯复位
	cancelSpeaker();           //喇叭消音
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->alarmInfo.addAlarmInform("复位");
	m_saveBuffer.alarmWriteToBuffer("","复位");       //在缓存中写入开机字符。	
	SetEvent(g_saveAlarmInfoHandle);		//发保存报警信息的事件。	还没有开始线程呢。	// TODO: Add your control notification handler code here
	
}

void CAdministrator::saveClick()
{
	if (m_saveResult==TRUE)
	{
		CButton* p= (CButton*)GetDlgItem(IDC_saveResult); 
		p->SetCheck(0);//复选框置0。
		
		m_saveResult=FALSE;  //数据更新	
		g_saveResultFlag=false;				
		closeCh16File();
		//m_saveWavelengthThread->SuspendThread();  //关闭保存文件，及停止响应线程,不可以在线程调用，好象不往下执行.
		AfxMessageBox("磁盘容量<100M,已停止保存,请清理磁盘");			
	}	
}

void CAdministrator::OnsampleCheck() 
{
	UpdateData(TRUE);
	//if (m_saveSample)	
}

void CAdministrator::Onset() 
{
	OnmodifyIP();
	UpdateData(true);
}

void CAdministrator::OnUdpSendChoice() 
{
	UpdateData(TRUE);
	g_udpSendChoice=m_udpSendChoice;	
}

void CAdministrator::OnsendFFTBuffer() 
{
	UpdateData(TRUE);
	g_sendFftBuffer=m_sendFftBuffer;
}
