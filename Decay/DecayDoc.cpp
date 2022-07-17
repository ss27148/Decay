// Copyright (c) 2022 SS

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DecayApp.h"
#endif

#include "DecayDoc.h"

#include <propkey.h>
#include "MainFrm.h"
#include "IniReader.h"
#include "NumString.h"
#include <afxstatusbar.h>
#include "PropsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDecayDoc, CDocument)

BEGIN_MESSAGE_MAP(CDecayDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)

	ON_UPDATE_COMMAND_UI(IDC_START, OnUpdateStart)
	ON_UPDATE_COMMAND_UI(IDC_STOP, OnUpdateStop)
	ON_UPDATE_COMMAND_UI(IDC_RESET, OnUpdateReset)

	ON_COMMAND(IDC_RESET, OnReset)
	ON_COMMAND(IDC_START, OnStart)
	ON_COMMAND(IDC_STOP, OnStop)

END_MESSAGE_MAP()



CDecayDoc::CDecayDoc()
{
}

CDecayDoc::~CDecayDoc()
{
}

BOOL CDecayDoc::OnNewDocument()
{
	if (!m_model)
	{
		NewModel();
	}
	else
	{
		if (AfxMessageBox(IDP_CHANGE_ISOTOPE, MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
			return FALSE;
	}

	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

BOOL CDecayDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!m_model)
		NewModel();

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return TRUE;
}

void CDecayDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CIniReader reader(ar);
		reader.WriteSection(_T("Decay"));
		reader.WriteUInt(_T("Z"), Model()->Isotope()->Z());
		reader.WriteUInt(_T("A"), Model()->Isotope()->A());
	}
	else
	{
		CIniReader reader(ar);
		CIniReader::CToken token;

		bool insideAppSection = false;

		UINT Z = 0;
		UINT A = 0;
		while (reader.ReadNextToken(token))
		{
			if (token.m_type == CIniReader::SectionToken)
			{
				insideAppSection = 
					(token.m_name.CompareNoCase(_T("Decay")) == 0);
			}
			else if (token.m_type == CIniReader::ParameterToken)
			{
				if (insideAppSection)
				{
					if (token.m_name.CompareNoCase(_T("Z")) == 0)
						StringToUInt(Z, token.m_value);
					else if (token.m_name.CompareNoCase(_T("A")) == 0)
						StringToUInt(A, token.m_value);
				}
			}
		}

		Decay::CIsotopeDB::iterator isot = Decay::CIsotopeDB::Instance().Find(Z, A);

		if (isot != Decay::CIsotopeDB::Instance().end())
		{
			SetIsotope(&(*isot));
		}
		else
		{
			if (Z != 0 && A != 0)
				AfxMessageBox(_T("WARNING! Isotope not found.\nA random isotope will be chosen."), MB_ICONEXCLAMATION | MB_OK);
			SetRandomIsotope();
		}
	}
}

void CDecayDoc::LogParameters()
{
	
}

void CDecayDoc::LogResults()
{
	
}

void CDecayDoc::OnCloseDocument()
{
	ASSERT(!m_model->IsRunning());

	if (((CDecayApp*)AfxGetApp())->m_bAutosave)
	{
		DoFileSave();
	}

	CDocument::OnCloseDocument();
}

void CDecayDoc::OnDocumentEvent(DocumentEvent deEvent)
{
	CDocument::OnDocumentEvent(deEvent);

	switch (deEvent)
	{
	case CDocument::onAfterNewDocument:
		AttachViews();

		SetRandomIsotope();
		Reset();

		break;

	case CDocument::onAfterOpenDocument:
		AttachViews();

		if (!m_model->Isotope())
			SetRandomIsotope();

		Reset();

		break;

	case CDocument::onAfterSaveDocument:
		break;

	case CDocument::onAfterCloseDocument:
		break;

	default:
		break;
	}
}

void CDecayDoc::UpdateAtomState(const Decay::CAtom& atom)
{
	UpdateAllViews(NULL, onUpdateAtomState, (CObject*) &atom);
}

void CDecayDoc::DecayStarting()
{
	UpdateUI();
	UpdateAllViews(NULL, onDecayStarting);
}

void CDecayDoc::PassComplete()
{
	UpdateAllViews(NULL, onPassComplete);

	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			break;
		}
	}
}

void CDecayDoc::DecayStopped()
{
	UpdateUI();
	UpdateAllViews(NULL, onDecayStopped);

	if (m_closeAfterStop)
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

void CDecayDoc::NewModel()
{
	Decay::EAlgorithm algorithm = Decay::EAlgorithm::CoinFlip;
	
	CDecayApp* app = DYNAMIC_DOWNCAST(CDecayApp, AfxGetApp());
	if (app->m_bApprModel)
		algorithm = Decay::EAlgorithm::Approximation;

	m_model = std::make_shared<Decay::CModel>(algorithm);
	m_model->AddObserver(*this);
}

void CDecayDoc::AttachViews()
{
	if (CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd()))
	{
		if (CPropertiesWnd* propsWnd = pMainFrame->PropertiesWnd())
		{
			if (CPropsDialog* propsDlg = propsWnd->PropsDialog())
			{
				if (!m_viewList.Find(propsDlg))
					AddView(propsDlg);
			}
		}
	}
}

Decay::CModel* CDecayDoc::Model()
{
	return m_model.get();
}

BOOL CDecayDoc::GetParameters()
{
	CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (!pMainFrame)
		return FALSE;

	CPropertiesWnd* propsWnd = pMainFrame->PropertiesWnd();
	if (!propsWnd)
		return FALSE;

	CPropsDialog* propsDlg = propsWnd->PropsDialog();
	if (!propsDlg)
		return FALSE;

	return propsDlg->UpdateData();
}

void CDecayDoc::SetIsotope(Decay::CIsotope* isot)
{
	m_model->SetIsotope(isot);
}

Decay::CIsotope* CDecayDoc::Isotope()
{
	return m_model->Isotope();
}

void CDecayDoc::SetRandomIsotope()
{
	auto isot = Decay::CIsotopeDB::Instance().GetRandom();
	if (isot != Decay::CIsotopeDB::Instance().end())
	{
		m_model->SetIsotope(&(*isot));

		m_strTitle.Format(_T("%s-%u"), 
			(LPCTSTR)m_model->Isotope()->Symbol(), 
			(UINT)m_model->Isotope()->A());
	}
}

void CDecayDoc::Reset()
{
	m_model->Reset();
}

void CDecayDoc::Start()
{
	if (!GetParameters())
		return;
	
	m_model->Start();
}

void CDecayDoc::Stop()
{
	m_model->Stop();
}

void CDecayDoc::OnParametersChanged()
{
	if (!GetParameters())
		return;
}

void CDecayDoc::OnUpdateFileNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CDecayDoc::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CDecayDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CDecayDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CDecayDoc::OnUpdateStart(CCmdUI* pCmdUI)
{
	bool bEnable = false;
	if (m_model)
		bEnable = !m_model->IsRunning();

	pCmdUI->Enable(bEnable);
}

void CDecayDoc::OnUpdateStop(CCmdUI* pCmdUI)
{
	bool bEnable = false;
	if (m_model)
		bEnable = m_model->IsRunning();

	pCmdUI->Enable(bEnable);
}

void CDecayDoc::OnUpdateReset(CCmdUI* pCmdUI)
{
	bool bEnable = false;
	if (m_model)
		bEnable = !m_model->IsRunning();

	pCmdUI->Enable(bEnable);
}

void CDecayDoc::OnStart()
{
	CWaitCursor waitCursor;
	UpdateUI();

	Start();

	UpdateUI();
}

void CDecayDoc::OnStop()
{
	CWaitCursor waitCursor;
	UpdateUI();

	Stop();

	UpdateUI();
}

void CDecayDoc::OnReset()
{
	CWaitCursor waitCursor;
	UpdateUI();

	Reset();

	UpdateUI();
}

void CDecayDoc::UpdateUI()
{
	CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (!pMainFrame)
		return;

	CMFCToolBar* pToolbar = &pMainFrame->m_wndToolBar;
	if (pToolbar)
	{
		pToolbar->OnUpdateCmdUI(pMainFrame, FALSE);
		pToolbar->UpdateWindow();
	}

	CMainStatusBar* pStatusBar = &pMainFrame->m_wndStatusBar;
	if (pStatusBar)
	{
		pStatusBar->OnUpdateCmdUI(pMainFrame, TRUE);
		pStatusBar->UpdateWindow();
	}
}
