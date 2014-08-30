#include "IEUIHandler.h"


CIEUIHandler::CIEUIHandler(void)
{
}


CIEUIHandler::~CIEUIHandler(void)
{
}


HRESULT CIEUIHandler::ShowContextMenu(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved){
	if(dwID!=CONTEXT_MENU_TEXTSELECT)return S_OK;
	return S_FALSE;
}

HRESULT WINAPI ShowContextMenu(DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved);
HRESULT CIEUIHandler::QueryInterface(const IID &,void **){
	return S_FALSE;
}
ULONG CIEUIHandler::AddRef(void){return 0;}
ULONG CIEUIHandler::Release(void){return 0;}
HRESULT CIEUIHandler::GetHostInfo(DOCHOSTUIINFO *info){
	info->dwFlags=DOCHOSTUIFLAG_NO3DBORDER;
	return S_OK;
}
HRESULT CIEUIHandler::ShowUI(DWORD,IOleInPlaceActiveObject *,IOleCommandTarget *,IOleInPlaceFrame *,IOleInPlaceUIWindow *){return S_FALSE;}
HRESULT CIEUIHandler::HideUI(void){return S_FALSE;}
HRESULT CIEUIHandler::UpdateUI(void){return S_FALSE;}
HRESULT CIEUIHandler::EnableModeless(BOOL){return S_FALSE;}
HRESULT CIEUIHandler::OnDocWindowActivate(BOOL){return S_FALSE;}
HRESULT CIEUIHandler::OnFrameWindowActivate(BOOL){return S_FALSE;}
HRESULT CIEUIHandler::ResizeBorder(LPCRECT,IOleInPlaceUIWindow *,BOOL){return S_FALSE;}
HRESULT CIEUIHandler::TranslateAcceleratorW(LPMSG,const GUID *,DWORD){return S_FALSE;}
HRESULT CIEUIHandler::GetOptionKeyPath(LPOLESTR* pchKey,DWORD dw){
	HRESULT hr;

    #define CCHMAX 256
    size_t cchLength;

    if (pchKey)
    {
        WCHAR* szMyKey = L"Software\\Sean\\VkIM";
        hr = wcslen(szMyKey);
        // TODO: Add error handling code here.
        
        *pchKey = (LPOLESTR)CoTaskMemAlloc((cchLength + 1) * sizeof(WCHAR));
        if (*pchKey)
            wcsncpy(*pchKey, szMyKey , cchLength + 1);
            // TODO: Add error handling code here.

        hr = (*pchKey) ? S_OK : E_OUTOFMEMORY;
    }
    else
        hr = E_INVALIDARG;

    return hr;
}
HRESULT CIEUIHandler::GetDropTarget(IDropTarget *,IDropTarget **){return S_FALSE;}
HRESULT CIEUIHandler::GetExternal(IDispatch **){return S_FALSE;}
HRESULT CIEUIHandler::TranslateUrl(DWORD,OLECHAR *,OLECHAR **){return S_FALSE;}
HRESULT CIEUIHandler::FilterDataObject(IDataObject *,IDataObject **){return S_FALSE;}