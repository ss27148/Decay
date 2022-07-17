// Copyright (c) 2017, 2022 SS

#include "stdafx.h"
#include "StdioFileEx.h"
#include "utf8.h"
#include <fcntl.h>
#include <io.h>
#include <codecvt>
#include <string>

// Thanks to WebKit
namespace Unicode
{
	template<typename CharType> inline bool isASCII(CharType c)
	{
		return !(c & ~0x7F);
	}

	inline int inlineUTF8SequenceLengthNonASCII(char b0)
	{
		if ((b0 & 0xC0) != 0xC0)
			return 0;
		if ((b0 & 0xE0) == 0xC0)
			return 2;
		if ((b0 & 0xF0) == 0xE0)
			return 3;
		if ((b0 & 0xF8) == 0xF0)
			return 4;
		return 0;
	}

	inline int inlineUTF8SequenceLength(char b0)
	{
		return isASCII(b0) ? 1 : inlineUTF8SequenceLengthNonASCII(b0);
	}

	int UTF8SequenceLength(char b0)
	{
		return isASCII(b0) ? 1 : inlineUTF8SequenceLengthNonASCII(b0);
	}

	int decodeUTF8Sequence(const char* sequence)
	{
		// Handle 0-byte sequences (never valid).
		const unsigned char b0 = sequence[0];
		const int length = inlineUTF8SequenceLength(b0);
		if (length == 0)
			return -1;

		// Handle 1-byte sequences (plain ASCII).
		if (length == 1) {
			//if (b1)
			//	return -1;
			return b0;
		}

		const unsigned char b1 = sequence[1];
		// Handle 2-byte sequences.
		if ((b1 & 0xC0) != 0x80)
			return -1;
		if (length == 2) {
			//if (b2)
			//	return -1;
			const int c = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
			if (c < 0x80)
				return -1;
			return c;
		}

		const unsigned char b2 = sequence[2];
		// Handle 3-byte sequences.
		if ((b2 & 0xC0) != 0x80)
			return -1;
		if (length == 3) {
			//if (b3)
			//	return -1;
			const int c = ((b0 & 0xF) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
			if (c < 0x800)
				return -1;
			// UTF-16 surrogates should never appear in UTF-8 data.
			if (c >= 0xD800 && c <= 0xDFFF)
				return -1;
			return c;
		}

		const unsigned char b3 = sequence[3];
		// Handle 4-byte sequences.
		if ((b3 & 0xC0) != 0x80)
			return -1;
		//const unsigned char b4 = sequence[4];
		if (length == 4) {
			//if (b4)
			//	return -1;
			const int c = ((b0 & 0x7) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
			if (c < 0x10000 || c > 0x10FFFF)
				return -1;
			return c;
		}

		return -1;
	} 
}

//CStdioFileEx::CStdioFileEx()
//{
//	m_nCharset = CP_UTF8;
//
//	m_pBuf = NULL;
//	m_nBufLen = 1024;
//	m_nDataLen = 0;
//	m_nPos = 0;
//
//}
//
//CStdioFileEx::~CStdioFileEx()
//{
//	if (m_pBuf)
//	{
//		delete [] m_pBuf;
//		m_pBuf = NULL;
//	}
//}
//
//BOOL CStdioFileEx::Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError /* = NULL */ )
//{
//	if (!CFile::Open(lpszFileName, nOpenFlags, pError))
//		return FALSE;
//
//	m_nBufLen = (UINT) CFile::GetLength();
//
//	if (!m_pBuf)
//		m_pBuf = new UCHAR[m_nBufLen];
//	m_nDataLen = m_nBufLen;
//	m_nPos = m_nBufLen;
//
//	if (!(nOpenFlags & CFile::modeWrite) || (nOpenFlags & CFile::modeReadWrite))
//	{
//		UCHAR pBuf[4];
//		UINT nRead = CFile::Read(pBuf, 4);
//
//		if (nRead >= 3)
//		{
//			if (pBuf[0] == 0xEF && pBuf[1] == 0xBB && pBuf[2] == 0xBF)
//			{
//				m_nCharset = CP_UTF8;
//				CFile::Seek(3, CFile::begin);
//			}
//			//else if (pBuf[0] == 0xFF && pBuf[1] == 0xFE)
//			//{
//			//	m_nCharset = 0;
//			//	CFile::Seek(3, CFile::begin);
//			//}
//		}
//	}
//
//	return TRUE;
//}
//
//BOOL CStdioFileEx::ReadLine( CString& rString )
//{
//	BOOL b;
//	TCHAR ch;
//	UINT n = 0;
//	
//	rString = _T("");
//
//	while (b = GetChar(ch))
//	{
//		n++;
//		switch (ch)
//		{
//		case '\n':
//			if (!rString.IsEmpty())
//			{
//				if (rString.GetAt(rString.GetLength() - 1) == _T('\r'))
//					rString.Truncate(rString.GetLength() - 1);
//			}
//			return TRUE;
//		default:
//			rString += ch;
//		}
//
//		if (m_nPos == m_nBufLen)
//			break;
//	}
//
//	
//	return (n != 0);
//}
//
//BOOL CStdioFileEx::FillBuffer()
//{
//	m_nDataLen = 0;
//	UINT nRead = CFile::Read(m_pBuf, m_nPos);
//	m_nDataLen += nRead;
//	m_nPos = 0;
//
//	return (nRead != 0);
//}
//
//BOOL CStdioFileEx::GetChar(TCHAR& ch)
//{
//	if (m_nPos >= m_nDataLen)
//	{
//		if (!FillBuffer())
//			return FALSE;
//	}
//
//	ASSERT(m_nDataLen > m_nPos);
//	
//	int nLen = Unicode::inlineUTF8SequenceLength(m_pBuf[m_nPos]);
//	if (nLen == 0)
//		return FALSE;
//	else if (nLen > (m_nDataLen - m_nPos))
//	{
//		if (!FillBuffer())
//			return FALSE;
//
//		if (nLen > (m_nDataLen - m_nPos))
//			return FALSE;
//	}
//
//	ch = Unicode::decodeUTF8Sequence((const char*) m_pBuf + m_nPos);
//	m_nPos += nLen;
//
//	return TRUE;
//}
//
//BOOL CStdioFileEx::WriteBOM()
//{
//	ASSERT(GetLength() == 0);
//
//	UCHAR pBuf[3];
//	pBuf[0] = 0xEF; pBuf[1] = 0xBB; pBuf[2] = 0xBF;
//	CFile::Write(pBuf, 3);
//
//	return TRUE;
//}
//
//void CStdioFileEx::Close()
//{
//	CFile::Close();
//
//	if (!m_pBuf)
//	{
//		delete [] m_pBuf;
//		m_pBuf = NULL;
//	}
//}
//
//BOOL CStdioFileEx::WriteLine( const CString& rString )
//{
//	if (GetLength() == 0)
//		WriteBOM();
//
//	CStringA utf8;
//	UTF16toUTF8Ref(utf8, rString);
//	utf8 += "\r\n";
//
//	CFile::Write((void*) (LPCSTR) utf8, utf8.GetLength());
//
//	return TRUE;
//}
//
//BOOL CStdioFileEx::WriteString(const CString& rString)
//{
//	if (GetLength() == 0)
//		WriteBOM();
//
//	CStringA utf8;
//	UTF16toUTF8Ref(utf8, rString);
//
//	CFile::Write((void*) (LPCSTR) utf8, utf8.GetLength());
//
//	return TRUE;
//}
//
//CStdioFileEx& CStdioFileEx::operator<<(const CString& rString)
//{
//	WriteString(rString);
//	return *this;
//}

// ---------
// CCSVFile
// ---------
CCSVFile::CCSVFile()
{
	m_chSeparator = _T(';'); m_bNewLine = true;
	m_nRowPos = 0;
}

bool CCSVFile::Open(const CString& filename, UINT openMode, UINT sharingMode)
{
	imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));

	open(filename, openMode, sharingMode);

	return is_open();
}

void CCSVFile::SetSeparator(TCHAR chSeparator)
{
	ASSERT(!is_open());
	m_chSeparator = chSeparator;
}

void CCSVFile::WriteRow(const CStringArray& strArray)
{
	CString strRow;
	CString str;

	for (INT_PTR i = 0; i < strArray.GetCount(); i++)
	{
		if (i > 0)
			strRow += m_chSeparator;
		EscapeField(str, strArray[i]);
		strRow += str;
	} 

	strRow += _T("\n");

	write(strRow, strRow.GetLength());
}

void CCSVFile::WriteField(const CString& rString)
{
	CString str;
	EscapeField(str, rString);

	if (!m_bNewLine)
		str = m_chSeparator + str;

	write(str, str.GetLength());

	m_bNewLine = false;
}

BOOL CCSVFile::ReadField(CString& rString)
{
	rString = _T("");
	m_bNewLine = FALSE;

	while (m_strRow.IsEmpty())
	{
		m_nRowPos = 0;
		std::wstring str;
		if (!std::getline(*this, str))
		{
			if (m_strRow.IsEmpty())
				m_bNewLine = TRUE;
			
			return FALSE;
		}

		m_strRow.SetString(str.data(), str.length());
	}

	const TCHAR chQuote = _T('\"');
	int n = m_nRowPos;
	int nState = afterSeparatorState;
	
	if (n == m_strRow.GetLength())
	{
		ResetRowBuffer();
		nState = afterSeparatorState;
		m_bNewLine = TRUE;
		return TRUE;
	}

	while (n < m_strRow.GetLength())
	{
		TCHAR ch = m_strRow[n];
		
		if (ch == m_chSeparator)
		{
			switch (nState)
			{
			case afterSeparatorState:
			case insideFieldState:
				rString += m_strRow.Mid(m_nRowPos, n - m_nRowPos);
				m_nRowPos = n + 1;
				nState = afterSeparatorState;
				return TRUE;

			case insideFieldQuotedState:
				break;
			}
		}
		else if (ch == chQuote)
		{
			switch (nState)
			{
			case afterSeparatorState:
				nState = insideFieldQuotedState;
				m_nRowPos++;
				break;
			case insideFieldState:
				// leave as it is
				break;
			case insideFieldQuotedState:
				if (n + 1 < m_strRow.GetLength())
				{
					if (m_strRow[n + 1] == chQuote)
					{
						n++;
						rString += m_strRow.Mid(m_nRowPos, n - m_nRowPos);
						m_nRowPos = min(n + 1, m_strRow.GetLength() - 1);
					}
					else
					{
						rString += m_strRow.Mid(m_nRowPos, n - m_nRowPos);
						m_nRowPos = min(n + 1, m_strRow.GetLength() - 1);
						nState = insideFieldState;
					}
				}
				else
				{
					rString += m_strRow.Mid(m_nRowPos, n - m_nRowPos);
					ResetRowBuffer();
					nState = afterSeparatorState;
					return TRUE;
				}
				break;
			}
		}
		else
		{
			switch (nState)
			{
			case afterSeparatorState:
				nState = insideFieldState;
				break;
			case insideFieldState:
			case insideFieldQuotedState:
				break;
			}
		}
		
		n++;

		if (n >= m_strRow.GetLength())
		{
			ASSERT(n == m_strRow.GetLength());

			switch (nState)
			{
			case insideFieldState:
				ASSERT(m_nRowPos < m_strRow.GetLength());
				rString += m_strRow.Mid(m_nRowPos, n - m_nRowPos);
				// Fall through
			case afterSeparatorState:
				ResetRowBuffer();
				nState = afterSeparatorState;
				m_bNewLine = TRUE;
				break;

			case insideFieldQuotedState:
				ASSERT(m_nRowPos < m_strRow.GetLength());
				rString += m_strRow.Mid(m_nRowPos, n - m_nRowPos);
				rString += _T("\n");

				if (!ReadRowBuffer(true))
					return FALSE;

				m_nRowPos = 0;
				n = 0;
				break;
			}
		}
	}

	ASSERT(nState == afterSeparatorState);

	return TRUE;
}

BOOL CCSVFile::ReadFile(int& nColCount, CArray<CString>& rData)
{
	nColCount = 0;
	rData.RemoveAll();

	CString strField;
	bool bHeader = true;
	int nCol = 0;
	while (ReadField(strField))
	{
		rData.Add(strField);
		nCol++;

		if (IsNewLine())
		{
			if (bHeader)
			{
				bHeader = false;
				nColCount = nCol;
			}
			else
			{
				if (nCol != nColCount)
					return FALSE;
			}
			nCol = 0;
		}
	}
	return TRUE;
}

BOOL CCSVFile::ReadRow(CStringArray& rData)
{
	rData.RemoveAll();

	CString strField;
	BOOL bReadField;
	while (bReadField = ReadField(strField))
	{
		rData.Add(strField);

		if (IsNewLine())
			break;
	}

	return bReadField;
}

CCSVFile& CCSVFile::operator<<(const CString& rString)
{
	WriteField(rString);
	return *this;
}

CCSVFile& CCSVFile::operator<<(int iVal)
{
	CString str;
	str.Format(_T("%i"), iVal);
	WriteField(str);
	return *this;
}

CCSVFile& CCSVFile::operator<<(unsigned int uVal)
{
	CString str;
	str.Format(_T("%u"), uVal);
	WriteField(str);
	return *this;
}

CCSVFile& CCSVFile::operator<<(float fVal)
{
	CString str;
	str.Format(_T("%f"), fVal);
	WriteField(str);
	return *this;
}

CCSVFile& CCSVFile::operator<<(double fVal)
{
	CString str;
	str.Format(_T("%lf"), fVal);
	WriteField(str);
	return *this;
}

CCSVFile& CCSVFile::operator<<(CCSVFile& (*pfn)(CCSVFile&))
{
	(*pfn)(*this);
	return *this;
}

void CCSVFile::EscapeField(CString& strOut, const CString& strIn)
{
	strOut = strIn;
	bool bQuote = false;
	if (strOut.FindOneOf(_T(" \t\r\n,;:")) != -1)
		bQuote = true;
	strOut.Replace(_T("\""), _T("\"\""));
	//strOut.Replace(_T("\r"), _T("\\r"));
	//strOut.Replace(_T("\n"), _T("\\n"));
	if (bQuote)
		strOut = _T("\"") + strOut + _T("\"");
}

CCSVFile& CCSVFile::Endl(CCSVFile& os)
{
	static_cast<std::wfstream&>(os).write(_T("\n"), 1);
	os.m_bNewLine = true;
	return os;
}

CCSVFile& CCSVFile::Endl()
{
	write(_T("\n"), 1);
	m_bNewLine = true;
	return *this;
}




BOOL CCSVFile::ReadRowBuffer(bool bForceRead)
{
	if (bForceRead)
		m_strRow = _T("");

	while (m_strRow.IsEmpty())
	{
		m_nRowPos = 0;

		std::wstring str;
		if (!std::getline(*this, str))
			return FALSE;

		m_strRow.SetString(str.data(), str.length());
	}

	return TRUE;
}

void CCSVFile::ResetRowBuffer()
{
	m_nRowPos = 0;
	m_strRow = _T("");
}
