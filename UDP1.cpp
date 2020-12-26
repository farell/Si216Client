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

#define  maxCnCal 1//5  //���֡����һ��
/////////////////////////////////////////////////////////////////////////////
// UDP1

UDP1::UDP1()
{
	// 	UdpCount1=0;    //��һ֡Ҫ���ָ��
	// 	CalCount=0;		//�����ָ��
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
// 	pDlg->m_countUdpErr=0; //δ�յ�UDP���ݵļ���������
// 	pDlg->flag_udpReceived=true; //�յ�UDP�����ˡ�
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
// 				//////////2012-9-22///���룬ԭ������SI216�У�������´������������������ԣ����Ǻ��������û����ա�����������
// 				//���ڽ��ն���Ҫ��ա�2013-10-28�����ã�WINDOWS��������԰�Ϊ��λ��գ����ùܼ��ɡ�
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
// 			//udpSendPrg.SendTo(&mRecvData,sizeof(UDPRESWAVELGSTRUCT),0x7500,ID_ipStr,0);	//Ҫ�ĳ��̷߳��͡�	
// 		}
// 		else
// 			break;//���û�����ݣ���Ҫ�˳��������ܳ�ʱ��������	
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
	pDlg->m_countUdpErr=0;       //δ�յ�UDP���ݵļ���������
	pDlg->flag_udpReceived=true; //�յ�UDP�����ˡ�
	
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
			fft_count_frame=g_UdpBuffer[m_udpCount.UdpCount1].frequncy;//Ƶ�ʸ�ֵ��FFT������������֡��һ��FFT����Լ1����					
			
			m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//����Ϊ��ˢ�£���ʾ�����õġ���ס�����ݵ�һ֡
			
			m_udpCount.calMiddNum++;
			m_udpCount.UdpCount1++; 
			
			if(m_udpCount.UdpCount1>=UdpBufSize)
				m_udpCount.UdpCount1=0;
			m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
			
			if(m_udpCount.calMiddNum>=800)//���۲쵽773������PC���ϡ�
			{
				m_udpCount.calMiddNum-=50;	//���������
				m_udpCount.CalCount+=50;	//����ܶ�֡�Ļ���������ʱ��һЩ��
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
			ch=tempResultCh.chn;//��ñ������ݵ�ͨ����			
			memcpy(&g_UdpBuffer[m_udpCount.UdpCount1].mChnRes[ch],&tempResultCh,sizeof(RESWAVELGSTRUCT));
			if (ch==15)//������һ��ͨ�����ˣ��ͻ����1,�е�ʱ���ڸ��ٴ����ʱ��ÿ��ͨ���İ��ᳬǰ�����Ϻ�һ֡�������಻�ϸ�
			{
				g_UdpBuffer[m_udpCount.UdpCount1].frequncy=tempResultCh.frequency;
				g_UdpBuffer[m_udpCount.UdpCount1].mcount=tempResultCh.mcount;

				m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//����Ϊ��ˢ�£���ʾ�����õġ���ס�����ݵ�һ֡			
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
fft_count_frame=mRecvData.frequncy;//Ƶ�ʸ�ֵ��FFT������������֡��һ��FFT����Լ1����			

  memcpy(&g_UdpBuffer[m_udpCount.UdpCount1],&mRecvData,sizeof(UDPRESWAVELGSTRUCT));
  
	m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//����Ϊ��ˢ�£���ʾ�����õġ���ס�����ݵ�һ֡			
	m_udpCount.calMiddNum++;
	m_udpCount.UdpCount1++; 
	
	  if(m_udpCount.UdpCount1>=UdpBufSize)
	  m_udpCount.UdpCount1=0;
	  m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
	  g_UdpBuffer[m_udpCount.UdpCount_next].mcount=-1;//��һ����־λ
	  
		sss.Format("\n UdpCount1= %d   old= %d   middnum= %d   frameNo= %d   savedFrameN= %d  next= %d ",
		m_udpCount.UdpCount1,m_udpCount.UdpCount_old,m_udpCount.calMiddNum,mRecvData.mcount,g_UdpBuffer[m_udpCount.UdpCount_old].mcount,
		g_UdpBuffer[m_udpCount.UdpCount_next].mcount);ReportErr(&sss);
		
		  if (m_udpCount.calMiddNum>=800)//���۲쵽773������PC���ϡ�
		  {
		  m_udpCount.calMiddNum-=50;	//���������
		  m_udpCount.CalCount+=50;	//����ܶ�֡�Ļ���������ʱ��һЩ��
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
			  fft_count_frame=g_UdpBuffer[m_udpCount.UdpCount1].frequncy;//Ƶ�ʸ�ֵ��FFT������������֡��һ��FFT����Լ1����					
			  
				m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//����Ϊ��ˢ�£���ʾ�����õġ���ס�����ݵ�һ֡
				
				  m_udpCount.calMiddNum++;
				  m_udpCount.UdpCount1++; 
				  
					if(m_udpCount.UdpCount1>=UdpBufSize)
					m_udpCount.UdpCount1=0;
					m_udpCount.UdpCount_next=m_udpCount.UdpCount1;
					
					  if(m_udpCount.calMiddNum>=800)//���۲쵽773������PC���ϡ�
					  {
					  m_udpCount.calMiddNum-=50;	//���������
					  m_udpCount.CalCount+=50;	//����ܶ�֡�Ļ���������ʱ��һЩ��
					  if (m_udpCount.CalCount>=UdpBufSize)
					  {
					  m_udpCount.CalCount-=UdpBufSize;
					  }
					  }			
					  //m_udpCount.onReceving=false;
					  SetEvent(g_fbgStrainHandle);			
					  } 
					  else if (temp11==sizeof(RESWAVELGSTRUCT))//����ǹ�����������ÿһͨ�������ݷֱ𷢹�����������ǵ�16ͨ���Ļ�����һ��
					  {
					  memcpy((char*)&tempResultCh,&mRecvData_Char,sizeof(RESWAVELGSTRUCT));
					  ch=tempResultCh.chn;//��ñ������ݵ�ͨ����
					  
						sss.Format("%d ",ch);ReportErr(&sss);
						
						  memcpy(&g_UdpBuffer[m_udpCount.UdpCount1].mChnRes[ch],&tempResultCh,sizeof(UDPRESWAVELGSTRUCT));
						  if (ch==15)//������һ��ͨ�����ˣ��ͻ����1
						  {
						  m_udpCount.UdpCount_old=m_udpCount.UdpCount1;//����Ϊ��ˢ�£���ʾ�����õġ���ס�����ݵ�һ֡			
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