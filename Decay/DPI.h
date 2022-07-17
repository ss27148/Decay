// Copyright (c) 2018 SS

#pragma once

class CScreen
{
public:
	CScreen();

	static const int kNormalDPI;
	static const int kMediumDPI;
	static const int kHighDPI;

	static int ToScreenDPI(int size, int dpi = kNormalDPI);
	static CPoint ToScreenDPI(const CPoint& pt, int dpi = kNormalDPI);
	static CSize ToScreenDPI(const CSize& size, int dpi = kNormalDPI);
	static CRect ToScreenDPI(const CRect& rect, int dpi = kNormalDPI);

	static int GetDPI();
	static int GetDPIX();
	static int GetDPIY();
	
	enum EDPIMode
	{
		NormalDPI,
		MediumDPI,
		HighDPI
	};

	static EDPIMode DPIMode();

	static UINT SelectResource(UINT uNormal, UINT uMedium = 0, UINT uHigh = 0);

protected:
	CPoint m_dpi;

	static CScreen m_screen;
};
