// Si216ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "Si216ClientDlg.h"
#include "Glbs.h"
#include "RWAcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSi216ClientDlg dialog

CSi216ClientDlg::CSi216ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSi216ClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSi216ClientDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_countUdpErr=0; //未收到UDP数据的计数秒数。
	flag_udpReceived=false; //收到UDP数据了。
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSi216ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSi216ClientDlg)
	DDX_Control(pDX, IDC_TAB1, m_CtrlTab);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSi216ClientDlg, CDialog)
	//{{AFX_MSG_MAP(CSi216ClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB1, OnSelchangingTab1)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSi216ClientDlg message handlers

BOOL CSi216ClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here		
	table_init(); //页面切换的控件初始化。
	init();  //其它初始化，如放在table_init();之前会出错。



	g_fbgStrainHandle=CreateEvent(NULL,TRUE,FALSE,_T("m_fbgCal"));  //保存波长事件。
	ResetEvent(g_fbgStrainHandle);
	g_fbgResultSendHandle=CreateEvent(NULL,TRUE,FALSE,_T("m_fbgResultSend"));//UDP发送
	ResetEvent(g_fbgResultSendHandle);
	g_saveAlarmInfoHandle=CreateEvent(NULL,TRUE,FALSE,_T("saveAlarmHandle"));  //保存报警信息事件。
	ResetEvent(g_saveAlarmInfoHandle);
	g_saveResultHandle=CreateEvent(NULL,TRUE,FALSE,_T("saveResultHandle"));  //保存结果的事件。
	ResetEvent(g_saveResultHandle);
	

	///////////////////////
	alarmInfo.addAlarmInform("开机");   //表格中加入
	AfxBeginThread(SaveAlarmInfoThread,NULL);    //保存报警信息线程
	m_saveBuffer.alarmWriteToBuffer("","开机");       //在缓存中写入开机字符。	
	SetEvent(g_saveAlarmInfoHandle);		//发保存报警信息的事件。	还没有开始线程呢。
	////////////////////////

	GetDlgItem(IDC_blue)->ShowWindow(SW_HIDE);       //此三个灯为通讯闪烁灯
	GetDlgItem(IDC_background)->ShowWindow(SW_HIDE);
 	GetDlgItem(IDC_yellow)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_normal)->ShowWindow(SW_SHOW);      //报警状态灯	
	GetDlgItem(IDC_fault)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_alarm)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_grey1)->ShowWindow(SW_HIDE);    //报警灯灰色
	GetDlgItem(IDC_grey2)->ShowWindow(SW_SHOW);
 	GetDlgItem(IDC_grey3)->ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSi216ClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSi216ClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSi216ClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSi216ClientDlg::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int i=m_CtrlTab.GetCurSel();
	TC_ITEM tci;
	tci.mask=TCIF_PARAM;
	m_CtrlTab.GetItem(i,&tci);
	//	ASSERT(tci.lParam);
	CWnd* pWnd=(CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_SHOW);
	pWnd->EnableWindow(TRUE);
	*pResult = 0;
}

void CSi216ClientDlg::OnSelchangingTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	int i=m_CtrlTab.GetCurSel();
	TC_ITEM tci;
	tci.mask=TCIF_PARAM;
	m_CtrlTab.GetItem(i,&tci);
	//	ASSERT(tci.lParam);
	CWnd* pWnd=(CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_HIDE);
	pWnd->EnableWindow(FALSE);
	*pResult = 0;
}

void CSi216ClientDlg::table_init()
{
	TC_ITEM tab1;	

	tab1.mask=TCIF_PARAM|TCIF_TEXT;
	tab1.lParam=(LPARAM)&m_strain;
	tab1.iImage=1;//IDB_BITMAP1;
	tab1.pszText="结果";
	VERIFY(m_strain.Create(CStrain::IDD,&m_CtrlTab));
	m_CtrlTab.InsertItem(0,&tab1);
	m_strain.SetWindowPos(NULL,5,25,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_strain.ShowWindow(SW_SHOW);
	m_strain.EnableWindow(TRUE);
	///////////////////////////////////////////////
	
	tab1.mask=TCIF_PARAM|TCIF_TEXT;
	tab1.lParam=(LPARAM)&m_wavesheet1;
	tab1.iImage=1;
	tab1.pszText="波长";
	VERIFY(m_wavesheet1.Create(CWaveSheet1::IDD,&m_CtrlTab));
	m_CtrlTab.InsertItem(1,&tab1);
	m_wavesheet1.SetWindowPos(NULL,5,25,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_wavesheet1.ShowWindow(SW_HIDE);
	m_wavesheet1.EnableWindow(FALSE);
	
	/////////////////////////////////////////////////////////////
	tab1.mask=TCIF_PARAM|TCIF_TEXT;
	tab1.lParam=(LPARAM)&m_paraSet;
	tab1.iImage=1;//IDB_BITMAP1;
	tab1.pszText="传感器参数";
	VERIFY(m_paraSet.Create(ParaSet::IDD,&m_CtrlTab));
	m_CtrlTab.InsertItem(2,&tab1);
	m_paraSet.SetWindowPos(NULL,5,30,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_paraSet.ShowWindow(SW_HIDE);
	m_paraSet.EnableWindow(FALSE);
	//////////////////////////////////////////////////
	tab1.mask=TCIF_PARAM|TCIF_TEXT;
	tab1.lParam=(LPARAM)&alarmInfo;
	tab1.iImage=1;//IDB_BITMAP1;
	tab1.pszText="报警信息";
	VERIFY(alarmInfo.Create(CAlarmInform::IDD,&m_CtrlTab));
	m_CtrlTab.InsertItem(3,&tab1);
	alarmInfo.SetWindowPos(NULL,5,30,0,0,SWP_NOSIZE|SWP_NOZORDER);
	alarmInfo.ShowWindow(SW_HIDE);
	alarmInfo.EnableWindow(FALSE);
	/////////////////////////////////////////////////////////////

	tab1.mask=TCIF_PARAM|TCIF_TEXT;
	tab1.lParam=(LPARAM)&m_admin;
	tab1.iImage=1;//IDB_BITMAP1;
	tab1.pszText="设置";
	VERIFY(m_admin.Create(CAdministrator::IDD,&m_CtrlTab));
	m_CtrlTab.InsertItem(4,&tab1);
	m_admin.SetWindowPos(NULL,5,30,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_admin.ShowWindow(SW_HIDE);
	m_admin.EnableWindow(FALSE);

	/*	
	tab1.mask=TCIF_PARAM|TCIF_TEXT;
	tab1.lParam=(LPARAM)&alarmInfo;
	tab1.iImage=1;//IDB_BITMAP1;
	tab1.pszText="报警信息";
	VERIFY(alarmInfo.Create(CAlarmInform::IDD,&m_CtrlTab));
	m_CtrlTab.InsertItem(3,&tab1);
	alarmInfo.SetWindowPos(NULL,5,30,0,0,SWP_NOSIZE|SWP_NOZORDER);
	alarmInfo.ShowWindow(SW_HIDE);
	alarmInfo.EnableWindow(FALSE);
	//////////////////////////////////////////////////////////
	tab1.mask=TCIF_PARAM|TCIF_TEXT;
	tab1.lParam=(LPARAM)&administrator;
	tab1.iImage=1;//IDB_BITMAP1;
	tab1.pszText="管理员";
	VERIFY(administrator.Create(CADMINSTRATOR::IDD,&m_CtrlTab));
	m_CtrlTab.InsertItem(4,&tab1);
	administrator.SetWindowPos(NULL,5,30,0,0,SWP_NOSIZE|SWP_NOZORDER);
	administrator.ShowWindow(SW_HIDE);
	administrator.EnableWindow(FALSE);
	*/
}


void CSi216ClientDlg::init()
{
	AfxOleInit();
	if(errReportFile.m_hFile==CFile::hFileNull)
		errReportFile.Open("错误日志.txt" ,CFile::modeCreate|CFile::modeReadWrite);

	//if(!mUdpSocket.Create(0x7500,SOCK_DGRAM,NULL))  //接收套接字29952
	if(!mUdpSocket.Create(0x7500,SOCK_DGRAM,NULL))  //接收套接字29952
		AfxMessageBox("Recv  UDP Socket Create error");
	
	int value,len;
	int nRecBuf=1800*1024;
	len=sizeof(value);
    mUdpSocket.SetSockOpt(SO_RCVBUF,&nRecBuf,sizeof(nRecBuf));//设置接收缓存大小。
	mUdpSocket.GetSockOpt(SO_RCVBUF,(char*)&value,&len); //查询大小自然状态是8192，而发进来的是8340,增大后明显降低丢包率。
	

	//if(!udpSendPrg.Create(0x7501,SOCK_DGRAM,NULL))   //发送套接字
	//	AfxMessageBox("Send  UDP Socket Create error");


	SetTimer(1,1000,NULL);
	SetTimer(2,1000,NULL);
	SetTimer(3,500,NULL);
	SetTimer(4,1000,NULL);//计数，看多长时间没有来UDP数据了。如果来了数据，就把计数器清零。
	SetTimer(5,200,NULL);//计数，每1秒保存一次文件//改为0。4S保存一次，否则100HZ的时候丢包率高，接收UDP的数据会丢。

	char	ddd[1024];
	GetCurrentDirectory(1024,ddd);
	sPath = ddd;
	//sPath =getWorkingPath();
//////////////////////////////////
	m_access.m_ParaTablePath=sPath+"\\FbgParameter.mdb";

	if (!isFileExist(m_access.m_ParaTablePath))	
	{
		AfxMessageBox("数据库文件不存在");
		return;
	}		
	if (m_access.ReadAllFbgParameter())	                   //计算FBG结果线程。
		m_fbgCalThread=AfxBeginThread(CalFbgThread,NULL);

	/**/	

	if(!fbgResultSendPrg.Create(0x8000,SOCK_DGRAM,NULL))   //发送端口0x8000,远程机接收端口0x8001;
			AfxMessageBox("Send  UDP Socket Create error");
	//else
	//	AfxBeginThread(sendFbgDataOut,NULL);     //去掉发送线程，直接发送。

	////////////////查发送缓存大小/试验/////////////////////////
 	int value1,len1;
 	int nSndBuf1=100*1024;
 	len1=sizeof(value1);
    fbgResultSendPrg.SetSockOpt(SO_SNDBUF,&nSndBuf1,sizeof(nSndBuf1));
 	fbgResultSendPrg.GetSockOpt(SO_SNDBUF,(char*)&value1,&len1); //自然状态也是8192，而发出去的是19204，传感器类型5个汉字或10个英文,增大后明显降低丢包率。

	//////////////////////////////////////////	
	if (m_access.readIP())          //读取参数表中的IP地址，并赋给管理员界面。
	{		
		CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
		pDlg->m_admin.init();
	}
	else
		sprintf(ID_ipStr,"%u.%u.%u.%u",0,0,0,0);
////////////////////////////////////////////////////////
	if(!isFileExist(sPath+"\\历史记录\\报警记录.mdb"))    //不存fbgPara..mdb直接退出.
	{		
		CString curpath2 =sPath+ "\\历史记录\\报警记录"+".mdb";  //报警记录
		CopyDBAlarm(curpath2);	
	}
	////////////////////////////////

}

void CSi216ClientDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static int i=0,j=0;
	switch (nIDEvent)
	{		
	case 1:
		m_wavesheet1.refresh_list();
		break;
	case 2:
		m_strain.refresh_StrainList();
		break;
	case 3: 	//正常灯闪。
		if (!flag_udpReceived) //未收到UDP数据
		{		
			if (j==0)
				showLight(yellow);
			else
				showLight(backGround);
			j++;
			if (j>1) j=0;
			GetDlgItem(IDC_dataErr)->ShowWindow(SW_SHOW);
			break;
		}
		else  //收到UDP数据了。
		{
			GetDlgItem(IDC_dataErr)->ShowWindow(SW_HIDE);
			if (i==0)
				showLight(blue);
			else
				showLight(backGround);
			i++;
			if (i>1) i=0;
		}		
		break;
	case 4:
		m_countUdpErr++;
		if (m_countUdpErr>3)  flag_udpReceived=false;
		break;
	case 5:  //0.4秒钟保存一次文件。
		if (g_saveResultFlag)  		
			SetEvent(g_saveResultHandle);
		break;
	default:
		break;
	}	
	CDialog::OnTimer(nIDEvent);
}

void CSi216ClientDlg::stopSoft()
{
	if(errReportFile.m_hFile!=CFile::hFileNull)
		errReportFile.Close();
	mUdpSocket.Close();

}

void CSi216ClientDlg::OnOK() 
{
	if(MessageBox(NULL,"确定退出吗?",MB_YESNO+MB_DEFBUTTON2)==IDNO)
		return;
	m_saveBuffer.alarmWriteToBuffer("","退出");       //在缓存中写入开机字符。	
	SetEvent(g_saveAlarmInfoHandle);		//发保存报警信息的事件。	还没有开始线程呢。
	Sleep(1200);
	CDialog::OnOK();
}

void CSi216ClientDlg::showLight(int color1)
{
	switch(color1)
	{
	case 1:
		GetDlgItem(IDC_blue)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_background)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_yellow)->ShowWindow(SW_HIDE);
		break;
	case 2:
		GetDlgItem(IDC_blue)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_background)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_yellow)->ShowWindow(SW_SHOW);
		break;
	case 0 :
		GetDlgItem(IDC_blue)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_background)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_yellow)->ShowWindow(SW_HIDE);
		break;
	default:
		break;
	}
}

void CSi216ClientDlg::alarmLight(int color1)
{
	switch(color1)
	{
	case NORMAL: //正常灯亮，背景灯灭，其它灯灭，背景灯亮
		GetDlgItem(IDC_normal)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_grey1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_fault)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_grey2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_alarm)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_grey3)->ShowWindow(SW_SHOW);
		break;
	case FAULT://正常灯灭，故障灯亮,报警灯不管
		GetDlgItem(IDC_normal)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_grey1)->ShowWindow(SW_SHOW);		
		GetDlgItem(IDC_fault)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_grey2)->ShowWindow(SW_HIDE);	
		break;	
	case ALARM://正常灯灭，故障灯不管，报警灯亮
		GetDlgItem(IDC_normal)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_grey1)->ShowWindow(SW_SHOW);			
		GetDlgItem(IDC_alarm)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_grey3)->ShowWindow(SW_HIDE);
		break;
	default:
		break;
	}
}
