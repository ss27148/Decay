// Copyright (c) 2017 SS

#include "stdafx.h"
#include "RichEditCtrlX.h"

// https://docs.microsoft.com/en-us/windows/desktop/controls/printing-rich-edit-controls
// Mike O'Neill https://www.codeproject.com/Articles/9541/A-Rich-Edit-Control-That-Displays-Bitmaps-and-Othe

// ---------------
// CRichEditCtrlX
// ---------------
IMPLEMENT_DYNAMIC(CRichEditCtrlX, CRichEditCtrl)

BEGIN_MESSAGE_MAP(CRichEditCtrlX, CRichEditCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CRichEditCtrlX::CRichEditCtrlX()
{
	m_bCallbackSet = FALSE;
	m_pIRichEditOleCallback = NULL;
}


HRESULT CRichEditCtrlX::LoadFromResource(UINT idResource, HINSTANCE hInst)
{
	HRSRC hrSrc = ::FindResource(hInst, MAKEINTRESOURCE(idResource), _T("RTF"));
	if (!hrSrc)
		return E_FAIL;

	HGLOBAL hResource = ::LoadResource(hInst, hrSrc);
	if (!hResource)
		return E_FAIL;

	BYTE* pData = (BYTE*) ::LockResource(hResource);
	UINT nSize = ::SizeofResource(hInst, hrSrc);

	CMemFile data;
	data.Attach(pData, nSize);

	::FreeResource(hResource);

	EDITSTREAM es = { 0 };
	es.dwCookie = (DWORD_PTR) &data;
	es.pfnCallback = Editstreamcallback;

	if (!StreamIn(SF_RTF, es))
		return E_FAIL;

	return S_OK;
}

DWORD CRichEditCtrlX::Editstreamcallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb)
{
	if (dwCookie == 0)
		return 1;

	CMemFile* pData = (CMemFile*) dwCookie;
	*pcb = pData->Read(pbBuff, (UINT) min(cb, pData->GetLength() - pData->GetPosition()));

	return 0;
}


void CRichEditCtrlX::PreSubclassWindow()
{
	CRichEditCtrl::PreSubclassWindow();

	m_pIRichEditOleCallback = NULL;
	//m_pIRichEditOleCallback = new IExRichEditOleCallback;
	//ASSERT(m_pIRichEditOleCallback != NULL);

	//m_bCallbackSet = SetOLECallback(m_pIRichEditOleCallback);
}

int CRichEditCtrlX::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(m_pIRichEditOleCallback != NULL);

	// set the IExRichEditOleCallback pointer if it wasn't set 
	// successfully in PreSubclassWindow

	if (!m_bCallbackSet)
	{
		SetOLECallback(m_pIRichEditOleCallback);
	}

	return 0;
}

CRichEditCtrlX::IExRichEditOleCallback::IExRichEditOleCallback()
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage

	HRESULT hResult = ::StgCreateDocfile(NULL,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */ | STGM_CREATE,
		0, &pStorage);

	if (pStorage == NULL ||
		hResult != S_OK)
	{
		AfxThrowOleException(hResult);
	}
}

CRichEditCtrlX::IExRichEditOleCallback::~IExRichEditOleCallback()
{
}

HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	m_iNumStorages++;
	WCHAR tName[50];
	swprintf_s(tName, _countof(tName), L"REOLEStorage%d", m_iNumStorages);

	HRESULT hResult = pStorage->CreateStorage(tName,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
		0, 0, lplpstg);

	if (hResult != S_OK)
	{
		::AfxThrowOleException(hResult);
	}

	return hResult;
}

HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;

	if (iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback)
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}

ULONG STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::Release()
{
	if (--m_dwRef == 0)
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}



HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE
CRichEditCtrlX::IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
HMENU FAR *lphmenu)
{
	return S_OK;
}


