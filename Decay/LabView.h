// LabLib
// Copyright (c) 2018 SS

#pragma once

#ifdef DXLABVIEW
#include <d3d9.h>
#endif

class CLabView : public CView
{
	DECLARE_DYNCREATE(CLabView)
	DECLARE_MESSAGE_MAP()

protected:
	CLabView();

public:
	virtual ~CLabView();

	virtual void OnDraw(CDC* pDC) { }

public:
	virtual HRESULT CaptureScreenshot(CBitmap& bmp);
	virtual HRESULT SaveScreenshot(const CString& strFileName);

	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	virtual HRESULT SaveBitmap(const CString& strFileName, CBitmap& bitmap);

	virtual void CopyScreenshot();

	virtual CString GetSaveBitmapFilter();
	virtual CString GetSaveBitmapDefaultType();

public:

	afx_msg void OnUpdateFileSaveScreenshot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);

	afx_msg void OnFileSaveScreenshot();
	afx_msg void OnEditCopy();

};


#ifdef DXLABVIEW
class CDXLabView : public CLabView
{
	DECLARE_DYNCREATE(CDXLabView)
protected:
	CDXLabView();

public:
	virtual ~CDXLabView();

protected:
	D3DPRESENT_PARAMETERS m_d3dpp;
	LPDIRECT3D9 m_d3d;
	LPDIRECT3DDEVICE9 m_d3ddev;

public:
	virtual void InitializeD3D();
	virtual void ClearD3D();

	virtual HRESULT CaptureScreenshot(CBitmap& bmp);
};
#endif // DXLABVIEW