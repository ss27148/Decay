// Copyright (c) 2022 SS

#include "stdafx.h"
#include "DecayApp.h"
#include "PropsDialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "DecayDoc.h"
#include "LabLog.h"
#include "NumString.h"

// -------------
// CPropsDialog
// -------------

IMPLEMENT_DYNCREATE(CPropsDialog, CFormView)

BEGIN_MESSAGE_MAP(CPropsDialog, CFormView)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)

	ON_BN_CLICKED(IDC_HALFLIFE, OnHalfLifeRadio)
	ON_BN_CLICKED(IDC_DECAY_CONST, OnDecayConstantRadio)

	ON_UPDATE_COMMAND_UI(IDC_HALFLIFE, OnUpdateHalfLifeRadio)
	ON_UPDATE_COMMAND_UI(IDC_DECAY_CONST, OnUpdateDecayConstantRadio)
	ON_UPDATE_COMMAND_UI(IDC_TIME_EDIT, OnUpdateTimeEdit)
	ON_UPDATE_COMMAND_UI(IDC_TIME_SPIN, OnUpdateTimeEdit)

	ON_WM_CTLCOLOR()

END_MESSAGE_MAP()

CPropsDialog::CPropsDialog(CWnd* pParent)
	: CFormView(CPropsDialog::IDD)
{
	m_pDoc = NULL;
	m_bInitialized = FALSE;
}

CPropsDialog::~CPropsDialog()
{
	Model()->RemoveObserver(*this);
}

BOOL CPropsDialog::Create(CWnd* parent, UINT id)
{
	return CFormView::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), parent, id, NULL);
}

Decay::CModel* CPropsDialog::Model()
{
	CDecayDoc* doc = DYNAMIC_DOWNCAST(CDecayDoc, GetDocument());
	if (doc)
		return doc->Model();

	return nullptr;
}

void CPropsDialog::ShowBaloonTip(UINT id, LPCTSTR szText, LPCTSTR szTitle, HICON hIcon)
{
	if (szTitle)
		m_toolTip.SetTitle((UINT) hIcon, szTitle);

	TOOLINFO ti;
	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = this->GetSafeHwnd();
	ti.uId = 1;
	m_toolTip.SendMessage(TTM_TRACKACTIVATE, (WPARAM) FALSE, (LPARAM) &ti);

	m_toolTip.SendMessage(TTM_GETTOOLINFO, 0, (LPARAM) &ti);
	ti.lpszText = (LPTSTR) szText;
	m_toolTip.SetToolInfo(&ti);
	m_toolTip.SendMessage(TTM_TRACKACTIVATE, (WPARAM) TRUE, (LPARAM) &ti);

	CRect rect;
	GetDlgItem(id)->GetWindowRect(&rect);
	CPoint pt((rect.left + rect.right) / 2, rect.bottom);
	m_toolTip.SendMessage(TTM_TRACKPOSITION, 0, (LPARAM) MAKELONG(pt.x, pt.y));

	SetTimer(ID_BALLOON_TIP_TIMER, kTooltipTimeout, NULL);
}

void CPropsDialog::ShowBaloonTip(UINT id, UINT idText, UINT idTitle, HICON hIcon)
{
	CString strText, strTitle;

	strText.LoadString(idText);

	if (idTitle)
		strTitle.LoadString(idTitle);

	ShowBaloonTip(id, strText, strTitle, hIcon);
}

void CPropsDialog::HideBaloonTip()
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = this->GetSafeHwnd();
	ti.uId = 1;
	m_toolTip.SendMessage(TTM_TRACKACTIVATE, (WPARAM) FALSE, (LPARAM) &ti);
}

void CPropsDialog::DecayStarting()
{
	UpdateDialogControls(this, FALSE);

	m_stopButton.SetFocus();
	m_stopButton.SetButtonStyle(m_stopButton.GetButtonStyle() | BS_DEFPUSHBUTTON);
	m_resetButton.SetButtonStyle(m_resetButton.GetButtonStyle() & ~BS_DEFPUSHBUTTON);
	m_startButton.SetButtonStyle(m_startButton.GetButtonStyle() & ~BS_DEFPUSHBUTTON);
}

void CPropsDialog::PassComplete()
{
	SetDlgItemInt(IDC_REMAINING_IND, Model()->RemainingAtoms(), FALSE);
	SetDlgItemInt(IDC_DECAYED_IND, Model()->DecayedAtoms(), FALSE);

	CString strTime;
	strTime.Format(_T("%0.2lf"), Model()->Time());

	CString strSeconds;
	VERIFY(strSeconds.LoadString(IDS_SECONDS));

	strTime += _T(" ") + strSeconds;

	SetDlgItemText(IDC_TIME_IND, strTime);
}

void CPropsDialog::DecayStopped()
{
	UpdateDialogControls(this, FALSE);

	m_resetButton.SetButtonStyle(m_resetButton.GetButtonStyle() & ~BS_DEFPUSHBUTTON);
	m_startButton.SetFocus();
	m_startButton.SetButtonStyle(m_startButton.GetButtonStyle() | BS_DEFPUSHBUTTON);
	m_stopButton.SetButtonStyle(m_stopButton.GetButtonStyle() & ~BS_DEFPUSHBUTTON);

	PassComplete();
}

void CPropsDialog::DecayReset()
{
	UpdateDialogControls(this, FALSE);

	m_resetButton.SetButtonStyle(m_resetButton.GetButtonStyle() & ~BS_DEFPUSHBUTTON);
	m_startButton.SetFocus();
	m_startButton.SetButtonStyle(m_startButton.GetButtonStyle() | BS_DEFPUSHBUTTON);
	m_stopButton.SetButtonStyle(m_stopButton.GetButtonStyle() & ~BS_DEFPUSHBUTTON);

	PassComplete();
}

void CPropsDialog::IsotopeChanged()
{
	CString strText;

	if (Decay::CIsotope* isot = Model()->Isotope())
	{
		strText.FormatMessage(IDS_ELEMENT_IND, isot->Symbol());

		CDecayApp* app = DYNAMIC_DOWNCAST(CDecayApp, AfxGetApp());
		app->LabLog() << (UINT)isot->Z() << _T("-")
			<< isot->Symbol() 
			<< _T("-") << (UINT)isot->A() << _T("\n")
			<< _T("Decay Mode: ") << isot->DecayMode().Format() << _T("\n")
			<< _T("T=") << isot->HalfLife().valueSec << _T(" s;")
			<< _T("  \x03bb=") << isot->DecayConstant() << _T(" s(-1);")
			<< _T("  \x03c4=") << isot->Lifetime() << _T(" s\n")
			<< _T("Q=") << isot->Q(Decay::CIsotopeDB::Instance()) << _T(" MeV\n");
	}

	SetDlgItemText(IDC_ELEMENT_IND, strText);
}

void CPropsDialog::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	Model()->AddObserver(*this);

	m_startButton.SetFocus();
	
	IsotopeChanged();
	PassComplete();
}

void CPropsDialog::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESET, m_resetButton);
	DDX_Control(pDX, IDC_START, m_startButton);
	DDX_Control(pDX, IDC_STOP, m_stopButton);
	DDX_Control(pDX, IDC_HALFLIFE, m_halfLifeRadio);
	DDX_Control(pDX, IDC_DECAY_CONST, m_decayConstantRadio);
	DDX_Control(pDX, IDC_TIME_EDIT, m_timeEdit);
	DDX_Control(pDX, IDC_TIME_SPIN, m_timeSpin);
	DDX_Control(pDX, IDC_TIME_LABEL, m_timeLabel);
	DDX_Control(pDX, IDC_TIME_UNIT, m_timeUnitLabel);

	if (pDX->m_bSaveAndValidate)
	{
		if (Model()->Mode() == Decay::EMode::DecayConstant)
		{
			double t = 0;
			m_timeEdit.GetValue(t);
			Model()->SetTimeLapse(t);
		}
	}
	else
	{
		m_timeSpin.SetRange(1, 10);
		m_timeEdit.SetValue(Model()->TimeLapse(), _T("%lg"));
	}
}


int CPropsDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);

	m_minSize = rect.Size();

	return 0;
}

void CPropsDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_BALLOON_TIP_TIMER)
	{
		HideBaloonTip();
		KillTimer(nIDEvent);
	}
}


void CPropsDialog::OnIdleUpdateCmdUI()
{
	UpdateDialogControls(this, FALSE);
}

void CPropsDialog::OnHalfLifeRadio()
{
	Model()->SetMode(Decay::EMode::HalfLife);
}

void CPropsDialog::OnDecayConstantRadio()
{
	Model()->SetMode(Decay::EMode::DecayConstant);
}

void CPropsDialog::OnUpdateHalfLifeRadio(CCmdUI* pCmdUI)
{
	bool bEnable = false;
	bool bOn = true;
	Decay::CModel* model = Model();
	if (model)
	{
		bEnable = !model->IsRunning();
		bOn = (model->Mode() == Decay::EMode::HalfLife);
	}

	pCmdUI->Enable(bEnable || bOn);
	pCmdUI->SetRadio(bOn);
}

void CPropsDialog::OnUpdateDecayConstantRadio(CCmdUI* pCmdUI)
{
	bool bEnable = false;
	bool bOn = false;
	Decay::CModel* model = Model();
	if (model)
	{
		bEnable = !model->IsRunning();
		bOn = (model->Mode() == Decay::EMode::DecayConstant);
	}

	pCmdUI->Enable(bEnable || bOn);
	pCmdUI->SetRadio(bOn);

}

void CPropsDialog::OnUpdateTimeEdit(CCmdUI* pCmdUI)
{
	bool bEnable = false;
	bool bShow = false;
	Decay::CModel* model = Model();
	if (model)
	{
		bEnable = !model->IsRunning();
		bShow = (model->Mode() == Decay::EMode::DecayConstant);
	}

	pCmdUI->Enable(bEnable);
	m_timeEdit.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	m_timeSpin.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	m_timeLabel.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	m_timeUnitLabel.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

HBRUSH CPropsDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	constexpr COLORREF remainingIndLabelBkColor   = RGB(255, 0, 0);
	constexpr COLORREF decayedIndLabelBkColor     = RGB(0, 255, 255);
	constexpr COLORREF timeIndLabelBkColor        = RGB(255, 255, 255);
	constexpr COLORREF remainingIndLabelTextColor = RGB(255, 255, 255);
	constexpr COLORREF decayedIndLabelTextColor   = RGB(0, 0, 0);
	constexpr COLORREF timeIndLabelTextColor      = RGB(0, 0, 0);

	static CBrush remainingIndLabelBrush(remainingIndLabelBkColor);
	static CBrush decayedIndLabelBrush(decayedIndLabelBkColor);
	static CBrush timeIndLabelBrush(timeIndLabelBkColor);

	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:

		if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_TIME_IND_LABEL)->GetSafeHwnd())
		{
			pDC->SetTextColor(timeIndLabelTextColor);
			pDC->SetBkColor(timeIndLabelBkColor);
			return timeIndLabelBrush;
		}
		else if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_REMAINING_IND_LABEL)->GetSafeHwnd())
		{
			pDC->SetTextColor(remainingIndLabelTextColor);
			pDC->SetBkColor(remainingIndLabelBkColor);
			return remainingIndLabelBrush;
		}
		else if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_DECAYED_IND_LABEL)->GetSafeHwnd())
		{
			pDC->SetTextColor(decayedIndLabelTextColor);
			pDC->SetBkColor(decayedIndLabelBkColor);
			return decayedIndLabelBrush;
		}

		break;

	default:
		break;
	}

	return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
}
