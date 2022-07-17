// Copyright (c) 2022 SS

#include "stdafx.h"
#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "DecayApp.h"


BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_propsDlg->SetWindowPos(
		NULL,
		rectClient.left,
		rectClient.top,
		rectClient.Width(),
		rectClient.Height(),
		SWP_NOACTIVATE|SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_propsDlg = DYNAMIC_DOWNCAST(CPropsDialog, CPropsDialog::CreateObject());
	if (!m_propsDlg)
		return -1;

	m_propsDlg->Create(this, AFX_IDW_DIALOGBAR);

	CRect rect(0, 0, m_propsDlg->MinSize().cx, m_propsDlg->MinSize().cy);
	AdjustWindowRect(&rect, GetStyle(), FALSE);
	SetMinSize(rect.Size());

	AdjustLayout();

	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}
