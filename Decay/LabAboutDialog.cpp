// LabLib
// Copyright (c) 2018 SS
#include "stdafx.h"
#include "LabAboutDialog.h"

BEGIN_MESSAGE_MAP(CLabAboutDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


CLabAboutDlg::CLabAboutDlg() : CDialog(CLabAboutDlg::IDD)
{
}

BOOL CLabAboutDlg::OnInitDialog()
{
	LOGFONT lf;
	if (afxGlobalData.fontRegular.GetLogFont(&lf))
	{
		lf.lfHeight = 3 * lf.lfHeight / 2;
		lf.lfWeight = FW_BOLD;
		m_titleFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_ABOUT_TITLE)->SetFont(&m_titleFont);
	}


	return CDialog::OnInitDialog();
}

void CLabAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CLabAboutDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);

	CRect rectOK;
	GetDlgItem(IDOK)->GetWindowRect(&rectOK);
	ScreenToClient(&rectOK);

	rect.top = rectOK.top - (rect.bottom - rectOK.bottom);

	pDC->FillSolidRect(&rect, GetSysColor(COLOR_BTNFACE));

	rect.bottom = rect.top;
	rect.top = 0;

	pDC->FillSolidRect(&rect, GetSysColor(COLOR_WINDOW));

	return TRUE;
}

HBRUSH CLabAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC)
		return GetSysColorBrush(COLOR_WINDOW);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}



