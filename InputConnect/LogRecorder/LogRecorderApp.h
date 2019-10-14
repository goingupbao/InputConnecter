#pragma once

#include "LogRecorder.h"

class CLogRecorderApp
{
public:
	CLogRecorderApp(void);
	virtual ~CLogRecorderApp(void);
	void Record(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""), RECORD_LEVEL rLevel = RECORD_LEVEL_INFO);
	void Record_Debug(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""))
	{
#ifdef _DEBUG
		CLogRecorderApp::Record(strContext, strPath, strFileName, strExt, RECORD_LEVEL_DEBUG);
#endif
	}
	void Record_Info(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""))
	{
		Record(strContext, strPath, strFileName, strExt, RECORD_LEVEL_INFO);
	}
	void Record_Warn(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""))
	{
		Record(strContext, strPath, strFileName, strExt, RECORD_LEVEL_WARN);
	}
	void Record_Err(CString strContext, CString strPath = _T(""), CString strFileName = _T(""), CString strExt = _T(""))
	{
		Record(strContext, strPath, strFileName, strExt, RECORD_LEVEL_ERROR);
	}
private:
	CString m_strCurrentDir;
};

