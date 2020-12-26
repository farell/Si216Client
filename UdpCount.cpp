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
	UdpCount1=0;	//UDP进来的数据的指针。
	UdpCount_old=0;	//上一帧的指针。这一帧是有数的，
	UdpCount_next=1;
	CalCount=0;		//计算的指针
	shouldCalNum=0;  //需要计算的个数
}

UdpCount::~UdpCount()
{

}


