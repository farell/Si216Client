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

#define GREEN	    0    //正常
#define YELLOW1		1    //丢失，无光栅数据
#define YELLOW2     2    //丢失，配对光栅丢失
#define PRERED      3    //预警
#define RED			4    //报警

#define sleepTime   2   //休息时间
//0 正常  1 丢失(无光栅数据) 2.丢失(无配对波长) 3预警  4报警 -1无传感器。
#define NORMAL 0
#define FAULT  1
#define FAULT1 1  //丢失，无光栅数据
#define FAULT2 2  //丢失，配对光栅丢失
#define PREALARM 3
#define ALARM  4
#define maxUdpClud 10//最大接收的UDP包

#define confirmTimeAlarm 3  //最大报警次数确认。
#define confirmTimeFault 3  //最大报警次数确认。

#define  maxChannelSensor 20   //每通道最多传感器数量为20个。
#define  timeInterval   1000   //二次波长进来的时间间隔。
#define maxSaveWavelengthFile	 100000   //每个文件最大保存多少帧数据
#define maxCharLength 1000       //定义，CHAR型变量的最长长度。40*20+200
#define UdpBufSize 2500  //UDP进来的缓存的大小
//#define maxSaveWaveNumberBuffer 100      //波长通道，每通道波长缓存的最大数目。
//#define maxSaveNumberOnce 200      // ACCESS的缓冲大小。一次最多存储最多报警的内容。
// #define maxSaveWaveNumberBuffer 100      //波长通道，每通道波长缓存的最大数目。
// #define maxSaveNumberOnce 200      // ACCESS的缓冲大小。一次最多存储最多报警的内容。
extern bool lan; //局域网
extern int fft_count_frame;//每多少帧进行一次FFT运算。
extern UdpCount m_udpCount;
extern int g_temp;//临时变量用。
extern CStdioFile	errReportFile;
extern CString sPath;    //目录
extern int maxSensor[16]; //定义每个通道中的传感器的个数.从参数表CHXX中读取,根据参数表中参数个数来确定.

extern BOOL	ReportErr(CString *pString);
extern CString getWorkingPath();
extern int isFileExist(CString fileName);
extern void sendDataToWeb();//网络数据发送
extern void sendFftBufferToWeb();//将加速度计的缓存发出去。

extern UINT	CalFbgThread(LPVOID pParam);		  //计算FBG 数据的线程
extern void g_calFbg(int num);//计算num帧的缓存中的数据
extern void g_calFbg_new(int num);//计算num帧的缓存中的数据,新的程序
extern char	ID_ipStr[20]; //IP地址。

extern	CSocket	fbgResultSendPrg;	// UDP发送FBG数据 

extern bool g_saveAlarmInfoFlag;    //保存报警信息标志
extern bool	flag_saveALarmOk;       //保存报警信息OK了


extern CString IntToString(int alarmInfo);   //把报警信息的数字变成字符串
extern bool isAlarmOrFault;         //有没有故障或报警。
extern bool g_saveResultFlag;       //保存数据结果的标志
extern HANDLE g_saveResultHandle;   //保存结果的事件。
extern bool	flag_saveResultOk;        //保存波长OK了

extern UINT GetLeftSpace(CString disk);//得到磁盘剩余空间//返回MB

typedef	struct _RESWAVELGSTRUCT
{
	int		chn;//通道数，
	int		num;//本通道几个传感器
	long	mcount;  //帧号   2018-2-23新加的，
	int     frequency;//频率 2018-2-23新加的，
	float	mWavelg[MAX_CHANNEL_PLUSE];//64	
	float	mPower[MAX_CHANNEL_PLUSE];//64
}RESWAVELGSTRUCT,*PRESWAVELGSTRUCT;


typedef struct _fbgResult  //光栅的结果数据。
{	
	char chn;		   //通道号  1
	char num;		   //顺序号	 1
	char sensorID[10];//传感器ID
	char sensorType[10];//传感器类型
	char unit[2];//结果单位
	float result;      
	float wavelength;//传感器的波长
	float fbgPower;//传感器的能量
	char alarmStatus;  //	
	frequncy fout[4];     //频率的结果 
}fbgResult,*PfbgResult;

struct _fbgResult_channel
{
	long frameNo;//帧号
	byte pcID; //本机ID号
	_fbgResult fbgResult[20];
};
//extern _fbgResult_channel m_fbgResult;
struct _SensorfftBuffer
{
	byte pcID;//本机ID
	char chn;//传感器通道
	char num;//传感器序号
	char sensorID[10];//传感器ID号
	float buffer[100];//最新100个数据
};

struct _fbgResult_arary  //光栅的结果数据。
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
extern struct _fbgData m_fbgData;    //定义FBG数据。

typedef	struct _UDPRESWAVELGSTRUCT
{
	long				mcount;
	int					frequncy;
	RESWAVELGSTRUCT		mChnRes[16];
}UDPRESWAVELGSTRUCT,*PUDPRESWAVELGSTRUCT;
extern BOOL g_autoSend; //UDP发送FBG结果。
extern bool g_udpSendChoice;//UDP是否间隔发送.
extern bool g_sendFftBuffer;//发送加速度计的缓存数据
extern int  g_udpSendFrame; //UDP发送间隔.
extern byte g_pcID;//本机ID号
extern UINT g_addressPort;//


struct fft_struct
{
	float fft_buffer[FFT_N];
	frequncy maxfre[4];
	DWORD n_need;//当FFT缓存的个数达到一定程序后才可以进行运算。
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
struct _fbgParameter   //传感器的参数//用于放置16个通道中,每个通道最多20个传感器.
{	
	int     sn;                     //在数据库中的自动编号。
	char    no;                     //传感器序号 0，1，2，3，
	CString ID	;    				//传感器ID号
	CString sensorType;             //传感器类型
	float   orignalWavelengh;		//标定波长
	float   orignalTemperature;		//标定温度
	float   temperatureCoefficnet;	//温度系数  a
	float   alarmMin;				//最小预警
	float   alarmMax;				//最大温度	
	CString position;				//地理位置
	bool    shield;	                //屏蔽否?
	char    parameterStatus;    	//0 无参数 1 有参数
	CString matchFbgID;             //配对的温度传感器  ID
	char    matchFbgNum;            //根据配对光栅ID找到相对应的传感器，如CH，NUM等号码，方便于计算。
	char    matchFbgCh;             //配对传感器的通道号。
	
	float k;                        //应变系数
	float tCompezation;             //温补系数  ，相当于B
	CString unit;                   //运算结果单位
	char    paraIsFull;             //=0 参数齐全了 !=0 ,参数不全
	CString note;
	////////////////////////////////////////////////////////
	float waveLengthLeftLimited;	//左边界,
	float waveLengthRightLimited;	//右边界.
	///////////////////  计算结果   ////////////////////////////
	bool  matchSuccess;             //光栅匹配（根据光栅数据同参数表一一对应）是否成功.
	float fbgWavelength;            //从仪器中得到的FBG波长数据,经配对后,才可以赋值.
	float fbgPower;                 //传感器的能量值.
	float temperature;				//经计算后得到的 FBG 温度数据
	float strain;                   //经计算后得到的 FBG 应变或其它数据 	
	
	int conCountFault;              //用于存储故障验证次数的次数。连续几次之后才确定报警
	int conCountFault2;				//温补光栅丢失计数
	int conCountAlarm;              //报警次数验证。	
	
	int  alarmStatus;		  //0 正常  1 丢失(无光栅数据) 2.丢失(无配对波长) 3预警  4报警  -1无此参数
	int  alarmStatusLast;	     
	int  oldAlarmStatus;      //上一次的报警状态。
	int  showAlarmStatus;     ////对外显示的总的报警状态.自上次复位以来
	int	 showAlarmStatusLast; //上次对外显示的总的报警状态，
	BH   bh;  //波汇要求加入的参数
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
extern saveBuffer m_saveBuffer;     //类saveBuffer的一个实例，所有的数据都放到这个类中去。方便用。

extern	CSocket   udpSendPrg;
extern int	      tempa;
extern bool       flag_udpRec;


extern bool CopyDB(CString &strDBPath);  //拷贝数据库
extern void matchFbg();//光栅配对。
extern void CalcFbg();//光栅计算
extern void CalcFbgBH();//波汇光栅计算
extern void calTemperature(int ch,int sn);//计算温度传感器。
extern void calStrain(int ch,int sn);//计算应变
extern void calStrainBH(int ch,int sn);//计算波汇应变

extern HANDLE g_fbgStrainHandle;   //FBG结果计算事件
extern HANDLE g_fbgResultSendHandle;//发送FBG结果事件。
extern HANDLE g_saveAlarmInfoHandle;    //保存报警信息的事件。


extern void ResponseToAlarmInfor();//处理报警
extern int  GetShowALarmStatus(int alarmStatus,int showAlarmStatus); //alarmStatus  当前的状态  
// showAlarmStatus历史的状态，对外显示的状态,返回总的新状态//根据新的传感器状态和历史的传感器状态，决定返回。


extern void okLight();           //灯复位
extern void cancelSpeaker();     //喇叭消音

extern void faultLight();  
extern void faultSpeaker();   

extern void alarmLight();
extern void alarmSpeaker();
extern UINT	sendFbgDataOut(LPVOID pParam);  //将FBG的计算结果发送出去。

extern bool CopyDBAlarm(CString &strDBPath);	//拷贝报警数据库


extern UINT SaveAlarmInfoThread(LPVOID pParam); //保存报警信息的线程。
extern UINT SaveResultThread(LPVOID pParam);    //保存结果的线程 
extern void resultWriteToBufferChar( );   //结果写入写入CHAR型缓存 waveInfoBuffer[32][maxSaveWaveNumberBuffer]; 中去
extern void UdpSendFbg( );  //将FBG的计算结果发送出去。
extern void g_calFFT(void);//计算FFT
class Glbs               
{
	//private CSocket udpSocket;
	public:
		Glbs();
		virtual ~Glbs();
		CSocket *udpSocket;
};

#endif // !defined(AFX_GLBS_H__2EDDD2E8_03E6_48FC_8496_B29C36636016__INCLUDED_)

