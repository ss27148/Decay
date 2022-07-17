// Copyright (c) 2017 SS

#ifndef GDIPLUSTOKEN_H
#define GDIPLUSTOKEN_H

#pragma once

#include <GdiPlus.h>

class CGdiPlusToken
{
public:
	CGdiPlusToken()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	}

	~CGdiPlusToken()
	{
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}

private:
	ULONG_PTR m_gdiplusToken;
};

#endif // GDIPLUSTOKEN_H
