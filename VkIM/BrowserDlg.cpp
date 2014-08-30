// BrowserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "BrowserDlg.h"
#include "Utils.h"
#include "Lang.h"
#include "Skin.h"

#include <string>
#include <vector>
#include <MsHTML.h>
#include <comutil.h>

//#define JSON_UNICODE

#include "JSON_Defs.h"   //for the libJSON namespace
#include "jsonmain.h"    //for JSONNode
// CBrowserDlg dialog
using namespace std;

extern char* sid;
extern unsigned int uid;
extern char* secret;
extern CSkin skin;

IMPLEMENT_DYNAMIC(CBrowserDlg, CDialog)

CBrowserDlg::CBrowserDlg(CWnd* pParent, bool _logout)
	: CDialog(CBrowserDlg::IDD, pParent)
{
	logout=_logout;
	logoutClicked=false;
	progressFrame = 0;
	__try{
#ifdef _RLS2000
	InitControlContainer();
#endif
#if _MSC_VER==1500
	InitControlContainer();
#endif
	}
	__except(EXCEPTION_EXECUTE_HANDLER){}
}

CBrowserDlg::~CBrowserDlg()
{
}

void CBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, ie);
}


BEGIN_MESSAGE_MAP(CBrowserDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBrowserDlg message handlers
BEGIN_EVENTSINK_MAP(CBrowserDlg, CDialog)
	ON_EVENT(CBrowserDlg, IDC_EXPLORER1, 252, CBrowserDlg::NavigateComplete2Explorer1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CBrowserDlg, IDC_EXPLORER1, 259, CBrowserDlg::DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

void CBrowserDlg::NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	// Тут мы ловим редиректы.
	KillTimer(100);
	CString u=ie.get_LocationURL();
	if(logout){
		if(u.Find(L"http://vkontakte.ru/login.php")==0){ // Значит в IE уже вышли, закрываем окно
			CDialog::OnOK();
		}
		if(logoutClicked){
			CDialog::OnOK();
		}
	}
	if(u.Find(L"http://vkontakte.ru/api/login_success.html")==0){
	wchar_t* _url=URL->bstrVal;
	wcstok(_url, L"=");
	wchar_t* session_data=wcstok(NULL, L"=");

	std::wstring sd(CUtils::urlDecode(session_data));

	JSONNode* json = libJSON::Parse(sd);

	wchar_t* wsid=(wchar_t*)json->GetNode(L"sid")->NodeAsString().c_str();
	wchar_t* wsecret=(wchar_t*)json->GetNode(L"secret")->NodeAsString().c_str();
	wchar_t* uid_s=(wchar_t*)json->GetNode(L"mid")->NodeAsString().c_str();
	char* uid_c=CUtils::wtoc(uid_s, wcslen(uid_s));
	uid=atoi(uid_c);
	sid=CUtils::wtoc(wsid, wcslen(wsid)+1);
	secret=CUtils::wtoc(wsecret, wcslen(wsecret)+1);
	delete [] wsid, wsecret, uid_s, uid_c;
	CDialog::OnOK();
	}

	if(u.Find(L"http://vkontakte.ru/api/login_failure.html")==0){
		//MessageBox(L"Авторизация не удалась.", NULL, MB_OK | MB_ICONEXCLAMATION);
	CDialog::OnCancel();
	}
}

BOOL CBrowserDlg::OnInitDialog(){
	CDialog::OnInitDialog();
	SetWindowText(CLang::Get(L"authorization"));
	CRect rect;
	GetClientRect(rect);
	ie.SetWindowPos(NULL, -2, -2, rect.Width()+4, rect.Height()+4, SWP_NOZORDER);
	progressFrame=0;
	SetTimer(100, 250, NULL);
	if(!logout){
	ie.Navigate(_T("http://vkontakte.ru/login.php?app=1911346&layout=popup&type=browser&settings=13342"), NULL, NULL, NULL, NULL);
	}else{
	ie.Navigate(_T("http://vkontakte.ru/profile.php"), NULL, NULL, NULL, NULL);
	}
	return TRUE;
}

void CBrowserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(ie)ie.SetWindowPos(NULL, 0, 0, cx+4, cy+4, SWP_NOZORDER | SWP_NOMOVE);
	// TODO: Add your message handler code here
}


void CBrowserDlg::OnOK()
{
}


void CBrowserDlg::OnCancel()
{
}


BOOL CBrowserDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(rect);
	rect.OffsetRect(-rect.left, -rect.top);
	pDC->FillSolidRect(rect, RGB(255,255,255));
	CPoint center=rect.CenterPoint();
	skin.progress.bmp.DrawUnscaled(pDC, center.x-skin.progress.w/2, center.y-skin.progress.h/2, skin.progress.w, skin.progress.h, skin.progress.w*progressFrame, 0);
	return true;
}


void CBrowserDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100){
		Invalidate();
		progressFrame++;
		progressFrame%=skin.progress.n;
	}
	CDialog::OnTimer(nIDEvent);
}


void CBrowserDlg::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	// TODO: Add your message handler code here
	if(logout){
	CString u=ie.get_LocationURL();
		if(u.Find(L"http://vkontakte.ru/profile.php")==0){
			CRect rect;
			ie.GetWindowRect(rect);
			IHTMLDocument2 *doc=(IHTMLDocument2*)ie.get_Document();
			IHTMLElementCollection* links;
			doc->get_links(&links);
			long len=1;
			links->get_length(&len);
			for(long i=0;i<len;i++){
				IDispatch* _item;
				_variant_t index = i;
				HRESULT r=links->item(index, index, &_item);
				IHTMLAnchorElement* item;
				_item->QueryInterface(__uuidof(IHTMLAnchorElement), (void**)&item);
				IHTMLElement* __item;
				_item->QueryInterface(__uuidof(IHTMLElement), (void**)&__item);
				wchar_t* href;
				item->get_href(&href);
				std::wstring whref=href;
				if(whref.substr(0, 19).compare(L"http://login.vk.com")==0){
					__item->click();
					logoutClicked=true;
					TRACE("Clicked!\n");
					return;
				}
				//delete href;
			}
		}
	}
}
