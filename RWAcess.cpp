// RWAcess.cpp: implementation of the RWAcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Si216Client.h"
#include "Si216ClientDlg.h"
#include "RWAcess.h"
#include "Glbs.h"
#include "Administrator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RWAcess::RWAcess()
{
	
}

RWAcess::~RWAcess()
{
	
}



bool RWAcess::OpenSheet(CString sheet)
{
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //初始化	
	//m_pRecordset->
	try                       //打开表CHN,看是否成功.
	{
		CString mm="SELECT * FROM "+sheet;       // 查询文件   CHN表   // 获取库接库的IDispatch指针	
		m_pRecordset->Open(_bstr_t(mm),m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return false;
	}
	return true;
}

bool RWAcess::ReadAllFbgParameter()
{
	int channel,j;
	CString temp;
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	//CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// 打开本地Access库FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("数据库连接失败，确认数据库FbgParameter.mdb是否正确!");
		return false;
	} 
	
	///////////////////////////////////////////////////////////////		
	for(channel=0;channel<16;channel++)	//	for(int channel=0;channel<2;channel++)
	{		
		m_pRecordset.CreateInstance(__uuidof(Recordset));  //初始化
		maxSensor[channel]=0;
		int sensorNumber=0;		   	//临时变量,用于存储本通道的传感器的数量.赋值给  os232Setup.maxSensor[channel]
		memset(&m_fbgPar[channel][sensorNumber].bh,0,sizeof(double)*7);//2015-6-25


		temp.Format("%d",channel+1);
		try                       //打开表CHN,看是否成功.
		{
			CString mm="SELECT * FROM CH"+temp+" ORDER BY 原始波长 ASC";// 查询文件   CHN表	// 获取库接库的IDispatch指针			
			m_pRecordset->Open(_bstr_t(mm),m_pConnection.GetInterfacePtr(),
				//adOpenDynamic,adLockOptimistic,adCmdText);			
				adOpenStatic,adLockOptimistic,adCmdText);			
		}
		catch(_com_error e)
		{
			AfxMessageBox("表 CH"+temp+" 出错");	//AfxMessageBox(e.ErrorMessage());
			m_pConnection->Close();	
			return false;
		}
		////////////////////// ///////////  读取CHN 表内数据。
		try
		{
			if(!m_pRecordset->BOF)    //如果CHN内有数据,则跳到第一行.
				m_pRecordset->MoveFirst();
			else
			{
				maxSensor[channel]=0;
				//AfxMessageBox("表  CH"+temp+"数据为空");   //如果为空, 应该赋值为0,继续下一通道,退出此循环
				continue;
			}
			//////////////////////原始波长等参数///////////////////////////////////////			
			while(!m_pRecordset->adoEOF)  
			{		////////////—读取表内数据。文件尾m_pRecordset->BOF=-1,!m_pRecordset->BOF=0;  非文件尾为0, !()=1;
				//SHORT m=m_pRecordset->adoEOF;
				//SHORT n=m_pRecordset->EOF;
				/**/
				_variant_t var1 = m_pRecordset->GetCollect(_variant_t("编号"));
				if(var1.vt != VT_NULL)				
					m_fbgPar[channel][sensorNumber].sn = var1.intVal;
				//else
				//	m_fbgPar[channel][sensorNumber].ID="";				
				
				//--------------------------------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("ID"));
				if(var1.vt != VT_NULL)				
					m_fbgPar[channel][sensorNumber].ID=var1.bstrVal;			
				else
					m_fbgPar[channel][sensorNumber].ID="";				
				//--------------------------------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("传感器类型"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].sensorType=var1.bstrVal;
				else
					m_fbgPar[channel][sensorNumber].sensorType="";
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("原始波长"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].orignalWavelengh=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].orignalWavelengh=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=1;//缺失原始波长。
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("波长下限"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].waveLengthLeftLimited=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].waveLengthLeftLimited=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=11;//缺失原始波长。
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("波长上限"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].waveLengthRightLimited=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].waveLengthRightLimited=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=11;//缺失原始波长。
				}
				
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("传感系数"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].k=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].k=0;					
					m_fbgPar[channel][sensorNumber].paraIsFull=2;//缺失原始温度。				
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("初始温度"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].orignalTemperature=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].orignalTemperature=0;//如果未赋预警温度,则赋给65.					
					m_fbgPar[channel][sensorNumber].paraIsFull=3;//缺失原始温度。				
				}

				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("温度系数"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].temperatureCoefficnet=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].orignalTemperature=0;//如果未赋预警温度,则赋给65.					
					m_fbgPar[channel][sensorNumber].paraIsFull=3;//缺失原始温度。				
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("温补系数"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].tCompezation=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].tCompezation=0;//如果未赋预警温度,则赋给65.					
					m_fbgPar[channel][sensorNumber].paraIsFull=4;//缺失原始温度。				
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("下限报警"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].alarmMin=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].alarmMin=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=5;
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("上限报警"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].alarmMax=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].alarmMax=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=6;
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("温补ID"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].matchFbgID=var1.bstrVal;
				else
				{
					m_fbgPar[channel][sensorNumber].matchFbgID="";//
					m_fbgPar[channel][sensorNumber].paraIsFull=7;
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("单位"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].unit=var1.bstrVal;
				else
				{
					m_fbgPar[channel][sensorNumber].unit="";//
					m_fbgPar[channel][sensorNumber].paraIsFull=8;
				}				
				//----------------------------------------------						
				var1 = m_pRecordset->GetCollect(_variant_t("地理位置"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].position=var1.bstrVal;
				else
					m_fbgPar[channel][sensorNumber].position="";
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("屏蔽"));//.bstrVal;
				if(var1.vt != VT_NULL)					
					m_fbgPar[channel][sensorNumber].shield=(bool)var1.boolVal;
				else
					m_fbgPar[channel][sensorNumber].shield=false;					
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("备注"));//.bstrVal;
				if(var1.vt != VT_NULL)					
					m_fbgPar[channel][sensorNumber].note=var1.bstrVal;
				else
					m_fbgPar[channel][sensorNumber].note="";
				//----------------------------------------------
				///////////////////////   波汇新加的参数215-6-25   /////////////////////////////////
				var1 = m_pRecordset->GetCollect(_variant_t("a"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].bh.a=var1.dblVal;

				var1 = m_pRecordset->GetCollect(_variant_t("b"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].bh.b=var1.dblVal;

				var1 = m_pRecordset->GetCollect(_variant_t("c"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].bh.c=var1.dblVal;

				var1 = m_pRecordset->GetCollect(_variant_t("d"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].bh.d=var1.dblVal;

				var1 = m_pRecordset->GetCollect(_variant_t("e"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].bh.e=var1.dblVal;

				var1 = m_pRecordset->GetCollect(_variant_t("f"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].bh.f=var1.dblVal;

				var1 = m_pRecordset->GetCollect(_variant_t("k"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].bh.k=var1.dblVal;
				/////////////////////////////////////////////////////////			
			
				m_fbgPar[channel][sensorNumber].no=sensorNumber;  //光栅传感器序号 
				m_fbgPar[channel][sensorNumber].alarmStatus=NORMAL;;	  //报警状态为0,正常			
				///////////////////////////////////////////////////////////
				sensorNumber++;				
				m_pRecordset->MoveNext();		
			}
			maxSensor[channel]=sensorNumber;
		}	
		catch(_com_error *e)
		{
			AfxMessageBox(e->ErrorMessage());
			return  false;
		}  
	}
	m_pConnection->Close();

	for (channel=0;channel<16;channel++)  //剩下的传感器，标注为无传感器编号。
	{
		for (j=maxSensor[channel];j<20;j++)
		{
			m_fbgPar[channel][j].alarmStatus=-1;//无此传感器。的编号
			m_fbgResult_1.result_ch[channel].fbgResult[j].alarmStatus=-1;
		}
	}
	return true;
}

bool RWAcess::readIP()
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	//CString m_ParaTablePath=sPath+"\\FbgParameter.mdb";
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// 打开本地Access库FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("数据库连接失败，确认数据库FbgParameter.mdb是否正确!");
		return false;
	} 
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //初始化
	///////////////////////////////////////////////////////////////		
	try                       //打开表CHN,看是否成功.
	{	
		CString mm="SELECT * FROM setup  ORDER BY 编号 ASC";
		m_pRecordset->Open(_bstr_t(mm),          // 查询文件CHN表中所有字段	
			m_pConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针
			adOpenDynamic,
			adLockOptimistic,
			adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox("表 setup 出错");//AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return false;
	}
	////////////////////// ///////////  读取CHN 表内数据。
	try
	{
		if(!m_pRecordset->BOF)    //如果CHN内有数据,则跳到第一行.
			m_pRecordset->MoveFirst();
		else
		{
			AfxMessageBox("表 setup 缺少数据");    //如果为空, 应该赋值为0,继续下一通道,退出此循环
			return false;
		}
		//////////////////////IP值///////////////////////////////////////	
		int n1=0;
		pDlg->m_admin.a[0]=127;pDlg->m_admin.a[1]=0;
		pDlg->m_admin.a[2]=0  ;pDlg->m_admin.a[3]=1;
		
		_variant_t var1 = m_pRecordset->GetCollect(_variant_t("IP发送"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_autoSend=(bool)var1.boolVal;
			g_autoSend=(bool)var1.boolVal;//2013-12-21加入
		}
		var1 = m_pRecordset->GetCollect(_variant_t("抽样发送"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_udpSendChoice=(bool)var1.boolVal;
			g_udpSendChoice=(bool)var1.boolVal;//2015-6-23加入
		}
		var1 = m_pRecordset->GetCollect(_variant_t("加速度波长发送"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_sendFftBuffer=(bool)var1.boolVal;
			g_sendFftBuffer=(bool)var1.boolVal;//2015-7-14加入
		}

		var1 = m_pRecordset->GetCollect(_variant_t("发送间隔"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_udpSendFrame=var1.intVal;
			g_udpSendFrame=var1.intVal;//2015-6-23加入
		}
		/////////////////////////////////////
		m_pRecordset->MoveNext();
		var1 = m_pRecordset->GetCollect(_variant_t("发送间隔"));//读端口号
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_addressPort=var1.intVal;//var1.intVal;
			g_addressPort=var1.intVal;//2018--28
		}
		else
		{
			pDlg->m_admin.m_addressPort=0x8001;//var1.intVal;
			g_addressPort=0x8001;
		}
		m_pRecordset->MoveFirst();//换了列，重新到第一行。
		/////////////////////////////////////

		var1 = m_pRecordset->GetCollect(_variant_t("本机ID"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_pcID=var1.intVal;//var1.intVal;
			g_pcID=var1.intVal;//2015-6-29加入			
		}
		else
		{
			pDlg->m_admin.m_pcID=0;//var1.intVal;
			g_pcID=0;//2015-6-29加入
		}	

/////////////////////BW wanted///////2017-8-23	
		var1 = m_pRecordset->GetCollect(_variant_t("保存结果"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_saveResult=(bool)var1.boolVal;//var1.intVal;
			g_saveResultFlag=TRUE;
		}
		var1 = m_pRecordset->GetCollect(_variant_t("间隔记录"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_sampleCheck=(bool)var1.boolVal;//var1.intVal;			
		}
		var1 = m_pRecordset->GetCollect(_variant_t("保存间隔"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_saveSample=var1.intVal;//var1.intVal;			
		}
		//else
	//	{
	//		pDlg->m_admin.m_saveResult=0;		
	//	}
/////////////////////////////////
		//m_fbgResult_1.head.pcID=g_pcID;
		for (int ch=0;ch<16;ch++)
			m_fbgResult_1.result_ch[ch].pcID=g_pcID;
		m_sensorfftBuffer.pcID=g_pcID;
		
		while(!m_pRecordset->adoEOF)  
		{	///////////—读取表内数据。文件尾m_pRecordset->BOF=-1,!m_pRecordset->BOF=0;  非文件尾为0, !()=1;
			var1 = m_pRecordset->GetCollect(_variant_t("IP"));
			if(var1.vt != VT_NULL)							
				pDlg->m_admin.a[n1]=var1.intVal;		
			m_pRecordset->MoveNext();	
			n1++;
		}			
	}	
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
		return  false;
	}  
	
	m_pConnection->Close();
	return true;
}

