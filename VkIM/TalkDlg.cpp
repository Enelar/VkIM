// TalkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "TalkDlg.h"
#include "Lang.h"
#include "Connector.h"
#include "Settings.h"

extern CConnector con;

#define WM_DWMCOMPOSITIONCHANGED 0x031E
// CTalkDlg dialog

IMPLEMENT_DYNAMIC(CTalkDlg, CDialog)

CTalkDlg::CTalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTalkDlg::IDD, pParent)
{
	isCreated=false;
	noFocus=false;
	//Create(IDD, GetDesktopWindow());
}

CTalkDlg::~CTalkDlg()
{
}

void CTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, tabs);
}


BEGIN_MESSAGE_MAP(CTalkDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB1, &CTalkDlg::OnTcnSelchangingTab1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTalkDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
	ON_WM_NCMOUSEHOVER()
	ON_MESSAGE(WM_OPENTAB, OnOpenTab)
	ON_WM_ACTIVATE()
	//ON_WM_DWMCOMPOSITIONCHANGED()
	ON_MESSAGE(WM_DWMCOMPOSITIONCHANGED, OnCompositionChanged)
	ON_WM_NCACTIVATE()
	ON_WM_EXITSIZEMOVE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

BOOL CTalkDlg::OnInitDialog(){
	CDialog::OnInitDialog();
	isCreated=true;
	hwndTabs=tabs.GetSafeHwnd();
	tabs.SetItemSize(CSize(150, 20));
	SetWindowText(CLang::Get(L"conversation_window"));
	SetIcon(con.clDlg->GetIcon(false), false);
	SetIcon(con.clDlg->GetIcon(true), true);
	tb=NULL;
	HMODULE hm=LoadLibrary(L"dwmapi.dll");
	if(hm){
		_DwmSetWindowAttribute=(DWMSETWINDOWATTRIBUTE*)GetProcAddress(hm, "DwmSetWindowAttribute");
		_DwmSetIconicThumbnail=(DWMSETICONICTHUMBNAIL*)GetProcAddress(hm, "DwmSetIconicThumbnail");
		_DwmInvalidateIconicBitmaps=(DWMINVIB*)GetProcAddress(hm, "DwmInvalidateIconicBitmaps");
		_DwmSetIconicLivePreviewBitmap=(DWMSETLPBITMAP*)GetProcAddress(hm, "DwmSetIconicLivePreviewBitmap");
		_DwmExtendFrameIntoClientArea=(DWMEXTENDFRAMEINTOCLIENTAREA*)GetProcAddress(hm, "DwmExtendFrameIntoClientArea");
		_DwmIsCompositionEnabled=(DWMISCOMPOSITIONENABLED*)GetProcAddress(hm, "DwmIsCompositionEnabled");
		MARGINS m={0, 0, TAB_TOP_MARGIN+TAB_HEIGHT, 0};
		_DwmExtendFrameIntoClientArea(m_hWnd, &m);
		FreeLibrary(hm);
	}
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_TaskbarList,0,CLSCTX_INPROC_SERVER,__uuidof(ITaskbarList3),(void**)&tb);
	//tb=NULL;

	if(CSettings::talkW>0 && CSettings::talkH>0){
		int scrw=GetSystemMetrics(SM_CXSCREEN);
		int scrh=GetSystemMetrics(SM_CYSCREEN);
		if(CSettings::talkX+100>scrw)CSettings::talkX=scrw-CSettings::talkW;
		if(CSettings::talkY+100>scrh)CSettings::talkY=scrh-CSettings::talkH;
		SetWindowPos(NULL, CSettings::talkX, CSettings::talkY, CSettings::talkW, CSettings::talkH, SWP_NOZORDER|SWP_NOACTIVATE);
	}

	return false;
}

// CTalkDlg message handlers


void CTalkDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	TRACE("Showwindow %i; %i\n", bShow, nStatus);
	CDialog::OnShowWindow(bShow, nStatus);
}

void CTalkDlg::OpenNewTab(ListEntry* info, bool newMsg, bool setSel){
	int uid=info->uid;
	wstring username=info->userName;
	if(openedTabs[uid]){
		return;
	}
	CTalkTab* twnd;
	TC_ITEM tci;
	//tci.mask = TCIF_TEXT;
	tci.iImage = -1;
	wchar_t* un=new wchar_t[username.length()+1];
	wcscpy(un, username.c_str());
	tci.pszText = un;
	int itemIdx=tabs.GetItemCount();
	if(itemIdx==0)setSel=true;
	//tabs.InsertItem(itemIdx, &tci);
	twnd=new CTalkTab(uid, &tabs, newMsg, !setSel, info->userBigPhotoURL);
	RECT rect;
	tabs.GetItemRect(itemIdx, &rect);
	RECT wrect;
	this->GetClientRect(&wrect);
	twnd->Create(IDD_TALKTAB, CWnd::FromHandle(hwndTabs));
	twnd->SetWindowPos(NULL, 0, rect.bottom, wrect.right-wrect.left, (wrect.bottom-wrect.top)-rect.bottom, SWP_NOZORDER);
	if(twnd->tabProxy)twnd->tabProxy->SetWindowTextW(username.c_str());
	if(setSel || tabs.GetItemCount()<2)twnd->ShowWindow(SW_SHOW);
	twnd->user_name=username;
	tci.lParam=(LPARAM)twnd;
	openedTabs[uid]=twnd;
	tci.mask=TCIF_PARAM | TCIF_IMAGE | TCIF_TEXT;
	tci.iImage=uid;
	tabs.InsertItem(itemIdx, &tci);
	//tabs.SetItem(itemIdx, &tci);
	if(tb){
		tb->RegisterTab(twnd->tabProxy->m_hWnd, m_hWnd);
		tb->SetTabOrder(twnd->tabProxy->m_hWnd, NULL);
		tb->SetThumbnailClip(twnd->tabProxy->m_hWnd, CRect(0, 0, 100, 100));
	}

	if(setSel || tabs.GetItemCount()<2){
	if(tabs.GetItemCount()>1){
	int iTab = tabs.GetCurSel();
	TC_ITEM tci1;
	tci1.mask = TCIF_PARAM;
	tabs.GetItem(iTab, &tci1);
	CWnd* pWnd = (CWnd *)tci1.lParam;
	if(pWnd)pWnd->ShowWindow(SW_HIDE);
	}
	if(tb)tb->SetTabActive(twnd->tabProxy->m_hWnd, m_hWnd, 0);
	tabs.SetCurSel(itemIdx);
	}
	delete un;
}

void CTalkDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(tabs.GetItemCount()>1){
	if(MessageBox(CLang::Get(L"confirm_close_tabs"), CLang::Get(L"confirm"), MB_YESNO|MB_ICONQUESTION)==IDNO)
		return;
	}
	ShowWindow(SW_HIDE);
	for(int i=0;i<tabs.GetItemCount();i++){
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	tabs.GetItem(i, &tci);
	CTalkTab* pWnd = (CTalkTab *)tci.lParam;
	if(tb && pWnd->tabProxy){
		tb->UnregisterTab(pWnd->tabProxy->m_hWnd);
	}
	delete pWnd;
	}
	openedTabs.clear();
	tabs.DeleteAllItems();
	if(tabs.m_pSpinCtrl)tabs.m_pSpinCtrl->SetPos(0);
	//CDialog::OnClose();
}


void CTalkDlg::OnTcnSelchangingTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	/*int iTab = tabs.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	tabs.GetItem(iTab, &tci);
	CWnd* pWnd = (CWnd *)tci.lParam;
	if(pWnd)pWnd->ShowWindow(SW_HIDE); */
	for(int i=0;i<tabs.GetItemCount();i++){
		TC_ITEM tci;
		tci.mask = TCIF_PARAM;
		tabs.GetItem(i, &tci);
		CWnd* pWnd = (CWnd *)tci.lParam;
		if(pWnd){
			pWnd->ShowWindow(SW_HIDE);
		}
	}
	*pResult = 0;
}


void CTalkDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int iTab = tabs.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	tabs.GetItem(iTab, &tci);
	CTalkTab* pWnd = (CTalkTab *)tci.lParam;
	if(pWnd){
		TRACE("Selchange %i\n", iTab);
	pWnd->hidden=false;
	pWnd->hasNewMsg=false;
	pWnd->MarkMsgsAsRead();
	pWnd->ShowWindow(SW_SHOW); 
	tabs.RedrawWindow();
	pWnd->ShowWindow(SW_HIDE);
	pWnd->ShowWindow(SW_SHOW);
	if(tb)tb->SetTabActive(pWnd->tabProxy->m_hWnd, m_hWnd, 0);
	}
	*pResult = 0;
}


void CTalkDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(tabs){
		tabs.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER|SWP_NOMOVE);
		for(int i=0;i<tabs.GetItemCount();i++){
			TC_ITEM tci;
			tci.mask = TCIF_PARAM;
			tabs.GetItem(i, &tci);
			CWnd* pWnd = (CWnd *)tci.lParam;
			WINDOWPLACEMENT wpl;
			pWnd->GetWindowPlacement(&wpl);
			pWnd->SetWindowPos(NULL, 0, 0, cx, cy-wpl.rcNormalPosition.top, SWP_NOZORDER|SWP_NOMOVE);
		}
	}

	// TODO: Add your message handler code here
}


void CTalkDlg::OnNcMouseHover(UINT nFlags, CPoint point)
{
	// This feature requires Windows 2000 or greater.
	// The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
	// TODO: Add your message handler code here and/or call default
	tabs.OnMouseLeave();
	CDialog::OnNcMouseHover(nFlags, point);
}


LRESULT CTalkDlg::OnOpenTab(WPARAM wParam, LPARAM lParam){
	OpenNewTab((ListEntry*)lParam, true, false);
	return 1;
}

void CTalkDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if(tabs){
		if(tabs.GetItemCount()>0){
			int iTab = tabs.GetCurSel();
			TC_ITEM tci;
			tci.mask = TCIF_PARAM;
			tabs.GetItem(iTab, &tci);
			CTalkTab* pWnd = (CTalkTab *)tci.lParam;
			pWnd->MarkMsgsAsRead();
		}
		tabs.RedrawWindow();
	}
	
	// TODO: Add your message handler code here
}


void CTalkDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}


void CTalkDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
}


LRESULT CTalkDlg::OnCompositionChanged(WPARAM wParam, LPARAM lParam)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	// TODO: Add your message handler code here and/or call default
	if(_DwmExtendFrameIntoClientArea){
		MARGINS m={0, 0, TAB_TOP_MARGIN+TAB_HEIGHT, 0};
		_DwmExtendFrameIntoClientArea(m_hWnd, &m);
	}
	for(int i=0;i<tabs.GetItemCount();i++){
			TC_ITEM tci;
			tci.mask = TCIF_PARAM;
			tabs.GetItem(i, &tci);
			CTalkTab* pWnd = (CTalkTab *)tci.lParam;
			if(pWnd->tabProxy)pWnd->tabProxy->SetAttributes();
		}
	CDialog::OnCompositionChanged();
	return 0;
}


BOOL CTalkDlg::OnNcActivate(BOOL bActive)
{
	// TODO: Add your message handler code here and/or call default
	bool result=CDialog::OnNcActivate(bActive);
	return result;
}


void CTalkDlg::OnExitSizeMove()
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetWindowRect(rect);
	CSettings::talkX=rect.left;
	CSettings::talkY=rect.top;
	CSettings::talkW=rect.Width();
	CSettings::talkH=rect.Height();
	CSettings::Save();
	CDialog::OnExitSizeMove();
}


void CTalkDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);
	// TODO: Add your message handler code here
}


void CTalkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if(nID=SC_RESTORE){
		//TRACE("Restore\n");
	}
	CDialog::OnSysCommand(nID, lParam);
}
