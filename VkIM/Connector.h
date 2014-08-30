#pragma once

#include "VkIMDlg.h"
#include "JSON_Defs.h"
#include "jsonmain.h"
#include "VkAPIRequest.h"
#include "TalkTab.h"
#include "VkIMDlg.h"
#include "StatusDlg.h"
#include <WinInet.h>
#include <string>


#define ATT_PHOTO 1
#define ATT_VIDEO 2
#define ATT_AUDIO 3
// CConnector command target

typedef struct
{
	wchar_t* text;
	int rcpt;
	int tmp_id;
}message;

typedef struct
{
	int from;
	wchar_t* text;
	time_t time;
}HistoryEntry;


class CConnector
{
public:
	CConnector();
	virtual ~CConnector();
	char* SendHTTP(char* host, unsigned int port, char* path, char* post_data, int* len=NULL, bool multipart=false, int post_len=-1);
	static UINT _LoadContactListThread(CConnector* con);
	void LoadContactListThread();
	void LoadContactList(CVkIMDlg* dlg);
	CVkIMDlg* clDlg;
	void SaveSessionData();
	JSONNode* SendAPIRequest(CVkAPIRequest* req);
	//char* CreateHTTPRequest(char* host, char* path, char* method, char* contentType=NULL,	char* content=NULL);
	wchar_t* DownloadImage(wchar_t* url);
	//char* SendHTTPRaw(char* host, unsigned int port, char* req, int* _len);
	int SendMsg(wchar_t* msg, int rcpt);
	static UINT _SendMsgThread(message* msg);
	void SendMsgThread(message* msg);
	void MarkAsRead(wchar_t* ids);
	static UINT _MarkAsReadThread(wchar_t* ids);
	void SetStatus(StatusUpdateReq* sr);
	static UINT _SetStatusThread(StatusUpdateReq* sr);
	void DeleteSessionData();

	wchar_t* my_name;
	bool connectionError;
	time_t lastLongPollResp;
private:
	
public:
	void ShowLastError(void);
	bool GetProfileByID(int uid, ListEntry* result);
	static UINT _GetCountersThread(CConnector* con);
	void GetCounters(void);
	void GetHistory(CTalkTab* tab);
	static UINT _GetHistoryThread(CTalkTab* tab);
	int authTries;
	void LoadFriendPhotos(void);
	static UINT DoLoadFriendPhotos(void* param);
	static UINT DoCheckUpdates(void* param);
	static UINT DoCheckOfflineMsgs(void* param);
	bool canExit;
	bool canExit2;
	bool needExit;
	HINTERNET httpSck;
	bool authorizing;
	void SetTrayIconOnline(bool state);
	void LoadFriends(void);
	void LongPoll(void);
	bool loadingPhotos;
	bool GetProfileByDomain(wchar_t* domain, ListEntry* result);
	bool AllowNotification(ListEntry* item);
	int tmp_msg_id;
	void ProcessIncomingMessage(int sender, int time, int mid, std::wstring msg);
	void DeleteCookies(void);
	static UINT CConnector::DoLoadUserInfo(void* param);
	void LoadUserInfo(CUserInfoTip* it);
	CRITICAL_SECTION cs;
	CRITICAL_SECTION cs2;
	CRITICAL_SECTION cs3;
	CRITICAL_SECTION tipCs;
	CRITICAL_SECTION lpCs;
	bool iconOnline;
	char* UploadFile(wstring fname, wstring _url, wstring field);
	char* DoBigPost(char* host, char* path, char* data, int len, void (__cdecl *callback)(int,int));
	static UINT SendOnline(void* a);
};


