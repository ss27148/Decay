// Copyright (c) 2022 SS

#include "stdafx.h"
#include "DecayApp.h"
#include "IsotopeListCtrl.h"
#include "IsotopeDB.h"
#include "NumString.h"
#include <map>

IMPLEMENT_DYNAMIC(CIsotopeListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CIsotopeListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDrawReflect)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfoReflect)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CIsotopeListCtrl::CIsotopeListCtrl()
{
}

CIsotopeListCtrl::~CIsotopeListCtrl()
{
}

void CIsotopeListCtrl::Load()
{
	LoadHLUnits();

	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	LoadHeader();

	Decay::CIsotopeDB& isotDB = Decay::CIsotopeDB::Instance();
	
	SetItemCount(isotDB.size());

	constexpr int kDecimalSeparatorLength = 2;
	GetLocaleInfo(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, m_decimalSeparator.GetBuffer(kDecimalSeparatorLength), kDecimalSeparatorLength);
	m_decimalSeparator.ReleaseBuffer(1);
}

void CIsotopeListCtrl::LoadHLUnits()
{
	CString strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_YOCTOSECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::ys] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_ZEPTOSECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::zs] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_ATTOSECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::as] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_FEMTOSECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::fs] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_PICOSECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::ps] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_NANOSECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::ns] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_MICROSECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::us] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_MILLISECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::ms] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_SECONDS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::s] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_MINUTES))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::m] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_HOURS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::h] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_DAYS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::d] = strHLUnit;

	if (!strHLUnit.LoadString(IDS_HALFLIFE_YEARS))
		return;
	m_hlUnits[Decay::CIsotope::HLUnit::y] = strHLUnit;
}

CString CIsotopeListCtrl::GetDecayModeText(UINT index)
{
	Decay::CIsotope& isot = Decay::CIsotopeDB::Instance().at(index);

	CString mode = isot.DecayMode().Format();

	if (mode == _T("STABLE"))
		mode.LoadString(IDS_STABLE);

	return mode;
}

void CIsotopeListCtrl::LoadHeader()
{
	CRect rect;
	GetClientRect(&rect);
	rect.right -= 3;

	CString str;
	str.LoadString(IDS_COLUMN_Z);
	InsertColumn(kZColumn, str, LVCFMT_LEFT, rect.Width() * 7 / 100);

	str.LoadString(IDS_COLUMN_SYMBOL);
	InsertColumn(kSymbolColumn, str, LVCFMT_LEFT, rect.Width() * 12 / 100);

	str.LoadString(IDS_COLUMN_A);
	InsertColumn(kAColumn, str, LVCFMT_LEFT, rect.Width() * 7 / 100);

	str.LoadString(IDS_COLUMN_MASS);
	InsertColumn(kMassColumn, str, LVCFMT_LEFT, rect.Width() * 20 / 100);

	str.LoadString(IDS_COLUMN_DECAY_MODE);
	InsertColumn(kDecayModeColumn, str, LVCFMT_LEFT, rect.Width() * 17 / 100);

	str.LoadString(IDS_COLUMN_HALFLIFE);
	InsertColumn(kHalfLifeColumn, str, LVCFMT_LEFT, rect.Width() * 21 / 100);

	str.LoadString(IDS_COLUMN_D);
	InsertColumn(kDColumn, str, LVCFMT_LEFT, rect.Width() * 13 / 100);

}

void CIsotopeListCtrl::OnGetDispInfoReflect(NMHDR* pNotifyStruct, LRESULT* result)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNotifyStruct);

	LV_ITEM& item = pDispInfo->item;

	if (item.mask & LVIF_TEXT)
	{
		CString text;

		switch (item.iSubItem)
		{
		case 0:
			text.Format(_T("%u"), Decay::CIsotopeDB::Instance().at(item.iItem).Z().N);
			break;

		case 1:
			text = Decay::CIsotopeDB::Instance().at(item.iItem).Symbol();
			break;

		case 2:
			text.Format(_T("%u"), Decay::CIsotopeDB::Instance().at(item.iItem).A().N);
			break;

		case 3:
			text = Decay::CIsotopeDB::Instance().at(item.iItem).Mass().text;
			text.Replace(_T("."), m_decimalSeparator);
			break;

		case 4:
			text = GetDecayModeText(item.iItem);
			break;

		case 5:
			text = Decay::CIsotopeDB::Instance().at(item.iItem).HalfLife().text;
			if (!text.IsEmpty())
			{
				text.Replace(_T("."), m_decimalSeparator);
				text += _T(" ") + m_hlUnits[Decay::CIsotopeDB::Instance().at(item.iItem).HalfLife().unit];
			}
			break;

		case 6:
			text = Decay::CIsotopeDB::Instance().at(item.iItem).D();
			text.Replace(_T("."), m_decimalSeparator);
			break;

		default:
			break;
		}

		_tcsncpy_s(item.pszText, item.cchTextMax, text, text.GetLength());
	}

	*result = 0;
}

void CIsotopeListCtrl::OnCustomDrawReflect(NMHDR* pNotifyStruct, LRESULT* result)
{
	*result = DoCustomDraw(reinterpret_cast<NMLVCUSTOMDRAW*>(pNotifyStruct));
}

void CIsotopeListCtrl::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CDecayApp* pApp = DYNAMIC_DOWNCAST(CDecayApp, AfxGetApp());
	pApp->GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_TABLE, pos.x, pos.y, GetParent(), TRUE);
}

void CIsotopeListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

LRESULT CIsotopeListCtrl::DoCustomDraw( NMLVCUSTOMDRAW* pNMCD )
{
	switch (pNMCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;

	case CDDS_ITEMPREPAINT:
		return CDRF_NOTIFYSUBITEMDRAW;

	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		if (pNMCD->iSubItem == 5)
		{
			CDC dc;
			dc.Attach(pNMCD->nmcd.hdc);

			CRect rect;
			GetSubItemRect(pNMCD->nmcd.dwItemSpec, pNMCD->iSubItem, LVIR_BOUNDS, rect);

			if (GetItemState(pNMCD->nmcd.dwItemSpec, LVIS_SELECTED) & LVIS_SELECTED)
			{
				if (GetFocus() == this)
				{
					dc.FillSolidRect(&rect, GetSysColor(COLOR_HIGHLIGHT));
					dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				}
				else
				{
					dc.FillSolidRect(&rect, GetSysColor(COLOR_BTNFACE));
					dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
				}
			}
			else
			{
				dc.FillSolidRect(&rect, GetSysColor(COLOR_WINDOW));
				dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			}

			GetSubItemRect(pNMCD->nmcd.dwItemSpec, pNMCD->iSubItem, LVIR_LABEL, rect);

			int margin = GetHeaderCtrl()->GetBitmapMargin();
			rect.DeflateRect(margin, 0, margin, 0);

			DrawHalfLife(dc, rect, GetItemText(pNMCD->nmcd.dwItemSpec, pNMCD->iSubItem));

			dc.Detach();

			return CDRF_SKIPDEFAULT;
		}
		else
		{
			return CDRF_DODEFAULT;
		}
		break;
	}

	return CDRF_DODEFAULT;
}

void CIsotopeListCtrl::DrawHalfLife(CDC& dc, CRect rect, const CString& text)
{
	CRect currRect(rect);

	int pos = 0;
	CString strValue = text.Tokenize(_T(" "), pos);
	DrawExponentialNumber(dc, currRect, strValue);

	if (!strValue.IsEmpty() && pos > 0)
	{
		CString strUnit = text.Mid(pos - 1);
		DrawTextAdvancePos(dc, currRect, strUnit, DT_VCENTER);
	}
}

void CIsotopeListCtrl::DrawExponentialNumber(CDC& dc, CRect& rect, const CString& text)
{
	CString strManitssa;
	CString strExponent;
	int iEChar = text.FindOneOf(_T("eE"));
	if (iEChar == -1)
	{
		strManitssa = text;
	}
	else
	{
		strManitssa = text.Left(iEChar) + _T("·10");
		strExponent = text.Mid(iEChar + 1);
		int exponent = 0;
		StringToInt(exponent, strExponent);
		strExponent.Format(_T("%i"), exponent);
	}

	DrawTextAdvancePos(dc, rect, strManitssa, DT_VCENTER);

	CFont* pPrevFont = dc.GetCurrentFont();
	if (pPrevFont)
	{
		LOGFONT lf;
		pPrevFont->GetLogFont(&lf);
		lf.lfHeight = lf.lfHeight * 75 / 100;
		lf.lfUnderline = FALSE;

		CFont font;
		font.CreateFontIndirect(&lf);

		dc.SelectObject(&font);
		DrawTextAdvancePos(dc, rect, strExponent, DT_TOP);
		dc.SelectObject(pPrevFont);
	}
	else
	{
		strExponent = _T("^") + strExponent;
		DrawTextAdvancePos(dc, rect, strExponent, DT_VCENTER);
	}
}

void CIsotopeListCtrl::DrawTextAdvancePos(CDC& dc, CRect& rect, const CString& text, DWORD format)
{
	CRect textRect(rect);
	dc.DrawText(text, &textRect, format | DT_SINGLELINE | DT_CALCRECT);
	dc.DrawText(text, &textRect, format | DT_SINGLELINE);

	rect = CRect(textRect.right, rect.top, rect.right, rect.bottom);
}

