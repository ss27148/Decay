// LabLib
// Copyright (c) 2018 SS

#include "stdafx.h"
#include "LabView.h"
#include "resource.h"
#include <gdiplus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifdef DXLABVIEW
#pragma comment (lib, "d3d9.lib")
#endif


// ---------
// CLabView
// ---------
IMPLEMENT_DYNCREATE(CLabView, CView)

BEGIN_MESSAGE_MAP(CLabView, CView)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_SCREENSHOT, OnUpdateFileSaveScreenshot)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	
	ON_COMMAND(ID_FILE_SAVE_SCREENSHOT, OnFileSaveScreenshot)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
END_MESSAGE_MAP()

CLabView::CLabView()
{
}

CLabView::~CLabView()
{
}

HRESULT CLabView::CaptureScreenshot(CBitmap& bmp)
{
	if (bmp.GetSafeHandle())
		return E_INVALIDARG;

	CDC dc;
	CDC* pScreenDC = GetDC();
	if (!dc.CreateCompatibleDC(pScreenDC))
	{
		ReleaseDC(pScreenDC);
		return E_FAIL;
	}

	CRect clientRect;
	GetClientRect(&clientRect);

	if (!bmp.CreateCompatibleBitmap(pScreenDC, clientRect.Width(), clientRect.Height()))
	{
		ReleaseDC(pScreenDC);
		return E_FAIL;
	}

	ReleaseDC(pScreenDC);

	CBitmap* pPrevBmp = (CBitmap*) dc.SelectObject(&bmp);
	OnDraw(&dc);
	dc.SelectObject(pPrevBmp);

	return S_OK;
}


HRESULT CLabView::SaveScreenshot(const CString& strFileName)
{
	HRESULT hr;
	CBitmap bmp;
	if (FAILED(hr = CaptureScreenshot(bmp)))
		return hr;

	return SaveBitmap(strFileName, bmp);
}

int CLabView::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

HRESULT CLabView::SaveBitmap(const CString& strFileName, CBitmap& bitmap)
{
	HRESULT hr;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
		return E_FAIL;

	Gdiplus::Bitmap* image = Gdiplus::Bitmap::FromHBITMAP((HBITMAP) bitmap, NULL);
	if (!image)
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return E_FAIL;
	}

	CLSID encoderClsid;
	CString strExt = PathFindExtension(strFileName);
	int iEncoder;
	if (strExt.CompareNoCase(_T(".png")) == 0)
		iEncoder = GetEncoderClsid(_T("image/png"), &encoderClsid);
	else if (strExt.CompareNoCase(_T(".jpg")) == 0 || strExt.CompareNoCase(_T(".jpeg")) == 0)
		iEncoder = GetEncoderClsid(_T("image/jpeg"), &encoderClsid);
	else if (strExt.CompareNoCase(_T(".gif")) == 0)
		iEncoder = GetEncoderClsid(_T("image/gif"), &encoderClsid);
	else if (strExt.CompareNoCase(_T(".tif")) == 0 || strExt.CompareNoCase(_T(".tiff")) == 0)
		iEncoder = GetEncoderClsid(_T("image/tiff"), &encoderClsid);
	else if (strExt.CompareNoCase(_T(".bmp")) == 0)
		iEncoder = GetEncoderClsid(_T("image/bmp"), &encoderClsid);
	else
		iEncoder = GetEncoderClsid(_T("image/png"), &encoderClsid);

	if (iEncoder == -1)
	{
		delete image;
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return E_FAIL;
	}

	hr = E_FAIL;
	Gdiplus::Status stat = image->Save(strFileName, &encoderClsid, NULL);
	if (stat == Gdiplus::Ok)
		hr = S_OK;

	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return hr;
}

void CLabView::CopyScreenshot()
{
	CBitmap bmp;
	if (SUCCEEDED(CaptureScreenshot(bmp)))
	{
		OpenClipboard();
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, bmp.Detach());
		CloseClipboard();
	}
}

CString CLabView::GetSaveBitmapFilter()
{
	return _T("BMP (*.bmp)|*.bmp|PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg;*.jpeg|GIF (*.gif)|*.gif|TIFF (*.tif)|*.tif;*.tiff");
}

CString CLabView::GetSaveBitmapDefaultType()
{
	return _T(".png");
}

void CLabView::OnUpdateFileSaveScreenshot(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CLabView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CLabView::OnFileSaveScreenshot()
{
	CString strFilter = GetSaveBitmapFilter();

	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += _T("|") + allFilter + _T("|*.*||");

	CString strDelaultExt = GetSaveBitmapDefaultType();

	CFileDialog dlg(FALSE, strDelaultExt, _T("Screenshot") + strDelaultExt, 
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
	dlg.m_ofn.nFilterIndex = 2;

	if (dlg.DoModal() == IDOK)
	{
		CString strFileName = dlg.GetPathName();
		CString strExt = PathFindExtension(strFileName);

		if (strExt.IsEmpty())
		{
			int iToken = 0;
			int iPos = 0;
			CString str = strFilter.Tokenize(_T("|"), iPos);
			CString strTokenExt;
			while (!str.IsEmpty())
			{
				if (iToken + 1 == 2 * dlg.m_ofn.nFilterIndex)
				{
					strExt = str;
					int iSemicolon = strExt.Find(_T(';'));
					if (iSemicolon != -1)
						strExt.Truncate(iSemicolon);
					break;
				}

				iToken++;
				str = strFilter.Tokenize(_T("|"), iPos);
			}

			if (strExt != _T("*.*"))
				strFileName += strExt;
		}

		SaveScreenshot(strFileName);
	}
}

void CLabView::OnEditCopy()
{
	CopyScreenshot();
}


#ifdef DXLABVIEW
// -----------
// CDXLabView
// -----------
IMPLEMENT_DYNCREATE(CDXLabView, CLabView)

CDXLabView::CDXLabView()
{
	m_d3d = NULL;
	m_d3ddev = NULL;
	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
}

CDXLabView::~CDXLabView()
{
	ClearD3D();

	if (m_d3d)
	{
		m_d3d->Release();
		m_d3d = NULL;
	}
}

void CDXLabView::InitializeD3D()
{
	if (!m_d3d)
		m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	CRect clientRect;
	GetClientRect(&clientRect);

	if (m_d3d && (!m_d3ddev || 
		clientRect.Width() != m_d3dpp.BackBufferWidth || 
		clientRect.Height() != m_d3dpp.BackBufferHeight))
	{
		ClearD3D();

		m_d3dpp.Windowed = TRUE;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.hDeviceWindow = m_hWnd;
		m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_d3dpp.BackBufferWidth = clientRect.Width();
		m_d3dpp.BackBufferHeight = clientRect.Height();
		m_d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;

		HRESULT hr = m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, m_hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&m_d3dpp, &m_d3ddev);
	}
}

void CDXLabView::ClearD3D()
{
	if (m_d3ddev)
	{
		m_d3ddev->Release();
		m_d3ddev = NULL;
	}
}

HRESULT CDXLabView::CaptureScreenshot(CBitmap& bmp)
{
	HRESULT hr;
	D3DDISPLAYMODE mode;
	if (FAILED(hr = m_d3d->GetAdapterDisplayMode(0, &mode)))
		return hr;

	D3DPRESENT_PARAMETERS d3dpp = { 0 };
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferHeight = mode.Height;
	d3dpp.BackBufferWidth = mode.Width;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = NULL;

	IDirect3DDevice9* d3ddev = NULL;
	if (FAILED(hr = m_d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev)))
		return hr;

	IDirect3DSurface9* d3dsurface = NULL;
	if (FAILED(hr = d3ddev->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &d3dsurface, nullptr)))
	{
		d3ddev->Release();
		return hr;
	}

	if (FAILED(hr = d3ddev->GetFrontBufferData(0, d3dsurface)))
	{
		d3ddev->Release();
		return hr;
	}

	D3DLOCKED_RECT lockedRect;
	if (FAILED(hr = d3dsurface->LockRect(&lockedRect, NULL, 0)))
	{
		d3dsurface->Release();
		d3ddev->Release();
		return hr;
	}

	int pitch = lockedRect.Pitch;
	BYTE* bits = new BYTE[pitch * mode.Height];
	CopyMemory(bits, lockedRect.pBits, pitch * mode.Height);
	
	if (FAILED(hr = d3dsurface->UnlockRect()))
	{
		d3dsurface->Release();
		d3ddev->Release();
		delete [] bits;
		return hr;
	}

	CRect clientRect;
	GetClientRect(&clientRect);
	ClientToScreen(&clientRect);

	int bitCount = 24;
	int scanLine = (bitCount * clientRect.Width() + 31) / 32 * sizeof(DWORD);
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = bitCount;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biWidth = clientRect.Width();
	bmi.bmiHeader.biHeight = clientRect.Height();
	bmi.bmiHeader.biSizeImage = scanLine * clientRect.Height();
	bmi.bmiHeader.biPlanes = 1;

	BYTE* bitmapBits = new BYTE[bmi.bmiHeader.biSizeImage];
	ZeroMemory(bitmapBits, bmi.bmiHeader.biSizeImage);

	int srcIndex, dstIndex;
	for (int j = clientRect.top; j < clientRect.bottom; j++)
	{
		for (int i = clientRect.left; i < clientRect.right; i++)
		{
			srcIndex = j * 4 * mode.Width + i * 4;
			dstIndex = (clientRect.bottom - 1 - j) * scanLine + (i - clientRect.left) * 3;
			bitmapBits[dstIndex + 0] = bits[srcIndex + 0];
			bitmapBits[dstIndex + 1] = bits[srcIndex + 1];
			bitmapBits[dstIndex + 2] = bits[srcIndex + 2];
		}
	}

	CDC* pScreenDC = GetDC();
	HBITMAP hBitmap = CreateDIBitmap(pScreenDC->GetSafeHdc(), (BITMAPINFOHEADER*) &bmi, CBM_INIT, bitmapBits, &bmi, DIB_RGB_COLORS);
	if (!hBitmap)
	{
		ReleaseDC(pScreenDC);
		delete [] bitmapBits;
		delete [] bits;
		d3dsurface->Release();
		d3ddev->Release();
		return E_FAIL;
	}

	bmp.Attach(hBitmap);
	ReleaseDC(pScreenDC);

	delete [] bitmapBits;	
	delete [] bits;

	d3dsurface->Release();
	d3ddev->Release();

	return hr;

}

#endif // DXLABVIEW