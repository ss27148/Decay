// Copyright (c) 2022 SS

#include "stdafx.h"
#include "DecayApp.h"
#include "TableDialog.h"
#include "DecayDoc.h"
#include "MainFrm.h"
#include "OleDataSourceEx.h"


IMPLEMENT_DYNAMIC(CTableDialog, CDialogEx)

BEGIN_MESSAGE_MAP(CTableDialog, CDialogEx)
	ON_WM_GETMINMAXINFO()
	ON_EN_CHANGE(IDC_SEARCH, OnEnChangeSearch)
	ON_NOTIFY(NM_DBLCLK, ID_LIST, OnNMDblclkList)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_TABLE_COPYMASS, OnCopyMass)
	ON_COMMAND(ID_TABLE_COPY_HALFLIFE, OnCopyHalfLife)
	ON_COMMAND(ID_TABLE_FINDCURRENT, OnFindCurrent)
	ON_COMMAND(ID_TABLE_SETCURRENT, OnSetCurrent)
	ON_UPDATE_COMMAND_UI(ID_TABLE_SETCURRENT, OnUpdateSetCurrent)

END_MESSAGE_MAP()

CTableDialog::CTableDialog(CWnd* pParent)
	: CDialogEx(CTableDialog::IDD, pParent)
{

}

CTableDialog::~CTableDialog()
{
}

BOOL CTableDialog::OnInitDialog()
{
	CenterWindow();
	if (!CDialogEx::OnInitDialog())
		return FALSE;

	if (m_font.GetSafeHandle() == NULL)
	{
		m_font.CreateFont(-13, 0, 0, 0, FW_NORMAL, 
			FALSE, FALSE, FALSE, 0, 0, 0, 0, 
			DEFAULT_PITCH | FF_ROMAN, _T("Cambria"));
	}

	m_list.SetFont(&m_font);

	m_list.Load();

	m_notes.LoadFromResource(IDR_NOTES_RTF, AfxGetResourceHandle());

	CString searchCue;
	if (searchCue.LoadString(IDS_SEARCH_CUE_BANNER))
		m_searchEdit.SetCueBanner(searchCue);

	CRect rect;
	GetWindowRect(&rect);
	m_minSize = rect.Size();

	return TRUE;
}

void CTableDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ID_LIST, m_list);
	DDX_Control(pDX, IDC_LEGEND, m_notes);
	DDX_Control(pDX, IDC_SEARCH, m_searchEdit);

}

void CTableDialog::OnCancel()
{
	DestroyWindow();
}

BOOL CTableDialog::PreTranslateMessage(MSG* pMsg)
{
	if (GetFocus()->GetSafeHwnd() == GetDlgItem(IDC_SEARCH)->GetSafeHwnd())
	{
		if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		{
			OnEnChangeSearch();
			return TRUE;
		}
		else if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTableDialog::SelectIsotope(Decay::CIsotopeDB::iterator& isot)
{
	if (isot != Decay::CIsotopeDB::Instance().end())
	{
		UINT index = isot - Decay::CIsotopeDB::Instance().begin();
		m_list.SetItemState(index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_list.SetRedraw(FALSE);
		m_list.EnsureVisible(m_list.GetItemCount() - 1, FALSE);
		m_list.EnsureVisible(index, FALSE);
		m_list.SetRedraw(TRUE);
	}
	else
	{
		if (POSITION pos = m_list.GetFirstSelectedItemPosition())
		{
			UINT index = m_list.GetNextSelectedItem(pos);
			m_list.SetItemState(index, 0, LVIS_FOCUSED | LVIS_SELECTED);
		}
	}
}

void CTableDialog::OnListDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
}

void CTableDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_minSize.cx;
	lpMMI->ptMinTrackSize.y = m_minSize.cy;
}

void CTableDialog::OnEnChangeSearch()
{
	CString searchString;
	GetDlgItemText(IDC_SEARCH, searchString);

	SelectIsotope(Decay::CIsotopeDB::Instance().Find(searchString));
}


void CTableDialog::OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem >= 0)
	{
		
	}

	*pResult = 0;
}

void CTableDialog::OnCopy()
{
	if (POSITION pos = m_list.GetFirstSelectedItemPosition())
	{
		UINT index = m_list.GetNextSelectedItem(pos);

		CString textToCopy;
		for (int column = 0; column < m_list.GetHeaderCtrl()->GetItemCount(); column++)
		{
			if (column != 0)
				textToCopy += _T("\t");

			textToCopy += m_list.GetItemText(index, column);
		}

		COleDataSourceEx* oleDS = new COleDataSourceEx;
		oleDS->CacheText(textToCopy, true);
		oleDS->SetClipboard();
	}
}

void CTableDialog::OnCopyMass()
{
	if (POSITION pos = m_list.GetFirstSelectedItemPosition())
	{
		UINT index = m_list.GetNextSelectedItem(pos);

		COleDataSourceEx* oleDS = new COleDataSourceEx;
		oleDS->CacheText(m_list.GetItemText(index, CIsotopeListCtrl::kMassColumn), true);
		oleDS->SetClipboard();
	}
}

void CTableDialog::OnCopyHalfLife()
{
	if (POSITION pos = m_list.GetFirstSelectedItemPosition())
	{
		UINT index = m_list.GetNextSelectedItem(pos);

		COleDataSourceEx* oleDS = new COleDataSourceEx;
		oleDS->CacheText(m_list.GetItemText(index, CIsotopeListCtrl::kHalfLifeColumn), true);
		oleDS->SetClipboard();
	}
}

void CTableDialog::OnFindCurrent()
{
	CMainFrame* mainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (!mainFrame || !mainFrame->GetModel() || !mainFrame->GetModel()->Isotope())
		return;

	auto isot = Decay::CIsotopeDB::Instance().Find(
		mainFrame->GetModel()->Isotope()->Z(),
		mainFrame->GetModel()->Isotope()->A());

	SelectIsotope(isot);
}

void CTableDialog::OnSetCurrent()
{
	if (POSITION pos = m_list.GetFirstSelectedItemPosition())
	{
		UINT index = m_list.GetNextSelectedItem(pos);

		if (CMainFrame* mainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd()))
		{
			if (AfxMessageBox(IDP_CHANGE_ISOTOPE, MB_YESNO | MB_DEFBUTTON2) == IDYES)
			{
				mainFrame->GetModel()->SetIsotope(
					&Decay::CIsotopeDB::Instance().at(index));
			}
		}
	}
}

void CTableDialog::OnUpdateSetCurrent(CCmdUI* pCmdUI)
{
	bool enable = false;

	if (POSITION pos = m_list.GetFirstSelectedItemPosition())
	{
		UINT index = m_list.GetNextSelectedItem(pos);
		Decay::CIsotope& isot = Decay::CIsotopeDB::Instance().at(index);
		
		enable = !isot.IsStable() &&
			(isot.HalfLife().valueSec >= Decay::CModel::kMinTime) && 
			(isot.HalfLife().valueSec <= Decay::CModel::kMaxTime);
	}

	pCmdUI->Enable(enable);
}
