// Udp.cpp : implementation file
//

#include "stdafx.h"
#include "Si216Client.h"
#include "Udp.h"
#include "Glbs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Udp

Udp::Udp()
{
}

Udp::~Udp()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(Udp, CSocket)
	//{{AFX_MSG_MAP(Udp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// Udp member functions

void Udp::OnReceive(int nErrorCode) 
{
	Udp::Receive(&mRecvData,sizeof(UDPRESWAVELGSTRUCT),0);
	if( strcmp(mRecvData.St_Flag,"UDPSTART")==0 ) 
	{
		if( strcmp(mRecvData.Ed_Flag,"!UDPEND!")==0 ) 
		{
			recvOK = true;
			recvFrNum++;
		}
	}		
	CSocket::OnReceive(nErrorCode);		
}
