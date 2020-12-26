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
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //��ʼ��	
	//m_pRecordset->
	try                       //�򿪱�CHN,���Ƿ�ɹ�.
	{
		CString mm="SELECT * FROM "+sheet;       // ��ѯ�ļ�   CHN��   // ��ȡ��ӿ��IDispatchָ��	
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
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// �򿪱���Access��FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("���ݿ�����ʧ�ܣ�ȷ�����ݿ�FbgParameter.mdb�Ƿ���ȷ!");
		return false;
	} 
	
	///////////////////////////////////////////////////////////////		
	for(channel=0;channel<16;channel++)	//	for(int channel=0;channel<2;channel++)
	{		
		m_pRecordset.CreateInstance(__uuidof(Recordset));  //��ʼ��
		maxSensor[channel]=0;
		int sensorNumber=0;		   	//��ʱ����,���ڴ洢��ͨ���Ĵ�����������.��ֵ��  os232Setup.maxSensor[channel]
		memset(&m_fbgPar[channel][sensorNumber].bh,0,sizeof(double)*7);//2015-6-25


		temp.Format("%d",channel+1);
		try                       //�򿪱�CHN,���Ƿ�ɹ�.
		{
			CString mm="SELECT * FROM CH"+temp+" ORDER BY ԭʼ���� ASC";// ��ѯ�ļ�   CHN��	// ��ȡ��ӿ��IDispatchָ��			
			m_pRecordset->Open(_bstr_t(mm),m_pConnection.GetInterfacePtr(),
				//adOpenDynamic,adLockOptimistic,adCmdText);			
				adOpenStatic,adLockOptimistic,adCmdText);			
		}
		catch(_com_error e)
		{
			AfxMessageBox("�� CH"+temp+" ����");	//AfxMessageBox(e.ErrorMessage());
			m_pConnection->Close();	
			return false;
		}
		////////////////////// ///////////  ��ȡCHN �������ݡ�
		try
		{
			if(!m_pRecordset->BOF)    //���CHN��������,��������һ��.
				m_pRecordset->MoveFirst();
			else
			{
				maxSensor[channel]=0;
				//AfxMessageBox("��  CH"+temp+"����Ϊ��");   //���Ϊ��, Ӧ�ø�ֵΪ0,������һͨ��,�˳���ѭ��
				continue;
			}
			//////////////////////ԭʼ�����Ȳ���///////////////////////////////////////			
			while(!m_pRecordset->adoEOF)  
			{		////////////����ȡ�������ݡ��ļ�βm_pRecordset->BOF=-1,!m_pRecordset->BOF=0;  ���ļ�βΪ0, !()=1;
				//SHORT m=m_pRecordset->adoEOF;
				//SHORT n=m_pRecordset->EOF;
				/**/
				_variant_t var1 = m_pRecordset->GetCollect(_variant_t("���"));
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
				var1 = m_pRecordset->GetCollect(_variant_t("����������"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].sensorType=var1.bstrVal;
				else
					m_fbgPar[channel][sensorNumber].sensorType="";
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("ԭʼ����"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].orignalWavelengh=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].orignalWavelengh=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=1;//ȱʧԭʼ������
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("��������"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].waveLengthLeftLimited=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].waveLengthLeftLimited=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=11;//ȱʧԭʼ������
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("��������"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].waveLengthRightLimited=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].waveLengthRightLimited=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=11;//ȱʧԭʼ������
				}
				
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("����ϵ��"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].k=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].k=0;					
					m_fbgPar[channel][sensorNumber].paraIsFull=2;//ȱʧԭʼ�¶ȡ�				
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("��ʼ�¶�"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].orignalTemperature=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].orignalTemperature=0;//���δ��Ԥ���¶�,�򸳸�65.					
					m_fbgPar[channel][sensorNumber].paraIsFull=3;//ȱʧԭʼ�¶ȡ�				
				}

				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("�¶�ϵ��"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].temperatureCoefficnet=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].orignalTemperature=0;//���δ��Ԥ���¶�,�򸳸�65.					
					m_fbgPar[channel][sensorNumber].paraIsFull=3;//ȱʧԭʼ�¶ȡ�				
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("�²�ϵ��"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].tCompezation=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].tCompezation=0;//���δ��Ԥ���¶�,�򸳸�65.					
					m_fbgPar[channel][sensorNumber].paraIsFull=4;//ȱʧԭʼ�¶ȡ�				
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("���ޱ���"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].alarmMin=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].alarmMin=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=5;
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("���ޱ���"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].alarmMax=var1.fltVal;
				else
				{
					m_fbgPar[channel][sensorNumber].alarmMax=0;//
					m_fbgPar[channel][sensorNumber].paraIsFull=6;
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("�²�ID"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].matchFbgID=var1.bstrVal;
				else
				{
					m_fbgPar[channel][sensorNumber].matchFbgID="";//
					m_fbgPar[channel][sensorNumber].paraIsFull=7;
				}
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("��λ"));
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].unit=var1.bstrVal;
				else
				{
					m_fbgPar[channel][sensorNumber].unit="";//
					m_fbgPar[channel][sensorNumber].paraIsFull=8;
				}				
				//----------------------------------------------						
				var1 = m_pRecordset->GetCollect(_variant_t("����λ��"));//.bstrVal;
				if(var1.vt != VT_NULL)	
					m_fbgPar[channel][sensorNumber].position=var1.bstrVal;
				else
					m_fbgPar[channel][sensorNumber].position="";
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("����"));//.bstrVal;
				if(var1.vt != VT_NULL)					
					m_fbgPar[channel][sensorNumber].shield=(bool)var1.boolVal;
				else
					m_fbgPar[channel][sensorNumber].shield=false;					
				//----------------------------------------------
				var1 = m_pRecordset->GetCollect(_variant_t("��ע"));//.bstrVal;
				if(var1.vt != VT_NULL)					
					m_fbgPar[channel][sensorNumber].note=var1.bstrVal;
				else
					m_fbgPar[channel][sensorNumber].note="";
				//----------------------------------------------
				///////////////////////   �����¼ӵĲ���215-6-25   /////////////////////////////////
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
			
				m_fbgPar[channel][sensorNumber].no=sensorNumber;  //��դ��������� 
				m_fbgPar[channel][sensorNumber].alarmStatus=NORMAL;;	  //����״̬Ϊ0,����			
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

	for (channel=0;channel<16;channel++)  //ʣ�µĴ���������עΪ�޴�������š�
	{
		for (j=maxSensor[channel];j<20;j++)
		{
			m_fbgPar[channel][j].alarmStatus=-1;//�޴˴��������ı��
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
	
	_ConnectionPtr m_pConnection ;   //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr  m_pRecordset;    //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	try                 
	{	// �򿪱���Access��FbgParameter.mdb
		m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
	}
	catch(_com_error e)
	{
		AfxMessageBox("���ݿ�����ʧ�ܣ�ȷ�����ݿ�FbgParameter.mdb�Ƿ���ȷ!");
		return false;
	} 
	m_pRecordset.CreateInstance(__uuidof(Recordset));  //��ʼ��
	///////////////////////////////////////////////////////////////		
	try                       //�򿪱�CHN,���Ƿ�ɹ�.
	{	
		CString mm="SELECT * FROM setup  ORDER BY ��� ASC";
		m_pRecordset->Open(_bstr_t(mm),          // ��ѯ�ļ�CHN���������ֶ�	
			m_pConnection.GetInterfacePtr(),	 // ��ȡ��ӿ��IDispatchָ��
			adOpenDynamic,
			adLockOptimistic,
			adCmdText);
	}
	catch(_com_error e)
	{
		AfxMessageBox("�� setup ����");//AfxMessageBox(e.ErrorMessage());
		m_pConnection->Close();	
		return false;
	}
	////////////////////// ///////////  ��ȡCHN �������ݡ�
	try
	{
		if(!m_pRecordset->BOF)    //���CHN��������,��������һ��.
			m_pRecordset->MoveFirst();
		else
		{
			AfxMessageBox("�� setup ȱ������");    //���Ϊ��, Ӧ�ø�ֵΪ0,������һͨ��,�˳���ѭ��
			return false;
		}
		//////////////////////IPֵ///////////////////////////////////////	
		int n1=0;
		pDlg->m_admin.a[0]=127;pDlg->m_admin.a[1]=0;
		pDlg->m_admin.a[2]=0  ;pDlg->m_admin.a[3]=1;
		
		_variant_t var1 = m_pRecordset->GetCollect(_variant_t("IP����"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_autoSend=(bool)var1.boolVal;
			g_autoSend=(bool)var1.boolVal;//2013-12-21����
		}
		var1 = m_pRecordset->GetCollect(_variant_t("��������"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_udpSendChoice=(bool)var1.boolVal;
			g_udpSendChoice=(bool)var1.boolVal;//2015-6-23����
		}
		var1 = m_pRecordset->GetCollect(_variant_t("���ٶȲ�������"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_sendFftBuffer=(bool)var1.boolVal;
			g_sendFftBuffer=(bool)var1.boolVal;//2015-7-14����
		}

		var1 = m_pRecordset->GetCollect(_variant_t("���ͼ��"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_udpSendFrame=var1.intVal;
			g_udpSendFrame=var1.intVal;//2015-6-23����
		}
		/////////////////////////////////////
		m_pRecordset->MoveNext();
		var1 = m_pRecordset->GetCollect(_variant_t("���ͼ��"));//���˿ں�
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
		m_pRecordset->MoveFirst();//�����У����µ���һ�С�
		/////////////////////////////////////

		var1 = m_pRecordset->GetCollect(_variant_t("����ID"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_pcID=var1.intVal;//var1.intVal;
			g_pcID=var1.intVal;//2015-6-29����			
		}
		else
		{
			pDlg->m_admin.m_pcID=0;//var1.intVal;
			g_pcID=0;//2015-6-29����
		}	

/////////////////////BW wanted///////2017-8-23	
		var1 = m_pRecordset->GetCollect(_variant_t("������"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_saveResult=(bool)var1.boolVal;//var1.intVal;
			g_saveResultFlag=TRUE;
		}
		var1 = m_pRecordset->GetCollect(_variant_t("�����¼"));
		if(var1.vt != VT_NULL)
		{
			pDlg->m_admin.m_sampleCheck=(bool)var1.boolVal;//var1.intVal;			
		}
		var1 = m_pRecordset->GetCollect(_variant_t("������"));
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
		{	///////////����ȡ�������ݡ��ļ�βm_pRecordset->BOF=-1,!m_pRecordset->BOF=0;  ���ļ�βΪ0, !()=1;
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

