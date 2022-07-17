// Copyright (c) 2022 SS

#pragma once
#include "Model.h"


class CDecayDoc : 
	public CDocument,
	public Decay::CDecayObserver
{
	DECLARE_DYNCREATE(CDecayDoc)
	DECLARE_MESSAGE_MAP()

protected:
	CDecayDoc();
	virtual ~CDecayDoc();

public:
	BOOL OnNewDocument() override;
	BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
	void Serialize(CArchive& ar) override;
	void OnCloseDocument() override;

	void OnDocumentEvent(DocumentEvent deEvent) override;

	void LogParameters();
	void LogResults();

	void SetCloseAfterStop() { m_closeAfterStop = true; }


private:
	void UpdateAtomState(const Decay::CAtom& atom) override;


	void DecayStarting() override;


	void PassComplete() override;

	void DecayStopped() override;

private:
	void NewModel();
	void AttachViews();

public:

	Decay::CModel* Model();
	Decay::CModel* CloneModel();

	enum UpdateHints
	{
		onInitialize,
		onUpdateAll,
		onDecayStarting,
		onUpdateAtomState,
		onPassComplete,
		onDecayStopped
	};

	BOOL GetParameters();

	void SetIsotope(Decay::CIsotope* isot);
	Decay::CIsotope* Isotope();

	void SetRandomIsotope();

	void Reset();

	void Start();
	void Stop();

	void OnParametersChanged();

	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);

	afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReset(CCmdUI* pCmdUI);

	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnReset();


private:
	BOOL Initialize();

	void UpdateUI();

protected:
	std::shared_ptr<Decay::CModel> m_model;
	bool m_closeAfterStop{ false };
};
