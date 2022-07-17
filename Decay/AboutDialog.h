// Copyright (c) 2022 SS

#pragma once
#include "resource.h"
#include "LabAboutDialog.h"


class CAboutDlg : public CLabAboutDlg
{
	DECLARE_MESSAGE_MAP()

public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };
};
