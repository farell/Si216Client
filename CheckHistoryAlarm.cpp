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
	HWND hWnd=m_historyALarm.m_hWnd;	         //获取句柄
	DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);		//获取显示格式
	if( (dwStyle&LVS_TYPEMASK)!=LVS_REPORT) 
		SetWindowLong(hWnd,GWL_STYLE,(dwStyle&~LVS_TYPEMASK|LVS_REPORT));//设置显示格式
	m_historyALarm.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_TYPEMASK);//加坐标线,全选一行 
	int nColumnCount =12;   // m_showAlarmControl.GetHeaderCtrl()()->GetItemCount();
	int MAX_CHANNEL_PLUSE1=29;	
	int i;
/*	for (i=0;i < nColumnCount;i++)
	{
		m_historyALarm.DeleteColumn(i);//?对吗?0->i?		
	}	
*/
	m_historyALarm.DeleteAllItems();
	
	m_historyALarm.InsertColumn(0,"序号",LVCFMT_CENTER,50);
	m_historyALarm.InsertColumn(1,"表中编号",LVCFMT_CENTER,80);
	m_historyALarm.InsertColumn(2,"时间",LVCFMT_LEFT,155);
	m_historyALarm.InsertColumn(3,"报警传感器",LVCFMT_LEFT,145);	
	m_historyALarm.InsertColumn(4,"报警信息",LVCFMT_LEFT,280);	
	
	CString	temp;
/**/
	for(i=0;i<100;i++) 
	{
		temp.Format("%d",i+1);
		m_historyALarm.InsertItem(i+1,"");  //插入行	
	}

}

void CCheckHistoryAlarm::OnExit() 
{
	CDialog::OnCancel();
}

void CCheckHistoryAlarm::ReadAlarmInfoFile()  //从报警记录文件中读取信息，并显示在表格中
{
	int fromNo=sheetStartCount;        //从表中开始的序号
	
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\历史记录\\报警记录.mdb";
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// 打开本地Access库FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("数据库连接失败  请查看'报警记录.mdb'  是否正确  3!");
		return;
	} 
	//////////////////   打开历史记录数据库  /////////////////////////////////////////////		
	
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //初始化	
	try                       //打开表CHN,看是否成功.
	{
		CString mm="SELECT * FROM 历史记录";
		m_pRecordset->Open(_bstr_t(mm),          // 查询文件   CHN表	
			m_pConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
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
	//////////////////////	——读取CHN 表内数据。
	try
	{
		if(!m_pRecordset->BOF)    //如果CHN内有数据,则跳到最后一行.
		{
			m_pRecordset->MoveLast();
			if(nCountAcess-sheetStartCount>=0)
			m_pRecordset->Move(sheetStartCount-nCountAcess);
		}
		else
		{
			AfxMessageBox("表内数据为空");   //如果为空, 应该赋值为0,继续下一通道,退出此循环			
		}
		//////////////////////原始波长等参数///////////////////////////////////////
		CString temp,str;
		int iii=0;
		while(iii<everySheet)  
		{		////////////—读取表内数据。文件尾m_pRecordset->BOF=-1,!m_pRecordset->BOF=0;  非文件尾为0, !()=1;
			temp.Format("%d",fromNo);				// 数字转换为字符串。   			
			m_historyALarm.SetItemText(iii,0,temp); 

			_variant_t var1 = m_pRecordset->GetCollect(_variant_t("编号"));//序号"));
			if(var1.vt != VT_NULL)	
			{				
				int ab=var1.iVal;			
				temp.Format("%d",ab);			// 数字转换为字符串。     						
				m_historyALarm.SetItemText(iii,1,temp); 	
			}
			else
			{
				m_historyALarm.SetItemText(iii,1,""); 					
			}	
			////////////////////////////////////////////////////////////////////////////////
			var1 = m_pRecordset->GetCollect(_variant_t("时间"));
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
			var1 = m_pRecordset->GetCollect(_variant_t("报警传感器"));
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
			var1 = m_pRecordset->GetCollect(_variant_t("报警内容"));
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
			if(fromNo<1)   //若计数号<1表示已经到文件头了。
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
	static int totalNo=0;  //表中的总序号
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\历史记录\\报警记录.mdb";
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// 打开本地Access库FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("数据库连接失败  请查看'报警记录.mdb'  是否正确  1!");
		return;
	} 
	//////////////////   打开历史记录数据库  /////////////////////////////////////////////		
	
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //初始化
	
	try                       //打开表CHN,看是否成功.
	{
		CString mm="SELECT * FROM 历史记录";
		m_pRecordset->Open(_bstr_t(mm),          // 查询文件   CHN表	
			m_pConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
			adOpenStatic,
			//				adOpenDynamic,
			adLockOptimistic,
			adCmdText);
		nCountAcess= m_pRecordset->GetRecordCount();//得到有几条数据
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
	}//按钮

	if(sheetStartCount<everySheet)  //
	{
		GetDlgItem(IDC_before)->EnableWindow(FALSE);			
	}
	else 
	{
		GetDlgItem(IDC_before)->EnableWindow(true);
	}
///////////////////////////////////////////////////////
	for(int i=0;i<everySheet;i++)//清空列表
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
	}//按钮
	
	if(sheetStartCount<everySheet)  //
	{
		GetDlgItem(IDC_before)->EnableWindow(FALSE);			
	}
	else 
	{
		GetDlgItem(IDC_before)->EnableWindow(true);
	}

	for(int i=0;i<everySheet;i++)//清空列表
	{
		m_historyALarm.SetItemText(i,0,""); 	
		m_historyALarm.SetItemText(i,1,""); 	
		m_historyALarm.SetItemText(i,2,""); 	
		m_historyALarm.SetItemText(i,3,""); 	
		m_historyALarm.SetItemText(i,4,""); 	
	}
	ReadAlarmInfoFile();	
}
