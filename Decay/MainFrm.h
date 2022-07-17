// Copyright (c) 2022 SS

#pragma once
#include "PropertiesWnd.h"
#include "Model.h"
#include "AtomView.h"
#include "TableDialog.h"

class CMainMenuBar : public CMFCMenuBar
{
public:
	virtual BOOL LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
	{
		return TRUE;
	}

	virtual BOOL SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
	{
		return TRUE;
	}
};

class CMainToolBar : public CMFCToolBar
{
public:
	virtual BOOL LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
	{
		return TRUE;
	}

	virtual BOOL SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
	{
		return TRUE;
	}
};

class CMainStatusBar : public CMFCStatusBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) { CMFCStatusBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler); }

};


class CMainFrame : public CFrameWndEx
{
	DECLARE_DYNCREATE(CMainFrame)
	DECLARE_MESSAGE_MAP()

private:
	CMainFrame();

public:
	virtual ~CMainFrame();

public:
	CDecayDoc* GetDoc();
	Decay::CModel* GetModel();
	CAtomView* MainView();

	CPropertiesWnd* PropertiesWnd() { return &m_wndProperties; }


public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

private:
	BOOL CreateDockingWindows();
	void InitializeToolbar();
	CString GetToolbarButtonText(UINT id);

public:
	CMainMenuBar      m_wndMenuBar;
	CMainToolBar      m_wndToolBar;
	CMainToolBar      m_wndMenuToolBar;
	CMainStatusBar    m_wndStatusBar;
	CPropertiesWnd    m_wndProperties;
	CTableDialog      m_tableDialog;

private:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	afx_msg void OnUpdateDockingWindows(CCmdUI* pCmdUI);

	afx_msg void OnClose();

	afx_msg void OnViewProperties();
	afx_msg void OnViewIsotopes();

public:
	afx_msg void OnHelpShortcuts();
	afx_msg void OnUpdateHelpShortcuts(CCmdUI *pCmdUI);
};


