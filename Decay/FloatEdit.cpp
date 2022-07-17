// Copyright (c) 2018 SS

#include "stdafx.h"
#include "DecayApp.h"
#include "FloatEdit.h"
#include "NumString.h"

IMPLEMENT_DYNAMIC(CFloatEdit, CEdit)

BEGIN_MESSAGE_MAP(CFloatEdit, CEdit)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChangeReflect)
END_MESSAGE_MAP()

CFloatEdit::CFloatEdit()
{

}

CFloatEdit::~CFloatEdit()
{
}


void CFloatEdit::Initialize()
{
	if (!m_toolTip.Create(this, WS_POPUP | TTS_NOPREFIX | TTS_BALLOON))
		return;

	TOOLINFO ti = { 0 };
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_TRANSPARENT | TTF_CENTERTIP | TTF_TRACK;
	ti.hwnd = this->GetSafeHwnd();
	ti.uId = 1;
	ti.hinst = NULL;

	CString str = _T("Test");
	ti.lpszText = (LPTSTR) (LPCTSTR) str;

	GetClientRect(&ti.rect);

	m_toolTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM) &ti);
	m_toolTip.Activate(TRUE);
}


BOOL CFloatEdit::GetValue( double& val )
{
	CString str;
	GetWindowText(str);
	str.Remove(_T(' '));
	return StringToDouble(val, str);
}

void CFloatEdit::SetValue(double val, const CString& format)
{
	CString str;
	str.Format(format, val);
	SetWindowText(str);
}

int CFloatEdit::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	Initialize();

	return 0;
}

BOOL CFloatEdit::OnChangeReflect()
{
	//TOOLINFO ti;
	//ti.cbSize = sizeof(TOOLINFO);
	//ti.hwnd = this->GetSafeHwnd();
	//ti.uId = 1;
	//m_toolTip.SendMessage(TTM_TRACKACTIVATE, (WPARAM) FALSE, (LPARAM) &ti);
	//m_toolTip.SendMessage(TTM_TRACKACTIVATE, (WPARAM) TRUE, (LPARAM) &ti);

	//CPoint pt(18, 14);
	//ClientToScreen(&pt);
	//m_toolTip.SendMessage(TTM_TRACKPOSITION, 0, (LPARAM) MAKELONG(pt.x, pt.y));

	return FALSE;
}

