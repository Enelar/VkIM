// ContactListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "ContactListCtrl.h"
#include "resource.h"
#include "TalkDlg.h"
#include "Lang.h"
#include "Connector.h"
#include "Settings.h"

extern CSkin skin;
extern CTalkDlg* talks;
extern CConnector con;
// CContactListCtrl

#define G_INDEX_BY_ID(idx, id) idx=id;for(int __j=0;__j<groupsIDs.size();__j++)if(groupsIDs[__j]==id){idx=__j;break;}
#define GID(gi) groupsMode?groupsIDs[gi]:gi

enum{
MENU_USER_PAGE=100,
MENU_PHOTO_WITH,
MENU_VIDEO_WITH,
//MENU_WISHES
};

IMPLEMENT_DYNAMIC(CContactListCtrl, CWnd)

CContactListCtrl::CContactListCtrl()
{
	groupsMode=true;
	showOffline=true;
	searching=false;
	mouseInside=false;
	noDraw=false;
	scrollOffset=0;
	itemHeight=BASE_ITEM_H;
	userTip=NULL;
	viewMode=0;
	fnt.CreateFontW(-11, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	bmpClosed.LoadBitmap( IDB_TREE_CLOSED );
	bmpOpened.LoadBitmap( IDB_TREE_OPENED );

	contactContextMenu.CreatePopupMenu();
	MENUITEMINFO ii;
	ii.cbSize=sizeof(MENUITEMINFO);
	ii.fType=MFT_STRING;
	ii.fMask=MIIM_TYPE|MIIM_ID;
	
	ii.dwTypeData=CLang::Get(L"friend_user_page");
	ii.wID=MENU_USER_PAGE;
	contactContextMenu.InsertMenuItemW(MENU_USER_PAGE, &ii, false);
	ii.dwTypeData=CLang::Get(L"friend_photo_with");
	ii.wID=MENU_PHOTO_WITH;
	contactContextMenu.InsertMenuItemW(MENU_PHOTO_WITH, &ii, false);
	ii.dwTypeData=CLang::Get(L"friend_video_with");
	ii.wID=MENU_VIDEO_WITH;
	contactContextMenu.InsertMenuItemW(MENU_VIDEO_WITH, &ii, false);

	selectedItem=hoverItem=-1;

	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);
	//ii.dwTypeData=CLang::Get(L"friend_wishes");
	//ii.wID=MENU_WISHES;
	//contactContextMenu.InsertMenuItemW(MENU_WISHES, &ii, false);
}

CContactListCtrl::~CContactListCtrl()
{
	if (bmpClosed.GetSafeHandle())
        bmpClosed.DeleteObject();
    if (bmpOpened.GetSafeHandle())
        bmpOpened.DeleteObject();

	for(int i=0;i<items.size();i++){
		//if(items[i].userName)delete items[i].userName;
		//if(items[i].userPhotoURL)delete items[i].userPhotoURL;
		if(items[i].userPhoto)DeleteObject(items[i].userPhoto);
		if(items[i].userPhoto30)DeleteObject(items[i].userPhoto30);
	}
}


BEGIN_MESSAGE_MAP(CContactListCtrl, CWnd)
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSELEAVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CContactListCtrl message handlers




void CContactListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int item=HitTest(point);
	if(item>=0){
		//TRACE("%i\n", items[item].uid);
		POINT* pt=&point;
		ClientToScreen(pt);
		menuUid=items[item].uid;
		contactContextMenu.TrackPopupMenu(TPM_LEFTALIGN, pt->x, pt->y, this);
	}
}


void CContactListCtrl::OnPaint()
{
	if(noDraw)return;
	CPaintDC _dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	CRect rect;
	GetClientRect(rect);
	width=rect.Width();
	int height=rect.Height();

	CDC dc; // Двойная буферизация, чтобы не мигало при перерисовке
	dc.CreateCompatibleDC(&_dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap( &_dc, width, rect.Height());
	if(!dc.m_hDC)return;
	CBitmap* obmp=dc.SelectObject(&bmp);

	if(viewMode==0){
		itemHeight=BASE_ITEM_H;
	}else{
		itemHeight=BIG_ITEM_H;
	}
	
	dc.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));
	CFont* of=dc.SelectObject(&fnt);
	dc.SetBkMode(TRANSPARENT);
	int offset=-scrollOffset;
	if(searching){
		for(int j=0;j<searchItems.size();j++){
			offset+=DrawItem(&dc, searchItems[j], offset);
			if(offset>height+scrollOffset)break;
		}
	}else if(groupsMode){
		for(int i=0;i<groupsNames.size();i++){
			offset+=DrawGroupHeader(&dc, i, offset);
			if(groupsExpanded[i]){
				for(int j=0;j<sortedItems[groupsIDs[i]].size();j++){
					offset+=DrawItem(&dc, sortedItems[groupsIDs[i]][j], offset);
					if(offset>height+scrollOffset)break;
				}
			}
		}
	}else{
		for(int j=0;j<sortedItems[1].size();j++){
			offset+=DrawItem(&dc, sortedItems[1][j], offset);
			if(offset>height+scrollOffset)break;
		}
	}

	skin.shadow1.bmp.DrawStretched(&dc, 0, 0, width, 5, 0, 0, 30, 5);
	skin.shadow2.bmp.DrawStretched(&dc, 0, height-5, width, 5, 0, 0, 30, 5);

	dc.SelectObject(of);
	_dc.BitBlt(0, 0, width, height, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(obmp);
	dc.DeleteDC();
}


void CContactListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if(nSBCode==SB_ENDSCROLL)return;
	CRect rect;
	GetWindowRect(rect);
	if(nSBCode==SB_LINEUP){
		scrollOffset-=BASE_ITEM_H;
	}else if(nSBCode==SB_LINEDOWN){
		scrollOffset+=BASE_ITEM_H;
	}else if(nSBCode==SB_PAGEUP){
		scrollOffset-=rect.Height();
	}else if(nSBCode==SB_PAGEDOWN){
		scrollOffset+=rect.Height();
	}else{
		scrollOffset=nPos;
	}
	if(scrollOffset<0)scrollOffset=0;
	if(scrollOffset>totalHeight-rect.Height())scrollOffset=totalHeight-rect.Height();

	TRACE("Scroll=%d [%d]\n", scrollOffset, nSBCode);
	SCROLLINFO si;
	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_POS;
	si.nPos=scrollOffset;
	SetScrollInfo(SB_VERT, &si);
	RedrawWindow();
	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


HTREEITEM CContactListCtrl::AddItem(ListEntry item, int group)
{
	return 0;
}


int CContactListCtrl::DrawGroupHeader(CDC* dc, int num, int y)
{
	dc->SetTextColor(skin.groupTitleColor);
	int numBG=num;
	numBG%=skin.groupsBG.n-1;
	if(num==groupsNames.size()-1)numBG=skin.groupsBG.n-1;
	skin.groupsBG.bmp.DrawStretched(dc, 0, y, width, BASE_ITEM_H, skin.groupsBG.w*numBG, 0, skin.groupsBG.w, skin.groupsBG.h);
	//dc->FillSolidRect(CRect(0, y, 300, y+BASE_ITEM_H), RGB(128,128,128));
	CDC mdc;
	mdc.CreateCompatibleDC(0);
	HGDIOBJ oo=mdc.SelectObject(groupsExpanded[num]?bmpOpened:bmpClosed);
	dc->BitBlt(1, y+1, 16, 16, &mdc, 0, 0, SRCAND);
	mdc.SelectObject(oo);
	mdc.DeleteDC();
	dc->DrawText(groupsNames[num].c_str(), -1, CRect(BASE_ITEM_H, y, 300, y+BASE_ITEM_H), DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	return BASE_ITEM_H;
}


int CContactListCtrl::DrawItem(CDC* dc, int item, int y)
{
	if(items.size()<=item)return 0;
	if(!items[item].online && !showOffline)return 0;
	if(y<-40){
		return viewMode==0?BASE_ITEM_H:BIG_ITEM_H;
	}
	if(selectedItem==item)dc->SetTextColor(skin.contactOnlineSelColor);
	else dc->SetTextColor(skin.contactOnlineColor);
	if(viewMode==0){
		//if(selectedItem==item)dc->FillSolidRect(CRect(BASE_ITEM_H, y, width, y+BASE_ITEM_H), RGB(0,0,255));
		if(selectedItem==item)skin.listHl.bmp.DrawStretched(dc, BASE_ITEM_H, y, width, BASE_ITEM_H, 0, 0, 0, 0);
		if(hoverItem==item && selectedItem!=item)skin.listHlHover.bmp.DrawStretched(dc, BASE_ITEM_H, y, width, BASE_ITEM_H, 0, 0, 0, 0);
		DrawIconEx(dc->GetSafeHdc(), 1, y+1, items[item].online?skin.iconOnline:skin.iconOffline, 16, 16, 0, 0, DI_NORMAL);
		dc->DrawText(items[item].userName.c_str(), -1, CRect(20, y, width, y+BASE_ITEM_H), DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		return BASE_ITEM_H;
	}else{
		//if(selectedItem==item)dc->FillSolidRect(CRect(0, y, width, y+BIG_ITEM_H), RGB(0,0,255));
		if(selectedItem==item)skin.listHl.bmp.DrawStretched(dc, 1, y, width-2, BIG_ITEM_H, 0, 0, 0, 0);
		if(hoverItem==item && selectedItem!=item)skin.listHlHover.bmp.DrawStretched(dc, 1, y, width-2, BIG_ITEM_H, 0, 0, 0, 0);
		if(hoverItem!=item && selectedItem!=item)dc->FillSolidRect(1, y+BIG_ITEM_H-1, width-2, 1, skin.listLinesColor);
		if(!items[item].online){
			dc->SetTextColor(selectedItem==item?skin.contactOfflineSelColor:skin.contactOfflineColor);
		}
		if(items[item].userPhoto){
			CDC mdc;
			mdc.CreateCompatibleDC(0);
			HBITMAP bmp=items[item].userPhoto;
			bool scale=true;
			if(items[item].userPhoto30){
				bmp=items[item].userPhoto30;
				scale=false;
			}
			HGDIOBJ oo=mdc.SelectObject(bmp);
			BLENDFUNCTION func;
			func.BlendOp=AC_SRC_OVER;
			func.AlphaFormat=0;
			func.SourceConstantAlpha=items[item].online?255:128;
			func.BlendFlags=0;
			if(scale)dc->AlphaBlend(3, y+1, 30, 30, &mdc, 0, 0, 50, 50, func);
			else dc->AlphaBlend(3, y+1, 30, 30, &mdc, 0, 0, 30, 30, func);
			mdc.SelectObject(oo);
			mdc.DeleteDC();
		}
		dc->DrawText(items[item].userName.c_str(), -1, CRect(36, y, width, y+BIG_ITEM_H), DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		return BIG_ITEM_H;
	}
}


void CContactListCtrl::SetOnlineStatus(int uid, bool online)
{
	int idx;
	if((idx=GetItemByUID(uid))!=-1){
		if(!online && IsItemVisible(idx))RedrawWindow();
		items[idx].online=online;
		for(int i=0;i<items[idx].groups.size();i++){
			int ix;
			G_INDEX_BY_ID(ix, items[idx].groups[i])
			Sort(ix, false);
		}
		if(IsItemVisible(idx) && showOffline)RedrawWindow();
		if(IsItemVisible(idx) && !showOffline)UpdateScroll();
	}
}


void CContactListCtrl::Sort(int group, bool redraw, bool idx)
{
	int gid=group;
	noDraw=true;
	if(!groupsMode){
		group=1;
		gid=1;
	}

	vector<int> si=sortedItems[group];

	//if(group==-1)group=gid=0xFFFFFF;
	if(!idx && group<groupsIDs.size() && groupsMode) group=groupsIDs[gid];
	si.clear();
	//TRACE("GID=%i GROUP=%i\n", gid, group);
	for(int i=0;i<items.size();i++){
		bool add=false;
		for(int j=0;j<items[i].groups.size();j++){
			//if(items[i].groups[j]==-1)items[i].groups[j]=groupsNames.size()-1;
			if(items[i].groups[j]==group || !groupsMode){
				add=true;
				break;
			}
		}
		if(add){
			si.push_back(i);
		}
	}

	

	for(int i=0;i<si.size();i++){
		for(int j=0;j<si.size()-1;j++){
			if((items[si[j]].online==items[si[j+1]].online && items[si[j]].userName.compare(items[si[j+1]].userName)>0) || 
				(!items[si[j]].online && items[si[j+1]].online)){
				int tmp=si[j+1];
				si[j+1]=si[j];
				si[j]=tmp;
			}
		}
	}

	sortedItems.erase(sortedItems.find(group));
	sortedItems[group]=si;
	noDraw=false;
	if(redraw){
		RedrawWindow();
		UpdateScroll();
	}
}


void CContactListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	int itm=HitTest(point);
	if(itm==0xF0000000)return;
	if(itm<0){
		groupsExpanded[-itm-1]=!groupsExpanded[-itm-1];
		UpdateScroll();
	}else{
		selectedItem=itm;
		RedrawWindow();
	}
	CWnd::OnLButtonDown(nFlags, point);
}


void CContactListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonUp(nFlags, point);
}


void CContactListCtrl::UpdateScroll(void)
{
	if(viewMode==0){
		itemHeight=BASE_ITEM_H;
	}else{
		itemHeight=BIG_ITEM_H;
	}
	int offset=0;
	if(searching){
		for(int j=0;j<searchItems.size();j++){
			if(!showOffline){
				if(items[searchItems[j]].online)offset+=itemHeight;
			}
			else offset+=itemHeight;
		}
	}else if(groupsMode){
		for(int i=0;i<groupsNames.size();i++){
			offset+=BASE_ITEM_H;
			if(groupsExpanded[i]){
				for(int j=0;j<sortedItems[groupsIDs[i]].size();j++){
					if(!showOffline){
						if(items[sortedItems[groupsIDs[i]][j]].online)offset+=itemHeight;
					}
					else offset+=itemHeight;
				}
			}
		}
	}else{
		if(!showOffline){
			for(int j=0;j<sortedItems[1].size();j++){
				if(items[sortedItems[1][j]].online)offset+=itemHeight;
			}
		}else{
			offset=itemHeight*items.size();
		}
	}
	CRect rect;
	GetClientRect(rect);
	SCROLLINFO si;
	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_RANGE|SIF_PAGE;
	si.nMax=offset;
	si.nMin=0;
	si.nPage=rect.Height();
	totalHeight=offset;
	SetScrollInfo(SB_VERT, &si);
	EnableScrollBar(SB_BOTH, ESB_ENABLE_BOTH);
	if(scrollOffset>totalHeight-rect.Height() && scrollOffset>0)scrollOffset=totalHeight-rect.Height();
	if(scrollOffset<0)scrollOffset=0;
	RedrawWindow();
}


void CContactListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int itm=HitTest(point);
	if(itm>=items.size())return;
	if(itm<0){
	}else{
		if(!talks->isCreated){
			talks->Create(IDD_TALKDLG, GetDesktopWindow());
		}
		talks->ShowWindow(SW_SHOW);
		talks->ShowWindow(SW_RESTORE);
		//talks->OpenNewTab(items[itm].uid, items[itm].userName);
		talks->OpenNewTab(&items[itm]);
		talks->SetForegroundWindow();
		talks->PostMessageW(WM_ACTIVATE, WA_ACTIVE);
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}


void CContactListCtrl::SetGroupsMode(bool gm)
{
	//if(gm==groupsMode)return;
	groupsMode=gm;
	sortedItems.clear();

	if(gm){
		for(int i=0;i<groupsNames.size();i++){
			std::vector<int> nv;
			sortedItems[groupsIDs[i]]=nv;
			Sort(i, false);
		}
	}else{
		std::vector<int> nv;
		sortedItems[1]=nv;
		Sort(1, false, true);
	}
	scrollOffset=0;
	UpdateScroll();
	RedrawWindow();
}


void CContactListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if(nChar==VK_DOWN && selectedItem<items.size()){
		selectedItem++;
		RedrawWindow();
	}
	if(nChar==VK_UP && selectedItem>0){
		selectedItem--;
		RedrawWindow();
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CContactListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(rect);
	if(totalHeight>rect.Height()){
	scrollOffset+=-zDelta/2;
	if(scrollOffset<0)scrollOffset=0;
	if(scrollOffset>totalHeight-rect.Height())scrollOffset=totalHeight-rect.Height();
	SCROLLINFO si;
	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_POS;
	si.nPos=scrollOffset;
	SetScrollInfo(SB_VERT, &si);
	RedrawWindow();
	}
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


UINT CContactListCtrl::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}


int CContactListCtrl::HitTest(CPoint point)
{
	int offset=-scrollOffset;
	if(searching){
		for(int j=0;j<searchItems.size();j++){
			if(point.y>=offset && point.y<offset+itemHeight){
				return searchItems[j];
			}
			if(!showOffline){
				if(items[searchItems[j]].online)offset+=itemHeight;
			}
			else offset+=itemHeight;
		}
	}else if(groupsMode){
		for(int i=0;i<groupsNames.size();i++){
			if(point.y>=offset && point.y<offset+BASE_ITEM_H){
					return -i-1;
				}
			offset+=BASE_ITEM_H;
			if(groupsExpanded[i]){
				for(int j=0;j<sortedItems[groupsIDs[i]].size();j++){
					if(point.y>=offset && point.y<offset+itemHeight){
						return sortedItems[groupsIDs[i]][j];
					}
					if(!showOffline){
						if(items[sortedItems[groupsIDs[i]][j]].online)offset+=itemHeight;
					}
					else offset+=itemHeight;
				}
			}
		}
	}else{
		for(int j=0;j<sortedItems[1].size();j++){
			if(point.y>=offset && point.y<offset+itemHeight){
				return sortedItems[1][j];
			}
			if(!showOffline){
				if(items[sortedItems[1][j]].online)offset+=itemHeight;
			}
			else offset+=itemHeight;
		}
	}
	return 0xF0000000;
}

void CContactListCtrl::StartSearch(void)
{
	searching=true;
	scrollOffset=0;
}


void CContactListCtrl::UpdateSearch(wchar_t* q)
{
	CString wq=q;
	wq.MakeLower();
	searchItems.clear();
	for(int i=0;i<wq.GetLength();i++){
		if(wq[i]>=L'А' && wq[i]<=L'Я'){
			wq.SetAt(i, wq[i]+(L'а'-L'А'));
		}
	}
	for(int i=0;i<items.size();i++){
		CString ws1=items[i].userName.c_str();
		ws1.MakeLower();
		for(int j=0;j<ws1.GetLength();j++){
			if(ws1[j]>=L'А' && ws1[j]<=L'Я'){
				ws1.SetAt(j, ws1[j]+(L'а'-L'А'));
			}
		}
		if(ws1.Find(wq)>=0){
			//_InsertItem(items_array[i], NULL);
			searchItems.push_back(i);
		}
	}

	for(int i=0;i<searchItems.size();i++){
		for(int j=0;j<searchItems.size()-1;j++){
			if((items[searchItems[j]].online==items[searchItems[j+1]].online && items[searchItems[j]].userName.compare(items[searchItems[j+1]].userName)>0) || 
				(!items[searchItems[j]].online && items[searchItems[j+1]].online)){
				int tmp=searchItems[j+1];
				searchItems[j+1]=searchItems[j];
				searchItems[j]=tmp;
			}
		}
	}

	UpdateScroll();
}


void CContactListCtrl::StopSearch(void)
{
	searching=false;
	UpdateScroll();
}


int CContactListCtrl::GetItemByUID(int uid){
	for(int i=0;i<items.size();i++){
		if(items[i].uid==uid)return i;
	}
	return -1;
}

void CContactListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	int ht=HitTest(point);
	if(ht!=hoverItem){
		hoverItem=ht;
		RedrawWindow();
		if(CSettings::flPopup && ht>=0){
		KillTimer(100);
		SetTimer(100, 1000, NULL);
		tipItem=ht;
		if(userTip){
			::EnterCriticalSection(&con.tipCs);
			delete userTip;
			userTip=NULL;
			::LeaveCriticalSection(&con.tipCs);
		}
		}
	}
	if(!mouseInside){
		mouseInside=true;
		TRACKMOUSEEVENT tme;
		memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
		tme.cbSize=sizeof(TRACKMOUSEEVENT);
		tme.dwFlags=TME_LEAVE;
		tme.hwndTrack=GetSafeHwnd();
		::TrackMouseEvent(&tme);
	}
	CWnd::OnMouseMove(nFlags, point);
}


bool CContactListCtrl::IsItemVisible(int item)
{
	bool result=false;
	CRect rect;
	GetClientRect(rect);
	int offset=-scrollOffset;
	if(searching){
		for(int j=0;j<searchItems.size();j++){
			if(searchItems[j]==item){
				//return offset>-itemHeight && offset<rect.Height()+itemHeight;
				if(!result)result=offset>-itemHeight && offset<rect.Height()+itemHeight;
			}
			offset+=itemHeight;
		}
	}else if(groupsMode){
		for(int i=0;i<groupsNames.size();i++){
			offset+=BASE_ITEM_H;
			if(groupsExpanded[i]){
				for(int j=0;j<sortedItems[GID(i)].size();j++){
					if(sortedItems[GID(i)][j]==item){
						//return offset>-itemHeight && offset<rect.Height()+itemHeight;
						if(!result)result=offset>-itemHeight && offset<rect.Height()+itemHeight;
					}
					offset+=itemHeight;
				}
			}
		}
	}else{
		for(int j=0;j<sortedItems[1].size();j++){
			if(sortedItems[1][j]==item){
				//return offset>-itemHeight && offset<rect.Height()+itemHeight;
				if(!result)result=offset>-itemHeight && offset<rect.Height()+itemHeight;
			}
			offset+=itemHeight;
		}
	}
	return result;
}


BOOL CContactListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(wParam==MENU_USER_PAGE){
		wchar_t* buf=new wchar_t[512];
		wsprintf(buf, L"http://vkontakte.ru/id%i", menuUid);
		ShellExecute(GetSafeHwnd(), L"open", buf, NULL, NULL, SW_SHOWNORMAL);
		delete buf;
		return true;
	}
	if(wParam==MENU_PHOTO_WITH){
		wchar_t* buf=new wchar_t[512];
		wsprintf(buf, L"http://vkontakte.ru/photos.php?act=user&id=%i", menuUid);
		ShellExecute(GetSafeHwnd(), L"open", buf, NULL, NULL, SW_SHOWNORMAL);
		delete buf;
		return true;
	}
	if(wParam==MENU_VIDEO_WITH){
		wchar_t* buf=new wchar_t[512];
		wsprintf(buf, L"http://vkontakte.ru/video.php?act=tagview&id=%i", menuUid);
		ShellExecute(GetSafeHwnd(), L"open", buf, NULL, NULL, SW_SHOWNORMAL);
		delete buf;
		return true;
	}
	return CWnd::OnCommand(wParam, lParam);
}


BOOL CContactListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;
	//return CWnd::OnEraseBkgnd(pDC);
}


void CContactListCtrl::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	mouseInside=false;
	if(hoverItem!=-1){
		hoverItem=-1;
		RedrawWindow();
	}
	if(userTip){
		EnterCriticalSection(&con.cs);
		delete userTip;
		userTip=NULL;
		LeaveCriticalSection(&con.cs);
	}
	CWnd::OnMouseLeave();
}


void CContactListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100){
		KillTimer(100);
		tipWaiting=false;
		if(tipItem==hoverItem){
			if(userTip){
				EnterCriticalSection(&con.cs);
				delete userTip;
				LeaveCriticalSection(&con.cs);
			}
			userTip=new CUserInfoTip();
			userTip->item=hoverItem;
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);
			POINT pt;
			GetCursorPos(&pt);
			int sw=GetSystemMetrics(SM_CXSCREEN);
			POINT p={!(sw-rect.right>250)?rect.left-250:rect.left+rect.Width(), pt.y-36};
			userTip->pos=p;
			userTip->flip=sw-rect.right>250;
			userTip->StartAnim();
		}
	}
	CWnd::OnTimer(nIDEvent);
}
