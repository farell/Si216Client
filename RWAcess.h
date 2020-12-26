// RWAcess.h: interface for the RWAcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWACESS_H__F7AB8676_79CE_45C2_95A6_9544FA56BB76__INCLUDED_)
#define AFX_RWACESS_H__F7AB8676_79CE_45C2_95A6_9544FA56BB76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RWAcess  
{
public:
	bool readIP();
	bool ReadAllFbgParameter();
	bool OpenSheet(CString sheet);
	HRESULT hr;
	_RecordsetPtr m_pRecordset;
	_ConnectionPtr m_pConnection;
	CString m_ConnectPath;

	CString m_ParaTablePath;       //要打开的文件名字。
	RWAcess();
	virtual ~RWAcess();

};

#endif // !defined(AFX_RWACESS_H__F7AB8676_79CE_45C2_95A6_9544FA56BB76__INCLUDED_)
