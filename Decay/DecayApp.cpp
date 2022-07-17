// Copyright (c) 2022 SS

#include "stdafx.h"
#include "afxwinappex.h"
#include "DecayApp.h"
#include "MainFrm.h"
#include "AboutDialog.h"
#include "DecayDoc.h"
#include "AtomView.h"
#include "Model.h"
#include "IsotopeDB.h"
#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------
//  App
// -----------------------------------

CDecayApp theApp;


BEGIN_MESSAGE_MAP(CDecayApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()

CDecayApp::CDecayApp()
{
	m_bDev = FALSE;

	m_pszProductName = _T("Decay");

	SetAppID(_T("SS.Decay"));

}

BOOL CDecayApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	AfxOleInit();

	AfxInitRichEdit5();

	_tsetlocale(LC_ALL, _T(""));

	CDecayCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CWinAppEx::InitInstance();

	EnableTaskbarInteraction(FALSE);

	// HKEY_CURRENT_USER\SS
	SetRegistryKey(_T("SS"));

	// HKEY_CURRENT_USER\SS\<ProductName>
	if (m_pszProfileName)
		free((void*) m_pszProfileName);
	m_pszProfileName = _tcsdup(m_pszProductName);

	// HKEY_CURRENT_USER\SS\<ProductName>\Settings
	SetRegistryBase(_T("Settings"));

	LoadStdProfileSettings(4);

	InitContextMenuManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	OpenIsotopeDB();

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDecayDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CAtomView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	CString labLogFile = CString(m_pszProductName) + _T(".log");
	if (!m_labLogFile.IsEmpty())
		labLogFile = m_labLogFile;

	LabLog().Open(labLogFile);
	LabLog() << _T("= ") << m_pszProductName << _T(" started ") << CTime::GetCurrentTime() << _T(" =\n");

	ProcessShellCommand(cmdInfo);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	

	return TRUE;
}

int CDecayApp::ExitInstance()
{
	LabLog() << _T("= End ") << CTime::GetCurrentTime() << _T(" =\n");
	return CWinAppEx::ExitInstance();
}

void CDecayApp::PumpMessageLoop()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (IsIdleMessage(&msg))
			continue;
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

bool CDecayApp::OpenIsotopeDB()
{
	CString dbPath;
	if (::PathIsRelative(m_dbFile))
	{
		TCHAR* szdbPath = dbPath.GetBuffer(MAX_PATH);
		::GetModuleFileName(NULL, szdbPath, MAX_PATH);
		::PathRemoveFileSpec(szdbPath);
		::PathAppend(szdbPath, m_dbFile.IsEmpty() ? _T("isot.csv") : (LPCTSTR)m_dbFile);
		dbPath.ReleaseBuffer();
	}
	else
	{
		dbPath = m_dbFile;
	}

	return Decay::CIsotopeDB::Instance().Open(dbPath);
}

void CDecayApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CDecayApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu(_T(""), IDR_POPUP_EDIT);
	GetContextMenuManager()->AddMenu(_T(""), IDR_POPUP_TABLE);
}

HINSTANCE CDecayApp::LoadAppLangResourceDLL()
{
	if (m_lang.IsEmpty())
		return CWinAppEx::LoadAppLangResourceDLL();
	else if (m_lang.CompareNoCase(_T("ENG")) == 0)
		return NULL;

	TCHAR szPath[MAX_PATH];
	LPTSTR pszExtension;

	int ret = ::GetModuleFileName(m_hInstance, szPath, MAX_PATH);
	if (ret == 0 || ret == MAX_PATH)
	{
		ASSERT(FALSE);
		return NULL;
	}
	pszExtension = ::PathFindExtension(szPath);
	*pszExtension = '\0';

	CString strLang = m_lang;
	strLang.Replace(_T("%"), _T("%%"));

	CString strFormat = _T("%s") + strLang + _T(".dll");

	return AfxLoadLangResourceDLL(strFormat, szPath);
}

// ---------------------
// CDecayCommandLineInfo
// ---------------------

CDecayCommandLineInfo::CDecayCommandLineInfo()
{
}

void CDecayCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag)
		m_flag = EFlagType::NoFlag;
	
	CDecayApp* app = (CDecayApp*)AfxGetApp();

	if (bFlag && _tcscmp(pszParam, _T("dev")) == 0)
	{
		app->m_bDev = TRUE;
	}
	else if (bFlag && _tcscmp(pszParam, _T("autosave")) == 0)
	{
		app->m_bAutosave = true;
	}
	else if (bFlag && _tcscmp(pszParam, _T("appr")) == 0)
	{
		app->m_bApprModel = true;
	}
	else if (bFlag && _tcscmp(pszParam, _T("lang")) == 0)
	{
		m_flag = EFlagType::LangFlag;
	}
	else if (!bFlag && m_flag == EFlagType::LangFlag)
	{
		app->m_lang = pszParam;
		m_flag = EFlagType::NoFlag;
	}
	else if (bFlag && _tcscmp(pszParam, _T("db")) == 0)
	{
		m_flag = EFlagType::DBFlag;
	}
	else if (!bFlag && m_flag == EFlagType::DBFlag)
	{
		app->m_dbFile = pszParam;
		m_flag = EFlagType::NoFlag;
	}
	else if (bFlag && _tcscmp(pszParam, _T("lablog")) == 0)
	{
		m_flag = EFlagType::DBFlag;
	}
	else if (!bFlag && m_flag == EFlagType::LablogFlag)
	{
		app->m_labLogFile = pszParam;
		m_flag = EFlagType::NoFlag;
	}
	else
	{
		CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	}

	ParseLast(bLast);
}
