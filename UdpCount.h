// UdpCount.h: interface for the UdpCount class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPCOUNT_H__0010197D_DB0A_4F21_8865_E54DB3C1DEA9__INCLUDED_)
#define AFX_UDPCOUNT_H__0010197D_DB0A_4F21_8865_E54DB3C1DEA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UdpCount  
{
public:	
	bool onReceving;//正在计算数据
	int calMiddNum;
	int UdpCount_next;	//缓存的下一帧，清零，用于防止过运算
	int UdpCount1;		//UDP进来的数据的指针。
	int UdpCount_old;	//上一帧的指针。这一帧是有数的，

	int CalCount;		//计算的指针
	int shouldCalNum;  //需要计算的个数
	UdpCount();
	virtual ~UdpCount();

};

#endif // !defined(AFX_UDPCOUNT_H__0010197D_DB0A_4F21_8865_E54DB3C1DEA9__INCLUDED_)
