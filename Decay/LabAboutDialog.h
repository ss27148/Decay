// LabLib
// Copyright (c) 2018 SS

#pragma once
#include "resource.h"


class CLabAboutDlg : public CDialog
{
	DECLARE_MESSAGE_MAP()

public:
	CLabAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	CFont m_titleFont;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};