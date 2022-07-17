// Copyright (c) 2017 SS

#include "stdafx.h"
#include "NumString.h"
#include <locale.h>


BOOL StringToInt( int& val, LPCTSTR str, int radix /*= 10*/ )
{
	TCHAR* p = NULL;
	val = _tcstol(str, &p, radix);
	return p != str;
}

BOOL StringToUInt( UINT& val, LPCTSTR str, int radix /*= 10*/ )
{
	TCHAR* p = NULL;
	val = _tcstoul(str, &p, radix);
	return p != str;
}

BOOL StringToFloat( float& val, LPCTSTR str )
{
	TCHAR* p = NULL;
	CString strtmp(str);
	strtmp.Replace(_T(','), _T('.'));

	CString locale = _tsetlocale(LC_NUMERIC, NULL);
	_tsetlocale(LC_NUMERIC, _T("C"));
	val = (float) _tcstod(strtmp, &p);
	_tsetlocale(LC_NUMERIC, locale);

	return p != str;
}

BOOL StringToDouble( double& val, LPCTSTR str )
{
	TCHAR* p = NULL;
	CString strtmp(str);
	strtmp.Replace(_T(','), _T('.'));
	
	CString locale = _tsetlocale(LC_NUMERIC, NULL);
	_tsetlocale(LC_NUMERIC, _T("C"));
	val = _tcstod(strtmp, &p);
	_tsetlocale(LC_NUMERIC, locale);

	return p != str;
}

