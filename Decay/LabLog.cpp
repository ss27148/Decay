// LabLib
// Copyright (c) 2018 SS
#include "stdafx.h"
#include "LabLog.h"
#include <codecvt>


CLabLog::CLabLog()
{
}

CLabLog::~CLabLog()
{
	close();
}

BOOL CLabLog::Open(const CString& strFileName)
{
	CString strFullPath = strFileName;
	if (::PathIsRelative(strFileName))
	{
		TCHAR szAppPath[MAX_PATH + 1];
		::GetModuleFileName(NULL, szAppPath, MAX_PATH);
		::PathRemoveFileSpec(szAppPath);
		::PathAppend(szAppPath, strFileName);
		strFullPath = szAppPath;
	}

	imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

	open(strFullPath, std::ios_base::out | std::ios_base::trunc, _SH_DENYWR);

	return is_open();
}

void CLabLog::WriteString(const CString& str)
{
	std::wofstream::write(str, str.GetLength());
}

CLabLog& CLabLog::operator<<(LPCTSTR sz)
{
	WriteString(sz);
	return *this;
}

CLabLog& CLabLog::operator<<(int value)
{
	CString str;
	str.Format(_T("%i"), value);
	WriteString(str);
	return *this;
}

CLabLog& CLabLog::operator<<(UINT value)
{
	CString str;
	str.Format(_T("%u"), value);
	WriteString(str);
	return *this;
}

CLabLog& CLabLog::operator<<(float value)
{
	CString str;
	str.Format(_T("%g"), value);
	WriteString(str);
	return *this;
}

CLabLog& CLabLog::operator<<(double value)
{
	CString str;
	str.Format(_T("%lg"), value);
	WriteString(str);
	return *this;
}

CLabLog& CLabLog::operator<<(const CTime& value)
{
	WriteString(value.Format(_T("%Y-%m-%d %H:%M:%S")));
	return *this;
}
