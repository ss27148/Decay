// Copyright (c) 2022 SS

#include "stdafx.h"

#include "DecayDoc.h"
#include "AtomView.h"
#include "MemDC.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Resource.h"
#include "DecayApp.h"
#include "MainFrm.h"
#include "DPI.h"


IMPLEMENT_DYNCREATE(CAtomView, CLabView)

BEGIN_MESSAGE_MAP(CAtomView, CLabView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()

END_MESSAGE_MAP()


CAtomView::CAtomView()
{
	CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());

}

CAtomView::~CAtomView()
{
	CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());

}

BOOL CAtomView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CLabView::PreCreateWindow(cs);
}

void CAtomView::OnInitialUpdate()
{
	GetModel()->AddObserver(*this);
}

void CAtomView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnPrepareDC(pDC);

	CRect clientRect;
	GetClientRect(&clientRect);

	const int atomSize = GetAtomSize(clientRect);

	if (!m_activeAtomDC || m_activeAtomDC->Width() != atomSize)
		m_activeAtomDC = std::make_unique<CAtomBrush>(pDC, atomSize, kBackColor, kActiveColor);

	if (!m_decayedAtomDC || m_decayedAtomDC->Width() != atomSize)
		m_decayedAtomDC = std::make_unique<CAtomBrush>(pDC, atomSize, kBackColor, kDecayedColor);

	if (Decay::CModel* model = GetModel())
	{
		CPoint origin(
			(clientRect.Width() - atomSize * model->Atoms().Cols()) / 2,
			(clientRect.Height() - atomSize * model->Atoms().Rows()) / 2);
		pDC->SetViewportOrg(origin);

		if (!m_backBuffer || (m_backBuffer && (m_backBuffer->Width() != clientRect.Width() || m_backBuffer->Height() != clientRect.Height())))
		{
			m_backBuffer = std::make_unique<CCanvas>(pDC, clientRect.Width(), clientRect.Height());
			//m_backBuffer->FillSolidRect(&clientRect, kBackColor);
			m_backBuffer->FillSolidRect(&clientRect, kBorderColor);
			m_backBuffer->SetViewportOrg(origin);

			DrawBorder();

			DrawAllAtoms(EUpdateScreen::No);
		}
	}
}

void CAtomView::OnDraw(CDC* pDC)
{
	CRect clipBox;
	pDC->GetClipBox(&clipBox);

	if (!m_backBuffer)
	{
		pDC->FillSolidRect(&clipBox, kBackColor);
		return;
	}

	pDC->BitBlt(clipBox.left, clipBox.top, clipBox.Width(), clipBox.Height(),
		m_backBuffer.get(), clipBox.left, clipBox.top, SRCCOPY);
}

int CAtomView::GetAtomSize(CRect& clientRect)
{
	int atomSize = 4;

	if (Decay::CModel* model = GetModel())
	{
		if ((model->Atoms().Cols() + 2) * clientRect.Height() > (model->Atoms().Rows() + 2) * clientRect.Width())
			atomSize = clientRect.Width() / (GetModel()->Atoms().Cols() + 2);
		else
			atomSize = clientRect.Height() / (model->Atoms().Rows() + 2);
	}

	return atomSize;
}

void CAtomView::UpdateAtomState(const Decay::CAtom& atom)
{
	DrawAtom(atom, EUpdateScreen::Yes);
}

void CAtomView::DecayStarting()
{
	DrawAllAtoms();
}

void CAtomView::DecayStopped()
{
	DrawAllAtoms();
}

void CAtomView::DecayReset()
{
	DrawAllAtoms();
}

int CAtomView::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CView::OnCreate(lpcs) == -1)
		return -1;

	return 0;
}

void CAtomView::OnDestroy()
{
	CView::OnDestroy();
}

BOOL CAtomView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CAtomView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}


BOOL CAtomView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CAtomView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CAtomView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CAtomView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (lHint == CDecayDoc::onUpdateAll)
		Invalidate(FALSE);
}

void CAtomView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAtomView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	CDecayApp* pApp = DYNAMIC_DOWNCAST(CDecayApp, AfxGetApp());
	pApp->GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


void CAtomView::OnSize(UINT nType, int cx, int cy)
{
	
}

Decay::CModel* CAtomView::GetModel()
{
	CDecayDoc* pDoc = DYNAMIC_DOWNCAST(CDecayDoc, GetDocument());
	if (pDoc)
		return pDoc->Model();

	return NULL;
}

void CAtomView::DrawAtom(const Decay::CAtom& atom, EUpdateScreen updateScreen)
{
	CCanvas* atomDC = atom.IsActive() ? m_activeAtomDC.get() : m_decayedAtomDC.get();

	CPoint pt(atom.Row() * atomDC->Width(), atom.Col() * atomDC->Width());

	m_backBuffer->BitBlt(
		pt.x, pt.y,
		atomDC->Width(), atomDC->Height(),
		atomDC, 0, 0, SRCCOPY);

	if (updateScreen == EUpdateScreen::Yes)
	{
		CRect rect(pt.x, pt.y, pt.x + atomDC->Width(), pt.y + atomDC->Height());
		m_backBuffer->LPtoDP(&rect);

		InvalidateRect(&rect, FALSE);
		UpdateWindow();
	}
}

void CAtomView::DrawAllAtoms(EUpdateScreen updateScreen /*= EUpdateScreen::Yes*/)
{
	Decay::CModel* model = GetModel();

	if (model)
	{
		Decay::CAtomSet& atoms = model->Atoms();

		for (UINT index = 0; index < atoms.Count(); index++)
		{
			DrawAtom(atoms.At(index), EUpdateScreen::No);
		}
	}

	if (updateScreen == EUpdateScreen::Yes)
	{
		Invalidate(FALSE);
		UpdateWindow();
	}
}

void CAtomView::DrawBorder()
{
	Decay::CModel* model = GetModel();
	if (!model)
		return;

	const int atomSize = m_activeAtomDC->Width();
	const int borderWidth = atomSize - 3 * atomSize / 4;

	m_backBuffer->FillSolidRect(
		-borderWidth,
		-borderWidth,
		borderWidth,
		atomSize * model->Atoms().Rows() + borderWidth,
		kBackColor);

	m_backBuffer->FillSolidRect(
		-borderWidth,
		-borderWidth,
		atomSize * model->Atoms().Cols() + borderWidth,
		borderWidth,
		kBackColor);
}

#ifdef _DEBUG
void CAtomView::AssertValid() const
{
	CLabView::AssertValid();
}

void CAtomView::Dump(CDumpContext& dc) const
{
	CLabView::Dump(dc);
}


CDecayDoc* CAtomView::GetDocument() const // non-debug version is inline
{
	if (!m_pDocument)
		return NULL;

	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDecayDoc)));
	return (CDecayDoc*)m_pDocument;
}
#endif //_DEBUG

