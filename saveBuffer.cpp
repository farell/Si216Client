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
	serialNo=0; //���ڻ���ļ�������.
	totalNo=0;  //���ڱ����ܵļ����á�
	BufCount=0;//д���������ָ��
	BufMiddCount=0;//д�뻺����м�ָ��
	saveCount=0;//д���ļ���ָ��
	saveMiddCount=0;//д���ļ����м�ָ��
	sampleCount=0;//����������
	
	//jstart[0]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//ѭ��С������׷��waveWriteToBuffer�е�BUFFER�е�istart
	// 	waveSaveCount[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
}

saveBuffer::~saveBuffer()
{
	
}


void saveBuffer::alarmWriteToBuffer(CString s1, CString s2)
{	
	CTime	saveTime=CTime::GetCurrentTime();
	CString tempTime=saveTime.Format("%Y-%m-%d  %H:%M:%S");	//��ʾ������
	
	alarmInformBuffer[serialNo].SN=totalNo+1;  //������Ϣд�뻺���С������ٶȿ졣
	alarmInformBuffer[serialNo].alarmTime=tempTime;
	alarmInformBuffer[serialNo].alarmChannelAndSensor=s1;
	alarmInformBuffer[serialNo].alarmInformation=s2;
	serialNo++;
	totalNo++;  
	if(serialNo>=maxSaveNumberOnce)  //serialNo=0~199.������200��
		serialNo=0;
	alarmInformBuffer[serialNo].SN=-1;   //����һ����д��ֹͣ���š������ʱ������SN==-1��ֹͣ�ˡ��������ǻ���β��
}