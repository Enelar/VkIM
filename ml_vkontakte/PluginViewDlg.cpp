// PluginViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ml_vkontakte.h"
#include "PluginViewDlg.h"

#include "../gen_ml/ml_ipc_0313.h"
#include "../gen_ml/childwnd.h"
#include "../Winamp/wa_dlg.h"
#include <api/service/waServiceFactory.h>
#include "../gen_ml/ml.h"

#include "VkAPIRequest.h"

#include <WinInet.h>
#pragma comment(lib, "wininet")


// CPluginViewDlg dialog

enum{
	MENU_OPEN = 100,
	MENU_ADD_TO_PLAYLIST,
	MENU_SEARCH_PERFORMER,
	MENU_ADD_ALL,
	MENU_ADD_TO_PAGE
};

extern winampMediaLibraryPlugin plugin;
extern unsigned int uid;

IMPLEMENT_DYNAMIC(CPluginViewDlg, CDialog)

CPluginViewDlg::CPluginViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginViewDlg::IDD, pParent)
	, searchValue(_T(""))
{
	currentUser=uid;
}

CPluginViewDlg::~CPluginViewDlg()
{
}

void CPluginViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, searchEdit);
	DDX_Control(pDX, IDC_BUTTON1, searchBtn);
	DDX_Control(pDX, IDC_LIST3, list);
	DDX_Control(pDX, IDC_LIST1, friendList);
	DDX_Text(pDX, IDC_EDIT1, searchValue);
	DDX_Control(pDX, IDC_CHECK1, removeRepeatCheck);
	DDX_Control(pDX, IDC_PROGR_LABEL, progrLabel);
}


BEGIN_MESSAGE_MAP(CPluginViewDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CPluginViewDlg::OnBnClickedButton1)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &CPluginViewDlg::OnNMDblclkList3)
	ON_NOTIFY(NM_RCLICK, IDC_LIST3, &CPluginViewDlg::OnNMRClickList3)
	ON_LBN_SELCHANGE(IDC_LIST1, &CPluginViewDlg::OnLbnSelchangeList1)
	ON_WM_TIMER()
	ON_EN_SETFOCUS(IDC_EDIT1, &CPluginViewDlg::OnEnSetfocusEdit1)
	ON_STN_CLICKED(IDC_RRPT_LABEL, &CPluginViewDlg::OnStnClickedRrptLabel)
END_MESSAGE_MAP()


// CPluginViewDlg message handlers


BOOL CPluginViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	MLSKINWINDOW sw;
	sw.skinType = SKINNEDWND_TYPE_DIALOG;
	sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
	sw.hwndToSkin = m_hWnd;
	MLSkinWindow(plugin.hwndLibraryParent, &sw);

	sw.hwndToSkin = list.m_hWnd;
	sw.skinType = SKINNEDWND_TYPE_LISTVIEW;
	sw.style = SWLVS_FULLROWSELECT | SWLVS_DOUBLEBUFFER | SWS_USESKINFONT | SWS_USESKINCOLORS | SWS_USESKINCURSORS;
	MLSkinWindow(plugin.hwndLibraryParent, &sw);

	sw.skinType = SKINNEDWND_TYPE_BUTTON;
	sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
	sw.hwndToSkin = searchBtn.m_hWnd;
	MLSkinWindow(plugin.hwndLibraryParent, &sw);

	sw.skinType = SKINNEDWND_TYPE_EDIT;
	sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT | SWES_VCENTER;
	sw.hwndToSkin = searchEdit.m_hWnd;
	MLSkinWindow(plugin.hwndLibraryParent, &sw);

	sw.hwndToSkin = friendList.m_hWnd;
	sw.skinType = SKINNEDWND_TYPE_LISTBOX;
	sw.style = SWLVS_FULLROWSELECT | SWLVS_DOUBLEBUFFER | SWS_USESKINFONT | SWS_USESKINCOLORS | SWS_USESKINCURSORS;
	MLSkinWindow(plugin.hwndLibraryParent, &sw);

	sw.skinType = SKINNEDWND_TYPE_STATIC;
	sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
	sw.hwndToSkin = GetDlgItem(IDC_RRPT_LABEL)->m_hWnd;
	MLSkinWindow(plugin.hwndLibraryParent, &sw);

	sw.skinType = SKINNEDWND_TYPE_STATIC;
	sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
	sw.hwndToSkin = progrLabel.m_hWnd;
	MLSkinWindow(plugin.hwndLibraryParent, &sw);

	list.InsertColumn(0, L"№", 0, 30);
	list.InsertColumn(1, L"Исполнитель", 0, 200);
	list.InsertColumn(2, L"Название", 0, 300);
	list.InsertColumn(3, L"Длит.", 0, 50);

	AfxBeginThread(CPluginViewDlg::LoadUserThread, this);
	AfxBeginThread(CPluginViewDlg::LoadFriendsThread, this);

	progrLabel.SetWindowTextW(L"░▒▓██▓▒░");
	progrLabel.SetWindowPos(0, 0, 0, 200, 20, SWP_NOZORDER|SWP_NOMOVE);

	contextMenu.CreatePopupMenu();

	MENUITEMINFO ii;
	ii.cbSize=sizeof(MENUITEMINFO);
	ii.fType=MFT_STRING;
	ii.fMask=MIIM_TYPE|MIIM_ID;
	ii.dwTypeData=L"Открыть";
	ii.wID=MENU_OPEN;
	contextMenu.InsertMenuItemW(MENU_OPEN, &ii, false);

	ii.dwTypeData=L"Добавить в плэйлист";
	ii.wID=MENU_ADD_TO_PLAYLIST;
	contextMenu.InsertMenuItemW(MENU_ADD_TO_PLAYLIST, &ii, false);

	ii.dwTypeData=L"Добавить все в плэйлист";
	ii.wID=MENU_ADD_ALL;
	contextMenu.InsertMenuItemW(MENU_ADD_ALL, &ii, false);

	ii.dwTypeData=L"Искать исполнителя";
	ii.wID=MENU_SEARCH_PERFORMER;
	contextMenu.InsertMenuItemW(MENU_SEARCH_PERFORMER, &ii, false);

	ii.dwTypeData=L"Добавить на мою страницу";
	ii.wID=MENU_ADD_TO_PAGE;
	contextMenu.InsertMenuItemW(MENU_ADD_TO_PAGE, &ii, false);

	contextMenu.SetDefaultItem(0, true);
	removeRepeatCheck.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPluginViewDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	friendList.SetCurSel(-1);
	AfxBeginThread(CPluginViewDlg::LoadSearchThread, this);
}


void CPluginViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect sbRect;
	CRect flRect;
	if(!searchBtn)return;
	removeRepeatCheck.SetWindowPos(0, 3, cy-20, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	GetDlgItem(IDC_RRPT_LABEL)->SetWindowPos(0, 20, cy-20, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	progrLabel.SetWindowPos(0, cx-200, cy-20, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	cy-=23;
	searchBtn.GetWindowRect(sbRect);
	friendList.GetWindowRect(flRect);
	searchEdit.SetWindowPos(NULL, 3, 3, cx-9-sbRect.Width(), sbRect.Height(), SWP_NOZORDER);
	searchBtn.SetWindowPos(NULL, cx-sbRect.Width()-3, 3, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	list.SetWindowPos(NULL, 3, 6+sbRect.Height(), cx-9-flRect.Width(), cy-9-sbRect.Height(), SWP_NOZORDER);
	friendList.SetWindowPos(NULL, cx-3-flRect.Width(), 6+sbRect.Height(), flRect.Width(), cy-9-sbRect.Height(), SWP_NOZORDER);
	// TODO: Add your message handler code here
}


void CPluginViewDlg::AddItem(int number, wchar_t* performer, wchar_t* title, int duration)
{
	wchar_t nbuf[15];
	wchar_t dbuf[30];
	wsprintf(nbuf, L"%i", number);
	wsprintf(dbuf, L"%i:%02i", (duration-(duration%60))/60, duration%60);
	int item=list.InsertItem(number-1, nbuf);
	list.SetItem(item, 1, LVIF_TEXT, performer, NULL, NULL, NULL, NULL, NULL);
	list.SetItem(item, 2, LVIF_TEXT, title, NULL, NULL, NULL, NULL, NULL);
	list.SetItem(item, 3, LVIF_TEXT, dbuf, NULL, NULL, NULL, NULL, NULL);
}


JSONNode* CPluginViewDlg::SendRequest(CVkAPIRequest* req)
{
	HINTERNET hInet=InternetOpen(L"Vk.IM", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hSession=InternetConnect(hInet, L"api.vkontakte.ru", 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	HINTERNET hRequest=HttpOpenRequest(hSession, L"POST", L"/api.php", NULL, NULL, NULL, 0, 0);
	wchar_t* hdr=L"Content-Type: application/x-www-form-urlencoded";
	char* pdata=req->GetRequestString();
	bool result=HttpSendRequest(hRequest, hdr, wcslen(hdr), pdata, strlen(pdata));
	char* rbuf=new char[1];
	int rsize=0;
	while(true){
		char* buf=new char[1024];
		DWORD numRead=0;
		bool read=InternetReadFile(hRequest, buf, 1024, &numRead);
		if(!read || numRead==0){
			delete buf;
			break;
		}else{
			rbuf=(char*)realloc(rbuf, rsize+numRead);
			memcpy(rbuf+rsize, buf, numRead);
			rsize+=numRead;
			delete buf;
		}
	}
	rbuf=(char*)realloc(rbuf, rsize+1);
	rbuf[rsize]=0;
	wchar_t* wresp=CUtils::Utf8ToWchar(rbuf);
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInet);
	delete pdata;
	JSONNode* ret=libJSON::Parse(wresp);
	delete wresp;
	delete rbuf;
	return ret;
}


void CPluginViewDlg::OnNMDblclkList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	Play(pNMItemActivate->iItem);
	*pResult = 0;
}


void CPluginViewDlg::OnNMRClickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	/**/
	POINT pt;
	GetCursorPos(&pt);
	contextMenu.TrackPopupMenu(0, pt.x, pt.y, this);
	*pResult = 0;
}


UINT CPluginViewDlg::LoadUserThread(void* param)
{
	CPluginViewDlg* d=(CPluginViewDlg*)param;
	d->BeginWaitCursor();
	//d->list.ShowWindow(SW_HIDE);
	d->ShowProgress();
	d->list.DeleteAllItems();
	d->slist.clear();
	CVkAPIRequest req(L"audio.get");
	wchar_t* buf=new wchar_t[10];
	wsprintf(buf, L"%i", d->currentUser);
	req.AddParameter(L"uid", buf);
	JSONNode* r=d->SendRequest(&req);
	if(r && r->GetNode(L"response")){
		for(int i=0;i<r->GetNode(L"response")->NodeSize();i++){
			JSONNode* ai=r->GetNode(L"response")->NodeAt(i);
			d->AddItem(i+1, (wchar_t*)ai->GetNode(L"artist")->NodeAsString().c_str(), 
				(wchar_t*)ai->GetNode(L"title")->NodeAsString().c_str(),
				_wtoi(ai->GetNode(L"duration")->NodeAsString().c_str()));
			Song s;
			s.artist=ai->GetNode(L"artist")->NodeAsString();
			s.title=ai->GetNode(L"title")->NodeAsString();
			s.url=ai->GetNode(L"url")->NodeAsString();
			s.duration=_wtoi(ai->GetNode(L"duration")->NodeAsString().c_str());
			s.aid=ai->GetNode(L"aid")->NodeAsString();
			s.oid=ai->GetNode(L"owner_id")->NodeAsString();
			d->slist.push_back(s);
		}
	}
	delete r;
	delete buf;
	d->EndWaitCursor();
	//d->list.ShowWindow(SW_SHOW);
	d->HideProgress();
	return 0;
}


UINT CPluginViewDlg::LoadFriendsThread(void* param)
{
	CPluginViewDlg* d=(CPluginViewDlg*)param;
	//d->friendList.ShowWindow(SW_HIDE);
	d->ShowProgress();
	d->friendList.InsertString(0, L"Мои Аудиозаписи");
	d->friendList.SetItemData(0, uid);
	CVkAPIRequest req(L"getFriends");
	req.AddParameter(L"fields", L"first_name,last_name");
	JSONNode* r=d->SendRequest(&req);
	if(r && r->GetNode(L"response")){
		vector<Friend> friends;
		for(int i=0;i<r->GetNode(L"response")->NodeSize();i++){
			Friend f;
			f.name=r->GetNode(L"response")->NodeAt(i)->GetNode(L"first_name")->NodeAsString()+wstring(L" ")+r->GetNode(L"response")->NodeAt(i)->GetNode(L"last_name")->NodeAsString();
			f.id=_wtoi(r->GetNode(L"response")->NodeAt(i)->GetNode(L"uid")->NodeAsString().c_str());
			friends.push_back(f);
		}
		for(int j=0;j<friends.size();j++){
			for(int i=0;i<friends.size()-1;i++){
				if(friends[i].name.compare(friends[i+1].name)>0){
					Friend tmp=friends[i+1];
					friends[i+1]=friends[i];
					friends[i]=tmp;
				}
			}
		}
		for(int i=0;i<friends.size();i++){
			d->friendList.InsertString(i+1, friends[i].name.c_str());
			d->friendList.SetItemData(i+1, friends[i].id);
		}
	}
	delete r;
	//d->friendList.ShowWindow(SW_SHOW);
	d->HideProgress();
	d->friendList.SetCurSel(0);
	return 0;
}


UINT CPluginViewDlg::LoadSearchThread(void* param)
{
	CPluginViewDlg* d=(CPluginViewDlg*)param;
	//d->list.ShowWindow(SW_HIDE);
	d->ShowProgress();
	d->list.DeleteAllItems();
	d->slist.clear();
	d->BeginWaitCursor();
	int offset=0;
	wchar_t obuf[15];
	vector<wstring> foundSongs;
	int itemN=0;
	for(int k=0;k<10;k++){
		CVkAPIRequest req(L"audio.search");
		req.AddParameter(L"q", d->searchValue.GetBuffer());
		req.AddParameter(L"count", L"100");
		_itow(offset, obuf, 10);
		req.AddParameter(L"offset", obuf);
		JSONNode* r=d->SendRequest(&req);
		if(r && r->GetNode(L"response")){
			if(!r->GetNode(L"response")->NodeSize()){
				delete r;
				break;
			}
			for(int i=1;i<r->GetNode(L"response")->NodeSize();i++){
				JSONNode* ai=r->GetNode(L"response")->NodeAt(i);
				bool found=false;
				wstring fss;
				if(d->removeRepeatCheck.GetCheck()){
					fss=ai->GetNode(L"artist")->NodeAsString()+L";"+ai->GetNode(L"title")->NodeAsString();
					int pos=0;
					while((pos=fss.find(L" ", pos))!=-1){
						fss.replace(pos, 1, L"");
					}
					for(int j=0;j<fss.length();j++){
						fss[j]=tolower(fss[j]);
					}
					for(int j=0;j<foundSongs.size();j++){
						if(foundSongs[j].compare(fss)==0)found=true;
					}
				}
				if(!found){
					d->AddItem(itemN+1, (wchar_t*)ai->GetNode(L"artist")->NodeAsString().c_str(), 
						(wchar_t*)ai->GetNode(L"title")->NodeAsString().c_str(),
						_wtoi(ai->GetNode(L"duration")->NodeAsString().c_str()));
					Song s;
					s.artist=ai->GetNode(L"artist")->NodeAsString();
					s.title=ai->GetNode(L"title")->NodeAsString();
					s.url=ai->GetNode(L"url")->NodeAsString();
					s.duration=_wtoi(ai->GetNode(L"duration")->NodeAsString().c_str());
					s.aid=ai->GetNode(L"aid")->NodeAsString();
					s.oid=ai->GetNode(L"owner_id")->NodeAsString();
					d->slist.push_back(s);
					foundSongs.push_back(fss);
					itemN++;
				}
			}
		}
		delete r;
		if(!d->removeRepeatCheck.GetCheck())break;
		offset+=100;
	}
	d->EndWaitCursor();
	//d->list.ShowWindow(SW_SHOW);
	d->HideProgress();
	return 0;
}


void CPluginViewDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
	currentUser=friendList.GetItemData(friendList.GetCurSel());
	AfxBeginThread(CPluginViewDlg::LoadUserThread, this);
}


void CPluginViewDlg::AddToPlaylist(int idx){
	wstring songname=slist[idx].artist+wstring(L" - ")+slist[idx].title;
	enqueueFileWithMetaStructW eFWMS = {0};
	eFWMS.filename=slist[idx].url.c_str();
	eFWMS.title = songname.c_str();
	eFWMS.length = slist[idx].duration;
	::SendMessage(plugin.hwndWinampParent,WM_WA_IPC,(WPARAM)&eFWMS,IPC_ENQUEUEFILEW);
}

void CPluginViewDlg::Play(int idx){
	wstring songname=slist[idx].artist+wstring(L" - ")+slist[idx].title;
	enqueueFileWithMetaStructW eFWMS = {0};
	eFWMS.filename=slist[idx].url.c_str();
	eFWMS.title = songname.c_str();
	eFWMS.length = slist[idx].duration;
	::SendMessage(plugin.hwndWinampParent,WM_WA_IPC,0,IPC_DELETE);
	::SendMessage(plugin.hwndWinampParent,WM_WA_IPC,(WPARAM)&eFWMS,IPC_ENQUEUEFILEW);
	::SendMessage(plugin.hwndWinampParent,WM_WA_IPC,0,IPC_STARTPLAY);
}

BOOL CPluginViewDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(wParam==MENU_OPEN){
		Play(list.GetHotItem());
		return 1;
	}
	if(wParam==MENU_ADD_TO_PLAYLIST){
		for(int i=0;i<list.GetItemCount();i++){
			if(list.GetItemState(i, LVIS_SELECTED)==LVIS_SELECTED)
				AddToPlaylist(i);
		}
		return 1;
	}
	if(wParam==MENU_ADD_ALL){
		for(int i=0;i<list.GetItemCount();i++){
				AddToPlaylist(i);
		}
		return 1;
	}
	if(wParam==MENU_SEARCH_PERFORMER){
		searchValue=CString(slist[list.GetHotItem()].artist.c_str());
		UpdateData(0);
		friendList.SetCurSel(-1);
		AfxBeginThread(CPluginViewDlg::LoadSearchThread, this);
		return 1;
	}
	if(wParam==MENU_ADD_TO_PAGE){
		for(int i=0;i<list.GetItemCount();i++){
			if(list.GetItemState(i, LVIS_SELECTED)==LVIS_SELECTED){
				CVkAPIRequest req(L"audio.add");
				req.AddParameter(L"aid", (wchar_t*)slist[i].aid.c_str());
				req.AddParameter(L"oid", (wchar_t*)slist[i].oid.c_str());
				JSONNode* r=SendRequest(&req);
				delete r;
			}
		}
		return 1;
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CPluginViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100){
		CRect rect;
		progrLabel.GetWindowRect(rect);
		rect.right+=2;
		if(rect.Width()>=270)rect.right=rect.left+1;
		progrLabel.SetWindowPos(0, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE|SWP_NOZORDER);
		progrLabel.RedrawWindow();
	}
	CDialog::OnTimer(nIDEvent);
}

void CPluginViewDlg::ShowProgress(void)
{
	progrLabel.ShowWindow(SW_SHOW);
	SetTimer(100, 50, NULL);
}

void CPluginViewDlg::HideProgress(void)
{
	progrLabel.ShowWindow(SW_HIDE);
	KillTimer(100);
}

void CPluginViewDlg::OnEnSetfocusEdit1()
{
	// TODO: Add your control notification handler code here
	//this->SetDefID(IDC_BUTTON1);
}

void CPluginViewDlg::OnStnClickedRrptLabel()
{
	// TODO: Add your control notification handler code here
	removeRepeatCheck.SetCheck(!removeRepeatCheck.GetCheck());
}
