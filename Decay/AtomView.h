// Copyright (c) 2022 SS

#pragma once
#include "LabView.h"
#include "DecayDoc.h"
#include "MemDC.h"

class CCanvas : public CDC
{
public:
	CCanvas(CDC* pDC, int cx, int cy) : CDC()
	{
		CreateCompatibleDC(pDC);

		m_width = cx;
		m_height = cy;
		if (!m_bitmap.CreateCompatibleBitmap(pDC, cx, cy))
			throw std::exception("CreateCompatibleBitmap");

		m_prevBitmap = SelectObject(&m_bitmap);
	}

	~CCanvas()
	{
		SelectObject(m_prevBitmap);
	}

	int Width() const { return m_width; }
	int Height() const { return m_height; }

private:
	CBitmap* m_prevBitmap;
	CBitmap m_bitmap;
	int m_width;
	int m_height;
};

class CAtomView :
	public CLabView,
	private Decay::CDecayObserver
{
	DECLARE_DYNCREATE(CAtomView)
	DECLARE_MESSAGE_MAP()

protected:
	CAtomView();

public:
	virtual ~CAtomView();


public:
	CDecayDoc* GetDocument() const;

public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL );
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	Decay::CModel* GetModel();

public:
	static constexpr COLORREF kBackColor = RGB(255, 255, 255);
	static constexpr COLORREF kActiveColor = RGB(255, 0, 0);
	static constexpr COLORREF kDecayedColor = RGB(0, 255, 255);
	static constexpr COLORREF kBorderColor = RGB(235, 235, 224);

	enum class EUpdateScreen { No, Yes };

	void DrawAtom(const Decay::CAtom& atom, EUpdateScreen updateScreen);
	void DrawAllAtoms(EUpdateScreen updateScreen = EUpdateScreen::Yes);
	void DrawBorder();

protected:
	int GetAtomSize(CRect& clientRect);

	class CAtomBrush : public CCanvas
	{
	public:
		CAtomBrush(CDC* pDC, int size, COLORREF backColor, COLORREF atomColor) :
			CCanvas(pDC, size, size)
		{
			const int fillSize = 3 * size / 4;
			FillSolidRect(0, 0, size, size, backColor);
			FillSolidRect(0, 0, fillSize, fillSize, atomColor);
		}
	};

	std::unique_ptr<CAtomBrush> m_activeAtomDC;
	std::unique_ptr<CAtomBrush> m_decayedAtomDC;
	std::unique_ptr<CCanvas> m_backBuffer;


	int m_base_dpi;
	int m_dpi;

protected:
	void UpdateAtomState(const Decay::CAtom& atom) override;
	void DecayStarting() override;
	void DecayStopped() override;
	void DecayReset() override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnUpdateResetView(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGrid(CCmdUI* pCmdUI);


	afx_msg void OnResetView();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnGrid();

};

#ifndef _DEBUG
inline CDecayDoc* CAtomView::GetDocument() const
   { return reinterpret_cast<CDecayDoc*>(m_pDocument); }
#endif

