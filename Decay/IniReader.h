// Copyright (c) 2018 SS

#pragma once

class CIniReader
{
public:
	CIniReader(CArchive& ar);

	enum TokenType
	{
		BlankToken,
		CommentToken,
		SectionToken,
		ParameterToken
	};

	class CToken
	{
	public:
		CToken() { Reset(); }

		void Reset()
		{
			m_type = BlankToken;
			m_name = _T("");
			m_value = _T("");
		}

		TokenType m_type;
		CString m_name;
		CString m_value;
	};

	void WriteLine(const CString& str);
	void WriteSection(LPCTSTR szName);
	void WriteInt(LPCTSTR szParam, int value);
	void WriteUInt(LPCTSTR szParam, UINT value);
	void WriteFloat(LPCTSTR szParam, double value);
	void WriteString(LPCTSTR szParam, const CString& value);

	BOOL ReadLine(CString& str);
	BOOL ReadNextToken(CToken& token);

protected:
	CArchive& m_ar;
};