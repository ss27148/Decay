// Copyright (c) 2018 SS

#pragma once

class CFloatEdit : public CEdit
{
	DECLARE_DYNAMIC(CFloatEdit)
	DECLARE_MESSAGE_MAP()

public:
	CFloatEdit();
	virtual ~CFloatEdit();

	void Initialize();

	BOOL GetValue(double& val);
	void SetValue(double val, const CString& format = _T("%lf"));

	CToolTipCtrl m_toolTip;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnChangeReflect();
};


