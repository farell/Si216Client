// saveBuffer.cpp: implementation of the saveBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Si216Client.h"
#include "saveBuffer.h"
#include "Glbs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

saveBuffer::saveBuffer()
{
	serialNo=0; //用于缓存的计数器用.
	totalNo=0;  //用于报警总的计数用。
	BufCount=0;//写入结果缓存的指针
	BufMiddCount=0;//写入缓存的中间指针
	saveCount=0;//写入文件的指针
	saveMiddCount=0;//写入文件的中间指针
	sampleCount=0;//抽样计数器
	
	//jstart[0]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//循环小计数，追赶waveWriteToBuffer中的BUFFER中的istart
	// 	waveSaveCount[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
}

saveBuffer::~saveBuffer()
{
	
}


void saveBuffer::alarmWriteToBuffer(CString s1, CString s2)
{	
	CTime	saveTime=CTime::GetCurrentTime();
	CString tempTime=saveTime.Format("%Y-%m-%d  %H:%M:%S");	//显示年月日
	
	alarmInformBuffer[serialNo].SN=totalNo+1;  //报警信息写入缓存中。这样速度快。
	alarmInformBuffer[serialNo].alarmTime=tempTime;
	alarmInformBuffer[serialNo].alarmChannelAndSensor=s1;
	alarmInformBuffer[serialNo].alarmInformation=s2;
	serialNo++;
	totalNo++;  
	if(serialNo>=maxSaveNumberOnce)  //serialNo=0~199.不会是200。
		serialNo=0;
	alarmInformBuffer[serialNo].SN=-1;   //在下一条中写入停止符号。保存的时候碰到SN==-1，停止了。表明这是缓存尾。
}