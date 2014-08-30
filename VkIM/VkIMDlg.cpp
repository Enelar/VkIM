// VkIMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "VkIMDlg.h"
#include "BrowserDlg.h"
#include "VkAPIRequest.h"
#include "Connector.h"
#include "Utils.h"
#include "StatusDlg.h"

#include <ShlObj.h>
#include <dwmapi.h>
#include <MMSystem.h>

#include "JSON_Defs.h"
#include "jsonmain.h"
#include <hash_map>
#include "TalkDlg.h"
#include "Skin.h"
#include "Lang.h"
#include "Settings.h"
#include "AboutDlg.h"
#include "OpenTalkDlg.h"


extern CSkin skin;
extern unsigned int uid;
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

#define WP_CAPTION 1
#define CS_ACTIVE 1
#define CS_INACTIVE 2

#define SNAP_DIST 10

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum{
	MENU_OPEN=100,
	MENU_PAGE,
	MENU_VK,
	MENU_SETTINGS,
	MENU_EXIT,
	MENU_V_FRIENDS=110,
	MENU_V_PHOTOS,
	MENU_V_VIDEOS,
	MENU_V_MUSIC,
	MENU_V_MESSAGES,
	MENU_V_NOTES,
	MENU_V_GROUPS,
	MENU_V_EVENTS,
	MENU_V_NEWS,
	MENU_V_BOOKMARKS,
	MENU_V_SETTINGS,
	MENU_V_OFFERS,
	MENU_V_OPINIONS,
	MENU_V_APPS,
	MENU_V_QUESTIONS,
	MENU_M_SHOW_OFFLINE,
	MENU_M_SHOW_LISTS,
	MENU_M_CL_MODE,
	MENU_M_ABOUT,
	MENU_M_LOGOUT,
	MENU_CL_NAMES,
	MENU_CL_PHOTOS,
	MENU_CL_EXTENDED,
	MENU_M_OPEN_TALK
};

extern CConnector con;
extern CTalkDlg* talks;

CVkIMDlg::CVkIMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVkIMDlg::IDD, pParent)
	, searchValue(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	settings=NULL;
	menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
	hasUxtheme=false;
	isWindowActive=true;
	mouseInNC=false;
	pwnd=pParent->GetSafeHwnd();
	startHidden=false;
	sdlg=NULL;
	logout=false;
}

CVkIMDlg::~CVkIMDlg(){
if(sdlg){
		delete sdlg;
	}
}

void CVkIMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TREE1, friendList);
	DDX_Control(pDX, IDC_USER_NAME, userNameLabel);
	DDX_Control(pDX, IDC_USER_STATUS, userStatusField);
	DDX_Control(pDX, IDC_AVA, userAvatar);
	DDX_Control(pDX, IDC_CL_SEARCH, searchField);
	DDX_Control(pDX, IDC_USER_STATUS_S, userStatusLabel);
	DDX_Text(pDX, IDC_CL_SEARCH, searchValue);
}

BEGIN_MESSAGE_MAP(CVkIMDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CVkIMDlg::OnNMDblclkTree1)
	ON_STN_CLICKED(IDC_USER_NAME, &CVkIMDlg::OnStnClickedUserName)
	ON_EN_SETFOCUS(IDC_USER_STATUS, &CVkIMDlg::OnEnSetfocusUserStatus)
	ON_WM_SIZE()
	ON_MESSAGE(WM_OPENTAB, OnOpenTab)
	ON_MESSAGE(WM_TRAY_ICON, OnTrayIcon)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_STN_CLICKED(IDC_AVA, &CVkIMDlg::OnStnClickedAva)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_SHOW_NOTIFICATION, OnShowNotification)
	ON_MESSAGE(WM_HIDE_NOTIFICATION, OnHideNotification)
	ON_MESSAGE(WM_HIDE_LOADING, &CVkIMDlg::OnHideLoading)
	ON_STN_CLICKED(IDC_USER_STATUS_S, &CVkIMDlg::OnStnClickedUserStatusS)
	ON_MESSAGE(WM_STATUS_ENTER, &CVkIMDlg::OnStatusEnter)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_EN_KILLFOCUS(IDC_USER_STATUS, &CVkIMDlg::OnEnKillfocusUserStatus)
	ON_WM_NCHITTEST()
	ON_EN_CHANGE(IDC_CL_SEARCH, &CVkIMDlg::OnEnChangeClSearch)
	ON_WM_NCPAINT()
	ON_WM_NCMOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_NCACTIVATE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_TIMER()
	ON_WM_NCMOUSELEAVE()
	ON_MESSAGE(WM_UPDATE_COUNTERS, &CVkIMDlg::OnUpdateCounters)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSELEAVE()
	ON_WM_MOVING()
	ON_WM_ENTERSIZEMOVE()
	ON_WM_EXITSIZEMOVE()
	ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, &CVkIMDlg::OnTaskbarcreated)
END_MESSAGE_MAP()


// CVkIMDlg message handlers

BOOL CVkIMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	//friendList.SetMode(CSettings::listMode);
	friendList.viewMode=CSettings::listMode;
	friendList.showOffline=CSettings::showOffline;

	friendList.Create(AfxRegisterWndClass(CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS), L"", WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_CLIPCHILDREN, CRect(0, 0, 300, 300), this, 0);
	con.LoadContactList(this);

	this->GetDlgItem(IDC_TREE1)->ShowWindow(SW_HIDE);

	NOTIFYICONDATA iconData;
	memset(&iconData, 0, sizeof(NOTIFYICONDATA));
	iconData.cbSize=sizeof(NOTIFYICONDATA);
	iconData.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP;
	iconData.hWnd=GetSafeHwnd();
	iconData.hIcon=::LoadIconW(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TRAY_D));
	iconData.uID=1234;
	memcpy(&iconData.szTip, L"Vk.IM", 128);
	iconData.uCallbackMessage=WM_TRAY_ICON;
	Shell_NotifyIcon(NIM_ADD, &iconData);

	HMODULE hm=LoadLibrary(L"dwmapi.dll");
	if(hm){
		DWMSETWINDOWATTRIBUTE* swa=(DWMSETWINDOWATTRIBUTE*)GetProcAddress(hm, "DwmSetWindowAttribute");
		int i=DWMNCRP_ENABLED;
		(swa)(GetSafeHwnd(), DWMWA_ALLOW_NCPAINT, &i, sizeof(i));
		FreeLibrary(hm);
	}

	// Грузим функции для тем из диэльэльки. Ее может не быть, поэтому напрямую их вызывать нежелательно.
	hm=LoadLibrary(L"uxtheme.dll");
	if(hm){
		hasUxtheme=true;
		_IsThemeActive=(ISTHEMEACTIVE*)GetProcAddress(hm, "IsThemeActive");
		_OpenThemeData=(OPENTHEMEDATA*)GetProcAddress(hm, "OpenThemeData");
		_CloseThemeData=(CLOSETHEMEDATA*)GetProcAddress(hm, "CloseThemeData");
		_DrawThemeBackground=(DRAWTHEMEBACKGROUND*)GetProcAddress(hm, "DrawThemeBackground");
		FreeLibrary(hm);
	}
	memset(&counters, 0, sizeof(Counters));

	// Создаем меню
	trayMenu.CreatePopupMenu();
	servMenu.CreatePopupMenu();
	mainMenu.CreatePopupMenu();
	listModeMenu.CreatePopupMenu();
	MENUITEMINFO ii;
	ii.cbSize=sizeof(MENUITEMINFO);
	ii.fType=MFT_STRING;
	ii.fMask=MIIM_TYPE|MIIM_ID;

	ii.dwTypeData=CLang::Get(L"menu_open_list");
	ii.wID=MENU_OPEN;
	trayMenu.InsertMenuItemW(100, &ii, false);

	ii.dwTypeData=CLang::Get(L"menu_my_page");
	ii.wID=MENU_PAGE;
	trayMenu.InsertMenuItemW(101, &ii, false);

	ii.dwTypeData=CLang::Get(L"menu_vk_services");
	ii.wID=MENU_VK;
	ii.fMask=MIIM_TYPE|MIIM_ID|MIIM_SUBMENU;
	ii.hSubMenu=servMenu.GetSafeHmenu();
	trayMenu.InsertMenuItemW(102, &ii, false);
	ii.hSubMenu=NULL;
	ii.fMask=MIIM_TYPE|MIIM_ID;

	ii.dwTypeData=CLang::Get(L"settings");
	ii.wID=MENU_SETTINGS;
	trayMenu.InsertMenuItemW(103, &ii, false);

	ii.dwTypeData=CLang::Get(L"mainmenu_logout");
	ii.wID=MENU_M_LOGOUT;
	trayMenu.InsertMenuItemW(129, &ii, false);

	ii.dwTypeData=CLang::Get(L"menu_exit_app");
	ii.wID=MENU_EXIT;
	trayMenu.InsertMenuItemW(104, &ii, false);

	trayMenu.SetDefaultItem(0, true);


	ii.dwTypeData=CLang::Get(L"menu_vk_friends");
	ii.wID=MENU_V_FRIENDS;
	servMenu.InsertMenuItemW(110, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_photos");
	ii.wID=MENU_V_PHOTOS;
	servMenu.InsertMenuItemW(111, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_videos");
	ii.wID=MENU_V_VIDEOS;
	servMenu.InsertMenuItemW(112, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_music");
	ii.wID=MENU_V_MUSIC;
	servMenu.InsertMenuItemW(113, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_messages");
	ii.wID=MENU_V_MESSAGES;
	servMenu.InsertMenuItemW(114, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_notes");
	ii.wID=MENU_V_NOTES;
	servMenu.InsertMenuItemW(115, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_groups");
	ii.wID=MENU_V_GROUPS;
	servMenu.InsertMenuItemW(116, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_events");
	ii.wID=MENU_V_EVENTS;
	servMenu.InsertMenuItemW(117, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_news");
	ii.wID=MENU_V_NEWS;
	servMenu.InsertMenuItemW(118, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_bookmarks");
	ii.wID=MENU_V_BOOKMARKS;
	servMenu.InsertMenuItemW(119, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_settings");
	ii.wID=MENU_V_SETTINGS;
	servMenu.InsertMenuItemW(120, &ii, false);
	servMenu.AppendMenuW(MF_SEPARATOR);
	ii.dwTypeData=CLang::Get(L"menu_vk_offers");
	ii.wID=MENU_V_OFFERS;
	servMenu.InsertMenuItemW(121, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_opinions");
	ii.wID=MENU_V_OPINIONS;
	servMenu.InsertMenuItemW(122, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_apps");
	ii.wID=MENU_V_APPS;
	servMenu.InsertMenuItemW(123, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_vk_questions");
	ii.wID=MENU_V_QUESTIONS;
	servMenu.InsertMenuItemW(124, &ii, false);

	ii.fMask|=MIIM_STATE;
	ii.fState=CSettings::showLists?MFS_CHECKED:MFS_UNCHECKED;
	ii.dwTypeData=CLang::Get(L"mainmenu_lists");
	ii.wID=MENU_M_SHOW_LISTS;
	mainMenu.InsertMenuItemW(126, &ii, false);

	ii.fState=CSettings::showOffline?MFS_CHECKED:MFS_UNCHECKED;
	ii.dwTypeData=CLang::Get(L"mainmenu_offline");
	ii.wID=MENU_M_SHOW_OFFLINE;
	mainMenu.InsertMenuItemW(127, &ii, false);

	ii.dwTypeData=CLang::Get(L"mainmenu_list_mode");
	ii.wID=MENU_M_CL_MODE;
	ii.fMask=MIIM_TYPE|MIIM_ID|MIIM_SUBMENU;
	ii.hSubMenu=listModeMenu;
	mainMenu.InsertMenuItemW(127, &ii, false);
	ii.fMask=MIIM_TYPE|MIIM_ID;

	ii.dwTypeData=CLang::Get(L"mainmenu_open_talk");
	ii.wID=MENU_M_OPEN_TALK;
	mainMenu.InsertMenuItemW(140, &ii, false);
	mainMenu.AppendMenuW(MF_SEPARATOR);
	ii.dwTypeData=CLang::Get(L"mainmenu_about");
	ii.wID=MENU_M_ABOUT;
	mainMenu.InsertMenuItemW(128, &ii, false);
	ii.dwTypeData=CLang::Get(L"mainmenu_logout");
	ii.wID=MENU_M_LOGOUT;
	mainMenu.InsertMenuItemW(129, &ii, false);
	ii.dwTypeData=CLang::Get(L"menu_exit_app");
	ii.wID=MENU_EXIT;
	mainMenu.InsertMenuItemW(104, &ii, false);

	ii.fMask|=MIIM_STATE;
	ii.fState=CSettings::listMode==0?MFS_CHECKED:MFS_UNCHECKED;
	ii.fType|=MFT_RADIOCHECK;
	ii.dwTypeData=CLang::Get(L"listmode_simple");
	ii.wID=MENU_CL_NAMES;
	listModeMenu.InsertMenuItemW(130, &ii, false);
	ii.dwTypeData=CLang::Get(L"listmode_standard");
	ii.fState=CSettings::listMode==1?MFS_CHECKED:MFS_UNCHECKED;
	ii.wID=MENU_CL_PHOTOS;
	listModeMenu.InsertMenuItemW(131, &ii, false);

	searchField.Resize();

	userStatusField.ShowWindow(SW_HIDE);
	userStatusField.callbackMsg=WM_STATUS_ENTER;
	CRect rect;
	GetClientRect(rect);
	searchField.SetWindowPos(NULL, 0, 56, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	friendList.SetWindowPos(NULL, 0, 79, rect.Width(), rect.Height()-103, SWP_NOZORDER);

	::SetClassLong(userNameLabel.GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_HAND));

	friendList.ShowWindow(SW_HIDE);
	SetTimer(100, 250, NULL);

	if(CSettings::enableEvents){
		SetTimer(101, CSettings::eventsRefresh, NULL);
		con.GetCounters();
	}
	onCounters=false;
	onCounter=-1;
	tooltip.init();

	userNameLabel.color=skin.nameColor;
	userStatusLabel.color=skin.statusColor;

	if(CSettings::listW>0 && CSettings::listH>0){
		int scrw=GetSystemMetrics(SM_CXSCREEN);
		int scrh=GetSystemMetrics(SM_CYSCREEN);
		if(CSettings::listX+100>scrw)CSettings::listX=scrw-CSettings::listW;
		if(CSettings::listY+100>scrh)CSettings::listY=scrh-CSettings::listH;
		SetWindowPos(NULL, CSettings::listX, CSettings::listY, CSettings::listW, CSettings::listH, SWP_NOZORDER);
	}

	userAvatar.SetWindowPos(NULL, 3, 3, 50, 50, SWP_NOZORDER);

	mouseInside=true;
	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);
	sProgressFrame=-1;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVkIMDlg::OnPaint()
{
		CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVkIMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVkIMDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
/*	HTREEITEM selectedItem=friendList.GetSelectedItem();
	if(friendList.ItemHasChildren(selectedItem) || (friendList.groupsMode && friendList.GetParentItem(selectedItem)==0))return;
	if(!talks->isCreated){
		talks->Create(IDD_TALKDLG, GetDesktopWindow());
	}
	talks->ShowWindow(SW_SHOW);
	talks->OpenNewTab(friendList.items[selectedItem].uid, friendList.items[selectedItem].userName);
	talks->SetForegroundWindow();
	talks->PostMessageW(WM_ACTIVATE, WA_ACTIVE);*/
}


void CVkIMDlg::OnStnClickedUserName()
{
	// TODO: Add your control notification handler code here
	wchar_t* buf=new wchar_t[256];
		wsprintf(buf, L"http://vkontakte.ru/id%i", uid);
		ShellExecute(GetSafeHwnd(), L"open", buf, NULL, NULL, SW_SHOWNORMAL);
		delete buf;
}


void CVkIMDlg::OnEnSetfocusUserStatus()
{
	// TODO: Add your control notification handler code here
	userStatusField.SetReadOnly(false);
}


void CVkIMDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	RECT rect;
	if(friendList){
		friendList.GetWindowRect(&rect);
		int d=103;
		if(!CSettings::enableEvents)d-=24;
		friendList.SetWindowPos(NULL, 0, 0, cx, cy-d, SWP_NOMOVE | SWP_NOZORDER);}
		if(userNameLabel)userNameLabel.SetWindowPos(NULL, 58, 8, cx-60, 16, SWP_NOZORDER);
		if(userStatusField)userStatusField.SetWindowPos(NULL, 58, 27, cx-60, 20, SWP_NOZORDER);
		if(userStatusLabel)userStatusLabel.SetWindowPos(NULL, 58, 30, cx-60, 16, SWP_NOZORDER);
		if(searchField)searchField.SetWindowPos(NULL, 0, 0, cx, 23, SWP_NOMOVE|SWP_NOZORDER);
		CRect rect1;
		if(userNameLabel){userNameLabel.GetClientRect(&rect1);
		userNameLabel.ClientToScreen(&rect1);
		ScreenToClient(&rect1);
		InvalidateRect(&rect1, TRUE);
		}
		if(userStatusLabel){
		userStatusLabel.GetClientRect(&rect1);
		userStatusLabel.ClientToScreen(&rect1);
		ScreenToClient(&rect1);
		InvalidateRect(&rect1, TRUE);
		CRect rect2(0, cy-24, cx, cy);
		InvalidateRect(rect2, true);
		}
		if(userStatusField)userStatusField.RedrawWindow();

	if(sdlg){
	CRect r;
	userStatusLabel.GetWindowRect(r);
	int sdy=r.top;
	GetWindowRect(r);
	CRect r2;
	sdlg->GetWindowRect(r2);
	int sdx=r.left-(r2.Width()-r.Width())/2;
	sdlg->SetWindowPos(NULL, sdx, sdy, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}
	// TODO: Add your message handler code here
}


BOOL CVkIMDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	RECT rect;
	GetClientRect(&rect);
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;
	CRect wrect;
	GetClientRect(wrect);
	CPoint center=CRect(0, 79, wrect.Width(), wrect.Height()-103+79).CenterPoint();
	if(!friendList.IsWindowVisible()){
	pDC->FillSolidRect(0, 0, w, h, RGB(255,255,255));
	skin.progress.bmp.DrawUnscaled(pDC, center.x-skin.progress.w/2, center.y-skin.progress.h/2, skin.progress.w, skin.progress.h, skin.progress.w*progressFrame, 0);
	}
	skin.userInfoBG.bmp.DrawStretched(pDC, 0, 0, w, 56, 0, 0, 0, 0);
	//TRACE("P=%i\n", skin.progressSmall.w*sProgressFrame);
	if(sProgressFrame>=0)skin.progressSmall.bmp.DrawUnscaled(pDC, 60, 30, skin.progressSmall.w, skin.progressSmall.h, skin.progressSmall.w*sProgressFrame, 0);
	
	if(CSettings::enableEvents){
		skin.listBottomBar.bmp.DrawStretched(pDC, 0, h-24, w, 24, 0, 0, 0, 0);
	CFont fnt;
	fnt.CreateFontW(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	CFont* oldFont=pDC->SelectObject(&fnt);

	int offset=2;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(skin.eventsCountColor);
	int* ct=(int*)&counters;
	bool hasEvents=false;
	countersWidth.clear();
	for(int i=0;i<10;i++){
		if(ct[i]>0 && CSettings::notifyForEvents[i]){
		DrawIconEx(pDC->m_hDC, offset, h-20, skin.vkServicesIcons[i], 16, 16, 0, NULL, DI_NORMAL);
		offset+=18;
		wchar_t* s=new wchar_t[10];
		_itow(ct[i], s, 10);
		CSize ts=pDC->GetTextExtent(s, wcslen(s));
		pDC->DrawText(s, -1, CRect(offset, h-24, offset+ts.cx, h), DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		offset+=ts.cx+8;
		delete s;
		hasEvents=true;
		countersWidth.push_back(18+ts.cx+8);
		}else{
			countersWidth.push_back(0);
		}
	}
	if(!hasEvents){
		pDC->SetTextColor(skin.noNewEventsColor);
		pDC->DrawText(CLang::Get(L"no_new_events"), -1, CRect(0, h-24, w, h), DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}

	pDC->SelectObject(oldFont);
	}

	return true;
}


LRESULT CVkIMDlg::OnOpenTab(WPARAM wParam, LPARAM lParam){
	talks->noFocus=true;
	talks->Create(IDD_TALKDLG, GetDesktopWindow());
	talks->ShowWindow(SW_SHOWMINNOACTIVE);
	return 1;
}


LRESULT CVkIMDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam){
	if(lParam==WM_LBUTTONDBLCLK){
		startHidden=false;
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}
	if(lParam==WM_RBUTTONDOWN){
		POINT pt;
		GetCursorPos(&pt);
		::SetForegroundWindow(pwnd);
		trayMenu.TrackPopupMenu(TPM_RIGHTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
	}
	return 0;
}

void CVkIMDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(sdlg)return;
	ShowWindow(SW_HIDE);
	//CDialog::OnClose();
}


BOOL CVkIMDlg::OnCommand(WPARAM wParam, LPARAM lParam){
	if(wParam==MENU_OPEN){
		startHidden=false;
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
		return true;
	}
	if(wParam==MENU_PAGE){
		wchar_t* buf=new wchar_t[256];
		wsprintf(buf, L"http://vkontakte.ru/id%i", uid);
		ShellExecute(GetSafeHwnd(), L"open", buf, NULL, NULL, SW_SHOWNORMAL);
		delete buf;
		return true;
	}
	if(wParam==MENU_SETTINGS){
		if(settings){delete settings;}
	settings=new CSettingsDlg();
	settings->Create(CSettingsDlg::IDD, CWnd::GetDesktopWindow());
	settings->ShowWindow(SW_SHOW);
		return true;
	}
	if(wParam==MENU_EXIT){
		if(GetDriveType(NULL)==DRIVE_REMOVABLE)con.DeleteCookies();
		CDialog::OnOK();
		return true;
	}

	if(wParam==MENU_V_FRIENDS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/friends.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_VIDEOS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/video.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_PHOTOS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/photos.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_MUSIC){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/audio.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_MESSAGES){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/mail.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_NOTES){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/notes.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_GROUPS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/groups.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_EVENTS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/events.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_NEWS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/newsfeed.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_BOOKMARKS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/fave.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_SETTINGS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/settings.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_OFFERS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/matches.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_OPINIONS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/opinions.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_APPS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/apps.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_V_QUESTIONS){
		ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/questions.php", NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
	if(wParam==MENU_M_SHOW_OFFLINE){
		if(friendList.showOffline){
			friendList.showOffline=false;
			friendList.UpdateScroll();
			MENUITEMINFO mi;
			mi.cbSize=sizeof(MENUITEMINFO);
			mi.fMask=MIIM_STATE;
			mi.fState=MFS_UNCHECKED;
			mainMenu.SetMenuItemInfoW(MENU_M_SHOW_OFFLINE, &mi);
			CSettings::showOffline=false;
			CSettings::Save();
		}else{
			friendList.showOffline=true;
			friendList.UpdateScroll();
			MENUITEMINFO mi;
			mi.cbSize=sizeof(MENUITEMINFO);
			mi.fMask=MIIM_STATE;
			mi.fState=MFS_CHECKED;
			mainMenu.SetMenuItemInfoW(MENU_M_SHOW_OFFLINE, &mi);
			CSettings::showOffline=true;
			CSettings::Save();
		}
		return true;
	}
	if(wParam==MENU_M_SHOW_LISTS){
		if(friendList.groupsMode){
			friendList.SetGroupsMode(false);
			MENUITEMINFO mi;
			mi.cbSize=sizeof(MENUITEMINFO);
			mi.fMask=MIIM_STATE;
			mi.fState=MFS_UNCHECKED;
			mainMenu.SetMenuItemInfoW(MENU_M_SHOW_LISTS, &mi);
			CSettings::showLists=false;
			CSettings::Save();
		}else{
			friendList.SetGroupsMode(true);
			MENUITEMINFO mi;
			mi.cbSize=sizeof(MENUITEMINFO);
			mi.fMask=MIIM_STATE;
			mi.fState=MFS_CHECKED;
			mainMenu.SetMenuItemInfoW(MENU_M_SHOW_LISTS, &mi);
			CSettings::showLists=true;
			CSettings::Save();
		}
		return true;
	}
	if(wParam==MENU_CL_NAMES){
		MENUITEMINFO mi;
		mi.cbSize=sizeof(MENUITEMINFO);
		mi.fMask=MIIM_STATE;
		mi.fState=MFS_CHECKED;
		mainMenu.SetMenuItemInfoW(MENU_CL_NAMES, &mi);
		mi.fState=MFS_UNCHECKED;
		mainMenu.SetMenuItemInfoW(MENU_CL_PHOTOS, &mi);
		mainMenu.SetMenuItemInfoW(MENU_CL_EXTENDED, &mi);
		//friendList.SetMode(DM_NAMES_LIST);
		friendList.viewMode=DM_NAMES_LIST;
		friendList.UpdateScroll();
		CSettings::listMode=DM_NAMES_LIST;
		CSettings::Save();
		return true;
	}
	if(wParam==MENU_CL_PHOTOS){
		MENUITEMINFO mi;
		mi.cbSize=sizeof(MENUITEMINFO);
		mi.fMask=MIIM_STATE;
		mi.fState=MFS_CHECKED;
		mainMenu.SetMenuItemInfoW(MENU_CL_PHOTOS, &mi);
		mi.fState=MFS_UNCHECKED;
		mainMenu.SetMenuItemInfoW(MENU_CL_NAMES, &mi);
		mainMenu.SetMenuItemInfoW(MENU_CL_EXTENDED, &mi);
		//friendList.SetMode(DM_NAMES_WITH_PHOTOS);
		friendList.viewMode=DM_NAMES_WITH_PHOTOS;
		friendList.UpdateScroll();
		CSettings::listMode=DM_NAMES_WITH_PHOTOS;
		CSettings::Save();
		return true;
	}
	if(wParam==MENU_M_ABOUT){
		CAboutDlg adlg;
		adlg.DoModal();
		return true;
	}
	if(wParam==MENU_M_LOGOUT){
		con.DeleteSessionData();
		//CBrowserDlg bdlg(this, true);
		//bdlg.DoModal();
		con.DeleteCookies();
		//MessageBox(CLang::Get(L"logout_notice"), L"Vk.IM", MB_OK|MB_ICONINFORMATION);
		logout=true;
		CDialog::OnCancel();
		return true;
	}
	if(wParam==MENU_M_OPEN_TALK){
		COpenTalkDlg dlg;
		int r=dlg.DoModal();
		if(r==IDOK){
			std::wstring input=dlg.urlOrID;
			if(input.find(L"http://")!=-1){
				input=input.substr(7);
			}
			if(input.find(L"vkontakte.ru/")!=-1){
				input=input.substr(13);
			}
			if(input.find(L"vk.com/")!=-1){
				input=input.substr(7);
			}
			if(input[0]>=L'0' && input[0]<=L'9' && input[input.length()-1]>=L'0' && input[input.length()-1]<=L'9'){
				input=L"id"+input;
			}
			TRACE("Input=%ws\n", input.c_str());
			ListEntry e;
			if(!con.GetProfileByDomain((wchar_t*)input.c_str(), &e)){
				MessageBox(CLang::Get(L"user_not_exist"), L"Vk.IM", MB_OK|MB_ICONEXCLAMATION);
				return true;
			}
			if(!talks->isCreated){
			talks->Create(IDD_TALKDLG, GetDesktopWindow());
			}
			talks->ShowWindow(SW_SHOW);
			talks->ShowWindow(SW_RESTORE);
			talks->OpenNewTab(&e);
			talks->SetForegroundWindow();
			talks->PostMessageW(WM_ACTIVATE, WA_ACTIVE);
		}
		return true;
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CVkIMDlg::OnDestroy()
{
	NOTIFYICONDATA iconData;
		memset(&iconData, 0, sizeof(NOTIFYICONDATA));
		iconData.cbSize=sizeof(NOTIFYICONDATA);
		iconData.uFlags=0;
		iconData.uID=1234;
		iconData.hWnd=GetSafeHwnd();
		Shell_NotifyIcon(NIM_DELETE, &iconData);

	CDialog::OnDestroy();
	// TODO: Add your message handler code here
}

HBRUSH CVkIMDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// вызывается при запросе сведений о цвете фона статика...
    /*if (nCtlColor == CTLCOLOR_STATIC && pWnd == &userNameLabel)
    {
    // установить отмену отрисовки фона перед изменениями
    pDC->SetBkMode(TRANSPARENT);
    // вернуть пустую кисть для любого закрашивания
    return (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    else*/
        return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}


void CVkIMDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
}


void CVkIMDlg::OnStnClickedAva()
{
	// TODO: Add your control notification handler code here
	wchar_t* buf=new wchar_t[256];
		wsprintf(buf, L"http://vkontakte.ru/id%i", uid);
		ShellExecute(GetSafeHwnd(), L"open", buf, NULL, NULL, SW_SHOWNORMAL);
		delete buf;
}


LRESULT CVkIMDlg::OnShowNotification(WPARAM wParam, LPARAM lParam){
	NotificationStruct* ns=(NotificationStruct*)wParam;
	int i=-1;
	for(i=1;i<20;i++){
		if(!notifications[i])break;
	}
	if(i==-1)return 1;
	CNotificationDlg* nw=NULL;
	nw=new CNotificationDlg(CWnd::GetDesktopWindow(), ns->title, ns->content, i, ns->uid);
	notifications[i]=nw;
	return 1;
}

LRESULT CVkIMDlg::OnHideNotification(WPARAM wParam, LPARAM lParam){
	delete notifications[wParam];
	notifications[wParam]=NULL;
	return 1;
}

afx_msg LRESULT CVkIMDlg::OnHideLoading(WPARAM wParam, LPARAM lParam)
{
	friendList.ShowWindow(SW_SHOW);
	KillTimer(100);
	friendList.UpdateScroll();
	return 0;
}


void CVkIMDlg::OnStnClickedUserStatusS()
{
	// TODO: Add your control notification handler code here
	/*userStatusLabel.ShowWindow(SW_HIDE);
	if(userHasStatus){
		wchar_t* s=new wchar_t[userStatusLabel.GetWindowTextLengthW()+1];
		memset(s, 0, userStatusLabel.GetWindowTextLengthW()+1);
		userStatusLabel.GetWindowTextW(s, userStatusLabel.GetWindowTextLengthW()+1);
		userStatusField.SetWindowTextW(s);
		delete s;
	}else{
		userStatusField.SetWindowTextW(L"");
	}
	userStatusField.ShowWindow(SW_SHOW);
	userStatusField.SetFocus();*/
	if(sProgressFrame!=-1)return;
	CRect r;
	userStatusLabel.GetWindowRect(r);
	int sdy=r.top;
	GetWindowRect(r);
	if(sdlg){
		delete sdlg;
	}
	sdlg=new CStatusDlg(this);
	sdlg->Create(CStatusDlg::IDD, this);
	CRect r2;
	sdlg->GetWindowRect(r2);
	int sdx=r.left-(r2.Width()-r.Width())/2;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &snap_wa, 0 );
	if(sdx+r2.Width()>=snap_wa.right)sdx=snap_wa.right-r2.Width();
	if(sdx<0)sdx=0;
	sdlg->SetWindowPos(NULL, sdx, sdy, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	sdlg->ShowWindow(SW_SHOW);
}


afx_msg LRESULT CVkIMDlg::OnStatusEnter(WPARAM wParam, LPARAM lParam)
{
	userStatusField.ShowWindow(SW_HIDE);
	wchar_t* s=new wchar_t[userStatusField.GetWindowTextLengthW()+1];
	memset(s, 0, userStatusField.GetWindowTextLengthW()+1);
	userStatusField.GetWindowTextW(s, userStatusField.GetWindowTextLengthW()+1);
	userStatusLabel.SetWindowTextW(s);
//	con.SetStatus(s);
	userStatusLabel.ShowWindow(SW_SHOW);
	return 0;
}


void CVkIMDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CVkIMDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CVkIMDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}


void CVkIMDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnCancel();
}


void CVkIMDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnChar(nChar, nRepCnt, nFlags);
}


void CVkIMDlg::OnEnKillfocusUserStatus()
{
	// TODO: Add your control notification handler code here
	userStatusField.ShowWindow(SW_HIDE);
	userStatusLabel.ShowWindow(SW_SHOW);
}


LRESULT CVkIMDlg::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetClientRect(rect);
	/*if(point.x>rect.right-10 && point.y>rect.bottom-10){
		return HTBOTTOMRIGHT;
	}*/
	CRect wrect;
	GetWindowRect(wrect);
	int x=point.x-wrect.left;
	int y=point.y-wrect.top;
	if(y<22 && y>0){
		if((x>=6 && x<=66) || (x>=wrect.Width()-115 && x<=wrect.Width()-6)){
			return HTBORDER;
		}
	}
	return CDialog::OnNcHitTest(point);
}


void CVkIMDlg::OnEnChangeClSearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	if(!friendList.searching)friendList.StartSearch();
	if(searchValue.GetLength()==0){
		friendList.StopSearch();
	}else{
		friendList.UpdateSearch(searchValue.GetBuffer());
	}
}

void CVkIMDlg::OnNcPaint()
{
	// TODO: Add your message handler code here
	// Do not call CDialog::OnNcPaint() for painting messages
	CDialog::OnNcPaint();
	bool useMemDC=false;
	CDC* _dc=GetWindowDC();
	CDC* dc=_dc;
	CRect rect;
	CBitmap bmp;
	CBitmap* obmp;
	GetWindowRect(rect);
	int bw=GetSystemMetrics(SM_CXFRAME);
	int bh=GetSystemMetrics(SM_CYFRAME);
	int ch=GetSystemMetrics(SM_CYCAPTION);
	int w=rect.Width();
	int h=rect.Height();
	BOOL b=false;
	HMODULE hm=LoadLibrary(L"dwmapi.dll");
	if(hm){
		DWMISCOMPOSITIONENABLED* ic=(DWMISCOMPOSITIONENABLED*)GetProcAddress(hm, "DwmIsCompositionEnabled");
		(ic)(&b);
		FreeLibrary(hm);
	}
	if(b){
		dc->FillSolidRect(0, 0, w, ch+bh, 0);
		dc->FillSolidRect(0, 0, bw, h, 0);
		dc->FillSolidRect(0, h-bh, w, bh, 0);
		dc->FillSolidRect(w-bw, 0, bw, h, 0);
	}else{
		useMemDC=true;
	bmp.CreateCompatibleBitmap(_dc, w, ch+bh);
	obmp=dc->SelectObject(&bmp);
		if(hasUxtheme && _IsThemeActive()){
			HTHEME hTheme=_OpenThemeData(NULL, L"WINDOW");
			_DrawThemeBackground(hTheme, dc->GetSafeHdc(), WP_CAPTION, isWindowActive?CS_ACTIVE:CS_INACTIVE, CRect(0, 0, w, ch+bh), NULL);
			_CloseThemeData(hTheme);
		}else{
			GetDesktopWindow()->DrawCaption(dc, CRect(2, 2, w-4, ch+bh-1), isWindowActive?DC_ACTIVE|DC_GRADIENT|DC_TEXT:DC_GRADIENT|DC_TEXT);
		}
	}
	if(!isWindowActive && closeBtnState==0){
		skin.wbuttons.DrawUnscaled(dc, w-49-6, 0, 49, 21, 60, 21*3);
	}else{
		skin.wbuttons.DrawUnscaled(dc, w-49-6, 0, 49, 21, 60, 21*closeBtnState);
	}
	if(!isWindowActive && soundBtnState==0){
		skin.wbuttons.DrawUnscaled(dc, w-49-6-29, 0, 29, 21, CSettings::globalSoundOn?138:167, 21*3);
	}else{
		skin.wbuttons.DrawUnscaled(dc, w-49-6-29, 0, 29, 21, CSettings::globalSoundOn?138:167, 21*soundBtnState);
	}
	if(!isWindowActive && settingsBtnState==0){
		skin.wbuttons.DrawUnscaled(dc, w-49-6-29-29, 0, 29, 21, 109, 21*3);
	}else{
		skin.wbuttons.DrawUnscaled(dc, w-49-6-29-29, 0, 29, 21, 109, 21*settingsBtnState);
	}

	skin.wbuttons.DrawUnscaled(dc, 6, 0, 60, 21, 0, 21*menuBtnState);
	if(useMemDC){
		_dc->BitBlt(0, 0, w, ch+bh, dc, 0, 0, SRCCOPY);
		dc->SelectObject(obmp);
		dc->DeleteDC();
		ReleaseDC(_dc);
	}else{
		ReleaseDC(dc);
	}
}


void CVkIMDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect wrect;
	GetWindowRect(wrect);
	int x=point.x-wrect.left;
	int y=point.y-wrect.top;
	if(!mouseInNC){
		mouseInNC=true;
		TRACKMOUSEEVENT tme;
		memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
		tme.cbSize=sizeof(TRACKMOUSEEVENT);
		tme.dwFlags=TME_NONCLIENT;
		tme.hwndTrack=GetSafeHwnd();
		::TrackMouseEvent(&tme);
	}
	if(y<22 && y>0){
		if(x>=6 && x<=66 && menuBtnState==0){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			menuBtnState=1;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
			/*CPoint pnt;
			GetCursorPos(&pnt);
			pnt.y+=20;
			tooltip.ShowTooltip(pnt, CLang::Get(L"main_menu"));*/
		}
		if(x>=wrect.Width()-115 && x<=wrect.Width()-86 && settingsBtnState==0){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			settingsBtnState=1;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
			/*CPoint pnt;
			GetCursorPos(&pnt);
			pnt.y+=20;
			tooltip.ShowTooltip(pnt, CLang::Get(L"settings"));*/
		}
		if(x>=wrect.Width()-85 && x<=wrect.Width()-57 && soundBtnState==0){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			soundBtnState=1;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
			/*CPoint pnt;
			GetCursorPos(&pnt);
			pnt.y+=20;
			tooltip.ShowTooltip(pnt, CSettings::globalSoundOn?CLang::Get(L"sound_off"):CLang::Get(L"sound_on"));*/
		}
		if(x>=wrect.Width()-56 && x<=wrect.Width()-6 && closeBtnState==0){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			closeBtnState=1;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
			/*CPoint pnt;
			GetCursorPos(&pnt);
			pnt.y+=20;
			tooltip.ShowTooltip(pnt, CLang::Get(L"close"));*/
		}
	}
	if((nHitTest==HTCAPTION || x<6 || x>wrect.Width()-6) && (menuBtnState==1 || settingsBtnState==1 || soundBtnState==1 || closeBtnState==1)){
		menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
		RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
		tooltip.HideTooltip();
	}
	CDialog::OnNcMouseMove(nHitTest, point);
}


void CVkIMDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
	RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
	tooltip.HideTooltip();
	// TODO: Add your message handler code here
}


BOOL CVkIMDlg::OnNcActivate(BOOL bActive)
{
	// TODO: Add your message handler code here and/or call default
	isWindowActive=bActive;
	RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
	return CDialog::OnNcActivate(bActive);
}


void CVkIMDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect wrect;
	GetWindowRect(wrect);
	int x=point.x-wrect.left;
	int y=point.y-wrect.top;
	if(y<22 && y>0){
		if(x>=6 && x<=66){
			/*menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			menuBtnState=1;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);*/
		}
		if(x>=wrect.Width()-115 && x<=wrect.Width()-86){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			settingsBtnState=2;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
		}
		if(x>=wrect.Width()-85 && x<=wrect.Width()-57){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			soundBtnState=2;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
		}
		if(x>=wrect.Width()-56 && x<=wrect.Width()-6){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			closeBtnState=2;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
		}
	}
	tooltip.HideTooltip();
	CDialog::OnNcLButtonDown(nHitTest, point);
}


void CVkIMDlg::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect wrect;
	GetWindowRect(wrect);
	int x=point.x-wrect.left;
	int y=point.y-wrect.top;
	if(y<22 && y>0){
		if(x>=6 && x<=66){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
			//MessageBox(L"Вместо этого окна тут когда-нибудь будет главное меню");
			/*if(friendList.displayMode==DM_NAMES_WITH_PHOTOS)friendList.SetMode(DM_NAMES_LIST);
			else friendList.SetMode(DM_NAMES_WITH_PHOTOS);*/
			RECT rect;
			GetWindowRect(&rect);
			mainMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left+6, rect.top+22, this);
		}
		if(x>=wrect.Width()-115 && x<=wrect.Width()-86){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
			if(settings){delete settings;}
			settings=new CSettingsDlg();
			settings->Create(CSettingsDlg::IDD, CWnd::GetDesktopWindow());
			settings->ShowWindow(SW_SHOW);
		}
		if(x>=wrect.Width()-85 && x<=wrect.Width()-57){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			soundBtnState=1;
			CSettings::globalSoundOn=!CSettings::globalSoundOn;
			CSettings::Save();
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
		}
		if(x>=wrect.Width()-56 && x<=wrect.Width()-6){
			menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
			RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
			SendMessage(WM_CLOSE);
		}
	}
	CDialog::OnNcLButtonUp(nHitTest, point);
}


void CVkIMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100){
		CRect rect;
		GetClientRect(rect);
		InvalidateRect(CRect(0, 79, rect.Width(), rect.Height()-103));
		progressFrame++;
		progressFrame%=skin.progress.n;
	}
	if(nIDEvent==101){
		KillTimer(101);
		con.GetCounters();
	}
	if(nIDEvent==102){
		CRect rect;
		GetClientRect(rect);
		userStatusLabel.SetWindowTextW(newStatusText.c_str());
		InvalidateRect(CRect(60, 30, rect.Width(), 46));
		sProgressFrame++;
		sProgressFrame%=skin.progressSmall.n;
	}
	CDialog::OnTimer(nIDEvent);
}


void CVkIMDlg::OnNcMouseLeave()
{
	// This feature requires Windows 2000 or greater.
	// The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
	// TODO: Add your message handler code here and/or call default

	if(menuBtnState!=0 || closeBtnState!=0 || soundBtnState!=0 || settingsBtnState!=0){
		menuBtnState=settingsBtnState=soundBtnState=closeBtnState=0;
		RedrawWindow(CRect(0, 0, 1, 1), NULL, RDW_FRAME|RDW_INVALIDATE);
	}
	mouseInNC=false;
	tooltip.HideTooltip();

	CDialog::OnNcMouseLeave();
}


afx_msg LRESULT CVkIMDlg::OnUpdateCounters(WPARAM wParam, LPARAM lParam)
{
	if(CSettings::enableEvents)SetTimer(101, CSettings::eventsRefresh, NULL);
	Counters* cnt=(Counters*)wParam;

	TRACE("Counters=%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\n",cnt->friends,cnt->photos,cnt->videos,cnt->notes,cnt->gifts,cnt->events,cnt->groups,cnt->opinions,cnt->offers,cnt->questions);

	if(cnt->friends>counters.friends && CSettings::notifyForEvents[0]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_friends", cnt->friends);
		ns.title=CLang::Get(L"menu_vk_friends");
		ns.uid=-1;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->photos>counters.photos && CSettings::notifyForEvents[1]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_photos", cnt->photos);
		ns.title=CLang::Get(L"menu_vk_photos");
		ns.uid=-2;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->videos>counters.videos && CSettings::notifyForEvents[2]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_videos", cnt->videos);
		ns.title=CLang::Get(L"menu_vk_videos");
		ns.uid=-3;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->notes>counters.notes && CSettings::notifyForEvents[3]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_notes", cnt->notes);
		ns.title=CLang::Get(L"menu_vk_notes");
		ns.uid=-4;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->gifts>counters.gifts && CSettings::notifyForEvents[4]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_gifts", cnt->gifts);
		ns.title=CLang::Get(L"menu_vk_gifts");
		ns.uid=-5;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->events>counters.events && CSettings::notifyForEvents[5]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_events", cnt->events);
		ns.title=CLang::Get(L"menu_vk_events");
		ns.uid=-6;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->groups>counters.groups && CSettings::notifyForEvents[6]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_groups", cnt->groups);
		ns.title=CLang::Get(L"menu_vk_groups");
		ns.uid=-7;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->opinions>counters.opinions && CSettings::notifyForEvents[7]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_opinions", cnt->opinions);
		ns.title=CLang::Get(L"menu_vk_opinions");
		ns.uid=-8;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->offers>counters.offers && CSettings::notifyForEvents[8]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_offers", cnt->offers);
		ns.title=CLang::Get(L"menu_vk_offers");
		ns.uid=-9;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}
	if(cnt->questions>counters.questions && CSettings::notifyForEvents[9]){
		if(CSettings::popupEvent){
		NotificationStruct ns;
		ns.content=CLang::GetNum(L"notify_questions", cnt->questions);
		ns.title=CLang::Get(L"menu_vk_questions");
		ns.uid=-10;
		SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		delete ns.content;}
		if(CSettings::soundEvent && CSettings::globalSoundOn)PlaySound(CSettings::soundEventFile, NULL, SND_ASYNC | SND_FILENAME);
	}

	CRect rect;
	GetClientRect(rect);
	InvalidateRect(CRect(0, rect.Height()-24, rect.Width(), rect.Height()));

	memcpy(&counters, cnt, sizeof(Counters));
	return 0;
}


void CVkIMDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if(startHidden){
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	if(lpwndpos->cx<=200)lpwndpos->cx=200;
	if(lpwndpos->cy<=200)lpwndpos->cy=200;
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}


void CVkIMDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(!mouseInside){
		//TRACE("hover\n");
		mouseInside=true;
		TRACKMOUSEEVENT tme;
		memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
		tme.cbSize=sizeof(TRACKMOUSEEVENT);
		tme.dwFlags=TME_LEAVE;
		tme.hwndTrack=GetSafeHwnd();
		::TrackMouseEvent(&tme);
	}
	if(CSettings::enableEvents){
	CRect rect;
	GetClientRect(rect);
	rect.OffsetRect(-rect.left, -rect.top);
	onCounters=false;
	if(point.y>rect.Height()-24){
		int off=0;
		for(int i=0;i<countersWidth.size();i++){
			if(point.x>off && point.x<off+countersWidth[i]){
				TRACE("cnt:%i\n", i);
				onCounters=true;
				if(onCounter!=i){
					wchar_t* tooltext=NULL;
					switch(i){
				case 0:
					tooltext=CLang::Get(L"menu_vk_friends");
					break;
				case 1:
					tooltext=CLang::Get(L"menu_vk_photos");
					break;
				case 2:
					tooltext=CLang::Get(L"menu_vk_videos");
					break;
				case 3:
					tooltext=CLang::Get(L"menu_vk_notes");
					break;
				case 4:
					tooltext=CLang::Get(L"menu_vk_gifts");
					break;
				case 5:
					tooltext=CLang::Get(L"menu_vk_events");
					break;
				case 6:
					tooltext=CLang::Get(L"menu_vk_groups");
					break;
				case 7:
					tooltext=CLang::Get(L"menu_vk_opinions");
					break;
				case 8:
					tooltext=CLang::Get(L"menu_vk_offers");
					break;
				case 9:
					tooltext=CLang::Get(L"menu_vk_questions");
					break;
				}
					onCounter=i;
				if(tooltext){
					CPoint pnt;
					GetCursorPos(&pnt);
					pnt.y+=20;
					if(pnt.y+20>=GetSystemMetrics(SM_CYSCREEN))pnt.y-=45;
					tooltip.ShowTooltip(pnt, tooltext);
				}
				}
				SetCursor(LoadCursor(NULL, IDC_HAND));
				break;
			}
			off+=countersWidth[i];
		}
	}
	if(!onCounters){
		tooltip.HideTooltip();
		onCounter=-1;
	}
	}
	CDialog::OnMouseMove(nFlags, point);
}


BOOL CVkIMDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if(!onCounters){
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	}else{
		return true;
	}
}


void CVkIMDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(CSettings::enableEvents){
	CRect rect;
	GetClientRect(rect);
	rect.OffsetRect(-rect.left, -rect.top);
	onCounters=false;
	if(point.y>rect.Height()-24){
		int off=0;
		for(int i=0;i<countersWidth.size();i++){
			if(point.x>off && point.x<off+countersWidth[i]){
				TRACE("cnt CLICK:%i\n", i);
				wchar_t* url=NULL;
				tooltip.HideTooltip();
				switch(i){
				case 0:
					url=L"http://vkontakte.ru/friends.php?filter=requests";
					break;
				case 1:
					url=L"http://vkontakte.ru/photos.php?act=added";
					break;
				case 2:
					url=L"http://vkontakte.ru/video.php?act=tagview";
					break;
				case 3:
					url=L"http://vkontakte.ru/notes.php?act=comms";
					break;
				case 4:
					url=L"http://vkontakte.ru/gifts.php";
					break;
				case 5:
					url=L"http://vkontakte.ru/events.php";
					break;
				case 6:
					url=L"http://vkontakte.ru/groups.php?filter=invitations";
					break;
				case 7:
					url=L"http://vkontakte.ru/opinions.php";
					break;
				case 8:
					url=L"http://vkontakte.ru/matches.php";
					break;
				case 9:
					url=L"http://vkontakte.ru/questions.php";
					break;
				}
				if(url)ShellExecute(GetSafeHwnd(), L"open", url, NULL, NULL, SW_SHOWNORMAL);
				break;
			}
			off+=countersWidth[i];
		}
	}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CVkIMDlg::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	//TRACE(L"leave\n");
	tooltip.HideTooltip();
	onCounter=-1;
	onCounters=false;
	CDialog::OnMouseLeave();
	mouseInside=false;
}


void CVkIMDlg::CountersEnableChanged(void)
{
	/*if(CSettings::enableEvents){

	}*/
	CRect rect;
	GetClientRect(rect);
	OnSize(0, rect.Width(), rect.Height());
}

BOOL isSnapClose( int a, int b ) { return (abs( a - b ) < SNAP_DIST);}

void CVkIMDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	snap_prc = pRect;

	snap_cur_pos.x=0;
	snap_cur_pos.y=0;
	snap_rcWindow.bottom=0;
	snap_rcWindow.left=0;
	snap_rcWindow.right=0;
	snap_rcWindow.top=0;

	GetCursorPos( &snap_cur_pos );
	OffsetRect( snap_prc,
		snap_cur_pos.x - (snap_prc->left + snap_x) ,
		snap_cur_pos.y - (snap_prc->top + snap_y) );

	SystemParametersInfo( SPI_GETWORKAREA, 0, &snap_wa, 0 );

	if (isSnapClose( snap_prc->left, snap_wa.left ))
	{OffsetRect( snap_prc, snap_wa.left - snap_prc->left, 0);}
	else 
		if (isSnapClose( snap_wa.right, snap_prc->right )) 
		{OffsetRect( snap_prc, snap_wa.right - snap_prc->right, 0);}

		if (isSnapClose( snap_prc->top, snap_wa.top )) 
		{OffsetRect( snap_prc, 0, snap_wa.top - snap_prc->top );}
		else 
			if (isSnapClose( snap_wa.bottom, snap_prc->bottom )) 
			{OffsetRect( snap_prc, 0, snap_wa.bottom - snap_prc->bottom );}


	if(sdlg){
	CRect r;
	userStatusLabel.GetWindowRect(r);
	int sdy=r.top;
	GetWindowRect(r);
	CRect r2;
	sdlg->GetWindowRect(r2);
	int sdx=r.left-(r2.Width()-r.Width())/2;
	if(sdx+r2.Width()>=snap_wa.right)sdx=snap_wa.right-r2.Width();
	if(sdx<0)sdx=0;
	sdlg->SetWindowPos(NULL, sdx, sdy, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}

	CDialog::OnMoving(fwSide, pRect);
}


void CVkIMDlg::OnEnterSizeMove()
{
	// TODO: Add your message handler code here and/or call default
	snap_cur_pos.x=0;
	snap_cur_pos.y=0;

	snap_rcWindow.bottom=0;
	snap_rcWindow.left=0;
	snap_rcWindow.right=0;
	snap_rcWindow.top=0;
	GetWindowRect(&snap_rcWindow );

	GetCursorPos( &snap_cur_pos );

	snap_x = snap_cur_pos.x - snap_rcWindow.left;
	snap_y = snap_cur_pos.y - snap_rcWindow.top;
	CDialog::OnEnterSizeMove();
}


void CVkIMDlg::OnExitSizeMove()
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetWindowRect(rect);
	CSettings::listX=rect.left;
	CSettings::listY=rect.top;
	CSettings::listW=rect.Width();
	CSettings::listH=rect.Height();
	CSettings::Save();
	CDialog::OnExitSizeMove();
}


afx_msg LRESULT CVkIMDlg::OnTaskbarcreated(WPARAM wParam, LPARAM lParam)
{
	NOTIFYICONDATA iconData;
	memset(&iconData, 0, sizeof(NOTIFYICONDATA));
	iconData.cbSize=sizeof(NOTIFYICONDATA);
	iconData.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP;
	iconData.hWnd=GetSafeHwnd();
	iconData.hIcon=::LoadIconW(::AfxGetInstanceHandle(), MAKEINTRESOURCE(con.iconOnline?IDI_TRAY_C:IDI_TRAY_D));
	iconData.uID=1234;
	wchar_t buf[512];
	if(con.my_name)wsprintf(buf, L"Vk.IM - %s", con.my_name);
	else wsprintf(buf, L"Vk.IM");
	memcpy(&iconData.szTip, buf, 128);
	iconData.uCallbackMessage=WM_TRAY_ICON;
	Shell_NotifyIcon(NIM_ADD, &iconData);

	return 0;
}


void CVkIMDlg::StartStatusProgress(void)
{
	sProgressFrame=0;
	CRect rect;
	userStatusLabel.GetWindowRect(rect);
	rect.OffsetRect(20, 0);
	ScreenToClient(rect);
	userStatusLabel.SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	SetTimer(102, 50, NULL);
}


void CVkIMDlg::StopStatusProgress(void)
{
	sProgressFrame=-1;
	CRect rect;
	userStatusLabel.GetWindowRect(rect);
	rect.OffsetRect(-20, 0);
	ScreenToClient(rect);
	userStatusLabel.SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	KillTimer(102);
}
