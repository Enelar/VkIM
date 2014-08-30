// Connector.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "Connector.h"
#include <winsock.h>
#include <string>
#include <hash_map>
#include <Shlwapi.h>
#include <WinInet.h>
#include "Utils.h"
#include "BrowserDlg.h"
#include "EnBitmap.h"
#include "skin.h"
#include "TalkDlg.h"
#include "mmsystem.h"
#include "Lang.h"
#include "NotificationWnd.h"
#include "Settings.h"
#include "CaptchaDlg.h"
#include "base64.h"
#include "UserInfoTip.h"

extern char* sid;
extern unsigned int uid;
extern char* secret;
extern CSkin skin;
extern CConnector con;
// CConnector

extern CTalkDlg* talks;

#define MSG_UNREAD 1
#define MSG_OUTBOX 2
#define MSG_REPLIED 4
#define MSG_IMPORTANT 8
#define MSG_CHAT 16
#define MSG_FRIENDS 32
#define MSG_SPAM 64
#define MSG_DELETED 128
#define MSG_FIXED 256
#define MSG_MEDIA 512

CConnector::CConnector()
{
	authTries=3;
	canExit=false;
	canExit2=false;
	needExit=false;
	httpSck=0;
	authorizing=false;
	connectionError=false;
	loadingPhotos=false;
	tmp_msg_id=0;
	iconOnline=false;
	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&cs2);
	InitializeCriticalSection(&cs3);
	InitializeCriticalSection(&lpCs);
	InitializeCriticalSection(&tipCs);
}

CConnector::~CConnector()
{
}

void CConnector::SendMsgThread(message* msg){
	CVkAPIRequest ar(L"messages.send");
	wchar_t ub[15];
	memset(ub, 0, 15);
	wsprintf(ub, L"%i", msg->rcpt);
	ar.AddParameter(L"uid", ub);
	ar.AddParameter(L"message", msg->text);
	if(CSettings::fromChat)ar.AddParameter(L"type", L"1");
	JSONNode* resp=SendAPIRequest(&ar);
	if(!resp){
		MessageBox(NULL, L"Ошибка при отправке сообщения.", NULL, MB_ICONEXCLAMATION);
	}
	if(resp->GetNode(L"response")){
		int mid=resp->GetNode(L"response")->NodeAsInt();
		if(talks->openedTabs[msg->rcpt]){
			talks->openedTabs[msg->rcpt]->sentIDs.push_back(mid);
			talks->openedTabs[msg->rcpt]->msgBoard.SetMsgID(mid, msg->tmp_id);
		}
	}
	delete resp;
}

UINT CConnector::_SendMsgThread(message* msg){
	__try{
	if(CSettings::globalSoundOn && CSettings::soundSend)PlaySound(CSettings::soundSendFile, NULL, SND_ASYNC | SND_FILENAME);
	con.SendMsgThread(msg);
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"SendMsg")){}
	return 0;
}

int CConnector::SendMsg(wchar_t* msg, int rcpt){
	message* m=new message;
	m->text=msg;
	m->rcpt=rcpt;
	m->tmp_id=tmp_msg_id;
	tmp_msg_id++;
	AfxBeginThread((AFX_THREADPROC)&_SendMsgThread, m); 
	return m->tmp_id;
}

UINT CConnector::_MarkAsReadThread(wchar_t* ids){
	__try{
	CVkAPIRequest ar(L"messages.markAsRead");
	ar.AddParameter(L"mids", ids);
	JSONNode* resp=con.SendAPIRequest(&ar);
	delete resp;
	delete ids;
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"MarkAsRead")){}
	return 0;
}

void CConnector::MarkAsRead(wchar_t* ids){
	AfxBeginThread((AFX_THREADPROC)&_MarkAsReadThread, ids); 
}

UINT CConnector::_SetStatusThread(StatusUpdateReq* sr){
	__try{
	CVkAPIRequest ar(L"wall.post");
	wstring attstr;
	con.clDlg->StartStatusProgress();
	con.clDlg->newStatusText=CLang::Get(L"loading");
	ar.AddParameter(L"message", (wchar_t*)sr->text.c_str());
	if(sr->att){
		if(sr->att->upload){
			if(sr->att->type==ATT_PHOTO){
				CVkAPIRequest ar2(L"photos.getWallUploadServer");
				JSONNode* rsp=con.SendAPIRequest(&ar2);
				wstring uploadServer=rsp->GetNode(L"response")->GetNode(L"upload_url")->NodeAsString();
				wstring aid=rsp->GetNode(L"response")->GetNode(L"aid")->NodeAsString();
				delete rsp;
				char* result=con.UploadFile(sr->att->filename, uploadServer, L"file1");
				wchar_t* wresult=CUtils::Utf8ToWchar(result);
				delete result;
				JSONNode* ud=libJSON::Parse(wresult);
				delete wresult;
				CVkAPIRequest ar3(L"photos.saveWallPhoto");
				ar3.AddParameter(L"server", (wchar_t*)ud->GetNode(L"server")->NodeAsString().c_str());
				ar3.AddParameter(L"photo", (wchar_t*)ud->GetNode(L"photo")->NodeAsString().c_str());
				ar3.AddParameter(L"hash", (wchar_t*)ud->GetNode(L"hash")->NodeAsString().c_str());
				
				JSONNode* ad=con.SendAPIRequest(&ar3);
				attstr=L"photo";
				attstr+=ad->GetNode(L"response")->NodeAt(0)->GetNode(L"owner_id")->NodeAsString();
				attstr+=L"_";
				attstr+=ad->GetNode(L"response")->NodeAt(0)->GetNode(L"pid")->NodeAsString();
				ar.AddParameter(L"attachment", (wchar_t*)attstr.c_str());
				delete ud;
			}

			if(sr->att->type==ATT_AUDIO){
				CVkAPIRequest ar2(L"audio.getUploadServer");
				JSONNode* rsp=con.SendAPIRequest(&ar2);
				wstring uploadServer=rsp->GetNode(L"response")->GetNode(L"upload_url")->NodeAsString();
				delete rsp;
				char* result=con.UploadFile(sr->att->filename, uploadServer, L"file");
				TRACE("%s\n", result);
				wchar_t* wresult=CUtils::Utf8ToWchar(result);
				delete result;
				JSONNode* ud=libJSON::Parse(wresult);
				delete wresult;
				CVkAPIRequest ar3(L"audio.save");
				ar3.AddParameter(L"server", (wchar_t*)ud->GetNode(L"server")->NodeAsString().c_str());
				ar3.AddParameter(L"audio", (wchar_t*)ud->GetNode(L"audio")->NodeAsString().c_str());
				ar3.AddParameter(L"hash", (wchar_t*)ud->GetNode(L"hash")->NodeAsString().c_str());
				
				JSONNode* ad=con.SendAPIRequest(&ar3);
				attstr=L"audio";
				attstr+=ad->GetNode(L"response")->GetNode(L"owner_id")->NodeAsString();
				attstr+=L"_";
				attstr+=ad->GetNode(L"response")->GetNode(L"aid")->NodeAsString();
				ar.AddParameter(L"attachment", (wchar_t*)attstr.c_str());
				delete ud;
			}

			if(sr->att->type==ATT_VIDEO){
				CVkAPIRequest ar2(L"video.save");
				JSONNode* rsp=con.SendAPIRequest(&ar2);
				wstring uploadServer=rsp->GetNode(L"response")->GetNode(L"upload_url")->NodeAsString();

				attstr=L"video";
				attstr+=rsp->GetNode(L"response")->GetNode(L"owner_id")->NodeAsString();
				attstr+=L"_";
				attstr+=rsp->GetNode(L"response")->GetNode(L"vid")->NodeAsString();
				ar.AddParameter(L"attachment", (wchar_t*)attstr.c_str());

				delete rsp;
				char* result=con.UploadFile(sr->att->filename, uploadServer, L"video_file");
				delete result;
			}
		}
	}
	JSONNode* resp=con.SendAPIRequest(&ar);
	con.clDlg->StopStatusProgress();
	con.clDlg->userStatusLabel.SetWindowTextW(sr->text.c_str());
	con.clDlg->userStatusLabel.RedrawWindow();
	con.clDlg->Invalidate();
	delete resp;
	delete sr;
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"SetStatus")){}
	return 0;
}

void CConnector::SetStatus(StatusUpdateReq* sr){
	AfxBeginThread((AFX_THREADPROC)&_SetStatusThread, sr); 
}

wchar_t* CConnector::DownloadImage(wchar_t* url){
	
	std::wstring wpath;
	int driveType=GetDriveType(NULL);
	if(driveType!=DRIVE_REMOVABLE){
		wchar_t* buf=new wchar_t[512];
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, buf);
		wpath=buf;
		delete buf;
		wpath+=L"\\VkIM\\Cache\\";
	}else{
		wchar_t* buf=new wchar_t[512];
		GetCurrentDirectory(512, buf);
		wpath=buf;
		delete buf;
		wpath+=L"\\Profile\\Cache\\";
	}
	
	if(!PathIsDirectory(wpath.c_str())){
	CreateDirectory(wpath.c_str(), NULL);
	}
	std::wstring wurl=url;
	wurl=wurl.substr(7); // Remove "http://"
	std::wstring host=wurl.substr(0, wurl.find_first_of(L'/'));
	std::wstring path=wurl.substr(wurl.find_first_of(L'/'));
	wchar_t* _url=url;
	_url+=7;
	for(int i=0;i<wcslen(_url);i++){
		if(_url[i]==L'/' || _url[i]==L':' || _url[i]==L'=' || _url[i]==L'?' || _url[i]==L'&'){
			_url[i]=L'_';
		}
	}
	wpath+=_url;
	if(wpath.find(L".jpg")==-1 && wpath.find(L".gif")==-1){
		wpath+=L".jpg";
	}
	FILE* check_exist=_wfopen(wpath.c_str(), L"rb");
	if(check_exist){
		fclose(check_exist);
		wchar_t* ret=new wchar_t[wpath.length()+1];
	wcscpy(ret, wpath.c_str());
		return ret;
	}
	char* _host=CUtils::wtoc(host.c_str(), host.length()+1);
	char* _path=CUtils::wtoc(path.c_str(), path.length()+1);
//	char* hreq=CreateHTTPRequest(_host, _path, "GET", NULL, NULL);
	int len=0;
//	char* data=SendHTTPRaw(_host, 80, hreq, &len);
	char* data=SendHTTP(_host, 80, _path, NULL, &len);
	if(data){
		FILE* f=_wfopen(wpath.c_str(), L"wb");
		fwrite(data, 1, len, f);
		fclose(f);
	}
	delete data;
	delete _host;
	delete _path;
//	delete hreq;
	wchar_t* ret=new wchar_t[wpath.length()+1];
	wcscpy(ret, wpath.c_str());
	return ret;
}

char* CConnector::SendHTTP(char* host, unsigned int port, char* path, char* post_data, int* len, bool multipart, int post_len){
	HINTERNET hInet=InternetOpen(L"Vk.IM", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hInet){
		ShowLastError();
		return NULL;
	}
	HINTERNET hSession=InternetConnectA(hInet, host, 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if(!hSession){
		ShowLastError();
		InternetCloseHandle(hInet);
		return NULL;
	}
	HINTERNET hRequest=HttpOpenRequestA(hSession, post_data?"POST":"GET", path, NULL, NULL, NULL, 0, 0);
	if(!hRequest){
		ShowLastError();
		InternetCloseHandle(hInet);
		InternetCloseHandle(hSession);
		return NULL;
	}
	httpSck=hRequest;
	char* header=NULL;
	if(post_data){
		header=multipart?"Content-Type: multipart/form-data; boundary=\"VK-IM-FILE-UPLOAD-BOUNDARY\"":"Content-Type: application/x-www-form-urlencoded";
	}
	bool result=HttpSendRequestA(hRequest, header, header?strlen(header):0, post_data, post_data?(post_len==-1?strlen(post_data):post_len):0);
	if(!result){
		if(!needExit)ShowLastError();
		InternetCloseHandle(hInet);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hRequest);
		httpSck=0;
		return NULL;
	}
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
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInet);
	rbuf=(char*)realloc(rbuf, rsize+1);
	rbuf[rsize]=0;
	if(len)*len=rsize;
	if(connectionError){	
		SetTrayIconOnline(true);
		connectionError=false;
	}
	httpSck=0;
	return rbuf;
}


JSONNode* CConnector::SendAPIRequest(CVkAPIRequest* req){
	char* rs=req->GetRequestString();
	char* apisrv=CUtils::wtoc(CSettings::apiServer, wcslen(CSettings::apiServer)+1);
	char* apipath=CUtils::wtoc(CSettings::apiReqPath, wcslen(CSettings::apiReqPath)+1);
	char* resp_utf=SendHTTP(apisrv, CSettings::apiPort, apipath, rs);
	delete apisrv;
	delete apipath;
	wchar_t* resp_w=CUtils::Utf8ToWchar(resp_utf);
	JSONNode* resp=libJSON::Parse(resp_w);
	if(!resp){
		/*std::wstring emsg=L"Error parsing API response!\n";
		for(int i=0;i<req->param_names.size();i++){
			emsg+=req->param_names[i];
			emsg+=L"=";
			emsg+=req->param_values[i];
			emsg+=L"\n";
		}
		MessageBox(NULL, emsg.c_str(), NULL, MB_ICONEXCLAMATION|MB_OK);*/
		_sleep(3000);
					delete rs;
					delete resp_utf;
					delete resp_w;
					delete resp;
					return SendAPIRequest(req);
	}else{
		if(resp->NodeAt(0)->NodeName().compare(L"error")==0){
			int errcode=resp->GetNode(L"error")->GetNode(L"error_code")->NodeAsInt();
			if(errcode==3 || errcode==4){
				if(authTries==0){
					MessageBox(NULL, CLang::Get(L"auth_proxy_error"), NULL, MB_ICONEXCLAMATION);
					authTries=3;
				}
				if(authorizing){
					while(authorizing)_sleep(100);
					delete rs;
					delete resp_utf;
					delete resp_w;
					delete resp;
					return SendAPIRequest(req);
				}else{
					authorizing=true;
				CBrowserDlg bd;
				INT_PTR res=bd.DoModal();
				if(res==IDOK){
					authorizing=false;
					authTries--;
					SaveSessionData();
					delete rs;
					delete resp_utf;
					delete resp_w;
					delete resp;
					return SendAPIRequest(req);
				}
				}
			}else if(errcode==6){
				_sleep(500);
				delete rs;
				delete resp_utf;
				delete resp_w;
				delete resp;
				return SendAPIRequest(req);
			}else if(errcode==14){
				wchar_t* url=(wchar_t*)resp->GetNode(L"error")->GetNode(L"captcha_img")->NodeAsString().c_str();
				wchar_t* fn=DownloadImage(url);
				CCaptchaDlg cdlg(CEnBitmap::LoadImageFile(fn), talks);
				int res=cdlg.DoModal();
				if(res==IDOK){
					req->AddParameter(L"captcha_key", cdlg.inputValue.GetBuffer());
					req->AddParameter(L"captcha_sid", (wchar_t*)resp->GetNode(L"error")->GetNode(L"captcha_sid")->NodeAsString().c_str());
				}
				_wunlink(fn);
				return SendAPIRequest(req);
			}else{
				wchar_t* e=new wchar_t[300];
				swprintf(e, L"#%i: %s", errcode, resp->GetNode(L"error")->GetNode(L"error_msg")->NodeAsString().c_str());
				int result=MessageBox(NULL, e, NULL, MB_ICONEXCLAMATION|MB_ABORTRETRYIGNORE);
				delete e;
				if(result==IDRETRY){
					delete rs;
					delete resp_utf;
					delete resp_w;
					delete resp;
					return SendAPIRequest(req);
				}
			}
		}
	}
	delete rs;
	delete resp_utf;
	delete resp_w;
	return resp;
}

void CConnector::LoadContactListThread(){
	AfxBeginThread((AFX_THREADPROC)&DoCheckUpdates, NULL);
	EnterCriticalSection(&lpCs);
	CFont* fnt=clDlg->userNameLabel.GetFont();
	LOGFONT lf;
	fnt->GetLogFont(&lf);
	lf.lfWeight=FW_BOLD;
	CFont fnt2;
	fnt2.CreateFontIndirectW(&lf);
	clDlg->userNameLabel.SetFont(&fnt2);

	CVkAPIRequest ar3(L"execute");
	wchar_t* xc=new wchar_t[512];
	wsprintf(xc, L"return {\"profile\":API.getProfiles({\"uids\":%i,\"fields\":\"photo_rec,rate\"}),\"activity\":API.activity.get()};",
		uid);
	ar3.AddParameter(L"code", xc);
	JSONNode* resp3=SendAPIRequest(&ar3);
	JSONNode* uinfo=resp3->GetNode(L"response");
	delete xc;

	wchar_t* ppath=NULL;
	if(uinfo->GetNode(L"profile")->NodeAt(0)->GetNode(L"photo_rec")){
		ppath=DownloadImage((wchar_t*)uinfo->GetNode(L"profile")->NodeAt(0)->GetNode(L"photo_rec")->NodeAsString().c_str());
		HBITMAP hBitmap=CEnBitmap::LoadImageFile(ppath);
		clDlg->userAvatar.SetBitmap(hBitmap);
	}
	std::wstring user_name=uinfo->GetNode(L"profile")->NodeAt(0)->GetNode(L"first_name")->NodeAsString();
	user_name+=' ';
	user_name+=uinfo->GetNode(L"profile")->NodeAt(0)->GetNode(L"last_name")->NodeAsString();

	clDlg->userNameLabel.SetWindowTextW(user_name.c_str());
	CRect rect1;
	clDlg->userNameLabel.GetClientRect(&rect1);
	clDlg->userNameLabel.ClientToScreen(&rect1);
	clDlg->ScreenToClient(&rect1);
	clDlg->InvalidateRect(&rect1, TRUE);

	my_name=(wchar_t*)user_name.c_str();

	if(uinfo->GetNode(L"activity")->GetNode(L"activity")->NodeAsString().length()>0){
		clDlg->userHasStatus=true;
		clDlg->userStatusLabel.SetWindowTextW(uinfo->GetNode(L"activity")->GetNode(L"activity")->NodeAsString().c_str());
	}else{
		clDlg->userHasStatus=false;
		clDlg->userStatusLabel.SetWindowTextW(CLang::Get(L"click_to_change_status"));
	}

	clDlg->userStatusLabel.GetClientRect(&rect1);
	clDlg->userStatusLabel.ClientToScreen(&rect1);
	clDlg->ScreenToClient(&rect1);
	clDlg->InvalidateRect(&rect1, TRUE);
	delete resp3;
	delete ppath;

	lastLongPollResp=time(NULL);

	while(true){
		LoadFriends();
		AfxBeginThread((AFX_THREADPROC)&DoCheckOfflineMsgs, NULL);
		LongPoll();
		if(needExit)break;
	}

	LeaveCriticalSection(&lpCs);
	TRACE("exited long poll 2\n");
}

UINT CConnector::_LoadContactListThread(CConnector* _con){
	__try{
	_con->LoadContactListThread();
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"Connector")){}
	return 0;
}

void CConnector::LoadContactList(CVkIMDlg* dlg){
	clDlg=dlg;
	AfxBeginThread((AFX_THREADPROC)&_LoadContactListThread, this); 
	AfxBeginThread((AFX_THREADPROC)&SendOnline, NULL); 
}

void CConnector::SaveSessionData(){
	std::wstring wpath;
	int driveType=GetDriveType(NULL);
	if(driveType!=DRIVE_REMOVABLE){
		wchar_t* buf=new wchar_t[512];
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, buf);
		wpath=buf;
		delete buf;
		wpath+=L"\\VkIM";
	}else{
		wchar_t* buf=new wchar_t[512];
		GetCurrentDirectory(512, buf);
		wpath=buf;
		delete buf;
		wpath+=L"\\Profile";
	}
	if(!PathIsDirectory(wpath.c_str())){
	CreateDirectory(wpath.c_str(), NULL);
	}
	FILE* sd;
	wchar_t* sd_file_name=new wchar_t[512];
	wsprintf(sd_file_name, L"%s\\session.dat", wpath.c_str());
		sd=_wfopen(sd_file_name, L"w");
		fprintf(sd, "%s\r\n%s\r\n%i", sid, secret, uid);
		fclose(sd);
	delete sd_file_name;
}

void CConnector::DeleteSessionData(){
	std::wstring wpath;
	int driveType=GetDriveType(NULL);
	if(driveType!=DRIVE_REMOVABLE){
		wchar_t* buf=new wchar_t[512];
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, buf);
		wpath=buf;
		delete buf;
		wpath+=L"\\VkIM";
	}else{
		wchar_t* buf=new wchar_t[512];
		GetCurrentDirectory(512, buf);
		wpath=buf;
		delete buf;
		wpath+=L"\\Profile";
	}
	FILE* sd;
	wchar_t* sd_file_name=new wchar_t[512];
	wsprintf(sd_file_name, L"%s\\session.dat", wpath.c_str());
	_wunlink(sd_file_name);
	delete sd_file_name;
}

void CConnector::ShowLastError(void)
{
	if(connectionError)return;
	LPVOID lpMsgBuf;

FormatMessage( 
FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
GetModuleHandle(_T("wininet.dll")),
GetLastError(),
MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
(LPTSTR) &lpMsgBuf,
0,
NULL 
);
	
// Display the string.
//MessageBox( NULL, (wchar_t*)lpMsgBuf, L"Connection error!", MB_OK|MB_ICONEXCLAMATION );
NotificationStruct ns;
ns.content=(wchar_t*)lpMsgBuf;
ns.title=CLang::Get(L"connection_error");
ns.uid=0;
clDlg->SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
connectionError=true;
SetTrayIconOnline(false);
LocalFree(lpMsgBuf);
}


bool CConnector::GetProfileByID(int uid, ListEntry* result)
{
	for(int i=0;i<clDlg->friendList.items.size();i++){
		if(clDlg->friendList.items[i].uid==uid){
			//result=&(clDlg->friendList.items_array[i]);
			//memcpy(result, &(clDlg->friendList.items_array[i]), sizeof(ListEntry));
			result->userName=clDlg->friendList.items[i].userName;
			result->online=clDlg->friendList.items[i].online;
			result->userBigPhotoURL=clDlg->friendList.items[i].userBigPhotoURL;
			result->uid=uid;
			return true;
		}
	}

	CVkAPIRequest req(L"getProfiles");
	wchar_t* uid_buf=new wchar_t[10];
	swprintf(uid_buf, L"%i", uid);
	req.AddParameter(L"uids", uid_buf);
	req.AddParameter(L"fields", L"photo_rec,photo_medium_rec");
	JSONNode* _resp=SendAPIRequest(&req);
	JSONNode* resp=_resp->GetNode(L"response");
	delete uid_buf;
	result->uid=uid;
	result->userName=new wchar_t[resp->NodeAt(0)->GetNode(L"first_name")->NodeAsString().length()+
		resp->NodeAt(0)->GetNode(L"last_name")->NodeAsString().length()+1];
	//swprintf(result->userName, L"%s %s", resp->NodeAt(0)->GetNode(L"first_name")->NodeAsString().c_str(),
	//	resp->NodeAt(0)->GetNode(L"last_name")->NodeAsString().c_str());
	result->userName=resp->NodeAt(0)->GetNode(L"first_name")->NodeAsString()+L" "+resp->NodeAt(0)->GetNode(L"last_name")->NodeAsString();
	result->userBigPhotoURL=resp->NodeAt(0)->GetNode(L"photo_medium_rec")->NodeAsString();
	delete _resp;
	return false;
}


UINT CConnector::_GetCountersThread(CConnector* con)
{
	CVkAPIRequest ar(L"getCounters");
	JSONNode* resp=con->SendAPIRequest(&ar);
	JSONNode* r=resp->GetNode(L"response");
	Counters cnt;
	memset(&cnt, 0, sizeof(Counters));
	if(r->GetNode(L"events"))cnt.events=r->GetNode(L"events")->NodeAsInt();
	if(r->GetNode(L"friends"))cnt.friends=r->GetNode(L"friends")->NodeAsInt();
	if(r->GetNode(L"gifts"))cnt.gifts=r->GetNode(L"gifts")->NodeAsInt();
	if(r->GetNode(L"groups"))cnt.groups=r->GetNode(L"groups")->NodeAsInt();
	if(r->GetNode(L"notes"))cnt.notes=r->GetNode(L"notes")->NodeAsInt();
	if(r->GetNode(L"offers"))cnt.offers=r->GetNode(L"offers")->NodeAsInt();
	if(r->GetNode(L"opinions"))cnt.opinions=r->GetNode(L"opinions")->NodeAsInt();
	if(r->GetNode(L"photos"))cnt.photos=r->GetNode(L"photos")->NodeAsInt();
	if(r->GetNode(L"questions"))cnt.questions=r->GetNode(L"questions")->NodeAsInt();
	if(r->GetNode(L"videos"))cnt.videos=r->GetNode(L"videos")->NodeAsInt();
	delete resp;
	con->clDlg->SendMessage(WM_UPDATE_COUNTERS, (WPARAM)&cnt);
	return 0;
}


void CConnector::GetCounters(void)
{
	AfxBeginThread((AFX_THREADPROC)&_GetCountersThread, this); 
}


void CConnector::GetHistory(CTalkTab* tab)
{
	AfxBeginThread((AFX_THREADPROC)&_GetHistoryThread, tab); 
}


UINT CConnector::_GetHistoryThread(CTalkTab* tab)
{
	TRACE("get history. %i\n", tab->uid);
	CVkAPIRequest ar(L"messages.getHistory");
	wchar_t* uid_buf=new wchar_t[10];
	_itow(tab->uid, uid_buf, 10);
	ar.AddParameter(L"uid", uid_buf);
	wchar_t* cnt_buf=new wchar_t[10];
	wchar_t* off_buf=NULL;
	_itow(CSettings::numHistory, cnt_buf, 10);
	ar.AddParameter(L"count", cnt_buf);
	
	if(tab->hasNewMsg && !tab->historyLoaded){
		ar.AddParameter(L"offset", L"1");
	}
	if(tab->historyLoaded){
		off_buf=new wchar_t[10];
		_itow(tab->msgCount, off_buf, 10);
		ar.AddParameter(L"offset", off_buf);
	}
	JSONNode* jn=con.SendAPIRequest(&ar);
	delete uid_buf;
	delete cnt_buf;
	if(off_buf)delete off_buf;
	TRACE("History Loaded\n");
	int cnt=jn->GetNode(L"response")->NodeSize()-1;//jn->GetNode(L"response")->NodeAt(0)->NodeAsInt();
	//if(cnt>CSettings::numHistory)cnt=CSettings::numHistory;
	//if(tab->hasNewMsg)cnt--;
	HistoryEntry* msgs=new HistoryEntry[cnt];
	for(int i=1;i<=cnt;i++){
		msgs[i-1].from=jn->GetNode(L"response")->NodeAt(i)->GetNode(L"from_id")->NodeAsInt();
		msgs[i-1].time=jn->GetNode(L"response")->NodeAt(i)->GetNode(L"date")->NodeAsInt();
		std::wstring msg=jn->GetNode(L"response")->NodeAt(i)->GetNode(L"body")->NodeAsString();
		msgs[i-1].text=new wchar_t[msg.length()+1];
		wcscpy(msgs[i-1].text, msg.c_str());
	}
	if(tab){ // Если вдруг кто-то закрыл вкладку, пока истроия загружалась
	while(!tab->msgBoard.ready)_sleep(100);
	tab->SendMessage(WM_HISTORY_LOADED, (WPARAM)msgs, cnt);
	}
	delete msgs;
	delete jn;
	return 0;
}


void CConnector::LoadFriendPhotos(void)
{
	AfxBeginThread((AFX_THREADPROC)&DoLoadFriendPhotos, NULL); 
}


UINT CConnector::DoLoadFriendPhotos(void* param)
{
	__try{
	EnterCriticalSection(&con.cs2);
	con.loadingPhotos=true;
	for(int i=0;i<con.clDlg->friendList.items.size();i++){
		wchar_t* url=(wchar_t*)con.clDlg->friendList.items[i].userPhotoURL.c_str();
		wchar_t* fname=con.DownloadImage(url);
		HBITMAP hBitmap=CEnBitmap::LoadImageFile(fname);
		if(con.clDlg->friendList.items.size()==0)return 0;
		con.clDlg->friendList.items[i].userPhoto=hBitmap;
		if(!con.clDlg->friendList)return 0; // Если приложение ВНЕЗАПНО закрылось...
		if(con.clDlg->friendList.IsItemVisible(i))con.clDlg->friendList.RedrawWindow();
		TRACE("%i/%i\n", i, con.clDlg->friendList.items.size());
		delete fname;
		if(con.needExit){
			con.canExit2=true;
			LeaveCriticalSection(&con.cs2);
			return 0;
		}
	}
	TRACE("All photos loaded\n");
	time_t t1=time(NULL);
	for(int i=0;i<con.clDlg->friendList.items.size();i++){
		if(!con.clDlg->friendList)return 0;
		HBITMAP hBitmap=CUtils::ResizeBitmap(con.clDlg->friendList.items[i].userPhoto, 50, 50, 30, 30);
		if(!con.clDlg->friendList)return 0;
		if(con.clDlg->friendList.items.size()==0)return 0;
		con.clDlg->friendList.items[i].userPhoto30=hBitmap;
		if(con.clDlg->friendList.IsItemVisible(i))con.clDlg->friendList.RedrawWindow();
		if(con.needExit){
			con.canExit2=true;
			LeaveCriticalSection(&con.cs2);
			return 0;
		}
	}
	time_t t2=time(NULL);
	TRACE("T=%i\n", t2-t1);
	con.canExit2=true;

	con.loadingPhotos=false;
	LeaveCriticalSection(&con.cs2);
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"LoadPhotos")){}

	return 0;
}

UINT CConnector::DoCheckUpdates(void* param){
	__try{
	if(CSettings::checkUpdates){
		//char* hreq=con.CreateHTTPRequest("v4ate.com", "/vkim/update.php?ver=1.1.5", "GET", NULL, NULL);
		//char* hreq=con.CreateHTTPRequest("localhost", "/update.php?ver=1.0.5", "GET", NULL, NULL);
		int len=0;
		//char* data=con.SendHTTPRaw("v4ate.com", 80, hreq, &len);
		char* data=con.SendHTTP("v4ate.com", 80, "/vkim/update.php?ver="APP_VER_STR, NULL, &len);
		//char* data=con.SendHTTPRaw("localhost", 80, hreq, &len);
		TRACE("upd=%s\n", data);
		if(data){
			if(data[0]=='1'){
				int rs=MessageBox(NULL, CLang::Get(L"update_avail"), L"Vk.IM", MB_YESNO);
				if(rs==IDYES){
					ShellExecute(NULL, L"open", L"http://vk-im.com/?upd", NULL, NULL, SW_SHOWNORMAL);
				}
			}
			delete data;
		}
		//delete hreq;
	}
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"AutoUpdate")){}
	return 0;
}

void CConnector::SetTrayIconOnline(bool state)
{
	iconOnline=state;
	NOTIFYICONDATA iconData;
	memset(&iconData, 0, sizeof(NOTIFYICONDATA));
	iconData.cbSize=sizeof(NOTIFYICONDATA);
	iconData.uFlags=NIF_ICON | NIF_TIP;
	iconData.hWnd=clDlg->GetSafeHwnd();
	iconData.hIcon=::LoadIconW(::AfxGetInstanceHandle(), MAKEINTRESOURCE(state?IDI_TRAY_C:IDI_TRAY_D));
	iconData.uID=1234;
	wchar_t* buf=new wchar_t[512];
	if(my_name)wsprintf(buf, L"Vk.IM - %s", my_name);
	else wsprintf(buf, L"Vk.IM");
	memcpy(&iconData.szTip, buf, 128);
	Shell_NotifyIcon(NIM_MODIFY, &iconData);
	delete buf;
}


void CConnector::LoadFriends(void)
{
	clDlg->friendList.SetGroupsMode(CSettings::showLists);
	CVkAPIRequest ar(L"friends.getLists");
	JSONNode* resp=SendAPIRequest(&ar);
	JSONNode* llist=resp->GetNode(L"response");
	stdext::hash_map<std::wstring, HTREEITEM> groups;
	vector<HTREEITEM> groups1;

	if(clDlg->friendList.items.size()>0){
		for(int i=0;i<clDlg->friendList.items.size();i++){
			//if(clDlg->friendList.items[i].userName)delete clDlg->friendList.items[i].userName;
			//if(clDlg->friendList.items[i].userPhotoURL)delete clDlg->friendList.items[i].userPhotoURL;
			if(clDlg->friendList.items[i].userPhoto)DeleteObject(clDlg->friendList.items[i].userPhoto);
			if(clDlg->friendList.items[i].userPhoto30)DeleteObject(clDlg->friendList.items[i].userPhoto30);
		}
	}

	clDlg->friendList.items.clear();
	clDlg->friendList.groupsExpanded.clear();
	clDlg->friendList.groupsNames.clear();
	clDlg->friendList.groupsIDs.clear();

	for(int i=0;i<llist->NodeSize();i++){
		//wchar_t* cc=new wchar_t[llist->NodeAt(i)->GetNode(L"name")->NodeAsString().length()+1];
		//wcscpy(cc, llist->NodeAt(i)->GetNode(L"name")->NodeAsString().c_str());
		std::vector<int> nv;
		clDlg->friendList.sortedItems[llist->NodeAt(i)->GetNode(L"lid")->NodeAsInt()]=nv;
		clDlg->friendList.groupsExpanded.push_back(false);
		clDlg->friendList.groupsNames.push_back(llist->NodeAt(i)->GetNode(L"name")->NodeAsString());
		clDlg->friendList.groupsIDs.push_back(llist->NodeAt(i)->GetNode(L"lid")->NodeAsInt());
	}
	delete resp;
	std::vector<int> nv;
	clDlg->friendList.sortedItems[-1]=nv;
	clDlg->friendList.groupsExpanded.push_back(false);
	clDlg->friendList.groupsNames.push_back(CLang::Get(L"ungrouped"));
	clDlg->friendList.groupsIDs.push_back(-1);

	CVkAPIRequest ar2(L"getFriends");
	ar2.AddParameter(L"fields", L"first_name,last_name,online,photo_rec,photo_medium_rec,rate");
	JSONNode* resp2=SendAPIRequest(&ar2);
	JSONNode* flist=resp2->GetNode(L"response");
	
	for(int i=0;i<flist->NodeSize();i++){
		JSONNode* n=flist->NodeAt(i);
		std::wstring uname=n->GetNode(L"first_name")->NodeAsString();
		uname+=' ';
		uname+=n->GetNode(L"last_name")->NodeAsString();
		ListEntry en;
				en.online=(n->GetNode(L"online")->NodeAsString().at(0)=='1');
				//en.userName=new wchar_t[uname.length()+2];
				//memset(en.userName, 0, uname.length()+2);
				//wcscpy(en.userName, uname.c_str());
				//en.userPhotoURL=new wchar_t[n->GetNode(L"photo_rec")->NodeAsString().length()+1];
				//wcscpy(en.userPhotoURL, n->GetNode(L"photo_rec")->NodeAsString().c_str());
				en.userPhotoURL=n->GetNode(L"photo_rec")->NodeAsString();
				en.userBigPhotoURL=n->GetNode(L"photo_medium_rec")->NodeAsString();
				en.userName=uname;
				char* _uid_c=CUtils::wtoc(n->GetNode(L"uid")->NodeAsString().c_str(), n->GetNode(L"uid")->NodeAsString().length()+1);
				en.uid=atoi(_uid_c);
				en.userPhoto=en.userPhoto30=NULL;
				delete _uid_c;
		if(n->GetNode(L"lists")){
			for(int j=0;j<n->GetNode(L"lists")->NodeSize();j++){
				en.groups.push_back(n->GetNode(L"lists")->NodeAt(j)->NodeAsInt());
			}
		}
		else{
			en.groups.push_back(-1);
			en.groups.push_back(clDlg->friendList.groupsNames.size()-1);
		}
		clDlg->friendList.items.push_back(en);
	}
	delete resp2;

	for(int i=0;i<clDlg->friendList.groupsNames.size();i++){
		clDlg->friendList.Sort(i, false);
	}

	clDlg->friendList.SetGroupsMode(CSettings::showLists);
	
	LoadFriendPhotos();
	_sleep(100);
	clDlg->friendList.UpdateScroll();
}


void CConnector::LongPoll(void)
{
	CVkAPIRequest ar4(L"messages.getLongPollServer");
	JSONNode* lpServerInfo=SendAPIRequest(&ar4);
	if(!lpServerInfo){
		return;
	}
	if(!lpServerInfo->GetNode(L"response")){
		delete lpServerInfo;
		return;
	}
	std::wstring server=lpServerInfo->GetNode(L"response")->GetNode(L"server")->NodeAsString();
	std::wstring key=lpServerInfo->GetNode(L"response")->GetNode(L"key")->NodeAsString();
	std::wstring ts=lpServerInfo->GetNode(L"response")->GetNode(L"ts")->NodeAsString();
	std::wstring host=server.substr(0, server.find_first_of(L'/'));

	clDlg->SendMessage(WM_HIDE_LOADING);
	SetTrayIconOnline(true);

	char* _host=CUtils::wtoc(host.c_str(), host.length()+1);
	delete lpServerInfo;
	while(true){
		std::wstring path=server.substr(server.find_first_of(L'/'));
		path+=L"?act=a_check&wait=25&ts=";
		path+=ts;
		path+=L"&key=";
		path+=key;
		path+=L"&rand=";
		wchar_t cb[10];
		wsprintf(cb, L"%i", rand());
		path+=cb;
		char* _path=CUtils::wtoc(path.c_str(), path.length()+1);
		int len;
		//TRACE("Sending.\n");
		char* lpRespUtf=SendHTTP(_host, 80, _path, NULL, &len);

		if(needExit){
			TRACE("Exiting long poll thread.\n");
			path.clear();
			server.clear();
			delete _host;
//			delete lpReq;
			delete _path;
			if(lpRespUtf)delete lpRespUtf;
			return;
		}

		if(!lpRespUtf){
			TRACE("NULL response; sending again.\n");
			delete _path;
			continue;
		}

		time_t rt=time(NULL);
		//TRACE("Delay=%d\n", rt-lastLongPollResp);
		if(rt-lastLongPollResp>120){
			TRACE("Long poll timeout, reloading all\n");
			path.clear();
			server.clear();
			delete _host;
			delete _path;
			if(lpRespUtf)delete lpRespUtf;
			SetTrayIconOnline(false);
			lastLongPollResp=time(NULL);
			_sleep(1000);
			return;
		}

		wchar_t* lpResp=CUtils::Utf8ToWchar(lpRespUtf);

		JSONNode* lr=libJSON::Parse(lpResp);
		TRACE("%s\n", lpRespUtf);
		
		if(!lr){
			/*std::wstring errmsg=L"Error parsing long poll response!!!\n";
			errmsg+=lpResp;
			MessageBox(NULL, errmsg.c_str(), NULL, MB_ICONEXCLAMATION);*/
			
			_sleep(10000);
		}else{
			if(!lr->GetNode(L"ts")){
				TRACE("Long poll error.\n");
			}
		if(lr->NodeAt(0)->NodeName().compare(L"ts")==0){
			lastLongPollResp=rt;
		ts=lr->GetNode(L"ts")->NodeAsString();
		if(lr->GetNode(L"updates")->NodeSize()==0)TRACE("No updates\n");
		for(int i=0;i<lr->GetNode(L"updates")->NodeSize();i++){
			int updType=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(0)->NodeAsInt();
			if(updType==0){
				TRACE("Msg deleted\n");
			}
			if(updType==1){
				int flags=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(2)->NodeAsInt();
				int mid=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(1)->NodeAsInt();
				TRACE("MSG %i replace flags -> %i\n", mid, flags);
			}
			if(updType==2){
				int flags=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(2)->NodeAsInt();
				int mid=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(1)->NodeAsInt();
				TRACE("MSG %i add flags %i\n", mid, flags);
			}
			if(updType==3){
				TRACE("MSG clear flags\n");
				int flags=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(2)->NodeAsInt();
				int mid=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(1)->NodeAsInt();
				if(flags & MSG_UNREAD && talks->isCreated){
					TRACE("is unread\n");
					for(int j=0;j<talks->tabs.GetItemCount();j++){
						TC_ITEM tci;
						tci.mask=TCIF_PARAM;
						talks->tabs.GetItem(j, &tci);
						CTalkTab* item=(CTalkTab*)tci.lParam;
						TRACE("get tab\n");
						bool found=false;
						if(item){
							TRACE("tab exists\n");
							for(int k=0;k<item->sentIDs.size();k++){
								TRACE("%i==%i\n", item->sentIDs[k], mid);
								if(item->sentIDs[k]==mid){
									TRACE("found\n");
									item->SendMessage(WM_SET_READ, mid);
									TRACE("sent msg\n");
									item->sentIDs.erase(item->sentIDs.begin()+k);
									TRACE("erased\n");
									found=true;
									break;
								}
							}
						}
						if(found){
							TRACE("FOUND\n");
							break;
						}
					}
				}
			}
			if(updType==4){
				std::wstring msg=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(6)->NodeAsString();
				int sender=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(3)->NodeAsInt();
				int flags=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(2)->NodeAsInt();
				int mid=lr->GetNode(L"updates")->NodeAt(i)->NodeAt(1)->NodeAsInt();
				//TRACE("MSG: '%ws' from %i flags %i\n", msg.c_str(), sender, lr->GetNode(L"updates")->NodeAt(i)->NodeAt(2)->NodeAsInt());
				if(!(flags & MSG_OUTBOX) && (flags & MSG_UNREAD)){
					ProcessIncomingMessage(sender, 0, mid, msg);
				}
			}
			if(updType==8){
				int onl_uid=-lr->GetNode(L"updates")->NodeAt(i)->NodeAt(1)->NodeAsInt();
				TRACE("%i now ON\n", onl_uid);
				clDlg->friendList.SetOnlineStatus(onl_uid, true);
				int idx;
				if((idx=clDlg->friendList.GetItemByUID(onl_uid))!=-1){
					ListEntry item=clDlg->friendList.items[idx];
					if(AllowNotification(&item)){
					if(CSettings::popupOnline){
						NotificationStruct ns;
						ns.content=CLang::Get(L"online");
						ns.title=(wchar_t*)item.userName.c_str();
						ns.uid=onl_uid;
						clDlg->SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
					}
					if(CSettings::soundOnline && CSettings::globalSoundOn){
						PlaySound(CSettings::soundOnlineFile, NULL, SND_ASYNC | SND_FILENAME);
					}
					}
				}
			}
			if(updType==9){
				int off_uid=-lr->GetNode(L"updates")->NodeAt(i)->NodeAt(1)->NodeAsInt();
				clDlg->friendList.SetOnlineStatus(off_uid, false);
				int idx;
				if((idx=clDlg->friendList.GetItemByUID(off_uid))!=-1){
					ListEntry item=clDlg->friendList.items[idx];
					if(AllowNotification(&item)){
					if(CSettings::popupOffline){
						NotificationStruct ns;
						ns.content=CLang::Get(L"offline");
						ns.title=(wchar_t*)item.userName.c_str();
						ns.uid=off_uid;
						clDlg->SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
					}
					if(CSettings::soundOffline && CSettings::globalSoundOn){
						PlaySound(CSettings::soundOfflineFile, NULL, SND_ASYNC | SND_FILENAME);
					}
					}
				}


			}

		}
		}else{
			TRACE("No TS\n");
			break;
		}
		}
		
		delete lr;
//		delete lpReq;
		delete lpRespUtf;
		delete lpResp;
		delete _path;
	}
	delete _host;
	TRACE("exited long poll!\n");
}


bool CConnector::GetProfileByDomain(wchar_t* domain, ListEntry* result)
{
	CVkAPIRequest req(L"getProfiles");
	req.AddParameter(L"domains", domain);
	req.AddParameter(L"fields", L"photo_rec,photo_medium_rec");
	JSONNode* _resp=SendAPIRequest(&req);
	JSONNode* resp=_resp->GetNode(L"response");
	if(resp->NodeSize()==0){
		delete _resp;
		return false;
	}
	result->uid=_wtoi(resp->NodeAt(0)->GetNode(L"uid")->NodeAsString().c_str());
	result->userName=new wchar_t[resp->NodeAt(0)->GetNode(L"first_name")->NodeAsString().length()+
		resp->NodeAt(0)->GetNode(L"last_name")->NodeAsString().length()+1];
	//swprintf(result->userName, L"%s %s", resp->NodeAt(0)->GetNode(L"first_name")->NodeAsString().c_str(),
	//	resp->NodeAt(0)->GetNode(L"last_name")->NodeAsString().c_str());
	result->userName=resp->NodeAt(0)->GetNode(L"first_name")->NodeAsString()+L" "+resp->NodeAt(0)->GetNode(L"last_name")->NodeAsString();
	result->userBigPhotoURL=resp->NodeAt(0)->GetNode(L"photo_medium_rec")->NodeAsString();
	delete _resp;
	return true;
}

bool CConnector::AllowNotification(ListEntry* item)
{
	if(CSettings::notifyOnOff==1){
	
					for(int j=0;j<CSettings::notifyForLists.size();j++){
						for(int k=0;k<item->groups.size();k++){
							//TRACE("%i; %i\n", item->groups[k], CSettings::notifyForLists[j]);
							if(item->groups[k]-1==CSettings::notifyForLists[j]){
								return true;
							}
							if(item->groups[k]==CSettings::notifyForLists[j] && CSettings::notifyForLists[j]==-1){
								return true;
							}
						}
					}
	}
	if(CSettings::notifyOnOff==0)return true;
	return false;
}



UINT CConnector::DoCheckOfflineMsgs(void* param){
	__try{
		EnterCriticalSection(&con.cs3);
	if(CSettings::checkOfflineMsgs){
		CVkAPIRequest req(L"messages.get");
		req.AddParameter(L"filters", L"1");
		req.AddParameter(L"preview_length", L"0");
		req.AddParameter(L"count", L"100");
		JSONNode* resp=con.SendAPIRequest(&req);
		if(!resp){
			return 0;
		}
		JSONNode* r=resp->GetNode(L"response");
		if(!r){
			delete resp;
			return 0;
		}
		for(int i=1;i<r->NodeSize();i++){
			con.ProcessIncomingMessage(r->NodeAt(i)->GetNode(L"uid")->NodeAsInt(),
									r->NodeAt(i)->GetNode(L"date")->NodeAsInt(),
									r->NodeAt(i)->GetNode(L"mid")->NodeAsInt(),
									r->NodeAt(i)->GetNode(L"body")->NodeAsString());
		}
		delete resp;
	}
	LeaveCriticalSection(&con.cs3);
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"OfflineMsg")){}
	return 0;
}

void CConnector::ProcessIncomingMessage(int sender, int time, int mid, std::wstring msg)
{
	if(talks->openedTabs[sender]){
		talks->openedTabs[sender]->unreadIDs.push_back(mid);
		int* md=new int[2];
		md[0]=mid;
		md[1]=time;
		talks->openedTabs[sender]->SendMessage(WM_ADD_MESSAGE, (WPARAM)msg.c_str(), (LPARAM)md);
		delete md;
		if(CSettings::flashMessage && GetActiveWindow()!=talks->m_hWnd){
			talks->FlashWindow(false);
		}
		if(CSettings::popupMessage){
			NotificationStruct ns;
			ns.content=(wchar_t*)msg.c_str();
			ns.title=(wchar_t*)talks->openedTabs[sender]->user_name.c_str();
			ns.uid=sender;
			clDlg->SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		}
	}else{
		ListEntry senderInfo;
		GetProfileByID(sender, &senderInfo);
		if(!talks->isCreated){
			clDlg->SendMessage(WM_OPENTAB);
		}
		talks->SendMessage(WM_OPENTAB, 0, (LPARAM)&senderInfo);
		talks->openedTabs[sender]->unreadIDs.push_back(mid);
		if(!talks->IsWindowVisible()){
			talks->ShowWindow(SW_SHOWMINNOACTIVE);
		}
		while(!talks->openedTabs[sender])_sleep(100);
		while(!talks->openedTabs[sender]->msgBoard.ready || (CSettings::loadHistory && !talks->openedTabs[sender]->historyLoaded))_sleep(100);
		int* md=new int[2];
		md[0]=mid;
		md[1]=time;
		talks->openedTabs[sender]->SendMessage(WM_ADD_MESSAGE, (WPARAM)msg.c_str(), (LPARAM)md);
		delete md;
		if(CSettings::flashMessage && GetActiveWindow()!=talks->m_hWnd){
			talks->FlashWindow(false);
		}
		if(CSettings::popupMessage && (GetActiveWindow()!=talks->m_hWnd || !talks->openedTabs[sender]->IsWindowVisible())){
			NotificationStruct ns;
			ns.content=(wchar_t*)msg.c_str();
			ns.title=(wchar_t*)senderInfo.userName.c_str();
			ns.uid=sender;
			clDlg->SendMessage(WM_SHOW_NOTIFICATION, (WPARAM)&ns);
		}
	}
	if(CSettings::soundMessage && CSettings::globalSoundOn)PlaySound(CSettings::soundMessageFile, NULL, SND_ASYNC | SND_FILENAME);
}


void CConnector::DeleteCookies(void)
{
	wchar_t* wb1=new wchar_t[1024];
	DWORD bs1=1024;
	InternetGetCookie(L"http://login.vk.com/", NULL, wb1, &bs1);
	wchar_t* p;
	p=wcstok(wb1, L";");
	while(p!=NULL){
		if(p[0]==L' ')p++;
		InternetSetCookie(L"http://login.vk.com/", p, L"");
		p=wcstok(NULL, L";");
	}
	delete wb1;

	wchar_t* wb2=new wchar_t[1024];
	DWORD bs2=1024;
	InternetGetCookie(L"http://vkontakte.ru/", NULL, wb2, &bs2);
	p=wcstok(wb2, L";");
	while(p!=NULL){
		if(p[0]==L' ')p++;
		InternetSetCookie(L"http://vkontakte.ru/", p, L"");
		p=wcstok(NULL, L";");
	}
	delete wb2;
}


UINT CConnector::DoLoadUserInfo(void* param){
	__try{
		
		CUserInfoTip* it=(CUserInfoTip*)param;
		CVkAPIRequest req(L"execute");
		wstring ec=L"var u=";
		wchar_t* ub=new wchar_t[20];
		_itow(con.clDlg->friendList.items[it->item].uid, ub, 10);
		ec+=ub;
		delete ub;
		ec+=L";var n=API.getProfiles({uids:u,name_case:\"gen\",fields:\"photo_medium,city,education,bdate,city\"});var a=API.activity.get({uid:u});return{n:n[0].first_name,p:n[0].photo_medium,a:a.activity,u:n[0].university_name+\"'\"+n[0].graduation[2]+n[0].graduation[3],b:n[0].bdate,c:API.getCities({cids:n[0].city})[0].name};";
		req.AddParameter(L"code", (wchar_t*)ec.c_str());
		JSONNode* resp=con.SendAPIRequest(&req);
		if(!resp){
			return 0;
		}
		JSONNode* r=resp->GetNode(L"response");
		if(!r){
			delete resp;
			return 0;
		}
		wchar_t* ifn=con.DownloadImage((wchar_t*)r->GetNode(L"p")->NodeAsString().c_str());
		HBITMAP ava=CEnBitmap::LoadImageFile(ifn);
		delete ifn;

		EnterCriticalSection(&con.cs);
		if(!con.clDlg->friendList.userTip){
			delete resp;
			LeaveCriticalSection(&con.cs);
			return 0;
		}
		
		it->ava=ava;
		if(r->GetNode(L"b"))it->bdate=r->GetNode(L"b")->NodeAsString();
		it->unv=r->GetNode(L"u")->NodeAsString();
		wstring act=r->GetNode(L"a")->NodeAsString();
		int al=act.length();
		int nPos;
		while((nPos=act.find(L"<br>"))!=-1){
			act.replace(nPos, 4, L"\n");
			al-=3;
		}
		while((nPos=act.find(L"&lt;"))!=-1){
			act.replace(nPos, 4, L"<");
			al-=3;
		}
		while((nPos=act.find(L"&gt;"))!=-1){
			act.replace(nPos, 4, L">");
			al-=3;
		}
		while((nPos=act.find(L"&quot;"))!=-1){
			act.replace(nPos, 6, L"\"");
			al-=5;
		}
		while((nPos=act.find(L"&amp;"))!=-1){
			act.replace(nPos, 5, L"&");
			al-=4;
		}
		act=act.substr(0, al);
		it->act=act;
		it->city=r->GetNode(L"c")->NodeAsString();
		it->DataLoaded();
		delete resp;
		LeaveCriticalSection(&con.cs);
	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"LoadUserInfo")){}
	return 0;
}



void CConnector::LoadUserInfo(CUserInfoTip* it)
{
	AfxBeginThread((AFX_THREADPROC)&DoLoadUserInfo, it); 
}

wstring GetMimeTypeByExtension(wstring x){
	for(int i=0;i<x.length();i++)x[i]=tolower(x[i]);
	if(x==L"jpg" || x==L"jpeg")return L"image/jpeg";
	if(x==L"gif")return L"image/gif";
	if(x==L"png")return L"image/png";
	if(x==L"mp3")return L"audio/mpeg";
	return L"application/octet-stream";
}

void my_callback(int sent, int total){
	//TRACE("%i / %i [%.1f%%]\n", sent, total, ((float)(sent)/(float)(total))*100);
	float prc=((float)(sent)/(float)(total))*100;
	wchar_t buf[50];
	swprintf(buf, L"%s, %.1f%%...", CLang::Get(L"uploading"), prc);
	con.clDlg->newStatusText=buf;//userStatusLabel.SetWindowTextW(buf);
	/*RECT rect1;
	con.clDlg->userStatusLabel.GetClientRect(&rect1);
	con.clDlg->userStatusLabel.ClientToScreen(&rect1);
	con.clDlg->ScreenToClient(&rect1);
	con.clDlg->InvalidateRect(&rect1, TRUE);*/
}

char* CConnector::UploadFile(wstring fname, wstring _url, wstring field)
{
	wstring uploadReq=L"\r\n--VK-IM-FILE-UPLOAD-BOUNDARY\r\nContent-Disposition: form-data; name=\""+field+L"\"; filename=\"";
				uploadReq+=fname.substr(fname.find_last_of(L"\\/")+1);
				uploadReq+=L"\"\r\nContent-Type: ";
				uploadReq+=GetMimeTypeByExtension(fname.substr(fname.find_last_of(L".")+1));
				uploadReq+=L"\r\n\r\n";
				char* reqUTF=CUtils::WcharToUtf8((wchar_t*)uploadReq.c_str());
				FILE* f=_wfopen(fname.c_str(), L"rb");
				fseek(f, 0, SEEK_END);
				int rlen=strlen(reqUTF)+ftell(f)+wcslen(L"\r\n--VK-IM-FILE-UPLOAD-BOUNDARY--\r\n");
				rewind(f);
				char* req=new char[rlen];
				memcpy(req, reqUTF, strlen(reqUTF));
				int foff=0;
				while(!feof(f)){
					char buf[1024];
					int rb=fread(buf, 1, 1024, f);
					memcpy(req+strlen(reqUTF)+foff, buf, rb);
					foff+=rb;
				}
				char* bb="\r\n--VK-IM-FILE-UPLOAD-BOUNDARY--\r\n";
				memcpy(req+strlen(reqUTF)+ftell(f), bb, strlen(bb));
				char* url=CUtils::WcharToUtf8((wchar_t*)_url.c_str());
				URL_COMPONENTSA uc;
				memset(&uc, 0, sizeof(uc));
				uc.dwStructSize=sizeof(URL_COMPONENTSA);
				uc.dwHostNameLength=512;
				uc.lpszHostName=new char[512];
				uc.dwUrlPathLength=512;
				uc.lpszUrlPath=new char[512];
				InternetCrackUrlA(url, strlen(url), 0, &uc);
				char* result=con.DoBigPost(uc.lpszHostName, uc.lpszUrlPath, req, rlen, &my_callback);//con.SendHTTP(uc.lpszHostName, 80, uc.lpszUrlPath, req, 0, true, rlen);
				delete reqUTF;
				delete url;
				delete req;
				delete uc.lpszHostName;
				delete uc.lpszUrlPath;
				return result;
}


char* CConnector::DoBigPost(char* host, char* path, char* data, int len, void (__cdecl *callback)(int,int))
{
	HINTERNET hInet=InternetOpen(L"Vk.IM", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hInet){
		ShowLastError();
		return NULL;
	}
	HINTERNET hSession=InternetConnectA(hInet, host, 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if(!hSession){
		InternetCloseHandle(hInet);
		ShowLastError();
		return NULL;
	}
	HINTERNET hRequest=HttpOpenRequestA(hSession, "POST", path, NULL, NULL, NULL, 0, 0);
	if(!hRequest){
		InternetCloseHandle(hInet);
		InternetCloseHandle(hSession);
		ShowLastError();
		return NULL;
	}
	char* header="Content-Type: multipart/form-data; boundary=\"VK-IM-FILE-UPLOAD-BOUNDARY\"";
	INTERNET_BUFFERSA BufferIn;
	BYTE pBuffer[1024];
	BOOL bRet;
	memset(&BufferIn, 0, sizeof(INTERNET_BUFFERS));
	BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS );
    BufferIn.dwBufferTotal = len;
	BufferIn.lpcszHeader=header;
	BufferIn.dwHeadersLength=strlen(header);
	bool result=HttpSendRequestExA( hRequest, &BufferIn, NULL, 0, 0);
	//bool result=HttpSendRequestA(hRequest, header, header?strlen(header):0, post_data, post_data?(post_len==-1?strlen(post_data):post_len):0);
	if(!result){
		InternetCloseHandle(hInet);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hRequest);
		ShowLastError();
		return NULL;
	}
	int numSent=0;
	int n=0;

	while(numSent<len){
		DWORD nw;
		result=InternetWriteFile(hRequest, data+numSent, (len-numSent>=1024)?1024:(len-numSent), &nw);
		if(!result)break;
		numSent+=nw;
		n++;
		if(callback && n%10==0)callback(numSent, len);
	}
	if(!result){
		InternetCloseHandle(hInet);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hRequest);
		ShowLastError();
		return NULL;
	}
	TRACE("UPLOAD COMPLETE\n");

	result=HttpEndRequest(hRequest, NULL, 0, 0);
	if(!result){
		InternetCloseHandle(hInet);
		InternetCloseHandle(hSession);
		InternetCloseHandle(hRequest);
		ShowLastError();
		return NULL;
	}

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
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInet);
	rbuf=(char*)realloc(rbuf, rsize+1);
	rbuf[rsize]=0;
	if(connectionError){	
		SetTrayIconOnline(true);
		connectionError=false;
	}
	
	return rbuf;
}


UINT CConnector::SendOnline(void* a)
{
	while(true){
		CVkAPIRequest req(L"activity.online");
		JSONNode* n=con.SendAPIRequest(&req);
		if(n)delete n;
		_sleep(180000); // 3 min
	}
	return 0;
}
