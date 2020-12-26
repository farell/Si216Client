// saveBuffer.h: interface for the saveBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVEBUFFER_H__F0E710B8_C65F_41ED_958A_D9E7EAC06982__INCLUDED_)
#define AFX_SAVEBUFFER_H__F0E710B8_C65F_41ED_958A_D9E7EAC06982__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Glbs.h"

#define maxSaveWaveNumberBuffer 100      //����ͨ����ÿͨ����������������Ŀ��
#define maxSaveNumberOnce 200      // ACCESS�Ļ����С��һ�����洢��౨�������ݡ� 

//�����ж��������飬һ���Ǳ��汨����Ϣ�Ļ����飬һ���Ǳ�������������ʱ���ݣ����ڴ�����ļ���ȥ��


struct  _resultInfoBufferChar   //�������õ�CHAR�����顣
{
	char WaBuChar[40*20+16];//�����ÿͨ���ı�����ֽڵĳ��ȵ����顣
};

struct _saveFileBuffer   //д���ļ��ı���������
{
	long SN;
	CString alarmTime; 
	CString alarmChannelAndSensor;
	CString alarmInformation;
};

class saveBuffer  //���ڱ������ݿ����ʱ���ݣ������������ʱ���ݵȡ�
{
public:
	int sampleCount;
	
	struct _resultInfoBufferChar m_RIBC[16];//struct _resultInfoBufferChar m_RIBC[16][maxSaveWaveNumberBuffer];
	void alarmWriteToBuffer(CString s1,CString s2);//������Ϣ��
	saveBuffer();
	virtual ~saveBuffer();
	struct _saveFileBuffer alarmInformBuffer[maxSaveNumberOnce];//0~199
	int serialNo; //���ڻ���ļ�������.
	int totalNo;  //���ڱ����ܵļ����á�
	
	int BufCount;//д���������ָ��
	int BufMiddCount;//д����������м�ָ��
	
	int saveCount;//д���ļ���ָ��
	int saveMiddCount;//д���ļ����м�ָ��
	int shouldSaveCount;//Ӧд���ļ�������
	
	int jstart[16];//={0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0};
	//ѭ��С������׷��waveWriteToBuffer�е�BUFFER�е�istart
	int waveSaveCount[16];//={0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0};
};

#endif // !defined(AFX_SAVEBUFFER_H__F0E710B8_C65F_41ED_958A_D9E7EAC06982__INCLUDED_)
