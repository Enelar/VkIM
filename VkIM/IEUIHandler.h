#pragma once

#include <MsHtmHst.h>
#include <MsHTML.h>
//#include "stdafx.h"

class CIEUIHandler :
	public IDocHostUIHandler//, public CObject
{
public:
	CIEUIHandler(void);
	~CIEUIHandler(void);

	HRESULT WINAPI ShowContextMenu(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved);
	HRESULT WINAPI QueryInterface(const IID &,void **);
	ULONG WINAPI AddRef(void);
	ULONG WINAPI Release(void);
	HRESULT WINAPI GetHostInfo(DOCHOSTUIINFO *);
	HRESULT WINAPI ShowUI(DWORD,IOleInPlaceActiveObject *,IOleCommandTarget *,IOleInPlaceFrame *,IOleInPlaceUIWindow *);
	HRESULT WINAPI HideUI(void);
	HRESULT WINAPI UpdateUI(void);
	HRESULT WINAPI EnableModeless(BOOL);
	HRESULT WINAPI OnDocWindowActivate(BOOL);
	HRESULT WINAPI OnFrameWindowActivate(BOOL);
	HRESULT WINAPI ResizeBorder(LPCRECT,IOleInPlaceUIWindow *,BOOL);
	HRESULT WINAPI TranslateAcceleratorW(LPMSG,const GUID *,DWORD);
	HRESULT WINAPI GetOptionKeyPath(LPOLESTR* pchKey,DWORD dw);
	HRESULT WINAPI GetDropTarget(IDropTarget *,IDropTarget **);
	HRESULT WINAPI GetExternal(IDispatch **);
	HRESULT WINAPI TranslateUrl(DWORD,OLECHAR *,OLECHAR **);
	HRESULT WINAPI FilterDataObject(IDataObject *,IDataObject **);
};

