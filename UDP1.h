#if !defined(AFX_UDP1_H__6F443DD0_4469_40D7_B821_37F954B317B7__INCLUDED_)
#define AFX_UDP1_H__6F443DD0_4469_40D7_B821_37F954B317B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UDP1.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// UDP1 command target

class UDP1 : public CSocket
{
	// Attributes
public:
	char mRecvData_Char[10000];	
	CString sss;
	int temp11,ch;
	// Operations
public:
	UDP1();
	virtual ~UDP1();
	
	// Overrides
public:
// 	int calMiddNum;
// 	int UdpCount1;		//UDP进来的数据的指针。
// 	int UdpCount_old;	//上一帧的指针。这一帧是有数的，
// 	int CalCount;		//计算的指针
// 	int shouldCalNum;  //需要计算的个数
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UDP1)
public:
	virtual void OnReceive(int nErrorCode);
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	//}}AFX_VIRTUAL
	
	// Generated message map functions
	//{{AFX_MSG(UDP1)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	
	// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UDP1_H__6F443DD0_4469_40D7_B821_37F954B317B7__INCLUDED_)
