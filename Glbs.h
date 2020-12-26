// Glbs.h: interface for the Glbs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLBS_H__2EDDD2E8_03E6_48FC_8496_B29C36636016__INCLUDED_)
#define AFX_GLBS_H__2EDDD2E8_03E6_48FC_8496_B29C36636016__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UDP1.h"
#include "saveBuffer.h"
#include "UdpCount.h"
#include "fft_Transform.h"



#define	MAX_CHANNEL_PLUSE	64//64
#define backGround 0
#define blue   1
#define yellow 2

#define GREEN	    0    //����
#define YELLOW1		1    //��ʧ���޹�դ����
#define YELLOW2     2    //��ʧ����Թ�դ��ʧ
#define PRERED      3    //Ԥ��
#define RED			4    //����

#define sleepTime   2   //��Ϣʱ��
//0 ����  1 ��ʧ(�޹�դ����) 2.��ʧ(����Բ���) 3Ԥ��  4���� -1�޴�������
#define NORMAL 0
#define FAULT  1
#define FAULT1 1  //��ʧ���޹�դ����
#define FAULT2 2  //��ʧ����Թ�դ��ʧ
#define PREALARM 3
#define ALARM  4
#define maxUdpClud 10//�����յ�UDP��

#define confirmTimeAlarm 3  //��󱨾�����ȷ�ϡ�
#define confirmTimeFault 3  //��󱨾�����ȷ�ϡ�

#define  maxChannelSensor 20   //ÿͨ����ഫ��������Ϊ20����
#define  timeInterval   1000   //���β���������ʱ������
#define maxSaveWavelengthFile	 100000   //ÿ���ļ���󱣴����֡����
#define maxCharLength 1000       //���壬CHAR�ͱ���������ȡ�40*20+200
#define UdpBufSize 2500  //UDP�����Ļ���Ĵ�С
//#define maxSaveWaveNumberBuffer 100      //����ͨ����ÿͨ����������������Ŀ��
//#define maxSaveNumberOnce 200      // ACCESS�Ļ����С��һ�����洢��౨�������ݡ�
// #define maxSaveWaveNumberBuffer 100      //����ͨ����ÿͨ����������������Ŀ��
// #define maxSaveNumberOnce 200      // ACCESS�Ļ����С��һ�����洢��౨�������ݡ�
extern bool lan; //������
extern int fft_count_frame;//ÿ����֡����һ��FFT���㡣
extern UdpCount m_udpCount;
extern int g_temp;//��ʱ�����á�
extern CStdioFile	errReportFile;
extern CString sPath;    //Ŀ¼
extern int maxSensor[16]; //����ÿ��ͨ���еĴ������ĸ���.�Ӳ�����CHXX�ж�ȡ,���ݲ������в���������ȷ��.

extern BOOL	ReportErr(CString *pString);
extern CString getWorkingPath();
extern int isFileExist(CString fileName);
extern void sendDataToWeb();//�������ݷ���
extern void sendFftBufferToWeb();//�����ٶȼƵĻ��淢��ȥ��

extern UINT	CalFbgThread(LPVOID pParam);		  //����FBG ���ݵ��߳�
extern void g_calFbg(int num);//����num֡�Ļ����е�����
extern void g_calFbg_new(int num);//����num֡�Ļ����е�����,�µĳ���
extern char	ID_ipStr[20]; //IP��ַ��

extern	CSocket	fbgResultSendPrg;	// UDP����FBG���� 

extern bool g_saveAlarmInfoFlag;    //���汨����Ϣ��־
extern bool	flag_saveALarmOk;       //���汨����ϢOK��


extern CString IntToString(int alarmInfo);   //�ѱ�����Ϣ�����ֱ���ַ���
extern bool isAlarmOrFault;         //��û�й��ϻ򱨾���
extern bool g_saveResultFlag;       //�������ݽ���ı�־
extern HANDLE g_saveResultHandle;   //���������¼���
extern bool	flag_saveResultOk;        //���沨��OK��

extern UINT GetLeftSpace(CString disk);//�õ�����ʣ��ռ�//����MB

typedef	struct _RESWAVELGSTRUCT
{
	int		chn;//ͨ������
	int		num;//��ͨ������������
	long	mcount;  //֡��   2018-2-23�¼ӵģ�
	int     frequency;//Ƶ�� 2018-2-23�¼ӵģ�
	float	mWavelg[MAX_CHANNEL_PLUSE];//64	
	float	mPower[MAX_CHANNEL_PLUSE];//64
}RESWAVELGSTRUCT,*PRESWAVELGSTRUCT;


typedef struct _fbgResult  //��դ�Ľ�����ݡ�
{	
	char chn;		   //ͨ����  1
	char num;		   //˳���	 1
	char sensorID[10];//������ID
	char sensorType[10];//����������
	char unit[2];//�����λ
	float result;      
	float wavelength;//�������Ĳ���
	float fbgPower;//������������
	char alarmStatus;  //	
	frequncy fout[4];     //Ƶ�ʵĽ�� 
}fbgResult,*PfbgResult;

struct _fbgResult_channel
{
	long frameNo;//֡��
	byte pcID; //����ID��
	_fbgResult fbgResult[20];
};
//extern _fbgResult_channel m_fbgResult;
struct _SensorfftBuffer
{
	byte pcID;//����ID
	char chn;//������ͨ��
	char num;//���������
	char sensorID[10];//������ID��
	float buffer[100];//����100������
};

struct _fbgResult_arary  //��դ�Ľ�����ݡ�
{
	_fbgResult_channel result_ch[16];
};
extern _fbgResult_arary m_fbgR_A[UdpBufSize+100];
extern _fbgResult_arary m_fbgResult_1;
extern _SensorfftBuffer m_sensorfftBuffer;

struct _fbgData
{
	RESWAVELGSTRUCT		fbg[16];
};
extern struct _fbgData m_fbgData;    //����FBG���ݡ�

typedef	struct _UDPRESWAVELGSTRUCT
{
	long				mcount;
	int					frequncy;
	RESWAVELGSTRUCT		mChnRes[16];
}UDPRESWAVELGSTRUCT,*PUDPRESWAVELGSTRUCT;
extern BOOL g_autoSend; //UDP����FBG�����
extern bool g_udpSendChoice;//UDP�Ƿ�������.
extern bool g_sendFftBuffer;//���ͼ��ٶȼƵĻ�������
extern int  g_udpSendFrame; //UDP���ͼ��.
extern byte g_pcID;//����ID��
extern UINT g_addressPort;//


struct fft_struct
{
	float fft_buffer[FFT_N];
	frequncy maxfre[4];
	DWORD n_need;//��FFT����ĸ����ﵽһ�������ſ��Խ������㡣
};  
struct BH
{
	double a;
	double b;
	double c;
	double k;
	double d;
	double e;
	double f;
};
struct _fbgParameter   //�������Ĳ���//���ڷ���16��ͨ����,ÿ��ͨ�����20��������.
{	
	int     sn;                     //�����ݿ��е��Զ���š�
	char    no;                     //��������� 0��1��2��3��
	CString ID	;    				//������ID��
	CString sensorType;             //����������
	float   orignalWavelengh;		//�궨����
	float   orignalTemperature;		//�궨�¶�
	float   temperatureCoefficnet;	//�¶�ϵ��  a
	float   alarmMin;				//��СԤ��
	float   alarmMax;				//����¶�	
	CString position;				//����λ��
	bool    shield;	                //���η�?
	char    parameterStatus;    	//0 �޲��� 1 �в���
	CString matchFbgID;             //��Ե��¶ȴ�����  ID
	char    matchFbgNum;            //������Թ�դID�ҵ����Ӧ�Ĵ���������CH��NUM�Ⱥ��룬�����ڼ��㡣
	char    matchFbgCh;             //��Դ�������ͨ���š�
	
	float k;                        //Ӧ��ϵ��
	float tCompezation;             //�²�ϵ��  ���൱��B
	CString unit;                   //��������λ
	char    paraIsFull;             //=0 ������ȫ�� !=0 ,������ȫ
	CString note;
	////////////////////////////////////////////////////////
	float waveLengthLeftLimited;	//��߽�,
	float waveLengthRightLimited;	//�ұ߽�.
	///////////////////  ������   ////////////////////////////
	bool  matchSuccess;             //��դƥ�䣨���ݹ�դ����ͬ������һһ��Ӧ���Ƿ�ɹ�.
	float fbgWavelength;            //�������еõ���FBG��������,����Ժ�,�ſ��Ը�ֵ.
	float fbgPower;                 //������������ֵ.
	float temperature;				//�������õ��� FBG �¶�����
	float strain;                   //�������õ��� FBG Ӧ����������� 	
	
	int conCountFault;              //���ڴ洢������֤�����Ĵ�������������֮���ȷ������
	int conCountFault2;				//�²���դ��ʧ����
	int conCountAlarm;              //����������֤��	
	
	int  alarmStatus;		  //0 ����  1 ��ʧ(�޹�դ����) 2.��ʧ(����Բ���) 3Ԥ��  4����  -1�޴˲���
	int  alarmStatusLast;	     
	int  oldAlarmStatus;      //��һ�εı���״̬��
	int  showAlarmStatus;     ////������ʾ���ܵı���״̬.���ϴθ�λ����
	int	 showAlarmStatusLast; //�ϴζ�����ʾ���ܵı���״̬��
	BH   bh;  //����Ҫ�����Ĳ���
	fft_struct fft_result;

};
extern struct _fbgParameter m_fbgPar[16][maxChannelSensor];

extern UDPRESWAVELGSTRUCT  mRecvData;	 
extern UDPRESWAVELGSTRUCT	g_UdpBuffer[UdpBufSize+100];  
extern	bool   recvOK;
extern	int	   recvFrNum;
//extern  int     g_maxCycle;
extern bool    flag_readFbgOk;
extern UDP1	   mUdpSocket;
extern saveBuffer m_saveBuffer;     //��saveBuffer��һ��ʵ�������е����ݶ��ŵ��������ȥ�������á�

extern	CSocket   udpSendPrg;
extern int	      tempa;
extern bool       flag_udpRec;


extern bool CopyDB(CString &strDBPath);  //�������ݿ�
extern void matchFbg();//��դ��ԡ�
extern void CalcFbg();//��դ����
extern void CalcFbgBH();//�����դ����
extern void calTemperature(int ch,int sn);//�����¶ȴ�������
extern void calStrain(int ch,int sn);//����Ӧ��
extern void calStrainBH(int ch,int sn);//���㲨��Ӧ��

extern HANDLE g_fbgStrainHandle;   //FBG��������¼�
extern HANDLE g_fbgResultSendHandle;//����FBG����¼���
extern HANDLE g_saveAlarmInfoHandle;    //���汨����Ϣ���¼���


extern void ResponseToAlarmInfor();//������
extern int  GetShowALarmStatus(int alarmStatus,int showAlarmStatus); //alarmStatus  ��ǰ��״̬  
// showAlarmStatus��ʷ��״̬��������ʾ��״̬,�����ܵ���״̬//�����µĴ�����״̬����ʷ�Ĵ�����״̬���������ء�


extern void okLight();           //�Ƹ�λ
extern void cancelSpeaker();     //��������

extern void faultLight();  
extern void faultSpeaker();   

extern void alarmLight();
extern void alarmSpeaker();
extern UINT	sendFbgDataOut(LPVOID pParam);  //��FBG�ļ��������ͳ�ȥ��

extern bool CopyDBAlarm(CString &strDBPath);	//�����������ݿ�


extern UINT SaveAlarmInfoThread(LPVOID pParam); //���汨����Ϣ���̡߳�
extern UINT SaveResultThread(LPVOID pParam);    //���������߳� 
extern void resultWriteToBufferChar( );   //���д��д��CHAR�ͻ��� waveInfoBuffer[32][maxSaveWaveNumberBuffer]; ��ȥ
extern void UdpSendFbg( );  //��FBG�ļ��������ͳ�ȥ��
extern void g_calFFT(void);//����FFT
class Glbs               
{
	//private CSocket udpSocket;
	public:
		Glbs();
		virtual ~Glbs();
		CSocket *udpSocket;
};

#endif // !defined(AFX_GLBS_H__2EDDD2E8_03E6_48FC_8496_B29C36636016__INCLUDED_)

