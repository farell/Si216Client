#if !defined(AFX_UDP_H__7322C83D_731A_4671_89DC_B825D7918D11__INCLUDED_)
#define AFX_UDP_H__7322C83D_731A_4671_89DC_B825D7918D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Udp.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// Udp command target

class Udp : public CSocket
{
// Attributes
public:

// Operations
public:
	Udp();
	virtual ~Udp();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Udp)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(Udp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UDP_H__7322C83D_731A_4671_89DC_B825D7918D11__INCLUDED_)
