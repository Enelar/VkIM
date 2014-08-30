// ClosableTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "ClosableTabCtrl.h"
#include "Skin.h"
#include "Connector.h"
#include "TalkDlg.h"
#include "Utils.h"

extern CSkin skin;
extern CConnector con;
extern CTalkDlg* talks;
// CClosableTabCtrl

IMPLEMENT_DYNAMIC(CClosableTabCtrl, CTabCtrl)

CClosableTabCtrl::CClosableTabCtrl()
{
	over_idx=-1;
	mouseInside=false;
	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);
	m_pSpinCtrl=NULL;
}

CClosableTabCtrl::~CClosableTabCtrl()
{
	if(m_pSpinCtrl) delete m_pSpinCtrl;
}


BEGIN_MESSAGE_MAP(CClosableTabCtrl, CTabCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(TCM_HITTEST, CClosableTabCtrl::HitTest)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CClosableTabCtrl::OnPaint(){
	//CTabCtrl::OnPaint();
	ModifyStyle(0, WS_CLIPCHILDREN);
	if( !m_pSpinCtrl )
    {
      CWnd * pWnd = FindWindowEx( GetSafeHwnd(), 0, _T("msctls_updown32"), 0 );
      if( pWnd )
      {
        m_pSpinCtrl = new CSpinButtonCtrl;
        m_pSpinCtrl->Attach(pWnd->GetSafeHwnd());
      }
    }

	BOOL aero=false;
	HMODULE hm=LoadLibrary(L"dwmapi.dll");
	if(hm){
		DWMISCOMPOSITIONENABLED* ic=(DWMISCOMPOSITIONENABLED*)GetProcAddress(hm, "DwmIsCompositionEnabled");
		(ic)(&aero);
		FreeLibrary(hm);
	}

	RECT wrect;
	GetWindowRect(&wrect);
	int w=wrect.right-wrect.left;
	int h=TAB_HEIGHT+TAB_TOP_MARGIN;
	CPaintDC _dc(this);
	if(!_dc.m_hDC)return;
	CDC dc;
	dc.CreateCompatibleDC(&_dc);
	if(!dc.m_hDC){
		ReleaseDC(&_dc);
		return;
	}
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&_dc, w, h);
	if(!bmp.m_hObject){
		ReleaseDC(&_dc);
		ReleaseDC(&dc);
		return;
	}
	CBitmap* obmp=dc.SelectObject(&bmp);

	
	CFont fnt;
	fnt.CreateFontW(-11, 0, 0, 0, 0, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_DONTCARE, L"Tahoma"); 
	CFont* ofnt=dc.SelectObject(&fnt);
	
	

	dc.SetBkMode(TRANSPARENT);

	//skin.tabbarBG.bmp.DrawStretched(&dc, 0, 0, w, 20, 0, 0, 0, 0);
	if(!aero){
		COLORREF bgcol=IsTopParentActive()?GetSysColor(COLOR_ACTIVECAPTION):GetSysColor(COLOR_INACTIVECAPTION);
		dc.FillSolidRect(0, 0, w, h, bgcol);
	}

	int offset=5;

	if(m_pSpinCtrl){
		int p=LOWORD(m_pSpinCtrl->GetPos());
		for(int i=0;i<p;i++){
			offset-=itemWidths[i];
		}
	}

	itemWidths.clear();
	int totalWidth=5;

	for(int i=0;i<GetItemCount();i++){
		TC_ITEM item;
		memset(&item, 0, sizeof(TC_ITEM));
		item.mask=TCIF_TEXT|TCIF_IMAGE|TCIF_PARAM;
		item.pszText=new wchar_t[512];
		item.cchTextMax=512;
		GetItem(i, &item);
		int tw=dc.GetTextExtent(item.pszText, wcslen(item.pszText)).cx+40;
		RECT rect;
		rect.top=TAB_TOP_MARGIN;
		rect.bottom=h;
		rect.left=offset;
		rect.right=offset+tw;
		CTalkTab* tab=(CTalkTab*)item.lParam;

		totalWidth+=tw;

		if(i==this->GetCurSel()){
			skin.tabActive.bmp.DrawStretched(&dc, offset, TAB_TOP_MARGIN, tw, TAB_HEIGHT, 0, 0, 0, 0);
			dc.SetTextColor(skin.tabActiveTextColor);
		}else if(over_idx==i){
			skin.tabOver.bmp.DrawStretched(&dc, offset, TAB_TOP_MARGIN, tw, TAB_HEIGHT, 0, 0, 0, 0);
			//dc.SetTextColor(skin.tabOverTextColor);
			dc.SetTextColor(IsTopParentActive()?GetSysColor(COLOR_CAPTIONTEXT):GetSysColor(COLOR_INACTIVECAPTIONTEXT));
		}else{
			skin.tabInactive.bmp.DrawStretched(&dc, offset, TAB_TOP_MARGIN, tw, TAB_HEIGHT, 0, 0, 0, 0);
			//dc.SetTextColor(skin.tabIncativeTextColor);
			dc.SetTextColor(IsTopParentActive()?GetSysColor(COLOR_CAPTIONTEXT):GetSysColor(COLOR_INACTIVECAPTIONTEXT));
		}

		HICON hIcon=skin.iconUnknown;

		bool newmsg=false;
		if(tab){
			newmsg=tab->hasNewMsg;
			if(newmsg){
				hIcon=skin.iconNewMsg;
			}
		}

		if(item.iImage>0 && !newmsg){
			bool inList=false;
			for(int j=0;j<con.clDlg->friendList.items.size();j++){
				if(con.clDlg->friendList.items[j].uid==item.iImage){
					inList=true;
					break;
				}
			}
			if(inList){
				if(con.clDlg->friendList.items[con.clDlg->friendList.GetItemByUID(item.iImage)].online){
					hIcon=skin.iconOnline;
				}else{
					hIcon=skin.iconOffline;
				}
			}
		}

		if(tab){
			if(tab->tabProxy)tab->tabProxy->SetIcon(hIcon, false);
		}

		DrawIconEx(dc.GetSafeHdc(), offset+2, TAB_HEIGHT/2+TAB_TOP_MARGIN-8, hIcon, 16, 16, 0, NULL, DI_NORMAL);

		if(i==this->GetCurSel()){
			skin.tabCloseAct.DrawUnscaled(&dc, offset+tw-18, TAB_HEIGHT/2+TAB_TOP_MARGIN-8, 15, 15, 0, 0);
		}else{
			skin.tabCloseInact.DrawUnscaled(&dc, offset+tw-18, TAB_HEIGHT/2+TAB_TOP_MARGIN-8, 15, 15, 0, 0);
		}

		if(!aero){
		dc.DrawText(item.pszText, wcslen(item.pszText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}else{
			if(i==this->GetCurSel()){
				CUtils::DrawAeroText(rect, &dc, 0, DT_CENTER|DT_VCENTER|DT_SINGLELINE, &fnt, item.pszText);
			}else{
				CUtils::DrawAeroText(rect, &dc, 3, DT_CENTER|DT_VCENTER|DT_SINGLELINE, &fnt, item.pszText);
			}
		}

		delete item.pszText;
		offset+=tw+TAB_SPACING;
		itemWidths.push_back(tw);
	}

	_dc.BitBlt(0, 0, w, h, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(obmp);
	dc.SelectObject(ofnt);
	ReleaseDC(&dc);
	ReleaseDC(&_dc);
	SetItemSize(CSize(totalWidth/GetItemCount(), 20));
}

void CClosableTabCtrl::DrawItem(LPDRAWITEMSTRUCT drawItemStruct){
	/*CDC cDC;
	cDC.Attach(drawItemStruct->hDC);
	cDC.FillSolidRect(&drawItemStruct->rcItem, RGB(255,0,255));
	ReleaseDC(&cDC);*/
}

LRESULT CClosableTabCtrl::HitTest(WPARAM wParam, LPARAM lParam){
	TC_HITTESTINFO* hti=(TC_HITTESTINFO*)lParam;
	int offset=5;
	for(int i=0;i<itemWidths.size();i++){
		offset+=itemWidths[i];
		if(offset<hti->pt.x)return i;
	}
	return -1;
}

void CClosableTabCtrl::OnMouseLeave(){
	mouseInside=false;
	if(over_idx==-1)return;
	over_idx=-1;
	RECT rect;
	GetWindowRect(&rect);
	rect.right-=rect.left;
	rect.left=0;
	rect.top=0;
	rect.bottom=TAB_HEIGHT+TAB_TOP_MARGIN;
	RedrawWindow();
}

void CClosableTabCtrl::OnLButtonDown(UINT nFlags, CPoint point){
	//CTabCtrl::OnLButtonDown(nFlags, point);
	int offset=5;
	int idx=-1;
	int firstItem=0;
	if(m_pSpinCtrl)firstItem=LOWORD(m_pSpinCtrl->GetPos());
	for(int i=firstItem;i<itemWidths.size();i++){
		offset+=itemWidths[i];
		if(offset>point.x){
			idx=i;
			break;
		}
	}

	if(offset-point.x<18 && idx!=-1){
		CloseTab(idx);
		return;
	}

	if(idx==-1 || idx==GetCurSel())return;

	NMHDR nmh;
	nmh.code = TCN_SELCHANGING;
	nmh.idFrom = NULL;
	nmh.hwndFrom = GetSafeHwnd();
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetSafeHwnd(), (LPARAM)&nmh);
	SetCurSel(idx);
	nmh.code = TCN_SELCHANGE;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetSafeHwnd(), (LPARAM)&nmh);
	RECT rect;
	GetWindowRect(&rect);
	rect.right-=rect.left;
	rect.left=0;
	rect.top=0;
	rect.bottom=TAB_HEIGHT+TAB_TOP_MARGIN;
	RedrawWindow();
}

void CClosableTabCtrl::OnMouseMove(UINT nFlags, CPoint point){
	if(!mouseInside){
		mouseInside=true;
		TRACKMOUSEEVENT tme;
		memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
		tme.cbSize=sizeof(TRACKMOUSEEVENT);
		tme.dwFlags=TME_LEAVE;
		tme.hwndTrack=GetSafeHwnd();
		::TrackMouseEvent(&tme);
	}
	int offset=5;
	int idx=-1;
	int firstItem=0;
	if(m_pSpinCtrl)firstItem=LOWORD(m_pSpinCtrl->GetPos());
	for(int i=firstItem;i<itemWidths.size();i++){
		offset+=itemWidths[i];
		if(offset>point.x){
			idx=i;
			break;
		}
	}
	if(point.x<5)idx=-1;
	if(idx==over_idx)return;
	over_idx=idx;
	RECT rect;
	GetWindowRect(&rect);
	rect.right-=rect.left;
	rect.left=0;
	rect.top=0;
	rect.bottom=TAB_TOP_MARGIN+TAB_HEIGHT;
	RedrawWindow();
}

void CClosableTabCtrl::OnLButtonUp(UINT nFlags, CPoint point){
	CTabCtrl::OnLButtonUp(nFlags, point);
	RECT rect;
	GetWindowRect(&rect);
	rect.right-=rect.left;
	rect.left=0;
	rect.top=0;
	rect.bottom=TAB_TOP_MARGIN+TAB_HEIGHT;
	RedrawWindow();
}

bool CClosableTabCtrl::GetItemRect(int item, LPRECT rect){
	/*if(item<0 || item>itemWidths.size() || itemWidths.size()==0){
		return false;
	}*/
	int offset=5;
	/*for(int i=0;i<item;i++){
		offset+=itemWidths[i];
	}*/
	rect->top=0;
	rect->left=offset;
	rect->bottom=TAB_TOP_MARGIN+TAB_HEIGHT;
	rect->right=offset+100;//itemWidths[item];
	return true;
}
// CClosableTabCtrl message handlers




void CClosableTabCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CTabCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	RedrawWindow();
}


void CClosableTabCtrl::CloseTab(int idx)
{
	int cnt=GetItemCount();
		int sel=GetCurSel();

		if(sel>=idx){
			NMHDR nmh;
			nmh.idFrom = NULL;
			nmh.hwndFrom = GetSafeHwnd();
			nmh.code = TCN_SELCHANGING;
			GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetSafeHwnd(), (LPARAM)&nmh);
			//GetParent()->RedrawWindow();
		}
		TC_ITEM tci;
		tci.mask = TCIF_PARAM;
		GetItem(idx, &tci);
		CTalkTab* pWnd = (CTalkTab *)tci.lParam;
		
		//if(pWnd)pWnd->SendMessage(WM_CLOSE);
		if(pWnd){
			if(talks->tb && pWnd->tabProxy){
			talks->tb->UnregisterTab(pWnd->tabProxy->m_hWnd);
		}
			stdext::hash_map<unsigned int,CTalkTab*>::iterator itr=talks->openedTabs.begin();
		for(int j=0;j<talks->openedTabs.size();j++){
			if(itr->second==pWnd){
				talks->openedTabs.erase(itr);
				break;
			}
			itr++;
		}
		delete pWnd;
		}

		DeleteItem(idx);
		if(cnt==1){
			GetParent()->SendMessage(WM_CLOSE);
		}
		if(sel>=idx){
			NMHDR nmh;
			nmh.idFrom = NULL;
			nmh.hwndFrom = GetSafeHwnd();
			if(sel!=0)SetCurSel(sel-1);
			else SetCurSel(0);
			nmh.code = TCN_SELCHANGE;
			GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetSafeHwnd(), (LPARAM)&nmh);
			//GetParent()->RedrawWindow();
		}
}

BOOL CClosableTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;
	//return CTabCtrl::OnEraseBkgnd(pDC);
}


LRESULT CClosableTabCtrl::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	return HTCLIENT;
}
