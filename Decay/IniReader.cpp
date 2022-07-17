// Copyright (c) 2018 SS

#include "stdafx.h"
#include "IniReader.h"
#include "utf8.h"



CIniReader::CIniReader(CArchive& ar) : m_ar(ar)
{

}

void CIniReader::WriteLine(const CString& str)
{
	CStringA strA;
	UTF16toUTF8Ref(strA, str + _T("\n"));
	m_ar.Write((LPCSTR) strA, strA.GetLength());
}

void CIniReader::WriteSection(LPCTSTR szName)
{
	CString str;
	str = _T("[") + CString(szName) + _T("]");
	WriteLine(str);
}

void CIniReader::WriteInt(LPCTSTR szParam, int value)
{
	CString str;
	str.Format(CString(szParam) + _T("=%i"), value);
	WriteLine(str);
}

void CIniReader::WriteUInt(LPCTSTR szParam, UINT value)
{
	CString str;
	str.Format(CString(szParam) + _T("=%d"), value);
	WriteLine(str);
}

void CIniReader::WriteFloat(LPCTSTR szParam, double value)
{
	CString str;
	str.Format(CString(szParam) + _T("=%g"), value);
	WriteLine(str);
}

void CIniReader::WriteString(LPCTSTR szParam, const CString& value)
{
	CString str;
	str = CString(szParam) + _T("=") + value;
	WriteLine(str);
}

BOOL CIniReader::ReadLine(CString& str)
{
	str = _T("");
	int nCount = 255;
	CStringA strA;
	LPSTR szBuf = strA.GetBuffer(nCount);

	int nRead = 0;
	char ch;

	TRY
	{
		while (nRead < nCount)
		{
			m_ar >> ch;

			// stop and end-of-line (trailing '\n' is ignored)
			if (ch == '\n' || ch == '\r')
			{
				if (ch == '\r')
					m_ar >> ch;
				szBuf[nRead++] = ch;
				break;
			}

			szBuf[nRead++] = ch;
		}

		strA.ReleaseBuffer(nRead);
		UTF8toUTF16Ref(str, strA);
	}
	CATCH(CArchiveException, e)
	{
		if (e && e->m_cause == CArchiveException::endOfFile)
		{
			strA.ReleaseBuffer(nRead);
			UTF8toUTF16Ref(str, strA);
			return FALSE;
		}
		else
		{
			THROW_LAST();
		}
	}
	END_CATCH

	return TRUE;
}

BOOL CIniReader::ReadNextToken(CToken& token)
{
	token.Reset();

	CString str;
	if (!ReadLine(str))
		return FALSE;

	enum ParseState
	{
		Initial,
		SectionName,
		ParameterName,
		ParameterValue
	};

	ParseState state = Initial;
	TCHAR ch;

	for (int i = 0; i < str.GetLength(); i++)
	{
		ch = str[i];
		switch (state)
		{
		case Initial:
			if (_istalnum(ch))
			{
				token.m_type = ParameterToken;
				token.m_name += ch;
				state = ParameterName;
			}
			else if (ch == '[')
			{
				token.m_type = SectionToken;
				state = SectionName;
			}
			else if (ch == ';')
			{
				token.m_type = CommentToken;
				state = SectionName;
			}

			break;

		case SectionName:
			if (token.m_type == SectionToken && ch == ']')
			{
				token.m_name.Trim();
				return TRUE;
			}
			else
			{
				token.m_name += ch;
			}
			break;

		case ParameterName:
			if (ch == '=')
			{
				token.m_name.Trim();
				state = ParameterValue;
			}
			else
			{
				token.m_name += ch;
			}
			break;

		case ParameterValue:
			token.m_value = str.Mid(i);
			token.m_value.Trim();
			if (token.m_value.GetLength() >= 2)
			{
				if (token.m_value[0] == '\"' && 
					token.m_value[token.m_value.GetLength() - 1] == '\"')
				{
					token.m_value = token.m_value.Mid(1, token.m_value.GetLength() - 2);
				}
			}
			return TRUE;

		}

	}

	return TRUE;

}
