// LabLib
// Copyright (c) 2018 SS

#pragma once
#include "StdioFileEx.h"
#include <fstream>

class CLabLog : public std::wofstream
{
public:
	CLabLog();
	virtual ~CLabLog();

	BOOL Open(const CString& strFileName);

	void WriteString(const CString& str);

	CLabLog& operator<<(LPCTSTR sz);
	CLabLog& operator<<(int value);
	CLabLog& operator<<(UINT value);
	CLabLog& operator<<(float value);
	CLabLog& operator<<(double value);
	CLabLog& operator<<(const CTime& value);

};