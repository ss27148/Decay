// Copyright (c) 2017, 2022 SS

#pragma once
#include <fstream>

class CCSVFile : public std::wfstream
{
public:
	CCSVFile();

	bool Open(const CString& filename, UINT openMode, UINT sharingMode);

	TCHAR GetSeparator() const { return m_chSeparator; }
	void SetSeparator(TCHAR chSeparator);

	void WriteRow(const CStringArray& strArray);
	void WriteField(const CString& rString);

	BOOL ReadField(CString& rString);
	BOOL ReadFile(int& nColCount, CArray<CString>& rData);
	BOOL ReadRow(CStringArray& rData);

	CCSVFile& operator<< (const CString& rString);
	CCSVFile& operator<< (int iVal);
	CCSVFile& operator<< (unsigned int uVal);
	CCSVFile& operator<< (float fVal);
	CCSVFile& operator<< (double fVal);
	CCSVFile& operator<< (CCSVFile& (*pfn)(CCSVFile&));

	void EscapeField(CString& strOut, const CString& strIn);

	static CCSVFile& Endl(CCSVFile& os);
	CCSVFile& Endl();

	BOOL IsNewLine() const { return m_bNewLine; }

protected:
	enum
	{
		afterSeparatorState,
		insideFieldState,
		insideFieldQuotedState,
	};

	BOOL ReadRowBuffer(bool bForceRead = false);
	void ResetRowBuffer();
protected:
	CString m_strRow;
	int m_nRowPos;
	//const CString ENDL = _T("\r\n");
	TCHAR m_chSeparator;
	BOOL m_bNewLine;
};