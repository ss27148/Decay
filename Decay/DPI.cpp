// Copyright (c) 2018 SS

#include "stdafx.h"
#include "DPI.h"

CScreen CScreen::m_screen;

const int CScreen::kNormalDPI = 96;
const int CScreen::kMediumDPI = 120;
const int CScreen::kHighDPI = 144;

CScreen::CScreen()
{
	m_dpi.SetPoint(96, 96);

	CWnd* pWnd = CWnd::FromHandle(GetDesktopWindow());
	if (pWnd)
	{
		CDC* pDC = pWnd->GetDC();
		if (pDC)
		{
			m_dpi.x = pDC->GetDeviceCaps(LOGPIXELSX);
			m_dpi.y = pDC->GetDeviceCaps(LOGPIXELSY);
		}
		pWnd->ReleaseDC(pDC);
	}
}

int CScreen::ToScreenDPI(int size, int dpi)
{
	return MulDiv(size, GetDPI(), dpi);
}

CPoint CScreen::ToScreenDPI(const CPoint& pt, int dpi)
{
	return CPoint(MulDiv(pt.x, GetDPIX(), dpi),
		MulDiv(pt.y, GetDPIY(), dpi));
}

CSize CScreen::ToScreenDPI(const CSize& size, int dpi)
{
	return CSize(MulDiv(size.cx, GetDPIX(), dpi),
		MulDiv(size.cy, GetDPIY(), dpi));
}

CRect CScreen::ToScreenDPI(const CRect& rect, int dpi)
{
	return CRect(MulDiv(rect.left, GetDPIX(), dpi),
		MulDiv(rect.top, GetDPIY(), dpi),
		MulDiv(rect.right, GetDPIX(), dpi),
		MulDiv(rect.bottom, GetDPIY(), dpi));
}

int CScreen::GetDPI()
{
	return (m_screen.m_dpi.x + m_screen.m_dpi.y) / 2;
}

int CScreen::GetDPIX()
{
	return m_screen.m_dpi.x;
}

int CScreen::GetDPIY()
{
	return m_screen.m_dpi.y;
}

CScreen::EDPIMode CScreen::DPIMode()
{
	int dpi = GetDPI();
	if (dpi <= 96)
		return NormalDPI;
	else if (dpi > 96 && dpi < 150)
		return MediumDPI;
	else
		return HighDPI;
}

UINT CScreen::SelectResource(UINT uNormal, UINT uMedium, UINT uHigh)
{
	switch (CScreen::DPIMode())
	{
	case CScreen::MediumDPI:
		if (uMedium)
			return uMedium;
		else
			return uNormal;

	case CScreen::HighDPI:
		if (uHigh)
			return uHigh;
		else if (uMedium)
			return uMedium;
		else
			return uNormal;

	case CScreen::NormalDPI:
	default:
		return uNormal;
	}
}

