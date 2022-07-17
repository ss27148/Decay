// Copyright (c) 2022 SS

#pragma once
#include "resource.h"
#include "DecayDoc.h"
#include "FloatEdit.h"


class CPropsDialog : 
	public CFormView,
	private Decay::CDecayObserver
{
	DECLARE_DYNCREATE(CPropsDialog)
	DECLARE_MESSAGE_MAP()

public:
	CPropsDialog(CWnd* pParent = NULL);
	virtual ~CPropsDialog();

	BOOL Create(CWnd* parent, UINT id);

	void SetDoc(CDecayDoc* pDoc) { m_pDoc = pDoc; }

	Decay::CModel* Model();

	enum { IDD = IDD_PROPS };

	const CSize& MinSize() const { return m_minSize; }

	void ShowBaloonTip(UINT id, LPCTSTR szText, LPCTSTR szTitle = NULL, HICON hIcon = NULL);
	void ShowBaloonTip(UINT id, UINT idText, UINT idTitle = 0, HICON hIcon = NULL);
	void HideBaloonTip();

	enum { ID_BALLOON_TIP_TIMER = 1 };


	void OnOK() { }
	void OnCancel() { }


	void DecayStarting() override;


	void PassComplete() override;


	void DecayStopped() override;


	void OnInitialUpdate() override;


	void DecayReset() override;


	void IsotopeChanged() override;

protected:
	static constexpr UINT kTooltipTimeout = 8000;

	CDecayDoc* m_pDoc;
	CSize m_minSize;
	BOOL m_bInitialized;

	CButton m_resetButton;
	CButton m_startButton;
	CButton m_stopButton;
	CButton m_halfLifeRadio;
	CButton m_decayConstantRadio;

	CFloatEdit m_timeEdit;
	CSpinButtonCtrl m_timeSpin;
	CStatic m_timeLabel;
	CStatic m_timeUnitLabel;
	CToolTipCtrl m_toolTip;

public:
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override { }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnIdleUpdateCmdUI();

	afx_msg void OnHalfLifeRadio();
	afx_msg void OnDecayConstantRadio();

	afx_msg void OnUpdateHalfLifeRadio(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDecayConstantRadio(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTimeEdit(CCmdUI* pCmdUI);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
