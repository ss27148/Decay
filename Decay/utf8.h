// Copyright (c) 2017 SS

#pragma once

BOOL UTF16toUTF8Ref(CStringA& utf8, const CStringW& utf16);
BOOL UTF16toUTF8Ref(CStringA& utf8, LPCWSTR utf16, int cchutf16 = -1);
CStringA UTF16toUTF8(const CStringW& utf16);
CStringA UTF16toUTF8(LPCWSTR utf16, int cchutf16 = -1);

BOOL UTF8toUTF16Ref(CStringW& utf16, const CStringA& utf8);
BOOL UTF8toUTF16Ref(CStringW& utf16, LPCSTR utf8, int cbutf8 = -1);
CStringW UTF8toUTF16(const CStringA& utf8);
CStringW UTF8toUTF16(LPCSTR utf8, int cbutf8 = -1);

int UTF8Length(const CStringA& utf8);
int UTF8Length(LPCSTR utf8, int cbutf8 = -1);

