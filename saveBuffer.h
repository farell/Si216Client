// saveBuffer.h: interface for the saveBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVEBUFFER_H__F0E710B8_C65F_41ED_958A_D9E7EAC06982__INCLUDED_)
#define AFX_SAVEBUFFER_H__F0E710B8_C65F_41ED_958A_D9E7EAC06982__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Glbs.h"

#define maxSaveWaveNumberBuffer 100      //波长通道，每通道波长缓存的最大数目。
#define maxSaveNumberOnce 200      // ACCESS的缓冲大小。一次最多存储最多报警的内容。 

//本类有二个缓存组，一个是保存报警信息的缓存组，一个是保存运算结果的临时数据，用于存放于文件中去。


struct  _resultInfoBufferChar   //保存结果用的CHAR型数组。
{
	char WaBuChar[40*20+16];//保存的每通道的保存的字节的长度的数组。
};

struct _saveFileBuffer   //写入文件的报警的内容
{
	long SN;
	CString alarmTime; 
	CString alarmChannelAndSensor;
	CString alarmInformation;
};

class saveBuffer  //用于保存数据库的临时数据，波长结果的临时数据等。
{
public:
	int sampleCount;
	
	struct _resultInfoBufferChar m_RIBC[16];//struct _resultInfoBufferChar m_RIBC[16][maxSaveWaveNumberBuffer];
	void alarmWriteToBuffer(CString s1,CString s2);//报警信息。
	saveBuffer();
	virtual ~saveBuffer();
	struct _saveFileBuffer alarmInformBuffer[maxSaveNumberOnce];//0~199
	int serialNo; //用于缓存的计数器用.
	int totalNo;  //用于报警总的计数用。
	
	int BufCount;//写入结果缓存的指针
	int BufMiddCount;//写入结果缓存的中间指针
	
	int saveCount;//写入文件的指针
	int saveMiddCount;//写入文件的中间指针
	int shouldSaveCount;//应写入文件的数量
	
	int jstart[16];//={0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0};
	//循环小计数，追赶waveWriteToBuffer中的BUFFER中的istart
	int waveSaveCount[16];//={0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0};
};

#endif // !defined(AFX_SAVEBUFFER_H__F0E710B8_C65F_41ED_958A_D9E7EAC06982__INCLUDED_)
