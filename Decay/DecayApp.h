// Copyright (c) 2022 SS

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"
#include "LabLog.h"
#include "GDIPlusToken.h"



class CDecayApp : public CWinAppEx
{
	friend class CDecayCommandLineInfo;

	DECLARE_MESSAGE_MAP()

public:
	CDecayApp();


public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void PumpMessageLoop();

private:
	bool OpenIsotopeDB();

public:
	LPCTSTR m_pszProductName;

	BOOL m_bDev;
	bool m_bAutosave{ false };
	CString m_lang;
	CString m_dbFile;
	CString m_labLogFile;
	bool m_bApprModel{ false };

	CLabLog m_labLog;
	CLabLog& LabLog() { return m_labLog; }

	CGdiPlusToken m_gdiplusToken;

	virtual void PreLoadState();

	virtual HINSTANCE LoadAppLangResourceDLL();

	afx_msg void OnAppAbout();
};

extern CDecayApp theApp;



class CDecayCommandLineInfo : public CCommandLineInfo
{
public:
	CDecayCommandLineInfo();

	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	enum class EFlagType
	{
		NoFlag,
		LangFlag,
		DBFlag,
		LablogFlag
	};

	EFlagType m_flag{ EFlagType::NoFlag };
};