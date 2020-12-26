// Glbs.cpp: implementation of the Glbs class.
//
//////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "fft_Transform.h"

#include "stdafx.h"
#include "Glbs.h"

#include "Si216Client.h"
#include "ParaSet.h"
#include "RWAcess.h"
#include "Si216ClientDlg.h"
#include "saveBuffer.h"

#include "UDP1.h"
#include "saveBuffer.h"


//#include "UdpCount.h"

#define  fft_Buffer_number   1024//512//m_fbgPar. FFT�еĻ����еĸ���������ڴ�ֵ���Ž��м���
//#define  fft_count_frame 100  //ÿ100֡����һ��FFT���㣬�����100HZ�Ļ����൱��1S����һ�μ��㡣

#define  maxSaveOnce 100

bool lan=true;//��������
int fft_count_frame=100;//ÿ����֡����һ��FFT����,Ҳ�൱��Ƶ��
static int count_fft=0;//���˶���֡��

UdpCount m_udpCount;//��������UDP�߳��еļ�������
UDPRESWAVELGSTRUCT	g_UdpBuffer[UdpBufSize+100];
BOOL g_autoSend;
bool g_udpSendChoice;
bool g_sendFftBuffer;
int  g_udpSendFrame;//UDP���ͼ��
byte g_pcID;
UINT g_addressPort;//

int g_temp;//��ʱ�����á�

CStdioFile	errReportFile;
CString sPath;    //Ŀ¼
int maxSensor[16];
struct _fbgParameter m_fbgPar[16][maxChannelSensor];

struct _fbgData m_fbgData;			   //����FBG����
//_fbgResult m_fbgResult[16][20]; //FBG����������

_fbgResult_arary m_fbgR_A[UdpBufSize+100];
_fbgResult_arary m_fbgResult_1;
_SensorfftBuffer m_sensorfftBuffer;

char	ID_ipStr[20]={'1','2','7','.','0','.','0','1'}; //IP��ַ��
CSocket	fbgResultSendPrg;	// UDP����FBG���� 

bool isAlarmOrFault=false;//��û�й��ϻ򱨾���
bool g_saveResultFlag=false;    //�������ݽ���ı�־
HANDLE g_saveResultHandle;   //���������¼���
bool	flag_saveResultOk;        //���沨��OK��
saveBuffer m_saveBuffer;//��saveBuffer��һ��ʵ�������е����ݶ��ŵ��������ȥ�������á�

BOOL	ReportErr(CString *pString)
{
	if(errReportFile.m_hFile==CFile::hFileNull)
		return FALSE;
	char ccc[512];
	memset(ccc,0,512);
	memcpy(ccc,pString->GetBuffer(pString->GetLength()),pString->GetLength());
	errReportFile.Write(ccc,strlen(ccc));
	return TRUE;
}
CString getWorkingPath()
{	
	CString sPathPara;
	GetModuleFileName(NULL,sPathPara.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   //��ȡ��ǰ�ļ�������Ŀ¼
	sPathPara.ReleaseBuffer();   	
	int nPos=sPathPara.ReverseFind('\\');   	
	sPathPara=sPathPara.Left(nPos);				    //�ҵ�ǰĿ¼
	return sPathPara;	
}

Glbs::Glbs()
{
	udpSocket = new CSocket();
}

Glbs::~Glbs()
{
	//udpSocket->SendTo()
	delete udpSocket;
}
void sendDataToWeb()
{
	if (lan)
		fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),g_addressPort,ID_ipStr,0);//m_fbgResult_1 ��С19204��
	else
	{
		for (int ch=0;ch<16;ch++)
		{
			fbgResultSendPrg.SendTo(&m_fbgResult_1.result_ch[ch],sizeof(_fbgResult_channel),g_addressPort,ID_ipStr,0);
			//Sleep(1);
		}
	}
/*
	int a=sizeof(m_fbgResult_1);//20608
	int b=sizeof(m_fbgResult_1.result_ch[0]);//1288
	int c=sizeof(_fbgResult_channel);//1288
	int d=sizeof(_fbgResult);//64
*/

}
void sendFftBufferToWeb()
{
	for (int ch=0;ch<16;ch++)
	{
		for (int num=0;num<maxSensor[ch];num++)
		{
			if(m_fbgPar[ch][num].sensorType=="���ٶȼ�" ||m_fbgPar[ch][num].sensorType=="ACCE")
			{
				m_sensorfftBuffer.chn=ch;
				m_sensorfftBuffer.num=num;
				memcpy(&m_sensorfftBuffer.sensorID[0],&m_fbgResult_1.result_ch[ch].fbgResult[num].sensorID[0],sizeof(char)*10);
				memcpy(&m_sensorfftBuffer.buffer[0],&m_fbgPar[ch][num].fft_result.fft_buffer[0],sizeof(float)*100);				
				fbgResultSendPrg.SendTo(&m_sensorfftBuffer,sizeof(m_sensorfftBuffer),g_addressPort,ID_ipStr,0);//m_sensorfftBuffer ��С416��
			}
		}
	}
}

int isFileExist(CString fileName)  //ȷ��Ŀ¼��ĳ���ļ��Ƿ����
{
	CFileFind  tempFind;   //����һ��CFileFind����������������� FbgParameter.mdb�Ƿ����.
	int	b = tempFind.FindFile(fileName);//  "\\FbgParameter.mdb");	
	return b;	
}

UDPRESWAVELGSTRUCT	mRecvData;
//UDPRESWAVELGSTRUCT	mRecvDataRec[maxUdpClud];

bool	recvOK;
int		recvFrNum=0;
//int     g_maxCycle=10;//����UDP�е�ѭ��
bool	flag_readFbgOk;
UDP1	mUdpSocket;

CSocket	udpSendPrg;

int		tempa=0;
bool    flag_udpRec=true;
HANDLE  g_fbgStrainHandle;   //��������¼�
HANDLE  g_fbgResultSendHandle;//����FBG����¼���
HANDLE  g_saveAlarmInfoHandle;    //���汨����Ϣ���¼�������ġ�
bool    g_saveAlarmInfoFlag=true;     //���汨����Ϣ��־
bool	flag_saveALarmOk=true;       //���汨����ϢOK�ˡ�

bool CopyDB(CString &strDBPath)  //�������ݿ�
{	
	CString curpath=sPath+ "\\HistoryAlarm.mdb";
	int ret = CopyFile(curpath,strDBPath,true);//curpath ԭ�ļ���STRDBPATH Ŀ��  True�����ǣ�FALSE���ǡ�
	if(0==ret)
	{ 
		AfxMessageBox("�������ݿ�ʧ��!");
		return  false ;
	}
	return  true;	
}

UINT	CalFbgThread(LPVOID pParam)  //����FBG ���ݵ��߳�
{
	//memset(m_fbgPar,0,sizeof(_fbgParameter)*16*20);//2015-6-19��Ҫ��Ϊ�����FFT����ء��д��󣬿��ܸ�������
	//CString�й�ϵ��

	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	//pDlg->m_access.ReadAllFbgParameter();//�������д������Ĳ�����
	//pDlg->m_access.readIP();//�������д������Ĳ�����
	CString sss;
	int tempNum=0;

	for (int ch=0;ch<16;ch++)        //��ʼ������״̬
	{
		for (int sn=0;sn<maxSensor[ch];sn++)
		{
			m_fbgPar[ch][sn].alarmStatus=NORMAL;
			m_fbgPar[ch][sn].oldAlarmStatus=NORMAL;
			m_fbgPar[ch][sn].showAlarmStatus=NORMAL;
			m_fbgPar[ch][sn].showAlarmStatusLast=NORMAL;
			m_fbgPar[ch][sn].conCountAlarm=0;
			m_fbgPar[ch][sn].conCountFault=0;
			memset(&m_fbgPar[ch][sn].fft_result,0,sizeof(fft_struct));	
			m_fbgResult_1.result_ch[ch].fbgResult[sn].chn=ch;  //2015-6-24
			m_fbgResult_1.result_ch[ch].fbgResult[sn].num=sn;  //2015-6-24
		}
	}
	pDlg->m_paraSet.findID();//�ҵ���Թ�դ�����µ������������Ҫ�������д���䡣
	///////////////////////////////////////
	while(1)
	{
		if(WaitForSingleObject(g_fbgStrainHandle,1.5*timeInterval) ==WAIT_OBJECT_0)
		{
		//	sss.Format("\n calmiddNum= %d",m_udpCount.calMiddNum);ReportErr(&sss);
			ResetEvent(g_fbgStrainHandle);
			//m_udpCount.onCalcing=true;

			tempNum=m_udpCount.calMiddNum;
			//m_udpCount.calMiddNum=0;  //����  2015-7-14���Ρ�
			g_calFbg_new(tempNum);		

			count_fft++;              //ÿһ֡��1��ÿfft_count_frame (100)֡����һ�μ���
			if (count_fft>=fft_count_frame)
			{
				g_calFFT();
				count_fft=0;
			}
			//m_udpCount.onCalcing=false;
		}
		//Sleep(sleepTime);
	}
}
void g_calFbg(int num)//����num֡�Ļ����е�����
{	
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	CString sss;
	static int UdpNumber=0;//�����õ�
	static int sendFftBufferCount=0;//����FFT���ٶȴ������Ļ�������

	for (int i=0;i<num;i++)
	{		
		//sss.Format("\n CCCnt1= %d  Cal= %d  Midd=  %d  i=  %d",m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum,i);ReportErr(&sss);
//		sss.Format("\n i=%d ",i);ReportErr(&sss);
		memcpy(&m_fbgData,&g_UdpBuffer[m_udpCount.CalCount].mChnRes,sizeof(RESWAVELGSTRUCT)*16);
		//memset(&m_fbgResult_1,0,sizeof(_fbgResult_arary) ); //�����0�Ļ�������������Ҳ��û��
		for (int ch=0;ch<16;ch++)
			m_fbgResult_1.result_ch[ch].frameNo=g_UdpBuffer[m_udpCount.CalCount].mcount;//ȡ֡��

		matchFbg();			//��դ��ԡ�
		CalcFbgBH();//���ڼ���BH�Ĺ�ʽ��ͳһһ����ʽ��//CalcFbg();			//��դ����ļ��㡣	
		ResponseToAlarmInfor();    //��������   //����Ľ��ֵ����m_fbgResult_1

		//int lee1=sizeof(m_fbgResult_1);
		if (g_autoSend)   //���UDP���ͽ��
		{
			if (g_udpSendChoice)   //�����������   
			{
				UdpNumber++;
				if (UdpNumber>=g_udpSendFrame)
				{
					UdpNumber=0;
					//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 ��С19204�����������Ͳ�Ҫ����5�����ֻ���10��Ӣ�ġ�
					sendDataToWeb();
				}
			}
			else
				sendDataToWeb();
				//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 ��С19204�����������Ͳ�Ҫ����5�����ֻ���10��Ӣ�ġ�
			if (g_sendFftBuffer && lan)
			{
				sendFftBufferCount++;
				if (sendFftBufferCount>=100)
				{
					sendFftBufferCount=0;
					sendFftBufferToWeb();
				}
				
			}
		}
			
		if (g_saveResultFlag)  //�������
		{		
			if (pDlg->m_admin.m_sampleCheck)//�����������
			{	
				m_saveBuffer.sampleCount++; //2015-7-14
				if (m_saveBuffer.sampleCount>=pDlg->m_admin.m_saveSample)
				{
					m_saveBuffer.sampleCount=0;
					memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //������浽���������ȥ				
					m_saveBuffer.saveMiddCount++; //����������������ڱ�����Ӧ�߳��и�λ��ʱ����ÿ5�뷢������Ϣ��
					m_saveBuffer.BufCount++;      //������������
					if (m_saveBuffer.BufCount>=UdpBufSize)
						m_saveBuffer.BufCount=0;				
				}
			}
			else
			{
				memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //������浽���������ȥ
				m_saveBuffer.saveMiddCount++; //����������������ڱ�����Ӧ�߳��и�λ��ʱ����ÿ5�뷢������Ϣ��
				m_saveBuffer.BufCount++;      //������������
				if (m_saveBuffer.BufCount>=UdpBufSize)
					m_saveBuffer.BufCount=0;				
			}
			if (m_saveBuffer.saveMiddCount>=800)//��������̫����.����һЩ
			{
				m_saveBuffer.saveMiddCount-=50;
				m_saveBuffer.saveCount+=50;
				if (m_saveBuffer.saveCount>=UdpBufSize)
					m_saveBuffer.saveCount-=UdpBufSize;
			}			
		}		
		//sss.Format("\n num=%d  i= %d  NEWCnt1= %d  Cal= %d  Midd=  %d",num,i,m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
		//UDP�����еĲ����ļ���+1
	
		m_udpCount.CalCount++;
		m_udpCount.calMiddNum--;	//2015-7-14  ��仰�е�ʱ��ִ�У�����ɶ���ָ�볬��ʵ�ʵ�ָ�롣

        //ͨ����¼���������д˺�����ʱ�򣬻�����м��ϣ�����UDP���ݵĽ���������������仰�м���
		//calMiddNum��������������һ����ԭ���Բ����������һ������ô�ͻ���ɶ���ֵ������ǰ���
		if (m_udpCount.CalCount>=UdpBufSize)
			m_udpCount.CalCount=0;

		//CString sss;
		//sss.Format("\n aaaCount1= %d  Cal= %d  Midd=  %d",m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
		
		if ( (m_udpCount.CalCount+m_udpCount.calMiddNum != m_udpCount.UdpCount1) &&
			(m_udpCount.CalCount+m_udpCount.calMiddNum != m_udpCount.UdpCount1+UdpBufSize) )	
		{
		/*	sss.Format("\n ABCDEFG  != num= %d i= %d NEWCnt1= %d  Cal= %d  Midd=  %d",num,i,m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
			if (m_udpCount.CalCount+m_udpCount.calMiddNum>m_udpCount.UdpCount1)
			{
				sss.Format("\n +>AA num= %d i= %d NEWCnt1= %d  Cal= %d  Midd=  %d",num,i,m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
			}
			else if (m_udpCount.CalCount+m_udpCount.calMiddNum<m_udpCount.UdpCount1)
			{
				sss.Format("\n -<BB num= %d i= %d NEWCnt1= %d  Cal= %d  Midd=  %d",num,i,m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
			}
			else
			{
				if (m_udpCount.CalCount+m_udpCount.calMiddNum==m_udpCount.UdpCount1)
				{
					sss.Format("\n == num= %d i= %d NEWCnt1= %d  Cal= %d  Midd=  %d",num,i,m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
				}
				else
				{
					sss.Format("\n ?? num= %d i= %d NEWCnt1= %d  Cal= %d  Midd=  %d",num,i,m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
				}
			}
			*///16Сʱ����39�β��Ƚ�������10��>��4��<������==25��֮�࣬�������̵߳ĸ������㡣
			
			m_udpCount.CalCount=m_udpCount.UdpCount_old;
			m_udpCount.calMiddNum=1;
		}//����һ������Ϊ�е�ʱ�������ʱ���󣬾�������Ϊ�����ָ�볬���˵�ǰ��ָ�롣2015-7-17
	}
}

void g_calFbg_new(int num)//����num֡�Ļ����е�����
{	
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	CString sss;
	static int UdpNumber=0;//�����õ�
	static int sendFftBufferCount=0;//����FFT���ٶȴ������Ļ�������

	for (int i=0;i<num;i++)
	{		
		memcpy(&m_fbgData,&g_UdpBuffer[m_udpCount.CalCount].mChnRes,sizeof(RESWAVELGSTRUCT)*16);
		//memset(&m_fbgResult_1,0,sizeof(_fbgResult_arary) ); //�����0�Ļ�������������Ҳ��û��
		if (g_UdpBuffer[m_udpCount.CalCount].mcount<0)		//��־֡�ű�Ϊ-1�������Ѿ��㵽����ѭ�����ĵط��ˡ���������仰û���á�ԭ�����⡣		
		{	
			m_udpCount.calMiddNum=0;			
			return;
		}
		for (int ch=0;ch<16;ch++)
			m_fbgResult_1.result_ch[ch].frameNo=g_UdpBuffer[m_udpCount.CalCount].mcount;	//ȡ֡��

		//sss.Format(" mcount= %d calCount= %d  mid= %d num= %d ",g_UdpBuffer[m_udpCount.CalCount].mcount,m_udpCount.CalCount,m_udpCount.calMiddNum,num );ReportErr(&sss);

		g_UdpBuffer[m_udpCount.CalCount].mcount=-1;//�Ѿ��������֡������־��
		m_udpCount.CalCount++;
		m_udpCount.calMiddNum--;
		if (m_udpCount.calMiddNum<0) return;  
		if (m_udpCount.CalCount>=UdpBufSize)
			m_udpCount.CalCount=0;
	   /*	
		if ( (m_udpCount.CalCount+m_udpCount.calMiddNum != m_udpCount.UdpCount1) &&
			(m_udpCount.CalCount+m_udpCount.calMiddNum != m_udpCount.UdpCount1+UdpBufSize) )	
		{			
			m_udpCount.CalCount=m_udpCount.UdpCount_old;
			m_udpCount.calMiddNum=1;
		}
       */
		matchFbg();			//��դ��ԡ�
		CalcFbgBH();//���ڼ���BH�Ĺ�ʽ��ͳһһ����ʽ��//CalcFbg();			//��դ����ļ��㡣	
		ResponseToAlarmInfor();    //��������   //����Ľ��ֵ����m_fbgResult_1
	
		if (g_autoSend)   //���UDP���ͽ��
		{
			if (g_udpSendChoice)   //�����������   
			{
				UdpNumber++;
				if (UdpNumber>=g_udpSendFrame)
				{
					UdpNumber=0;
					//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 ��С19204�����������Ͳ�Ҫ����5�����ֻ���10��Ӣ�ġ�
					sendDataToWeb();
				}
			}
			else
				sendDataToWeb();
				//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 ��С19204�����������Ͳ�Ҫ����5�����ֻ���10��Ӣ�ġ�
			if (g_sendFftBuffer && lan)
			{
				sendFftBufferCount++;
				if (sendFftBufferCount>=100)
				{
					sendFftBufferCount=0;
					sendFftBufferToWeb();
				}				
			}
		}
			
		if (g_saveResultFlag)  //�������
		{		
			if (pDlg->m_admin.m_sampleCheck)//�����������
			{	
				m_saveBuffer.sampleCount++; //2015-7-14
				if (m_saveBuffer.sampleCount>=pDlg->m_admin.m_saveSample)
				{
					m_saveBuffer.sampleCount=0;
					memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //������浽���������ȥ				
					m_saveBuffer.saveMiddCount++; //����������������ڱ�����Ӧ�߳��и�λ��ʱ����ÿ5�뷢������Ϣ��
					m_saveBuffer.BufCount++;      //������������
					if (m_saveBuffer.BufCount>=UdpBufSize)
						m_saveBuffer.BufCount=0;				
				}
			}
			else
			{
				memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //������浽���������ȥ
				m_saveBuffer.saveMiddCount++; //����������������ڱ�����Ӧ�߳��и�λ��ʱ����ÿ5�뷢������Ϣ��
				m_saveBuffer.BufCount++;      //������������
				if (m_saveBuffer.BufCount>=UdpBufSize)
					m_saveBuffer.BufCount=0;				
			}
			if (m_saveBuffer.saveMiddCount>=800)//��������̫����.����һЩ
			{
				m_saveBuffer.saveMiddCount-=50;
				m_saveBuffer.saveCount+=50;
				if (m_saveBuffer.saveCount>=UdpBufSize)
					m_saveBuffer.saveCount-=UdpBufSize;
			}			
		}		
	}
}

void matchFbg()
{	
	int ch,sn;
	CString sss;

	for (ch=0;ch<16;ch++)		/////�������
	{
		for (sn=0;sn<maxSensor[ch];sn++)//2015-6-23//for (sn=0;sn<maxChannelSensor;sn++)
		{
			m_fbgPar[ch][sn].fbgWavelength=0;
			m_fbgPar[ch][sn].fbgPower=-70;
			m_fbgPar[ch][sn].matchSuccess=false;
		}
		//////���������������ͬ�����������һ������ô����������Ȼ������ˡ�
		if (maxSensor[ch]==m_fbgData.fbg[ch].num) 
		{			
			for (sn=0;sn<maxSensor[ch];sn++)	  //�����������ݸ�ֵһһ��ֵ
			{
				m_fbgPar[ch][sn].fbgWavelength=m_fbgData.fbg[ch].mWavelg[sn];
				m_fbgPar[ch][sn].fbgPower=m_fbgData.fbg[ch].mPower[sn];				
				m_fbgPar[ch][sn].matchSuccess=true;
			}
			continue;//��һ��CHͨ��
		}
		////////   ��������������ͬ������������һ���������ֵ
		else
		{				
			int jstart=0;	// ����������ԵĿ�ʼ��� // j ��ѭ�����Բ������0��ʼ,�ɹ����++,���ɹ���++
			for (int sn=0;sn<maxSensor[ch];sn++)
			{
				int success=0;			//������Գɹ��ĸ���
				float m_left=m_fbgPar[ch][sn].waveLengthLeftLimited;    //�����������ҽ硣
				float m_right=m_fbgPar[ch][sn].waveLengthRightLimited;  //
			
				
				for (int j=jstart;j<m_fbgData.fbg[ch].num;j++)//2015-6-24//for (int j=jstart;j<MAX_CHANNEL_PLUSE;j++)
				{
					float m_wave=m_fbgData.fbg[ch].mWavelg[j];
					float m_power=m_fbgData.fbg[ch].mPower[j];	//��ǰ��դ�����ݣ�
				
				
					if (m_wave >1590 || m_wave<1510) break;		//��ǰ��դ���ݲ��ڴ˷�Χ�ڣ��˳�������Ѱ�ҡ�
					if (m_wave<m_right && m_wave>m_left)		//��ǰ���������ҽ�֮��
					{
						if (m_fbgPar[ch][sn].matchSuccess==false)   //ֻ��û����Ե�ʱ��Ž��и�ֵ��
						{
							m_fbgPar[ch][sn].fbgWavelength=m_wave;
							m_fbgPar[ch][sn].fbgPower=m_power;
						}
						success++;
						m_fbgPar[ch][sn].matchSuccess=true;
						jstart++;
						if (success>1)
						{
							if (m_power>m_fbgPar[ch][sn].fbgPower)
							{
								m_fbgPar[ch][sn].fbgWavelength=m_wave;
								m_fbgPar[ch][sn].fbgPower=m_power;
							}//if
						}//if
					}//if
				} //for
			} //for				
		}//else	
	}//for

	//////////////////////////�Լ��ٶȼƽ��л��渳ֵ��/////////////////
	for (ch=0;ch<16;ch++)	
	{
		for ( sn=0;sn<maxSensor[ch];sn++)
		{
			if(m_fbgPar[ch][sn].sensorType=="���ٶȼ�" ||m_fbgPar[ch][sn].sensorType=="ACCE")
			{
				memmove(&m_fbgPar[ch][sn].fft_result.fft_buffer[1],&m_fbgPar[ch][sn].fft_result.fft_buffer[0],sizeof(float)*(FFT_N-1));
				m_fbgPar[ch][sn].fft_result.fft_buffer[0]=m_fbgPar[ch][sn].fbgWavelength;
				m_fbgPar[ch][sn].fft_result.n_need++;
				if (m_fbgPar[ch][sn].fbgWavelength==0)//����ж�ʧ�Ļ�������������Ϊ0
					m_fbgPar[ch][sn].fft_result.n_need=0;	
				if (m_fbgPar[ch][sn].fft_result.n_need>0xffff0000)
					m_fbgPar[ch][sn].fft_result.n_need=FFT_N;
			}
		}
	}
	////////////////////////////////////////////////////////
	
}

/*void CalcFbg()  //��դ����
{
	for (int ch=0;ch<16;ch++)	
	{
		for (int sn=0;sn<maxSensor[ch];sn++)
		{	
			//m_fbgResult_1.fbgResult[ch][sn].chn=ch;
			//m_fbgResult_1.fbgResult[ch][sn].num=sn;
			//m_fbgResult_1.fbgResult[ch][sn].result=0;
			
			if (m_fbgPar[ch][sn].sensorType=="�¶ȼ�" ||m_fbgPar[ch][sn].sensorType=="T")  //�¶ȼ�			
			{
				calTemperature(ch,sn);//�����¶Ȳ�������״̬�ȡ�			
				m_fbgResult_1.fbgResult[ch][sn].result=m_fbgPar[ch][sn].temperature;
			}
			else//���¶ȼ�			
			{
				calStrain(ch,sn);//����Ӧ�䲢������״̬�ȡ�			
				m_fbgResult_1.fbgResult[ch][sn].result=m_fbgPar[ch][sn].strain;
			}
		}
	}
}
*/
void CalcFbgBH()  //���㹫ʽ�ļ���
{
	for (int ch=0;ch<16;ch++)	
	{
		for (int sn=0;sn<maxSensor[ch];sn++)
		{
			calStrainBH(ch,sn);//����Ӧ�䲢������״̬�ȡ�
			//m_fbgResult_1.fbgResult[ch][sn].result=m_fbgPar[ch][sn].strain;			
		}
	}
}

void ResponseToAlarmInfor()
{
	for(int channel=0;channel<16;channel++)
	{			
		for (int i = 0; i <maxSensor[channel]; i++)    //�����¶�     i����������д����������
		{
			//	if(m_fbgPar[channel][i].alarmStatus!=m_fbgPar[channel][i].alarmStatusLast&&
			//	   m_fbgPar[channel][i].alarmStatus>=m_fbgPar[channel][i].showAlarmStatus)//&&i==0)//���ͬ�ϴβ�һ����			  
			//!!!!!���ԣ���һֱ��������Ϊһ��֮�󣬿��������������ˣ�Ȼ���������ˡ������᲻�ϳ���������
			//�������ܵ�״̬Ϊ׼�����ܵ�״̬�����仯֮�󣬲���������ǵ��ָ���ʱ��û����ʾ��
			if(m_fbgPar[channel][i].showAlarmStatus!=m_fbgPar[channel][i].showAlarmStatusLast)//��״̬�����˱仯��
			{
				CString temp0,temp1,temp2,temp3,temp,temp4,tempValue;
				temp0.Format("%d",channel+1);
				temp1.Format("%d",i+1);
				temp4=IntToString(m_fbgPar[channel][i].showAlarmStatusLast)+"->"+IntToString(m_fbgPar[channel][i].showAlarmStatus);
				if(m_fbgPar[channel][i].showAlarmStatus!=FAULT1)//fault1�Ƕ�ʧ��������
				{
					if (m_fbgPar[channel][i].sensorType=="�¶ȼ�"||m_fbgPar[channel][i].sensorType=="T")					
						tempValue.Format("    �¶�=%7.1f ��",m_fbgPar[channel][i].temperature);
					else
					{
						tempValue.Format("    ��ֵ=%7.1f",m_fbgPar[channel][i].strain);
						tempValue+=m_fbgPar[channel][i].unit;
					}
				}
				else
					tempValue="";				
				temp4+=tempValue;
				
				temp="ͨ��"+temp0+" ��������"+temp1+"     "+temp4;  //+"  "+temp5;
				
				//if(1)  //2012-8-21��������û�У��������
				if(m_fbgPar[channel][i].shield==false)  //��������û������
				{
					CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);					
					pDlg->alarmInfo.addAlarmInform(temp);   //��ʾ������Ϣ
					m_saveBuffer.alarmWriteToBuffer("ͨ��"+temp0+"  ��������"+temp1,temp4);				
					SetEvent(g_saveAlarmInfoHandle);		//�����汨����Ϣ���¼���
					if (m_fbgPar[channel][i].showAlarmStatus==FAULT2||m_fbgPar[channel][i].showAlarmStatus==FAULT1)  //���ϵĻ������ơ�
					{
						faultLight();						
						faultSpeaker();     //ֻ��״̬�仯�Ž���������������ˣ�״̬���䣬�ƺ����Ȳ����ٴδ���
						isAlarmOrFault=true;//�б�������ˡ�
					}
					if (m_fbgPar[channel][i].showAlarmStatus==ALARM ||m_fbgPar[channel][i].showAlarmStatus==PREALARM)
					{
						alarmLight();
						alarmSpeaker();
						isAlarmOrFault=true;//�б�������ˡ�
					}
					if (isAlarmOrFault==false)
					{
						okLight();
						cancelSpeaker();
					}					
				}			
			}
			m_fbgPar[channel][i].showAlarmStatusLast=m_fbgPar[channel][i].showAlarmStatus;//�����ܵ�״̬�����ϴ�
			m_fbgPar[channel][i].alarmStatusLast=m_fbgPar[channel][i].alarmStatus;		  //����״̬�����ϴ�

			//m_fbgResult_1.fbgResult[channel][i].alarmStatus=m_fbgPar[channel][i].alarmStatus;
			m_fbgResult_1.result_ch[channel].fbgResult[i].alarmStatus=m_fbgPar[channel][i].alarmStatus;
			
		}
	}
}

int  GetShowALarmStatus(int alarmStatus,int showAlarmStatus) //alarmStatus  ��ǰ��״̬   showAlarmStatus��ʷ��״̬��������ʾ��״̬,�����ܵ���״̬//�����µĴ�����״̬����ʷ�Ĵ�����״̬���������ء�
{  
	switch(showAlarmStatus)
	{	
	case NORMAL:           //0 ��Ϊ�������򷵻��µ�״̬��
		return alarmStatus;
		break;
	case FAULT1:
		if (alarmStatus!=NORMAL)
			return alarmStatus;
		else
			return FAULT1;
		break;
	case FAULT2:
		if (alarmStatus!=NORMAL)
			return alarmStatus;
		else
			return FAULT2;
		break;
	case PREALARM:         //3  ����״̬ΪԤ���������ر�Ԥ���͵�״̬��
		if (alarmStatus==ALARM)   
			return alarmStatus;
		else
			return PREALARM;
		break;
	case ALARM:           //��Ϊ��������һֱҪ���ر�����
		return ALARM;
		break;
	default:
		return NORMAL;
		break;
	}
}

void okLight()                 //�Ƹ�λ
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->alarmLight(NORMAL);
}
void cancelSpeaker()           //��������
{

}

void calTemperature(int ch,int sn)//�����¶ȴ�������
{
	float tempW=m_fbgPar[ch][sn].fbgWavelength;  //��ǰ��Ժ�Ĳ���
	if (tempW >1510 && tempW <1590)    //��Ժ�Ĺ�դ�������ڷ�Χ�ڡ�
	{
		m_fbgPar[ch][sn].conCountFault=0;//û�ж�ʧ�������¶ȼ���
		if (m_fbgPar[ch][sn].temperatureCoefficnet!=0) //����¶�ϵ����Ϊ0�������¶�
		{					
			m_fbgPar[ch][sn].temperature=m_fbgPar[ch][sn].orignalTemperature+
				(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*1000/m_fbgPar[ch][sn].temperatureCoefficnet;
		}			
		if (  (m_fbgPar[ch][sn].temperature>=m_fbgPar[ch][sn].alarmMax && m_fbgPar[ch][sn].alarmMax!=0)
			|| m_fbgPar[ch][sn].temperature<=m_fbgPar[ch][sn].alarmMin && m_fbgPar[ch][sn].alarmMin!=0)//�жϱ������
		{
			m_fbgPar[ch][sn].conCountAlarm++;
			if (m_fbgPar[ch][sn].conCountAlarm>confirmTimeAlarm)
			{
				m_fbgPar[ch][sn].alarmStatus=ALARM;			
			}
		}
		else
		{
			m_fbgPar[ch][sn].conCountAlarm=0;  //����
			m_fbgPar[ch][sn].alarmStatus=NORMAL;			
		}	
		m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(m_fbgPar[ch][sn].alarmStatus,m_fbgPar[ch][sn].showAlarmStatus);
	}
	else    //�������ڷ�Χ�ڡ���ʾû�в������ݡ�
	{
		m_fbgPar[ch][sn].conCountFault++;
		m_fbgPar[ch][sn].temperature=0;
		if (m_fbgPar[ch][sn].conCountFault>confirmTimeFault)
		{
			m_fbgPar[ch][sn].temperature=NULL;
			m_fbgPar[ch][sn].alarmStatus=FAULT1;      //�޹�դ����
			m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT1,m_fbgPar[ch][sn].showAlarmStatus);
		}	
	}

}

void calStrainBH(int ch, int sn) //����Ӧ���
{
	float w=m_fbgPar[ch][sn].fbgWavelength;  //��ǰ��Ժ�Ĳ���
	int chCom=m_fbgPar[ch][sn].matchFbgCh;//�²���դ��ͨ���ż�˳��š�
	int snCom=m_fbgPar[ch][sn].matchFbgNum;
	float w0=m_fbgPar[ch][sn].orignalWavelengh;;	
	double a,b,c,d,e,f,bhk;

	a=m_fbgPar[ch][sn].bh.a;
	b=m_fbgPar[ch][sn].bh.b;
	c=m_fbgPar[ch][sn].bh.c;
	d=m_fbgPar[ch][sn].bh.d;
	e=m_fbgPar[ch][sn].bh.e;
	f=m_fbgPar[ch][sn].bh.f;
	bhk=m_fbgPar[ch][sn].bh.k;
// 	if (ch==1 && sn==3)
// 	{
// 		int abc=10;
// 	}
	if (m_fbgPar[ch][sn].sensorType=="���ٶȼ�" ||m_fbgPar[ch][sn].sensorType=="ACCE" )
	{
		if (m_fbgPar[ch][sn].fbgWavelength==0)//������ʧ
		{
			m_fbgPar[ch][sn].strain=NULL;
			m_fbgPar[ch][sn].conCountFault++;
			if (m_fbgPar[ch][sn].conCountFault>confirmTimeFault)
			{
				m_fbgPar[ch][sn].alarmStatus=FAULT1;
				m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT1,m_fbgPar[ch][sn].showAlarmStatus);
			}
		}
		else
		{
			m_fbgPar[ch][sn].conCountFault=0;
			m_fbgPar[ch][sn].alarmStatus=NORMAL;
		}
		return;
	}
	
	if (w>1510 && w<1590)//����û�ж�ʧ�����ǻ�Ҫ���²�ID�Ĵ����������Ƿ�Ҳû�ж�ʧ��
	{
		m_fbgPar[ch][sn].conCountFault=0;   //������û�ж�ʧ��

		if (m_fbgPar[ch][sn].matchFbgCh>=0 && m_fbgPar[ch][sn].matchFbgNum>=0)	//������²��Ĺ�դ -1��Ժ��Ҳ�����-2û����Ժ�
		{
			float wt=m_fbgPar[chCom][snCom].fbgWavelength;//�²���դ�Ĳ�����
			float wt0=m_fbgPar[chCom][snCom].orignalWavelengh;
			if (wt >1505 && wt <1595)		 //�²���դû�ж�ʧ
			{
				m_fbgPar[ch][sn].conCountFault2=0; 
				//m_fbgPar[ch][sn].strain=a*(w-w0)*(w-w0)+b*(w-w0)+c-bhk*(d*(wt-wt0)*(wt-wt0)+e*(wt-wt0)+f);
				m_fbgPar[ch][sn].strain=a*((w-w0)-(wt-wt0))*((w-w0)-(wt-wt0))
					                   +b*((w-w0)-bhk*(wt-wt0))+c;
				
			}
			else      //�²���դ��ʧ�ˡ�
			{
				m_fbgPar[ch][sn].conCountFault2++;
				if (m_fbgPar[ch][sn].conCountFault2>confirmTimeFault)
				{
					m_fbgPar[ch][sn].alarmStatus=FAULT2;//2013-6-26 //m_fbgPar[ch][sn].alarmStatus=FAULT2;
					m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT2,m_fbgPar[ch][sn].showAlarmStatus);
					///��Ȼ�²���ʧ�ˣ����ǻ�������Ӧ��ȡ�//������?
					//m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
					m_fbgPar[ch][sn].strain=0;//(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
				}
			}
		}
		else if (m_fbgPar[ch][sn].matchFbgCh==-2 && m_fbgPar[ch][sn].matchFbgNum==-2)	 //û���²���դ
		{	
			m_fbgPar[ch][sn].strain=a*(w-w0)*(w-w0)+b*(w-w0)+c;

		}
		else        //==-1 �Ҳ�����,����Թ�դ�������Ҳ����úţ�ͬ��ʧ��Թ�դһ����˼
		{
			m_fbgPar[ch][sn].conCountFault2++;
			if (m_fbgPar[ch][sn].conCountFault2>confirmTimeFault)
			{
				m_fbgPar[ch][sn].alarmStatus=FAULT2;//2013-6-26 //m_fbgPar[ch][sn].alarmStatus=FAULT2;
				m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT2,m_fbgPar[ch][sn].showAlarmStatus);
				///��Ȼ�²���ʧ�ˣ����ǻ�������Ӧ��ȡ�//������?
				//m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
				m_fbgPar[ch][sn].strain=0;//(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
			}
		}
		float tempStrain=m_fbgPar[ch][sn].strain;

		if ( (m_fbgPar[ch][sn].alarmMax!=0 &&  tempStrain>=m_fbgPar[ch][sn].alarmMax)
			|| (m_fbgPar[ch][sn].alarmMin!=0 && tempStrain <=m_fbgPar[ch][sn].alarmMin)	)
		{
			m_fbgPar[ch][sn].conCountAlarm++;
			if (m_fbgPar[ch][sn].conCountAlarm>confirmTimeAlarm)
			{
				m_fbgPar[ch][sn].alarmStatus=ALARM;// 				
				m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(ALARM,m_fbgPar[ch][sn].showAlarmStatus);
			}
		}
		else if (m_fbgPar[ch][sn].alarmStatus!=FAULT2)//2015-6-26 �Ҳ�����Թ�դ
		{
			m_fbgPar[ch][sn].conCountAlarm=0;
			m_fbgPar[ch][sn].alarmStatus=NORMAL;
			m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(NORMAL,m_fbgPar[ch][sn].showAlarmStatus);
		}
	}
	else//�������Ĳ�����ʧ�ˡ�	
	{
		m_fbgPar[ch][sn].strain=NULL;
		m_fbgPar[ch][sn].conCountFault++;
		if (m_fbgPar[ch][sn].conCountFault>confirmTimeFault)
		{
			m_fbgPar[ch][sn].alarmStatus=FAULT1;
			m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT1,m_fbgPar[ch][sn].showAlarmStatus);
		}
	}
	//m_fbgResult_1.fbgResult[ch][sn].result=m_fbgPar[ch][sn].strain;
	m_fbgResult_1.result_ch[ch].fbgResult[sn].result=m_fbgPar[ch][sn].strain;
	m_fbgResult_1.result_ch[ch].fbgResult[sn].wavelength=m_fbgPar[ch][sn].fbgWavelength;
	m_fbgResult_1.result_ch[ch].fbgResult[sn].fbgPower=m_fbgPar[ch][sn].fbgPower;
}

void calStrain(int ch, int sn) //����Ӧ���
{
	float tempW=m_fbgPar[ch][sn].fbgWavelength;  //��ǰ��Ժ�Ĳ���
	int chCom=m_fbgPar[ch][sn].matchFbgCh;//�²���դ��ͨ���ż�˳��š�
	int snCom=m_fbgPar[ch][sn].matchFbgNum;

	if (tempW>1510 && tempW<1590)//����û�ж�ʧ�����ǻ�Ҫ���²�ID�Ĵ����������Ƿ�Ҳû�ж�ʧ��
	{
		m_fbgPar[ch][sn].conCountFault=0;   //������û�ж�ʧ��
		
		if (m_fbgPar[ch][sn].matchFbgCh>=0 && m_fbgPar[ch][sn].matchFbgNum>=0)	//������²��Ĺ�դ
		{
			float tempComW=m_fbgPar[chCom][snCom].fbgWavelength;//�²���դ�Ĳ�����
			if (tempComW >1510 && tempComW <1590)		 //�²���դû�ж�ʧ
			{
				m_fbgPar[ch][sn].conCountFault2=0; 
				m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k
					-m_fbgPar[ch][sn].tCompezation*(m_fbgPar[chCom][snCom].fbgWavelength-m_fbgPar[chCom][snCom].orignalWavelengh);
			}
			else      //�²���դ��ʧ�ˡ�
			{
				m_fbgPar[ch][sn].conCountFault2++;
				if (m_fbgPar[ch][sn].conCountFault2>confirmTimeFault)
				{
					m_fbgPar[ch][sn].alarmStatus=FAULT2;
					m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT2,m_fbgPar[ch][sn].showAlarmStatus);
					///��Ȼ�²���ʧ�ˣ����ǻ�������Ӧ��ȡ�//Ҫ����?
					m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
				}
			}
		}
		else  //û���²���դ
		{
			m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;			
		}
		float tempStrain=m_fbgPar[ch][sn].strain;
		
		if (   (m_fbgPar[ch][sn].alarmMax!=0 &&  tempStrain>=m_fbgPar[ch][sn].alarmMax)
			|| (m_fbgPar[ch][sn].alarmMin!=0 && tempStrain <=m_fbgPar[ch][sn].alarmMin)	)
			
		{
			m_fbgPar[ch][sn].conCountAlarm++;
			if (m_fbgPar[ch][sn].conCountAlarm>confirmTimeAlarm)
			{
				m_fbgPar[ch][sn].alarmStatus=ALARM;// 				
				m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(ALARM,m_fbgPar[ch][sn].showAlarmStatus);
			}
		}
		else
		{
			m_fbgPar[ch][sn].conCountAlarm=0;
			m_fbgPar[ch][sn].alarmStatus=NORMAL;
			m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(NORMAL,m_fbgPar[ch][sn].showAlarmStatus);
		}
	}
	else//�������Ĳ�����ʧ�ˡ�	
	{
		m_fbgPar[ch][sn].strain=NULL;
		m_fbgPar[ch][sn].conCountFault++;
		if (m_fbgPar[ch][sn].conCountFault>confirmTimeFault)
		{
			m_fbgPar[ch][sn].alarmStatus=FAULT1;
			m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT1,m_fbgPar[ch][sn].showAlarmStatus);
		}
	}		
}
UINT	sendFbgDataOut(LPVOID pParam)  //��FBG�ļ��������ͳ�ȥ�������ˣ�ֱ�Ӽ�����ɺ󷢳�ȥ��
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);	
	BYTE a1,a2,a3,a4;
	pDlg->m_admin.m_ipAddress.GetAddress(a1,a2,a3,a4);	
	memset(ID_ipStr,0,sizeof(ID_ipStr));
	sprintf(ID_ipStr,"%u.%u.%u.%u",a1,a2,a3,a4);
	
	while(1)
	{
		if(WaitForSingleObject(g_fbgResultSendHandle,1.5*timeInterval) ==WAIT_OBJECT_0)
		{
			ResetEvent(g_fbgResultSendHandle);
			UdpSendFbg();
		}
	}
	return 0;
}
void UdpSendFbg()  //��FBG�ļ��������ͳ�ȥ��
{
// 	for (int ch=0;ch<16;ch++)
// 	{
// 		for (int sn=0;sn<maxChannelSensor;sn++)
// 		{
// 			m_fbgResult[ch][sn].chn=ch;
// 			m_fbgResult[ch][sn].num=sn;
// 			if (m_fbgPar[ch][sn].sensorType=="�¶ȼ�"||m_fbgPar[ch][sn].sensorType=="T")
// 				m_fbgResult[ch][sn].result=m_fbgPar[ch][sn].temperature;
// 			else
// 				m_fbgResult[ch][sn].result=m_fbgPar[ch][sn].strain;
// 			m_fbgResult[ch][sn].alarmStatus=m_fbgPar[ch][sn].alarmStatus;
// 		}
// 	}
	fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),g_addressPort,ID_ipStr,0);//����5120��

}

bool CopyDBAlarm(CString &strDBPath)  //�����������ݿ�
{	
	CString curpath=sPath+ "\\HistoryAlarm.mdb";
	int ret = CopyFile(curpath,strDBPath,true);//curpath ԭ�ļ���STRDBPATH Ŀ��  True�����ǣ�FALSE���ǡ�
	if(0==ret)
	{ 
		AfxMessageBox("�������ݿ�ʧ��!");
		return  false ;
	}
	return  true;	
}

UINT SaveAlarmInfoThread(LPVOID pParam)   //���汨����Ϣ���̡߳�
{
	static int iStart=0;
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\��ʷ��¼\\������¼.mdb";
	//SaveAlarmInfoThread
	//////////////////////////////////////////////////////////////
	CString temp1=sPath+"\\��ʷ��¼\\������¼.mdb";
	CFileFind  tempFind;					 //����һ��CFileFind����������������� FbgParameter.mdb�Ƿ����.
	int	b = tempFind.FindFile(m_ParaTablePath);		//  "\\FbgParameter.mdb");	
	if(!b)
	{
		AfxMessageBox("û�� '������¼.mdb'  ���ݿ�!,����!");
		g_saveAlarmInfoFlag=false;  //�����汨����Ϣ�ˡ���Ϊû�����ݿ�
		return 0;
	}
	g_saveAlarmInfoFlag=true;
	///////////////////////////////// // //////////////////////////	
	_ConnectionPtr m_pConnection ;    //_ConnectionPtr����ָ�룬ͨ�����ڴ򿪡�//�ر�һ�������ӻ�������Execute������ִ��һ�������ؽ�����������	//(�÷���_CommandPtr�е�Execute��������)����һ�������ӡ��ȴ���һ��ʵ��	//ָ�룬����Open��һ�������ӣ���������һ��IUnknown���Զ����ӿ�ָ�롣	//	AfxOleInit();//��MFC�л����Բ�����һ�ַ�����ʵ�ֳ�ʼ��COM�����ַ���ֻ��Ҫһ����������Զ�Ϊ����ʵ�ֳ�ʼ��COM�ͽ���ʱ�ر�COM�Ĳ��������������ʾ�� 
	_RecordsetPtr  m_pRecordset;      //_RecordsetPtr����ָ�룬����ר��Ϊͨ����¼���������ݿ��������ָ�룬ͨ����	�ӿڿ��Զ����ݿ�ı��ڵļ�¼���ֶεȽ��и��ֲ���
	
	
	////////////////////////////////////////////////////////////////////////
	while(g_saveAlarmInfoFlag)
	{		
		if(WaitForSingleObject(g_saveAlarmInfoHandle,INFINITE)==WAIT_OBJECT_0) //�õ�
		{
			flag_saveALarmOk=false;
			ResetEvent(g_saveAlarmInfoHandle);
			//////////////////////////////////2015-6-19
			HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
			m_pRecordset.CreateInstance(__uuidof(Recordset));
			
			try
			{	
				m_pConnection->Open(_bstr_t(m_ConnectPath+m_ParaTablePath),"","",adModeUnknown);			
			}
			catch(_com_error e)
			{
				AfxMessageBox("���ݿ�����ʧ�ܣ�ȷ�� '������¼.mdb'  �Ƿ���ȷ  2!");  //��ETALON����ȷ��ʱ�䳤�˻ᱨ�������
				return 0;
			} 
			//m_pRecordset.CreateInstance(__uuidof(Recordset));
			///////////////////////////////////
			try{                   //�򿪱� ����ʷ��¼��,���Ƿ�ɹ�.
				CString mm="SELECT * FROM ��ʷ��¼";
				m_pRecordset->Open(_bstr_t(mm),          // ��ѯ�ļ�   ��ʷ��¼ ��	
					m_pConnection.GetInterfacePtr(),	 // ��ȡ��ӿ��IDispatchָ��			
					adOpenDynamic,
					adLockOptimistic,
					adCmdText);
			}
			catch(_com_error e)	{
				AfxMessageBox(e.ErrorMessage());
				m_pConnection->Close();	
				return 0;
	}

			/////////////////////////////////////
			
			for(int i=iStart;i<maxSaveNumberOnce;i++)
			{
				if(m_saveBuffer.alarmInformBuffer[i].SN==-1)
					break;
				//if (i>=m_saveBuffer.serialNo)//����Ҫ�������ָ�롣
				//	break;				
				try
				{
					m_pRecordset->AddNew();					
					m_pRecordset->PutCollect("��λ�����", _variant_t(m_saveBuffer.alarmInformBuffer[i].SN));		
					m_pRecordset->PutCollect("ʱ��", _variant_t(m_saveBuffer.alarmInformBuffer[i].alarmTime));
					m_pRecordset->PutCollect("����������", _variant_t(m_saveBuffer.alarmInformBuffer[i].alarmChannelAndSensor));
					m_pRecordset->PutCollect("��������", _variant_t(m_saveBuffer.alarmInformBuffer[i].alarmInformation));
					
					m_pRecordset->Update();	
				}
				catch(_com_error *e)
				{
					AfxMessageBox(e->ErrorMessage());
				}  			 
				flag_saveALarmOk=true;
				iStart++;
				if(iStart>=maxSaveNumberOnce)
					iStart=0;
				
			}//ѭ��д��
			m_pConnection->Close();	
		}		
	}
	
	/**/
	return 0;
}

CString IntToString(int alarmInfo)//�ѱ�����Ϣ�����ֱ���ַ���
{//0 ����  1 ��ʧ(�޹�դ����) 2.��ʧ(����Բ���) 3Ԥ��  4���� -1�޴�������
	switch(alarmInfo)
	{
	case NORMAL:
		return "����";
		break;
	case PREALARM:
		return "Ԥ��";
		break;
	case ALARM:
		return "����";
		break;
	case FAULT1:
		return "����������";
		break;
	case  FAULT2:
		return "�²�����";
		break;
	default:
		return "";
		break;
	}
}

void faultLight()
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->alarmLight(FAULT);
}
void faultSpeaker()
{

}

void alarmLight()
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->alarmLight(ALARM);	
}

void alarmSpeaker()
{

}

UINT SaveResultThread(LPVOID pParam)  //���沨�����߳� 
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);	
	static int count1=0,totalSave=0;//�����̼��ܹ��������
	int maxSave;

	while(1)
	{		
		if(WaitForSingleObject(g_saveResultHandle,6000) ==WAIT_OBJECT_0)
		{		
			ResetEvent(g_saveResultHandle);	
			if (m_saveBuffer.saveMiddCount==0) //���������Ϣ�����������е�����0����ô�ͼ����ȡ�
				continue;
			maxSave=m_saveBuffer.saveMiddCount;
			//m_saveBuffer.saveMiddCount=0;//��λ�������  2015-7-14����
			CString sss;
			//sss.Format("\n maxSave= %d bufCount=  %d  saveMidd= %d  saveCount= %d  %d", maxSave,m_saveBuffer.BufCount,m_saveBuffer.saveMiddCount,m_saveBuffer.saveCount, m_fbgR_A[m_saveBuffer.saveCount].head.frameNo);
			//ReportErr(&sss);

			for (int ii=0;ii<maxSave;ii++)
			{
				//sss.Format("\n ii= %d maxSave= %d bufCount=  %d  saveMidd= %d  saveCount= %d  FramNo= %d",ii, maxSave,m_saveBuffer.BufCount,m_saveBuffer.saveMiddCount,m_saveBuffer.saveCount, m_fbgR_A[m_saveBuffer.saveCount].head.frameNo);
				//ReportErr(&sss);
				if (m_fbgR_A[m_saveBuffer.saveCount].result_ch[0].frameNo ==-1 )
				{
					m_saveBuffer.saveMiddCount=0;
					break;
				}
				totalSave++;
				resultWriteToBufferChar();
				m_saveBuffer.saveCount++;
				m_saveBuffer.saveMiddCount--;   //2015-7-14

				if (m_saveBuffer.saveCount>=UdpBufSize)
					m_saveBuffer.saveCount=0;
				flag_saveResultOk=true;         //�����������					
				
				if (count1==0)//ÿ3000֡���һ�δ��̴�С��
				{
					UINT leftspace=GetLeftSpace(sPath);
					if (leftspace < 5000)  //if (leftspace<100)
					{				
						CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);				
						pDlg->m_admin.saveClick();
						return 0;//�����߳�ֹͣ�ˡ�
					}				
				}
				count1++;
				if (count1>=3000)
					count1=0;	
			}
			if (totalSave>100000)
			{		
				totalSave=0;
				pDlg->m_admin.closeCh16File();                //�رվ��ļ�
				pDlg->m_admin.creatCh16FileName();             //�������ļ���				
			}
		}		
	}
	return 0;
}

/*
UINT SaveResultThread(LPVOID pParam)  //���沨�����߳� 
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	static int count1 = 0, totalSave = 0;//�����̼��ܹ��������
	int maxSave;

	while (1)
	{
		if (WaitForSingleObject(g_saveResultHandle, 6000) == WAIT_OBJECT_0)
		{
			ResetEvent(g_saveResultHandle);
			if (m_saveBuffer.saveMiddCount == 0) //���������Ϣ�����������е�����0����ô�ͼ����ȡ�
				continue;
			maxSave = m_saveBuffer.saveMiddCount;
			//m_saveBuffer.saveMiddCount=0;//��λ�������  2015-7-14����
			CString sss;
			//sss.Format("\n maxSave= %d bufCount=  %d  saveMidd= %d  saveCount= %d  %d", maxSave,m_saveBuffer.BufCount,m_saveBuffer.saveMiddCount,m_saveBuffer.saveCount, m_fbgR_A[m_saveBuffer.saveCount].head.frameNo);
			//ReportErr(&sss);

			for (int ii = 0; ii < maxSave; ii++)
			{
				//sss.Format("\n ii= %d maxSave= %d bufCount=  %d  saveMidd= %d  saveCount= %d  FramNo= %d",ii, maxSave,m_saveBuffer.BufCount,m_saveBuffer.saveMiddCount,m_saveBuffer.saveCount, m_fbgR_A[m_saveBuffer.saveCount].head.frameNo);
				//ReportErr(&sss);
				if (m_fbgR_A[m_saveBuffer.saveCount].result_ch[0].frameNo == -1)
				{
					m_saveBuffer.saveMiddCount = 0;
					break;
				}
				totalSave++;
				resultWriteToBufferChar();
				m_saveBuffer.saveCount++;
				m_saveBuffer.saveMiddCount--;   //2015-7-14

				if (m_saveBuffer.saveCount >= UdpBufSize)
					m_saveBuffer.saveCount = 0;
				flag_saveResultOk = true;         //�����������					

				if (count1 == 0)//ÿ3000֡���һ�δ��̴�С��
				{
					UINT leftspace = GetLeftSpace(sPath);
					if (leftspace < 5000)  //if (leftspace<100)
					{
						CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
						pDlg->m_admin.saveClick();
						return 0;//�����߳�ֹͣ�ˡ�
					}
				}
				count1++;
				if (count1 >= 3000)
					count1 = 0;
			}
			if (totalSave > 100000)
			{
				totalSave = 0;
				pDlg->m_admin.closeCh16File();                //�رվ��ļ�
				pDlg->m_admin.creatCh16FileName();             //�������ļ���				
			}
		}
	}
	return 0;
}
*/

UINT GetLeftSpace(CString disk)//
{
	_ULARGE_INTEGER temp1 ;
    _ULARGE_INTEGER temp2 ;
    _ULARGE_INTEGER temp3 ;    	
	BOOL RET = GetDiskFreeSpaceEx(disk ,&temp1 ,&temp2 ,&temp3);	
	if(RET)
	{
        //UINT SpaceTotal = (UINT)(temp2.QuadPart/1024/1024) ;
		UINT SpaceFree = (UINT)(temp3.QuadPart/1024/1024) ;
		//int percent = (SpaceFree*100/SpaceTotal) ;
		return SpaceFree;
	}
	else
		return 0;
}

void resultWriteToBufferChar() //���д��д��CHAR�ͻ��� waveInfoBuffer[32][maxSaveWaveNumberBuffer]; ��ȥ
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	char line[maxCharLength+100];

	CTime	saveTime=CTime::GetCurrentTime();
	CString tempTime=saveTime.Format("%Y-%m-%d %H:%M:%S");	//��ʾ������
	char temp[maxCharLength+100],temp1[maxCharLength],temp2[30],temp3[30],temp4[30];

	//_fbgResult m_temp_1[16][20]; //��ʱ�õĽ��	
	//memcpy(&m_temp_1,&m_fbgR_A[m_saveBuffer.saveCount].fbgResult,sizeof(m_temp_1));
	_fbgResult_arary m_temp_1;//2015-7-15
	memcpy(&m_temp_1,&m_fbgR_A[m_saveBuffer.saveCount],sizeof(m_temp_1));
	m_fbgR_A[m_saveBuffer.saveCount].result_ch[0].frameNo=-1;//����־λ

	for(int ich=0;ich<16;ich++)    //�����0���������Ļ�����д��!!!!
	{  //��32��ͨ���Ĳ�����Ϣд�뵽���� waveInfoBufferChar[32][maxSaveWaveNumberBuffer],��Ϊ���ݽṹ��; 
		int channelSensorNumber=0,tempj=0;   //��ͨ���������ĸ���
		temp[0]=0;  //��ʼ����
		temp1[0]=0;	
		line[0]=0;
		sprintf(temp4,"%5d",maxSensor[ich]);
		
		for(int isensor=0;isensor<maxSensor[ich];isensor++)   //��ÿ��ͨ�������д���������Ϣ��������
		{	
			//if (m_temp_1[ich][isensor].alarmStatus!=FAULT1)							
			if (m_temp_1.result_ch[ich].fbgResult[isensor].alarmStatus!=FAULT1)							
				sprintf(temp3,"%9.1f",m_temp_1.result_ch[ich].fbgResult[isensor].result);//���
			else
				sprintf(temp3,"%9s","#");//���
			strcat(temp1,temp3);  //����ӵ�β��ȥ�������0;		
		}			
		sprintf(temp2,"%9d",m_fbgR_A[m_saveBuffer.saveCount].result_ch[ich].frameNo); //ȡ��֡�š�		
		strcat(temp,tempTime);//ʱ��
		strcat(temp,temp2);   //֡��
		strcat(temp,temp4);   //����������		
		strcat(temp,temp1);   //���
		strcat(temp,"\r\n");	//ʱ��+֡��+��ͨ����������Ŀ+���
		strcat(line,temp);//
		pDlg->m_admin.Datafile[ich].Write(line,strlen(line));
	}	
}


// UINT	CalFbgThread(LPVOID pParam)  //����FBG ���ݵ��߳�
// {
// 	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
// 	//pDlg->m_access.ReadAllFbgParameter();//�������д������Ĳ�����
// 	//pDlg->m_access.readIP();//�������д������Ĳ�����
// 	
// 	pDlg->m_paraSet.findID();//�ҵ���Թ�դ�����µ������������Ҫ�������д���䡣
// 	
// 	for (int ch=0;ch<16;ch++)        //��ʼ������״̬
// 	{
// 		for (int sn=0;sn<maxSensor[ch];sn++)
// 		{
// 			m_fbgPar[ch][sn].alarmStatus=NORMAL;
// 			m_fbgPar[ch][sn].oldAlarmStatus=NORMAL;
// 			m_fbgPar[ch][sn].showAlarmStatus=NORMAL;
// 			m_fbgPar[ch][sn].showAlarmStatusLast=NORMAL;
// 			m_fbgPar[ch][sn].conCountAlarm=0;
// 			m_fbgPar[ch][sn].conCountFault=0;
// 		}
// 	}
// 	///////////////////////////////////////
// 	while(1)
// 	{
// 		if(WaitForSingleObject(g_fbgStrainHandle,1.5*timeInterval) ==WAIT_OBJECT_0)
// 		{
// 			ResetEvent(g_fbgStrainHandle);	
// 			//memcpy(&m_fbgData,&mRecvData.mChnRes,sizeof(RESWAVELGSTRUCT)*16);//�����ݿ���m_fbgData��ȥ��
// 			memcpy(&m_fbgData,&g_UdpBuffer[mUdpSocket.CalCount].mChnRes,sizeof(RESWAVELGSTRUCT)*16);
// 			matchFbg();			//��դ��ԡ�
// 			CalcFbg();			//��դ����ļ��㡣
// 			if (pDlg->m_admin.m_autoSend)
// 				SetEvent(g_fbgResultSendHandle);//UDP�̷߳��ͽ���¼���
// 			ResponseToAlarmInfor();//��������
// 		}
// 		Sleep(sleepTime);
// 	}
// }

void g_calFFT(void)
{
	fft_Transform fft1;
	fft1.init();
	fft1.FFT_N_cal=fft_Buffer_number;//fft_Buffer_number/2;	//Ҫ����ĵ���
	fft1.baseFre=(float)fft_count_frame/(float)fft1.FFT_N_cal;
	//   f0=1/T=1/(512*dt)=1/(512*1/f)=f/512;  fft_count_frame��Ƶ��ֵ


	for (int ch=0;ch<16;ch++)	
	{
		for (int sn=0;sn<maxSensor[ch];sn++)////maxChannelSensor
		{
			if(m_fbgPar[ch][sn].sensorType=="���ٶȼ�" ||m_fbgPar[ch][sn].sensorType=="ACCE")
			{								
				if (m_fbgPar[ch][sn].fft_result.n_need>=fft1.FFT_N_cal)//fft_Buffer_number)//����ķ���������ࡣ
				{					
					fft1.init();
					for (int ii=0;ii<fft1.FFT_N_cal;ii++)//for (int ii=0;ii<fft_Buffer_number;ii++)
					{
						fft1.FFT_input[ii].real=m_fbgPar[ch][sn].fft_result.fft_buffer[ii];
					}				
					fft1.FFT_Run();
					fft1.findMaxFre();					
					memcpy(&m_fbgPar[ch][sn].fft_result.maxfre,&fft1.maxfre[0],sizeof(frequncy)*4);
					memcpy(&m_fbgResult_1.result_ch[ch].fbgResult[sn].fout[0],&fft1.maxfre[0],sizeof(frequncy)*4);		
				/*	CString sss;
					if (ch==0 && sn==1)
					{					
						sss.Format("\n111 %f %f ",fft1.maxfre[0].f,fft1.maxfre[0].amplitude);ReportErr(&sss);
						sss.Format("    222 %f %f ",m_fbgResult_1.fbgResult[ch][sn].fout[1].f,m_fbgResult_1.fbgResult[ch][sn].fout[1].amplitude);ReportErr(&sss);															
					}
				*/
				}
				else//����������������
				{
					memset(&m_fbgPar[ch][sn].fft_result.maxfre[0],0,sizeof(frequncy)*4);
					memset(&m_fbgResult_1.result_ch[ch].fbgResult[sn].fout[0],0,sizeof(frequncy)*4);
				}
				if (m_fbgResult_1.result_ch[ch].fbgResult[sn].fout[0].amplitude>=0.00015)
					m_fbgResult_1.result_ch[ch].fbgResult[sn].result=m_fbgResult_1.result_ch[ch].fbgResult[sn].fout[0].f;
				else
					m_fbgResult_1.result_ch[ch].fbgResult[sn].result=0;
			}
		}
	}
}
