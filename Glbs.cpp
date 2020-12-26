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

#define  fft_Buffer_number   1024//512//m_fbgPar. FFT中的缓存中的个数必须大于此值，才进行计算
//#define  fft_count_frame 100  //每100帧进行一次FFT计算，如果是100HZ的话，相当于1S进行一次计算。

#define  maxSaveOnce 100

bool lan=true;//局域网。
int fft_count_frame=100;//每多少帧进行一次FFT运算,也相当于频率
static int count_fft=0;//来了多少帧。

UdpCount m_udpCount;//用来计数UDP线程中的计数器。
UDPRESWAVELGSTRUCT	g_UdpBuffer[UdpBufSize+100];
BOOL g_autoSend;
bool g_udpSendChoice;
bool g_sendFftBuffer;
int  g_udpSendFrame;//UDP发送间隔
byte g_pcID;
UINT g_addressPort;//

int g_temp;//临时变量用。

CStdioFile	errReportFile;
CString sPath;    //目录
int maxSensor[16];
struct _fbgParameter m_fbgPar[16][maxChannelSensor];

struct _fbgData m_fbgData;			   //定义FBG数据
//_fbgResult m_fbgResult[16][20]; //FBG的运算结果。

_fbgResult_arary m_fbgR_A[UdpBufSize+100];
_fbgResult_arary m_fbgResult_1;
_SensorfftBuffer m_sensorfftBuffer;

char	ID_ipStr[20]={'1','2','7','.','0','.','0','1'}; //IP地址。
CSocket	fbgResultSendPrg;	// UDP发送FBG数据 

bool isAlarmOrFault=false;//有没有故障或报警。
bool g_saveResultFlag=false;    //保存数据结果的标志
HANDLE g_saveResultHandle;   //保存结果的事件。
bool	flag_saveResultOk;        //保存波长OK了
saveBuffer m_saveBuffer;//类saveBuffer的一个实例，所有的数据都放到这个类中去。方便用。

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
	GetModuleFileName(NULL,sPathPara.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   //获取当前文件的完整目录
	sPathPara.ReleaseBuffer();   	
	int nPos=sPathPara.ReverseFind('\\');   	
	sPathPara=sPathPara.Left(nPos);				    //找当前目录
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
		fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),g_addressPort,ID_ipStr,0);//m_fbgResult_1 大小19204。
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
			if(m_fbgPar[ch][num].sensorType=="加速度计" ||m_fbgPar[ch][num].sensorType=="ACCE")
			{
				m_sensorfftBuffer.chn=ch;
				m_sensorfftBuffer.num=num;
				memcpy(&m_sensorfftBuffer.sensorID[0],&m_fbgResult_1.result_ch[ch].fbgResult[num].sensorID[0],sizeof(char)*10);
				memcpy(&m_sensorfftBuffer.buffer[0],&m_fbgPar[ch][num].fft_result.fft_buffer[0],sizeof(float)*100);				
				fbgResultSendPrg.SendTo(&m_sensorfftBuffer,sizeof(m_sensorfftBuffer),g_addressPort,ID_ipStr,0);//m_sensorfftBuffer 大小416。
			}
		}
	}
}

int isFileExist(CString fileName)  //确定目录下某个文件是否存在
{
	CFileFind  tempFind;   //声明一个CFileFind类变量，以用来搜索 FbgParameter.mdb是否存在.
	int	b = tempFind.FindFile(fileName);//  "\\FbgParameter.mdb");	
	return b;	
}

UDPRESWAVELGSTRUCT	mRecvData;
//UDPRESWAVELGSTRUCT	mRecvDataRec[maxUdpClud];

bool	recvOK;
int		recvFrNum=0;
//int     g_maxCycle=10;//接收UDP中的循环
bool	flag_readFbgOk;
UDP1	mUdpSocket;

CSocket	udpSendPrg;

int		tempa=0;
bool    flag_udpRec=true;
HANDLE  g_fbgStrainHandle;   //结果计算事件
HANDLE  g_fbgResultSendHandle;//发送FBG结果事件。
HANDLE  g_saveAlarmInfoHandle;    //保存报警信息的事件，必须的。
bool    g_saveAlarmInfoFlag=true;     //保存报警信息标志
bool	flag_saveALarmOk=true;       //保存报警信息OK了。

bool CopyDB(CString &strDBPath)  //拷贝数据库
{	
	CString curpath=sPath+ "\\HistoryAlarm.mdb";
	int ret = CopyFile(curpath,strDBPath,true);//curpath 原文件，STRDBPATH 目标  True不覆盖，FALSE覆盖。
	if(0==ret)
	{ 
		AfxMessageBox("创建数据库失败!");
		return  false ;
	}
	return  true;	
}

UINT	CalFbgThread(LPVOID pParam)  //计算FBG 数据的线程
{
	//memset(m_fbgPar,0,sizeof(_fbgParameter)*16*20);//2015-6-19主要是为了清空FFT的相关。有错误，可能跟里面有
	//CString有关系。

	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	//pDlg->m_access.ReadAllFbgParameter();//读出所有传感器的参数。
	//pDlg->m_access.readIP();//读出所有传感器的参数。
	CString sss;
	int tempNum=0;

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
			memset(&m_fbgPar[ch][sn].fft_result,0,sizeof(fft_struct));	
			m_fbgResult_1.result_ch[ch].fbgResult[sn].chn=ch;  //2015-6-24
			m_fbgResult_1.result_ch[ch].fbgResult[sn].num=sn;  //2015-6-24
		}
	}
	pDlg->m_paraSet.findID();//找到配对光栅。重新调整参数表后，需要重新运行此语句。
	///////////////////////////////////////
	while(1)
	{
		if(WaitForSingleObject(g_fbgStrainHandle,1.5*timeInterval) ==WAIT_OBJECT_0)
		{
		//	sss.Format("\n calmiddNum= %d",m_udpCount.calMiddNum);ReportErr(&sss);
			ResetEvent(g_fbgStrainHandle);
			//m_udpCount.onCalcing=true;

			tempNum=m_udpCount.calMiddNum;
			//m_udpCount.calMiddNum=0;  //清零  2015-7-14屏蔽。
			g_calFbg_new(tempNum);		

			count_fft++;              //每一帧加1，每fft_count_frame (100)帧进行一次计算
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
void g_calFbg(int num)//计算num帧的缓存中的数据
{	
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	CString sss;
	static int UdpNumber=0;//计数用的
	static int sendFftBufferCount=0;//发送FFT加速度传感器的缓存数据

	for (int i=0;i<num;i++)
	{		
		//sss.Format("\n CCCnt1= %d  Cal= %d  Midd=  %d  i=  %d",m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum,i);ReportErr(&sss);
//		sss.Format("\n i=%d ",i);ReportErr(&sss);
		memcpy(&m_fbgData,&g_UdpBuffer[m_udpCount.CalCount].mChnRes,sizeof(RESWAVELGSTRUCT)*16);
		//memset(&m_fbgResult_1,0,sizeof(_fbgResult_arary) ); //如果清0的话，传感器类型也就没了
		for (int ch=0;ch<16;ch++)
			m_fbgResult_1.result_ch[ch].frameNo=g_UdpBuffer[m_udpCount.CalCount].mcount;//取帧号

		matchFbg();			//光栅配对。
		CalcFbgBH();//用于计算BH的公式，统一一个公式。//CalcFbg();			//光栅结果的计算。	
		ResponseToAlarmInfor();    //报警处理   //计算的结果值赋到m_fbgResult_1

		//int lee1=sizeof(m_fbgResult_1);
		if (g_autoSend)   //如果UDP发送结果
		{
			if (g_udpSendChoice)   //如果抽样发送   
			{
				UdpNumber++;
				if (UdpNumber>=g_udpSendFrame)
				{
					UdpNumber=0;
					//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 大小19204。传感器类型不要超过5个汉字或者10个英文。
					sendDataToWeb();
				}
			}
			else
				sendDataToWeb();
				//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 大小19204。传感器类型不要超过5个汉字或者10个英文。
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
			
		if (g_saveResultFlag)  //如果保存
		{		
			if (pDlg->m_admin.m_sampleCheck)//如果抽样保存
			{	
				m_saveBuffer.sampleCount++; //2015-7-14
				if (m_saveBuffer.sampleCount>=pDlg->m_admin.m_saveSample)
				{
					m_saveBuffer.sampleCount=0;
					memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //将结果存到结果缓存中去				
					m_saveBuffer.saveMiddCount++; //保存个数计数器，在保存响应线程中复位。时间上每5秒发保存消息。
					m_saveBuffer.BufCount++;      //计算结果计数器
					if (m_saveBuffer.BufCount>=UdpBufSize)
						m_saveBuffer.BufCount=0;				
				}
			}
			else
			{
				memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //将结果存到结果缓存中去
				m_saveBuffer.saveMiddCount++; //保存个数计数器，在保存响应线程中复位。时间上每5秒发保存消息。
				m_saveBuffer.BufCount++;      //计算结果计数器
				if (m_saveBuffer.BufCount>=UdpBufSize)
					m_saveBuffer.BufCount=0;				
			}
			if (m_saveBuffer.saveMiddCount>=800)//如果保存的太多了.丢掉一些
			{
				m_saveBuffer.saveMiddCount-=50;
				m_saveBuffer.saveCount+=50;
				if (m_saveBuffer.saveCount>=UdpBufSize)
					m_saveBuffer.saveCount-=UdpBufSize;
			}			
		}		
		//sss.Format("\n num=%d  i= %d  NEWCnt1= %d  Cal= %d  Midd=  %d",num,i,m_udpCount.UdpCount1,m_udpCount.CalCount, m_udpCount.calMiddNum);ReportErr(&sss);
		//UDP缓存中的波长的计数+1
	
		m_udpCount.CalCount++;
		m_udpCount.calMiddNum--;	//2015-7-14  这句话有的时候不执行，会造成读的指针超过实际的指针。

        //通过记录发现在运行此函数的时候，会可能中间打断，进行UDP数据的进来，如果在上两句话中间打断
		//calMiddNum会比正常多或者少一个，原因仍不明。如果多一个，那么就会造成读的值超过当前存的
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
			*///16小时出现39次不等进来，有10次>，4次<，还有==25次之多，可能是线程的复杂运算。
			
			m_udpCount.CalCount=m_udpCount.UdpCount_old;
			m_udpCount.calMiddNum=1;
		}//加这一段是因为有的时候会现延时现象，经查是因为计算的指针超过了当前的指针。2015-7-17
	}
}

void g_calFbg_new(int num)//计算num帧的缓存中的数据
{	
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	CString sss;
	static int UdpNumber=0;//计数用的
	static int sendFftBufferCount=0;//发送FFT加速度传感器的缓存数据

	for (int i=0;i<num;i++)
	{		
		memcpy(&m_fbgData,&g_UdpBuffer[m_udpCount.CalCount].mChnRes,sizeof(RESWAVELGSTRUCT)*16);
		//memset(&m_fbgResult_1,0,sizeof(_fbgResult_arary) ); //如果清0的话，传感器类型也就没了
		if (g_UdpBuffer[m_udpCount.CalCount].mcount<0)		//标志帧号变为-1表明，已经算到下面循环处的地方了。单独加这句话没有用。原因不是这。		
		{	
			m_udpCount.calMiddNum=0;			
			return;
		}
		for (int ch=0;ch<16;ch++)
			m_fbgResult_1.result_ch[ch].frameNo=g_UdpBuffer[m_udpCount.CalCount].mcount;	//取帧号

		//sss.Format(" mcount= %d calCount= %d  mid= %d num= %d ",g_UdpBuffer[m_udpCount.CalCount].mcount,m_udpCount.CalCount,m_udpCount.calMiddNum,num );ReportErr(&sss);

		g_UdpBuffer[m_udpCount.CalCount].mcount=-1;//已经计算过的帧号作标志。
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
		matchFbg();			//光栅配对。
		CalcFbgBH();//用于计算BH的公式，统一一个公式。//CalcFbg();			//光栅结果的计算。	
		ResponseToAlarmInfor();    //报警处理   //计算的结果值赋到m_fbgResult_1
	
		if (g_autoSend)   //如果UDP发送结果
		{
			if (g_udpSendChoice)   //如果抽样发送   
			{
				UdpNumber++;
				if (UdpNumber>=g_udpSendFrame)
				{
					UdpNumber=0;
					//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 大小19204。传感器类型不要超过5个汉字或者10个英文。
					sendDataToWeb();
				}
			}
			else
				sendDataToWeb();
				//fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),0x8001,ID_ipStr,0);//m_fbgResult_1 大小19204。传感器类型不要超过5个汉字或者10个英文。
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
			
		if (g_saveResultFlag)  //如果保存
		{		
			if (pDlg->m_admin.m_sampleCheck)//如果抽样保存
			{	
				m_saveBuffer.sampleCount++; //2015-7-14
				if (m_saveBuffer.sampleCount>=pDlg->m_admin.m_saveSample)
				{
					m_saveBuffer.sampleCount=0;
					memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //将结果存到结果缓存中去				
					m_saveBuffer.saveMiddCount++; //保存个数计数器，在保存响应线程中复位。时间上每5秒发保存消息。
					m_saveBuffer.BufCount++;      //计算结果计数器
					if (m_saveBuffer.BufCount>=UdpBufSize)
						m_saveBuffer.BufCount=0;				
				}
			}
			else
			{
				memcpy(&m_fbgR_A[m_saveBuffer.BufCount],&m_fbgResult_1,sizeof(m_fbgResult_1)); //将结果存到结果缓存中去
				m_saveBuffer.saveMiddCount++; //保存个数计数器，在保存响应线程中复位。时间上每5秒发保存消息。
				m_saveBuffer.BufCount++;      //计算结果计数器
				if (m_saveBuffer.BufCount>=UdpBufSize)
					m_saveBuffer.BufCount=0;				
			}
			if (m_saveBuffer.saveMiddCount>=800)//如果保存的太多了.丢掉一些
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

	for (ch=0;ch<16;ch++)		/////先行清空
	{
		for (sn=0;sn<maxSensor[ch];sn++)//2015-6-23//for (sn=0;sn<maxChannelSensor;sn++)
		{
			m_fbgPar[ch][sn].fbgWavelength=0;
			m_fbgPar[ch][sn].fbgPower=-70;
			m_fbgPar[ch][sn].matchSuccess=false;
		}
		//////如果传感器的数量同参数表的数量一样，那么不用排序，自然序就行了。
		if (maxSensor[ch]==m_fbgData.fbg[ch].num) 
		{			
			for (sn=0;sn<maxSensor[ch];sn++)	  //将过来的数据赋值一一赋值
			{
				m_fbgPar[ch][sn].fbgWavelength=m_fbgData.fbg[ch].mWavelg[sn];
				m_fbgPar[ch][sn].fbgPower=m_fbgData.fbg[ch].mPower[sn];				
				m_fbgPar[ch][sn].matchSuccess=true;
			}
			continue;//下一个CH通道
		}
		////////   传感器来的数量同参数表数量不一样，排序后赋值
		else
		{				
			int jstart=0;	// 传感器找配对的开始序号 // j 的循环可以不必须从0开始,成功后才++,不成功不++
			for (int sn=0;sn<maxSensor[ch];sn++)
			{
				int success=0;			//设置配对成功的个数
				float m_left=m_fbgPar[ch][sn].waveLengthLeftLimited;    //参数表中左右界。
				float m_right=m_fbgPar[ch][sn].waveLengthRightLimited;  //
			
				
				for (int j=jstart;j<m_fbgData.fbg[ch].num;j++)//2015-6-24//for (int j=jstart;j<MAX_CHANNEL_PLUSE;j++)
				{
					float m_wave=m_fbgData.fbg[ch].mWavelg[j];
					float m_power=m_fbgData.fbg[ch].mPower[j];	//当前光栅的数据，
				
				
					if (m_wave >1590 || m_wave<1510) break;		//当前光栅数据不在此范围内，退出，不再寻找。
					if (m_wave<m_right && m_wave>m_left)		//当前波长在左右界之内
					{
						if (m_fbgPar[ch][sn].matchSuccess==false)   //只有没有配对的时候才进行赋值。
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

	//////////////////////////对加速度计进行缓存赋值。/////////////////
	for (ch=0;ch<16;ch++)	
	{
		for ( sn=0;sn<maxSensor[ch];sn++)
		{
			if(m_fbgPar[ch][sn].sensorType=="加速度计" ||m_fbgPar[ch][sn].sensorType=="ACCE")
			{
				memmove(&m_fbgPar[ch][sn].fft_result.fft_buffer[1],&m_fbgPar[ch][sn].fft_result.fft_buffer[0],sizeof(float)*(FFT_N-1));
				m_fbgPar[ch][sn].fft_result.fft_buffer[0]=m_fbgPar[ch][sn].fbgWavelength;
				m_fbgPar[ch][sn].fft_result.n_need++;
				if (m_fbgPar[ch][sn].fbgWavelength==0)//如果有丢失的话，将计数器赋为0
					m_fbgPar[ch][sn].fft_result.n_need=0;	
				if (m_fbgPar[ch][sn].fft_result.n_need>0xffff0000)
					m_fbgPar[ch][sn].fft_result.n_need=FFT_N;
			}
		}
	}
	////////////////////////////////////////////////////////
	
}

/*void CalcFbg()  //光栅计算
{
	for (int ch=0;ch<16;ch++)	
	{
		for (int sn=0;sn<maxSensor[ch];sn++)
		{	
			//m_fbgResult_1.fbgResult[ch][sn].chn=ch;
			//m_fbgResult_1.fbgResult[ch][sn].num=sn;
			//m_fbgResult_1.fbgResult[ch][sn].result=0;
			
			if (m_fbgPar[ch][sn].sensorType=="温度计" ||m_fbgPar[ch][sn].sensorType=="T")  //温度计			
			{
				calTemperature(ch,sn);//计算温度并看报警状态等。			
				m_fbgResult_1.fbgResult[ch][sn].result=m_fbgPar[ch][sn].temperature;
			}
			else//非温度计			
			{
				calStrain(ch,sn);//计算应变并看报警状态等。			
				m_fbgResult_1.fbgResult[ch][sn].result=m_fbgPar[ch][sn].strain;
			}
		}
	}
}
*/
void CalcFbgBH()  //波汇公式的计算
{
	for (int ch=0;ch<16;ch++)	
	{
		for (int sn=0;sn<maxSensor[ch];sn++)
		{
			calStrainBH(ch,sn);//计算应变并看报警状态等。
			//m_fbgResult_1.fbgResult[ch][sn].result=m_fbgPar[ch][sn].strain;			
		}
	}
}

void ResponseToAlarmInfor()
{
	for(int channel=0;channel<16;channel++)
	{			
		for (int i = 0; i <maxSensor[channel]; i++)    //计算温度     i代表参数表中传感器的序号
		{
			//	if(m_fbgPar[channel][i].alarmStatus!=m_fbgPar[channel][i].alarmStatusLast&&
			//	   m_fbgPar[channel][i].alarmStatus>=m_fbgPar[channel][i].showAlarmStatus)//&&i==0)//如果同上次不一样。			  
			//!!!!!不对，会一直报警，因为一会之后，可能温升就正常了，然后又温升了。这样会不断出现温升。
			//下面以总的状态为准，当总的状态发生变化之后，才输出，但是当恢复的时候没有提示。
			if(m_fbgPar[channel][i].showAlarmStatus!=m_fbgPar[channel][i].showAlarmStatusLast)//若状态发生了变化。
			{
				CString temp0,temp1,temp2,temp3,temp,temp4,tempValue;
				temp0.Format("%d",channel+1);
				temp1.Format("%d",i+1);
				temp4=IntToString(m_fbgPar[channel][i].showAlarmStatusLast)+"->"+IntToString(m_fbgPar[channel][i].showAlarmStatus);
				if(m_fbgPar[channel][i].showAlarmStatus!=FAULT1)//fault1是丢失传感器。
				{
					if (m_fbgPar[channel][i].sensorType=="温度计"||m_fbgPar[channel][i].sensorType=="T")					
						tempValue.Format("    温度=%7.1f ℃",m_fbgPar[channel][i].temperature);
					else
					{
						tempValue.Format("    数值=%7.1f",m_fbgPar[channel][i].strain);
						tempValue+=m_fbgPar[channel][i].unit;
					}
				}
				else
					tempValue="";				
				temp4+=tempValue;
				
				temp="通道"+temp0+" 传感器号"+temp1+"     "+temp4;  //+"  "+temp5;
				
				//if(1)  //2012-8-21不管屏蔽没有，都输出。
				if(m_fbgPar[channel][i].shield==false)  //若传感器没有屏蔽
				{
					CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);					
					pDlg->alarmInfo.addAlarmInform(temp);   //显示报警信息
					m_saveBuffer.alarmWriteToBuffer("通道"+temp0+"  传感器号"+temp1,temp4);				
					SetEvent(g_saveAlarmInfoHandle);		//发保存报警信息的事件。
					if (m_fbgPar[channel][i].showAlarmStatus==FAULT2||m_fbgPar[channel][i].showAlarmStatus==FAULT1)  //故障的话，亮灯。
					{
						faultLight();						
						faultSpeaker();     //只有状态变化才进来啊。如果消音了，状态不变，灯和喇叭不会再次触发
						isAlarmOrFault=true;//有报警输出了。
					}
					if (m_fbgPar[channel][i].showAlarmStatus==ALARM ||m_fbgPar[channel][i].showAlarmStatus==PREALARM)
					{
						alarmLight();
						alarmSpeaker();
						isAlarmOrFault=true;//有报警输出了。
					}
					if (isAlarmOrFault==false)
					{
						okLight();
						cancelSpeaker();
					}					
				}			
			}
			m_fbgPar[channel][i].showAlarmStatusLast=m_fbgPar[channel][i].showAlarmStatus;//本次总的状态赋给上次
			m_fbgPar[channel][i].alarmStatusLast=m_fbgPar[channel][i].alarmStatus;		  //本次状态赋给上次

			//m_fbgResult_1.fbgResult[channel][i].alarmStatus=m_fbgPar[channel][i].alarmStatus;
			m_fbgResult_1.result_ch[channel].fbgResult[i].alarmStatus=m_fbgPar[channel][i].alarmStatus;
			
		}
	}
}

int  GetShowALarmStatus(int alarmStatus,int showAlarmStatus) //alarmStatus  当前的状态   showAlarmStatus历史的状态，对外显示的状态,返回总的新状态//根据新的传感器状态和历史的传感器状态，决定返回。
{  
	switch(showAlarmStatus)
	{	
	case NORMAL:           //0 若为正常，则返回新的状态。
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
	case PREALARM:         //3  若旧状态为预警，不返回比预警低的状态。
		if (alarmStatus==ALARM)   
			return alarmStatus;
		else
			return PREALARM;
		break;
	case ALARM:           //若为报警，则一直要返回报警。
		return ALARM;
		break;
	default:
		return NORMAL;
		break;
	}
}

void okLight()                 //灯复位
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	pDlg->alarmLight(NORMAL);
}
void cancelSpeaker()           //喇叭消音
{

}

void calTemperature(int ch,int sn)//计算温度传感器。
{
	float tempW=m_fbgPar[ch][sn].fbgWavelength;  //当前配对后的波长
	if (tempW >1510 && tempW <1590)    //配对后的光栅，波长在范围内。
	{
		m_fbgPar[ch][sn].conCountFault=0;//没有丢失。进行温度计算
		if (m_fbgPar[ch][sn].temperatureCoefficnet!=0) //如果温度系数不为0，计算温度
		{					
			m_fbgPar[ch][sn].temperature=m_fbgPar[ch][sn].orignalTemperature+
				(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*1000/m_fbgPar[ch][sn].temperatureCoefficnet;
		}			
		if (  (m_fbgPar[ch][sn].temperature>=m_fbgPar[ch][sn].alarmMax && m_fbgPar[ch][sn].alarmMax!=0)
			|| m_fbgPar[ch][sn].temperature<=m_fbgPar[ch][sn].alarmMin && m_fbgPar[ch][sn].alarmMin!=0)//判断报警与否
		{
			m_fbgPar[ch][sn].conCountAlarm++;
			if (m_fbgPar[ch][sn].conCountAlarm>confirmTimeAlarm)
			{
				m_fbgPar[ch][sn].alarmStatus=ALARM;			
			}
		}
		else
		{
			m_fbgPar[ch][sn].conCountAlarm=0;  //正常
			m_fbgPar[ch][sn].alarmStatus=NORMAL;			
		}	
		m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(m_fbgPar[ch][sn].alarmStatus,m_fbgPar[ch][sn].showAlarmStatus);
	}
	else    //波长不在范围内。表示没有波长数据。
	{
		m_fbgPar[ch][sn].conCountFault++;
		m_fbgPar[ch][sn].temperature=0;
		if (m_fbgPar[ch][sn].conCountFault>confirmTimeFault)
		{
			m_fbgPar[ch][sn].temperature=NULL;
			m_fbgPar[ch][sn].alarmStatus=FAULT1;      //无光栅数据
			m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT1,m_fbgPar[ch][sn].showAlarmStatus);
		}	
	}

}

void calStrainBH(int ch, int sn) //计算应变等
{
	float w=m_fbgPar[ch][sn].fbgWavelength;  //当前配对后的波长
	int chCom=m_fbgPar[ch][sn].matchFbgCh;//温补光栅的通道号及顺序号。
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
	if (m_fbgPar[ch][sn].sensorType=="加速度计" ||m_fbgPar[ch][sn].sensorType=="ACCE" )
	{
		if (m_fbgPar[ch][sn].fbgWavelength==0)//波长丢失
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
	
	if (w>1510 && w<1590)//波长没有丢失，但是还要看温补ID的传感器波长是否也没有丢失。
	{
		m_fbgPar[ch][sn].conCountFault=0;   //传感器没有丢失。

		if (m_fbgPar[ch][sn].matchFbgCh>=0 && m_fbgPar[ch][sn].matchFbgNum>=0)	//如果有温补的光栅 -1配对号找不到，-2没有配对号
		{
			float wt=m_fbgPar[chCom][snCom].fbgWavelength;//温补光栅的波长。
			float wt0=m_fbgPar[chCom][snCom].orignalWavelengh;
			if (wt >1505 && wt <1595)		 //温补光栅没有丢失
			{
				m_fbgPar[ch][sn].conCountFault2=0; 
				//m_fbgPar[ch][sn].strain=a*(w-w0)*(w-w0)+b*(w-w0)+c-bhk*(d*(wt-wt0)*(wt-wt0)+e*(wt-wt0)+f);
				m_fbgPar[ch][sn].strain=a*((w-w0)-(wt-wt0))*((w-w0)-(wt-wt0))
					                   +b*((w-w0)-bhk*(wt-wt0))+c;
				
			}
			else      //温补光栅丢失了。
			{
				m_fbgPar[ch][sn].conCountFault2++;
				if (m_fbgPar[ch][sn].conCountFault2>confirmTimeFault)
				{
					m_fbgPar[ch][sn].alarmStatus=FAULT2;//2013-6-26 //m_fbgPar[ch][sn].alarmStatus=FAULT2;
					m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT2,m_fbgPar[ch][sn].showAlarmStatus);
					///虽然温补丢失了，但是还可以算应变等。//不可算?
					//m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
					m_fbgPar[ch][sn].strain=0;//(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
				}
			}
		}
		else if (m_fbgPar[ch][sn].matchFbgCh==-2 && m_fbgPar[ch][sn].matchFbgNum==-2)	 //没有温补光栅
		{	
			m_fbgPar[ch][sn].strain=a*(w-w0)*(w-w0)+b*(w-w0)+c;

		}
		else        //==-1 找不到号,有配对光栅，但是找不到该号，同丢失配对光栅一个意思
		{
			m_fbgPar[ch][sn].conCountFault2++;
			if (m_fbgPar[ch][sn].conCountFault2>confirmTimeFault)
			{
				m_fbgPar[ch][sn].alarmStatus=FAULT2;//2013-6-26 //m_fbgPar[ch][sn].alarmStatus=FAULT2;
				m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT2,m_fbgPar[ch][sn].showAlarmStatus);
				///虽然温补丢失了，但是还可以算应变等。//不可算?
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
		else if (m_fbgPar[ch][sn].alarmStatus!=FAULT2)//2015-6-26 找不到配对光栅
		{
			m_fbgPar[ch][sn].conCountAlarm=0;
			m_fbgPar[ch][sn].alarmStatus=NORMAL;
			m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(NORMAL,m_fbgPar[ch][sn].showAlarmStatus);
		}
	}
	else//传感器的波长丢失了。	
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

void calStrain(int ch, int sn) //计算应变等
{
	float tempW=m_fbgPar[ch][sn].fbgWavelength;  //当前配对后的波长
	int chCom=m_fbgPar[ch][sn].matchFbgCh;//温补光栅的通道号及顺序号。
	int snCom=m_fbgPar[ch][sn].matchFbgNum;

	if (tempW>1510 && tempW<1590)//波长没有丢失，但是还要看温补ID的传感器波长是否也没有丢失。
	{
		m_fbgPar[ch][sn].conCountFault=0;   //传感器没有丢失。
		
		if (m_fbgPar[ch][sn].matchFbgCh>=0 && m_fbgPar[ch][sn].matchFbgNum>=0)	//如果有温补的光栅
		{
			float tempComW=m_fbgPar[chCom][snCom].fbgWavelength;//温补光栅的波长。
			if (tempComW >1510 && tempComW <1590)		 //温补光栅没有丢失
			{
				m_fbgPar[ch][sn].conCountFault2=0; 
				m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k
					-m_fbgPar[ch][sn].tCompezation*(m_fbgPar[chCom][snCom].fbgWavelength-m_fbgPar[chCom][snCom].orignalWavelengh);
			}
			else      //温补光栅丢失了。
			{
				m_fbgPar[ch][sn].conCountFault2++;
				if (m_fbgPar[ch][sn].conCountFault2>confirmTimeFault)
				{
					m_fbgPar[ch][sn].alarmStatus=FAULT2;
					m_fbgPar[ch][sn].showAlarmStatus=GetShowALarmStatus(FAULT2,m_fbgPar[ch][sn].showAlarmStatus);
					///虽然温补丢失了，但是还可以算应变等。//要算吗?
					m_fbgPar[ch][sn].strain=(m_fbgPar[ch][sn].fbgWavelength-m_fbgPar[ch][sn].orignalWavelengh)*m_fbgPar[ch][sn].k;
				}
			}
		}
		else  //没有温补光栅
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
	else//传感器的波长丢失了。	
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
UINT	sendFbgDataOut(LPVOID pParam)  //将FBG的计算结果发送出去。不用了，直接计算完成后发出去。
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
void UdpSendFbg()  //将FBG的计算结果发送出去。
{
// 	for (int ch=0;ch<16;ch++)
// 	{
// 		for (int sn=0;sn<maxChannelSensor;sn++)
// 		{
// 			m_fbgResult[ch][sn].chn=ch;
// 			m_fbgResult[ch][sn].num=sn;
// 			if (m_fbgPar[ch][sn].sensorType=="温度计"||m_fbgPar[ch][sn].sensorType=="T")
// 				m_fbgResult[ch][sn].result=m_fbgPar[ch][sn].temperature;
// 			else
// 				m_fbgResult[ch][sn].result=m_fbgPar[ch][sn].strain;
// 			m_fbgResult[ch][sn].alarmStatus=m_fbgPar[ch][sn].alarmStatus;
// 		}
// 	}
	fbgResultSendPrg.SendTo(&m_fbgResult_1,sizeof(m_fbgResult_1),g_addressPort,ID_ipStr,0);//长度5120。

}

bool CopyDBAlarm(CString &strDBPath)  //拷贝报警数据库
{	
	CString curpath=sPath+ "\\HistoryAlarm.mdb";
	int ret = CopyFile(curpath,strDBPath,true);//curpath 原文件，STRDBPATH 目标  True不覆盖，FALSE覆盖。
	if(0==ret)
	{ 
		AfxMessageBox("创建数据库失败!");
		return  false ;
	}
	return  true;	
}

UINT SaveAlarmInfoThread(LPVOID pParam)   //保存报警信息的线程。
{
	static int iStart=0;
	CString m_ConnectPath = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
	CString m_ParaTablePath=sPath+"\\历史记录\\报警记录.mdb";
	//SaveAlarmInfoThread
	//////////////////////////////////////////////////////////////
	CString temp1=sPath+"\\历史记录\\报警记录.mdb";
	CFileFind  tempFind;					 //声明一个CFileFind类变量，以用来搜索 FbgParameter.mdb是否存在.
	int	b = tempFind.FindFile(m_ParaTablePath);		//  "\\FbgParameter.mdb");	
	if(!b)
	{
		AfxMessageBox("没有 '报警记录.mdb'  数据库!,请检查!");
		g_saveAlarmInfoFlag=false;  //不保存报警信息了。因为没有数据库
		return 0;
	}
	g_saveAlarmInfoFlag=true;
	///////////////////////////////// // //////////////////////////	
	_ConnectionPtr m_pConnection ;    //_ConnectionPtr智能指针，通常用于打开、//关闭一个库连接或用它的Execute方法来执行一个不返回结果的命令语句	//(用法和_CommandPtr中的Execute方法类似)。开一个库连接。先创建一个实例	//指针，再用Open打开一个库连接，它将返回一个IUnknown的自动化接口指针。	//	AfxOleInit();//在MFC中还可以采用另一种方法来实现初始化COM，这种方法只需要一条语句便可以自动为我们实现初始化COM和结束时关闭COM的操作，语句如下所示： 
	_RecordsetPtr  m_pRecordset;      //_RecordsetPtr智能指针，它是专门为通过记录集操作数据库而设立的指针，通过该	接口可以对数据库的表内的记录、字段等进行各种操作
	
	
	////////////////////////////////////////////////////////////////////////
	while(g_saveAlarmInfoFlag)
	{		
		if(WaitForSingleObject(g_saveAlarmInfoHandle,INFINITE)==WAIT_OBJECT_0) //得到
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
				AfxMessageBox("数据库连接失败，确认 '报警记录.mdb'  是否正确  2!");  //若ETALON不正确，时间长了会报这个警。
				return 0;
			} 
			//m_pRecordset.CreateInstance(__uuidof(Recordset));
			///////////////////////////////////
			try{                   //打开表 “历史记录”,看是否成功.
				CString mm="SELECT * FROM 历史记录";
				m_pRecordset->Open(_bstr_t(mm),          // 查询文件   历史记录 表	
					m_pConnection.GetInterfacePtr(),	 // 获取库接库的IDispatch指针			
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
				//if (i>=m_saveBuffer.serialNo)//超过要保存类的指针。
				//	break;				
				try
				{
					m_pRecordset->AddNew();					
					m_pRecordset->PutCollect("复位后序号", _variant_t(m_saveBuffer.alarmInformBuffer[i].SN));		
					m_pRecordset->PutCollect("时间", _variant_t(m_saveBuffer.alarmInformBuffer[i].alarmTime));
					m_pRecordset->PutCollect("报警传感器", _variant_t(m_saveBuffer.alarmInformBuffer[i].alarmChannelAndSensor));
					m_pRecordset->PutCollect("报警内容", _variant_t(m_saveBuffer.alarmInformBuffer[i].alarmInformation));
					
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
				
			}//循环写入
			m_pConnection->Close();	
		}		
	}
	
	/**/
	return 0;
}

CString IntToString(int alarmInfo)//把报警信息的数字变成字符串
{//0 正常  1 丢失(无光栅数据) 2.丢失(无配对波长) 3预警  4报警 -1无传感器。
	switch(alarmInfo)
	{
	case NORMAL:
		return "正常";
		break;
	case PREALARM:
		return "预警";
		break;
	case ALARM:
		return "报警";
		break;
	case FAULT1:
		return "传感器故障";
		break;
	case  FAULT2:
		return "温补故障";
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

UINT SaveResultThread(LPVOID pParam)  //保存波长的线程 
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);	
	static int count1=0,totalSave=0;//检查磁盘及总共保存次数
	int maxSave;

	while(1)
	{		
		if(WaitForSingleObject(g_saveResultHandle,6000) ==WAIT_OBJECT_0)
		{		
			ResetEvent(g_saveResultHandle);	
			if (m_saveBuffer.saveMiddCount==0) //如果来的消息，而计数器中的数是0，那么就继续等。
				continue;
			maxSave=m_saveBuffer.saveMiddCount;
			//m_saveBuffer.saveMiddCount=0;//复位保存计数  2015-7-14屏蔽
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
				flag_saveResultOk=true;         //波长保存完成					
				
				if (count1==0)//每3000帧检查一次磁盘大小。
				{
					UINT leftspace=GetLeftSpace(sPath);
					if (leftspace < 5000)  //if (leftspace<100)
					{				
						CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);				
						pDlg->m_admin.saveClick();
						return 0;//保存线程停止了。
					}				
				}
				count1++;
				if (count1>=3000)
					count1=0;	
			}
			if (totalSave>100000)
			{		
				totalSave=0;
				pDlg->m_admin.closeCh16File();                //关闭旧文件
				pDlg->m_admin.creatCh16FileName();             //建立新文件。				
			}
		}		
	}
	return 0;
}

/*
UINT SaveResultThread(LPVOID pParam)  //保存波长的线程 
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	static int count1 = 0, totalSave = 0;//检查磁盘及总共保存次数
	int maxSave;

	while (1)
	{
		if (WaitForSingleObject(g_saveResultHandle, 6000) == WAIT_OBJECT_0)
		{
			ResetEvent(g_saveResultHandle);
			if (m_saveBuffer.saveMiddCount == 0) //如果来的消息，而计数器中的数是0，那么就继续等。
				continue;
			maxSave = m_saveBuffer.saveMiddCount;
			//m_saveBuffer.saveMiddCount=0;//复位保存计数  2015-7-14屏蔽
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
				flag_saveResultOk = true;         //波长保存完成					

				if (count1 == 0)//每3000帧检查一次磁盘大小。
				{
					UINT leftspace = GetLeftSpace(sPath);
					if (leftspace < 5000)  //if (leftspace<100)
					{
						CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
						pDlg->m_admin.saveClick();
						return 0;//保存线程停止了。
					}
				}
				count1++;
				if (count1 >= 3000)
					count1 = 0;
			}
			if (totalSave > 100000)
			{
				totalSave = 0;
				pDlg->m_admin.closeCh16File();                //关闭旧文件
				pDlg->m_admin.creatCh16FileName();             //建立新文件。				
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

void resultWriteToBufferChar() //结果写入写入CHAR型缓存 waveInfoBuffer[32][maxSaveWaveNumberBuffer]; 中去
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	char line[maxCharLength+100];

	CTime	saveTime=CTime::GetCurrentTime();
	CString tempTime=saveTime.Format("%Y-%m-%d %H:%M:%S");	//显示年月日
	char temp[maxCharLength+100],temp1[maxCharLength],temp2[30],temp3[30],temp4[30];

	//_fbgResult m_temp_1[16][20]; //临时用的结果	
	//memcpy(&m_temp_1,&m_fbgR_A[m_saveBuffer.saveCount].fbgResult,sizeof(m_temp_1));
	_fbgResult_arary m_temp_1;//2015-7-15
	memcpy(&m_temp_1,&m_fbgR_A[m_saveBuffer.saveCount],sizeof(m_temp_1));
	m_fbgR_A[m_saveBuffer.saveCount].result_ch[0].frameNo=-1;//作标志位

	for(int ich=0;ich<16;ich++)    //如果是0个传感器的话，不写入!!!!
	{  //将32个通道的波长信息写入到缓存 waveInfoBufferChar[32][maxSaveWaveNumberBuffer],此为数据结构。; 
		int channelSensorNumber=0,tempj=0;   //本通道传感器的个数
		temp[0]=0;  //初始化。
		temp1[0]=0;	
		line[0]=0;
		sprintf(temp4,"%5d",maxSensor[ich]);
		
		for(int isensor=0;isensor<maxSensor[ich];isensor++)   //将每个通道的所有传感器的信息加起来。
		{	
			//if (m_temp_1[ich][isensor].alarmStatus!=FAULT1)							
			if (m_temp_1.result_ch[ich].fbgResult[isensor].alarmStatus!=FAULT1)							
				sprintf(temp3,"%9.1f",m_temp_1.result_ch[ich].fbgResult[isensor].result);//结果
			else
				sprintf(temp3,"%9s","#");//结果
			strcat(temp1,temp3);  //结果加到尾上去，并添加0;		
		}			
		sprintf(temp2,"%9d",m_fbgR_A[m_saveBuffer.saveCount].result_ch[ich].frameNo); //取得帧号。		
		strcat(temp,tempTime);//时间
		strcat(temp,temp2);   //帧号
		strcat(temp,temp4);   //传感器数量		
		strcat(temp,temp1);   //结果
		strcat(temp,"\r\n");	//时间+帧号+本通道传感器数目+结果
		strcat(line,temp);//
		pDlg->m_admin.Datafile[ich].Write(line,strlen(line));
	}	
}


// UINT	CalFbgThread(LPVOID pParam)  //计算FBG 数据的线程
// {
// 	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
// 	//pDlg->m_access.ReadAllFbgParameter();//读出所有传感器的参数。
// 	//pDlg->m_access.readIP();//读出所有传感器的参数。
// 	
// 	pDlg->m_paraSet.findID();//找到配对光栅。重新调整参数表后，需要重新运行此语句。
// 	
// 	for (int ch=0;ch<16;ch++)        //初始化报警状态
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
// 			//memcpy(&m_fbgData,&mRecvData.mChnRes,sizeof(RESWAVELGSTRUCT)*16);//将数据拷到m_fbgData中去。
// 			memcpy(&m_fbgData,&g_UdpBuffer[mUdpSocket.CalCount].mChnRes,sizeof(RESWAVELGSTRUCT)*16);
// 			matchFbg();			//光栅配对。
// 			CalcFbg();			//光栅结果的计算。
// 			if (pDlg->m_admin.m_autoSend)
// 				SetEvent(g_fbgResultSendHandle);//UDP线程发送结果事件。
// 			ResponseToAlarmInfor();//报警处理
// 		}
// 		Sleep(sleepTime);
// 	}
// }

void g_calFFT(void)
{
	fft_Transform fft1;
	fft1.init();
	fft1.FFT_N_cal=fft_Buffer_number;//fft_Buffer_number/2;	//要计算的点数
	fft1.baseFre=(float)fft_count_frame/(float)fft1.FFT_N_cal;
	//   f0=1/T=1/(512*dt)=1/(512*1/f)=f/512;  fft_count_frame是频率值


	for (int ch=0;ch<16;ch++)	
	{
		for (int sn=0;sn<maxSensor[ch];sn++)////maxChannelSensor
		{
			if(m_fbgPar[ch][sn].sensorType=="加速度计" ||m_fbgPar[ch][sn].sensorType=="ACCE")
			{								
				if (m_fbgPar[ch][sn].fft_result.n_need>=fft1.FFT_N_cal)//fft_Buffer_number)//缓存的非零个数够多。
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
				else//不够缓冲数则，清零
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
