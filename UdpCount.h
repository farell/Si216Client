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
	bool onReceving;//���ڼ�������
	int calMiddNum;
	int UdpCount_next;	//�������һ֡�����㣬���ڷ�ֹ������
	int UdpCount1;		//UDP���������ݵ�ָ�롣
	int UdpCount_old;	//��һ֡��ָ�롣��һ֡�������ģ�

	int CalCount;		//�����ָ��
	int shouldCalNum;  //��Ҫ����ĸ���
	UdpCount();
	virtual ~UdpCount();

};

#endif // !defined(AFX_UDPCOUNT_H__0010197D_DB0A_4F21_8865_E54DB3C1DEA9__INCLUDED_)
