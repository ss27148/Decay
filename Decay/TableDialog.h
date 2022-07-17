// Copyright (c) 2022 SS

#pragma once
#include "afxcmn.h"
#include "IsotopeListCtrl.h"
#include "RichEditCtrlX.h"

class CTableDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTableDialog)
	DECLARE_MESSAGE_MAP()

public:
	CTableDialog(CWnd* pParent = NULL);
	virtual ~CTableDialog();

	enum { IDD = IDD_TABLE };

protected:
	virtual BOOL OnInitDialog();

protected:
	CIsotopeListCtrl m_list;
	CFont m_font;
	CRichEditCtrlX m_notes;
	CEdit m_searchEdit;
	CSize m_minSize;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK() { }
	virtual void OnCancel();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void SelectIsotope(Decay::CIsotopeDB::iterator& isot);

	afx_msg void OnListDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnEnChangeSearch();
	afx_msg void OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopy();
	afx_msg void OnCopyMass();
	afx_msg void OnCopyHalfLife();
	afx_msg void OnFindCurrent();
	afx_msg void OnSetCurrent();

	afx_msg void OnUpdateSetCurrent(CCmdUI* pCmdUI);
};
