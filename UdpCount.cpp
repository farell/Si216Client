// UdpCount.cpp: implementation of the UdpCount class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Si216Client.h"
#include "UdpCount.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UdpCount::UdpCount()
{
	onReceving=false;
	calMiddNum=0;
	UdpCount1=0;	//UDP���������ݵ�ָ�롣
	UdpCount_old=0;	//��һ֡��ָ�롣��һ֡�������ģ�
	UdpCount_next=1;
	CalCount=0;		//�����ָ��
	shouldCalNum=0;  //��Ҫ����ĸ���
}

UdpCount::~UdpCount()
{

}


