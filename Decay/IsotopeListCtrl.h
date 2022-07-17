// Copyright (c) 2022 SS

#pragma once
#include "Model.h"

class CIsotopeListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CIsotopeListCtrl)
	DECLARE_MESSAGE_MAP()

public:
	CIsotopeListCtrl();
	virtual ~CIsotopeListCtrl();

	void Load();

	static constexpr UINT kZColumn = 0;
	static constexpr UINT kSymbolColumn = 1;
	static constexpr UINT kAColumn = 2;
	static constexpr UINT kMassColumn = 3;
	static constexpr UINT kDecayModeColumn = 4;
	static constexpr UINT kHalfLifeColumn = 5;
	static constexpr UINT kDColumn = 6;

private:
	std::map<Decay::CIsotope::HLUnit, CString> m_hlUnits;
	CString m_decimalSeparator;

	void LoadHLUnits();

	CString GetDecayModeText(UINT index);

	void LoadHeader();
	void AutosizeColumns();

	afx_msg void OnGetDispInfoReflect(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnCustomDrawReflect(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	LRESULT DoCustomDraw(NMLVCUSTOMDRAW* pNMCD);
	void DrawHalfLife(CDC& dc, CRect rect, const CString& text);
	void DrawExponentialNumber(CDC& dc, CRect& rect, const CString& text);
	void DrawTextAdvancePos(CDC& dc, CRect& rect, const CString& text, DWORD format);
};


