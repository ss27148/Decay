// Copyright (c) 2017 SS

#include "stdafx.h"
#include "utf8.h"

CStringA UTF16toUTF8(const CStringW& utf16)
{
	CStringA utf8;
	UTF16toUTF8Ref(utf8, utf16);
	return utf8;
}

CStringA UTF16toUTF8( LPCWSTR utf16, int cchutf16 /*= -1*/ )
{
	CStringA utf8;
	UTF16toUTF8Ref(utf8, utf16, cchutf16);
	return utf8;
}

BOOL UTF16toUTF8Ref( CStringA& utf8, const CStringW& utf16 )
{
	if (utf16.IsEmpty())
	{
		utf8.Empty();
		return TRUE;
	}

	int cbutf8 = WideCharToMultiByte(CP_UTF8, 0, utf16, utf16.GetLength(), NULL, 0, 0, 0);
	if (cbutf8)
	{
		LPSTR putf8buf = utf8.GetBuffer(cbutf8);
		if (!WideCharToMultiByte(CP_UTF8, 0, utf16, utf16.GetLength(), putf8buf, cbutf8, 0, 0))
		{
			utf8.ReleaseBuffer(0);
			return FALSE;
		}
		utf8.ReleaseBuffer(cbutf8);
	}
	else
		return FALSE;

	return TRUE;
}

BOOL UTF16toUTF8Ref( CStringA& utf8, LPCWSTR utf16, int cchutf16 /*= -1*/ )
{
	if (cchutf16 == -1)
		cchutf16 = wcslen(utf16);

	if (cchutf16 == 0)
	{
		utf8.Empty();
		return TRUE;
	}

	int cbutf8 = WideCharToMultiByte(CP_UTF8, 0, utf16, cchutf16, NULL, 0, 0, 0);
	if (cbutf8)
	{
		LPSTR putf8buf = utf8.GetBuffer(cbutf8);
		if (!WideCharToMultiByte(CP_UTF8, 0, utf16, cchutf16, putf8buf, cbutf8, 0, 0))
		{
			utf8.ReleaseBuffer(0);
			return FALSE;
		}
		utf8.ReleaseBuffer(cbutf8);
	}
	else
		return FALSE;

	return TRUE;
}

CStringW UTF8toUTF16(const CStringA& utf8)
{
	CStringW utf16;
	UTF8toUTF16Ref(utf16, utf8);
	return utf16;
}

CStringW UTF8toUTF16( LPCSTR utf8, int cbutf8 /*= -1*/ )
{
	CStringW utf16;
	UTF8toUTF16Ref(utf16, utf8, cbutf8);
	return utf16;
}

int UTF8Length(const CStringA& utf8)
{
	return UTF8Length((LPCSTR) utf8, utf8.GetLength());
}

int UTF8Length(LPCSTR utf8, int cbutf8)
{
	if (cbutf8 == 0)
		return 0;
	return MultiByteToWideChar(CP_UTF8, 0, utf8, cbutf8, NULL, 0);
}

BOOL UTF8toUTF16Ref( CStringW& utf16, const CStringA& utf8 )
{
	return UTF8toUTF16Ref(utf16, (LPCSTR) utf8, utf8.GetLength());
}

BOOL UTF8toUTF16Ref( CStringW& utf16, LPCSTR utf8, int cbutf8 /*= -1*/ )
{
	if (cbutf8 == 0)
	{
		utf16.Empty();
		return TRUE;
	}
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, cbutf8, NULL, 0);
	if (len >= 1)
	{
		LPWSTR putf16buf = utf16.GetBuffer(cbutf8 == -1 ? len - 1 : len);
		if (putf16buf)
			MultiByteToWideChar(CP_UTF8, 0, utf8, cbutf8, putf16buf, len);
		utf16.ReleaseBuffer(cbutf8 == -1 ? len - 1 : len);
	}
	return TRUE;
}


