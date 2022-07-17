// Copyright (c) 2022 SS

#include "stdafx.h"
#include "DecayApp.h"

#include "MainFrm.h"
#include "DecayDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, OnToolbarCreateNew)
	ON_WM_SETTINGCHANGE()

	ON_WM_CLOSE()

	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateDockingWindows)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ISOTOPES, OnUpdateDockingWindows)
	ON_UPDATE_COMMAND_UI(ID_HELP_SHORTCUTS, OnUpdateHelpShortcuts)

	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_COMMAND(ID_VIEW_ISOTOPES, OnViewIsotopes)
	ON_COMMAND(ID_HELP_SHORTCUTS, OnHelpShortcuts)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC;
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, dwStyle, CRect(1, 1, 1, 1), AFX_IDW_TOOLBAR) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	InitializeToolbar();

	//CString strCustomize;
	//bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	//ASSERT(bNameValid);
	//m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Menu toolbar (hidden)
	if (!m_wndMenuToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD, CRect(1, 1, 1, 1), IDR_MENU_TOOLBAR) ||
		!m_wndMenuToolBar.LoadToolBar(IDR_MENU_TOOLBAR))
	{
		TRACE0("Failed to create menu toolbar\n");
		return -1;      // fail to create
	}


	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);


	// Enable toolbar and docking window menu replacement
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	//CMFCToolBar::EnableQuickCustomization();
	
	return 0;
}

CDecayDoc* CMainFrame::GetDoc()
{
	return DYNAMIC_DOWNCAST(CDecayDoc, GetActiveDocument());
}

Decay::CModel* CMainFrame::GetModel()
{
	CDecayDoc* pDoc = DYNAMIC_DOWNCAST(CDecayDoc, GetActiveDocument());
	if (pDoc)
		return pDoc->Model();

	return NULL;
}

CAtomView* CMainFrame::MainView()
{
	CDecayDoc* pDoc = GetDoc();
	if (!pDoc)
		return NULL;

	CAtomView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos)
	{
		pView = DYNAMIC_DOWNCAST(CAtomView, pDoc->GetNextView(pos));

		if (pView)
			return pView;
	}

	return NULL;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	UpdateFrameTitleForDocument(NULL);
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI;
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), 
		TRUE, ID_VIEW_PROPERTIES, dwStyle))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	return TRUE;
}


void CMainFrame::InitializeToolbar()
{
	CString strToolBarName;
	strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	m_wndToolBar.SetWindowText(strToolBarName);

	int iButton = m_wndToolBar.CommandToIndex(IDC_RESET);
	m_wndToolBar.GetButton(iButton)->m_bText = TRUE;
	m_wndToolBar.GetButton(iButton)->m_strText = GetToolbarButtonText(IDC_RESET);

	iButton = m_wndToolBar.CommandToIndex(IDC_START);
	m_wndToolBar.GetButton(iButton)->m_bText = TRUE;
	m_wndToolBar.GetButton(iButton)->m_strText = GetToolbarButtonText(IDC_START);

	iButton = m_wndToolBar.CommandToIndex(IDC_STOP);
	m_wndToolBar.GetButton(iButton)->m_bText = TRUE;
	m_wndToolBar.GetButton(iButton)->m_strText = GetToolbarButtonText(IDC_STOP);

	iButton = m_wndToolBar.CommandToIndex(ID_VIEW_PROPERTIES);
	m_wndToolBar.GetButton(iButton)->m_bText = TRUE;
	m_wndToolBar.GetButton(iButton)->m_strText = GetToolbarButtonText(ID_VIEW_PROPERTIES);

	iButton = m_wndToolBar.CommandToIndex(ID_VIEW_ISOTOPES);
	m_wndToolBar.GetButton(iButton)->m_bText = TRUE;
	m_wndToolBar.GetButton(iButton)->m_strText = GetToolbarButtonText(ID_VIEW_ISOTOPES);

	m_wndToolBar.RecalcLayout();
}

CString CMainFrame::GetToolbarButtonText(UINT id)
{
	CString str;
	str.LoadString(id);
	int iSeparator = str.Find(_T('\n'));
	if (iSeparator != -1)
		str = str.Mid(iSeparator + 1);
	return str;
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	return lres;
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
}

void CMainFrame::OnUpdateDockingWindows(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnClose()
{
	//if (GetDoc()->Mode() == CDecayDoc::Ident && 
	//	!decay::CParticleList::Instance().IsParticleIdentified() && 
	//	AfxMessageBox(IDP_QUERY_CLOSE, MB_YESNO|MB_DEFBUTTON2) == IDNO)
	//	return;
	if (GetDoc()->Model()->State() == Decay::EState::Running)
	{
		GetDoc()->Model()->Stop();
		GetDoc()->SetCloseAfterStop();
		return;
	}

	CFrameWndEx::OnClose();
}

void CMainFrame::OnViewProperties()
{
	ShowPane(&m_wndProperties, TRUE, FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnViewIsotopes()
{
	if (!m_tableDialog.GetSafeHwnd())
		m_tableDialog.Create(IDD_TABLE, this);

	m_tableDialog.ShowWindow(SW_SHOW);
}

void CMainFrame::OnHelpShortcuts()
{

}


void CMainFrame::OnUpdateHelpShortcuts(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
