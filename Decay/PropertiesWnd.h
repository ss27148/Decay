// Copyright (c) 2022 SS

#pragma once
#include "PropsDialog.h"

class CPropertiesWnd : public CDockablePane
{
	DECLARE_MESSAGE_MAP()

public:
	CPropertiesWnd();

	void AdjustLayout();


	CPropsDialog* PropsDialog() { return m_propsDlg; }

protected:
	CPropsDialog* m_propsDlg;

public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

};

