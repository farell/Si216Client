// UDP1.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "UDP1.h"
#include "Glbs.h"
#include "Si216ClientDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  maxCnCal 1//5  //最大几帧计算一次
/////////////////////////////////////////////////////////////////////////////
// UDP1

UDP1::UDP1()
{
	// 	UdpCount1=0;    //下一帧要存的指针
	// 	CalCount=0;		//计算的指针
	// 	shouldCalNum=0; 
	// 	calMiddNum=0;
}

UDP1::~UDP1()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(UDP1, CSocket)
//{{AFX_MSG_MAP(UDP1)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// UDP1 member functions

// void UDP1::OnReceive(int nErrorCode) 
// {
// 	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
// 	
// 	pDlg->m_countUdpErr=0; //未收到UDP数据的计数秒数。
// 	pDlg->flag_udpReceived=true; //收到UDP数据了。
// 	static g_maxCycle=1;
// 	
// 	if (flag_udpRec)     //if (a%3==0)	
// 	{
// 		int temp11=UDP1::Receive(&mRecvData,sizeof(UDPRESWAVELGSTRUCT),0);
// 		//UDP1::Receive(&mRecvData,sizeof(UDPRESWAVELGSTRUCT),0);
// 		if( strcmp(mRecvData.St_Flag,"UDPSTART")==0 ) 
// 		{
// 			if( strcmp(mRecvData.Ed_Flag,"!UDPEND!")==0 ) 
// 			{
// 				recvOK = true;
// 				recvFrNum++;	
// 				//////////2012-9-22///加入，原因是在SI216中，如果拔下传感器，传感器数量对，但是后面的数据没有清空。继续发来。
// 				//故在接收端需要清空。2013-10-28：不用，WINDOWS好象可以以包为单位清空，不用管即可。
// 				for (int ch=0;ch<16;ch++)
// 				{
// 					for (int j=mRecvData.mChnRes[ch].num;j<64;j++)
// 					{
// 						mRecvData.mChnRes[ch].mWavelg[j]=0;
// 						mRecvData.mChnRes[ch].mPower[j]=-70;
// 					}
// 				}
// 				SetEvent(g_fbgStrainHandle);
// 				//	if (g_maxCycle<100) g_maxCycle++;				
// 			}		
// 			//udpSendPrg.SendTo(&mRecvData,sizeof(UDPRESWAVELGSTRUCT),0x7500,ID_ipStr,0);	//要改成线程发送。	
// 		}
// 		else
// 			break;//如果没有数据，需要退出，否则会很长时间滞留。	
// 	}
// 	else
// 	{
// 		//UDP1::Receive(&temp1[0],1,0);
// 		UDP1::Receive(&mRecvData,sizeof(UDPRESWAVELGSTRUCT),0);
// 	}
// 	//Sleep(1);
// 	
// 	CSocket::OnReceive(nErrorCode);
// }
//RESWAVELGSTRUCT tempResultCh;


void UDP1::OnReceive(int nErrorCode) 
{
	CSi216ClientDlg* pDlg = (CSi216ClientDlg*)(AfxGetApp()->m_pMainWnd);
	RESWAVELGSTRUCT tempResultCh;
	pDlg->m_countUdpErr=0;       //未收到UDP数据的计数秒数。
	pDlg->flag_udpReceived=true; //收到UDP数据了。
	
	int aaa=sizeof(UDPRESWAVELGSTRUCT);
	int bbb=sizeof(RESWAVELGSTRUCT);

	if (flag_udpRec)     //if (a%3==0)	
	{		
		temp11=UDP1::Receive(&mRecvData_Char,sizeof(mRecvData_Char),0);//	
		switch (temp11)
		{
		case sizeof(UDPRESWAVELGSTRUCT):
			memcpy((char*)&g_UdpBuffer[m_udpCount.UdpCount1],&mRecvData_Char,sizeof(UDPRESWAVELGSTRUCT));
			recvFrNum++;				
			g_temp=g_UdpBuffer[m_udpCount.UdpCount1].mcount-recvFrNum;
			fft_count_frame=g_UdpBuffer[m_udpCount.UdpCount1].frequncy;//频率赋值给FFT计数器，多少帧算一次FFT。大约1秒钟					
			
			m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//这是为了刷新，显示数据用的。记住有数据的一帧
			
			m_udpCount.calMiddNum++;
			m_udpCount.UdpCount1++; 
			
			if(m_udpCount.UdpCount1>=UdpBufSize)
				m_udpCount.UdpCount1=0;
			m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
			
			if(m_udpCount.calMiddNum>=800)//最多观察到773个。在PC机上。
			{
				m_udpCount.calMiddNum-=50;	//保险起见。
				m_udpCount.CalCount+=50;	//如果很多帧的话，则计算的时候丢一些。
				if (m_udpCount.CalCount>=UdpBufSize)
				{
					m_udpCount.CalCount-=UdpBufSize;
				}
			}			
			//m_udpCount.onReceving=false;
			SetEvent(g_fbgStrainHandle);
			break;
		case sizeof(RESWAVELGSTRUCT):
			memcpy((char*)&tempResultCh,&mRecvData_Char,sizeof(RESWAVELGSTRUCT));
			ch=tempResultCh.chn;//获得本次数据的通道号			
			memcpy(&g_UdpBuffer[m_udpCount.UdpCount1].mChnRes[ch],&tempResultCh,sizeof(RESWAVELGSTRUCT));
			if (ch==15)//如果最后一个通道来了，就缓存加1,有的时候在高速传输的时候，每个通道的包会超前或者拖后一帧，这样编不严格。
			{
				g_UdpBuffer[m_udpCount.UdpCount1].frequncy=tempResultCh.frequency;
				g_UdpBuffer[m_udpCount.UdpCount1].mcount=tempResultCh.mcount;

				m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//这是为了刷新，显示数据用的。记住有数据的一帧			
				m_udpCount.calMiddNum++;
				m_udpCount.UdpCount1++; 
				
				if(m_udpCount.UdpCount1>=UdpBufSize)
					m_udpCount.UdpCount1=0;
				m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
				SetEvent(g_fbgStrainHandle);
			}
			break;
		default:
			break;		
		}		
	}
	else
	{			
		UDP1::Receive(&mRecvData_Char,sizeof(mRecvData_Char),0);//		
	}
	
	CSocket::OnReceive(nErrorCode);
}

int UDP1::Receive(void* lpBuf, int nBufLen, int nFlags) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CSocket::Receive(lpBuf, nBufLen, nFlags);
}

/*
recvFrNum++;				
g_temp=mRecvData.mcount-recvFrNum;
fft_count_frame=mRecvData.frequncy;//频率赋值给FFT计数器，多少帧算一次FFT。大约1秒钟			

  memcpy(&g_UdpBuffer[m_udpCount.UdpCount1],&mRecvData,sizeof(UDPRESWAVELGSTRUCT));
  
	m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//这是为了刷新，显示数据用的。记住有数据的一帧			
	m_udpCount.calMiddNum++;
	m_udpCount.UdpCount1++; 
	
	  if(m_udpCount.UdpCount1>=UdpBufSize)
	  m_udpCount.UdpCount1=0;
	  m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
	  g_UdpBuffer[m_udpCount.UdpCount_next].mcount=-1;//作一个标志位
	  
		sss.Format("\n UdpCount1= %d   old= %d   middnum= %d   frameNo= %d   savedFrameN= %d  next= %d ",
		m_udpCount.UdpCount1,m_udpCount.UdpCount_old,m_udpCount.calMiddNum,mRecvData.mcount,g_UdpBuffer[m_udpCount.UdpCount_old].mcount,
		g_UdpBuffer[m_udpCount.UdpCount_next].mcount);ReportErr(&sss);
		
		  if (m_udpCount.calMiddNum>=800)//最多观察到773个。在PC机上。
		  {
		  m_udpCount.calMiddNum-=50;	//保险起见。
		  m_udpCount.CalCount+=50;	//如果很多帧的话，则计算的时候丢一些。
		  if (m_udpCount.CalCount>=UdpBufSize)
		  {
		  m_udpCount.CalCount-=UdpBufSize;
		  }
		  }			
		  //m_udpCount.onReceving=false;
		  SetEvent(g_fbgStrainHandle);
		  
			
			  if(temp11==sizeof(UDPRESWAVELGSTRUCT))
			  {
			  memcpy((char*)&g_UdpBuffer[m_udpCount.UdpCount1],&mRecvData_Char,sizeof(UDPRESWAVELGSTRUCT));
			  recvFrNum++;				
			  g_temp=g_UdpBuffer[m_udpCount.UdpCount1].mcount-recvFrNum;
			  fft_count_frame=g_UdpBuffer[m_udpCount.UdpCount1].frequncy;//频率赋值给FFT计数器，多少帧算一次FFT。大约1秒钟					
			  
				m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//这是为了刷新，显示数据用的。记住有数据的一帧
				
				  m_udpCount.calMiddNum++;
				  m_udpCount.UdpCount1++; 
				  
					if(m_udpCount.UdpCount1>=UdpBufSize)
					m_udpCount.UdpCount1=0;
					m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
					
					  if(m_udpCount.calMiddNum>=800)//最多观察到773个。在PC机上。
					  {
					  m_udpCount.calMiddNum-=50;	//保险起见。
					  m_udpCount.CalCount+=50;	//如果很多帧的话，则计算的时候丢一些。
					  if (m_udpCount.CalCount>=UdpBufSize)
					  {
					  m_udpCount.CalCount-=UdpBufSize;
					  }
					  }			
					  //m_udpCount.onReceving=false;
					  SetEvent(g_fbgStrainHandle);			
					  } 
					  else if (temp11==sizeof(RESWAVELGSTRUCT))//如果是广域网，则是每一通道的数据分别发过来。，如果是第16通道的话，加一。
					  {
					  memcpy((char*)&tempResultCh,&mRecvData_Char,sizeof(RESWAVELGSTRUCT));
					  ch=tempResultCh.chn;//获得本次数据的通道号
					  
						sss.Format("%d ",ch);ReportErr(&sss);
						
						  memcpy(&g_UdpBuffer[m_udpCount.UdpCount1].mChnRes[ch],&tempResultCh,sizeof(UDPRESWAVELGSTRUCT));
						  if (ch==15)//如果最后一个通道来了，就缓存加1
						  {
						  m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//这是为了刷新，显示数据用的。记住有数据的一帧			
						  m_udpCount.calMiddNum++;
						  m_udpCount.UdpCount1++; 
						  
							if(m_udpCount.UdpCount1>=UdpBufSize)
							m_udpCount.UdpCount1=0;
							m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
							sss.Format("\n");ReportErr(&sss);
							SetEvent(g_fbgStrainHandle);
							}			
							}
							else
							{
							sss.Format("error temp11= %d",temp11);ReportErr(&sss);
							}
*/