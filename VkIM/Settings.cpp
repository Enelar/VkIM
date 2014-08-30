#include "Settings.h"
#include "Utils.h"

// Значения для настроек по умолчению.

wchar_t CSettings::langFile[MAXLEN]=L"ru.txt";
wchar_t CSettings::skinFolder[MAXLEN]=L"VK Style";
wchar_t* CSettings::settings_filename;
bool CSettings::soundOnline=false;
bool CSettings::popupOnline=true;
wchar_t CSettings::soundOnlineFile[MAXLEN]=L"Sounds\\user_status.wav";
bool CSettings::soundOffline=false;
bool CSettings::popupOffline=false;
wchar_t CSettings::soundOfflineFile[MAXLEN]=L"Sounds\\user_status.wav";
bool CSettings::soundMessage=true;
bool CSettings::popupMessage=true;
bool CSettings::flashMessage=true;
bool CSettings::globalSoundOn=true;
bool CSettings::loadHistory=true;
bool CSettings::fromChat=true;
bool CSettings::flPopup=true;
int CSettings::numHistory=10;
wchar_t CSettings::soundMessageFile[MAXLEN]=L"Sounds\\incoming_message.wav";
wchar_t CSettings::soundSendFile[MAXLEN]=L"Sounds\\outgoing_message.wav";
bool CSettings::soundSend=true;
std::vector<int> CSettings::notifyForLists;
bool CSettings::useProxy=false;
wchar_t* CSettings::proxyServer=L"";
wchar_t* CSettings::proxyUser=L"";
wchar_t* CSettings::proxyPass=L"";
int CSettings::proxyPort=0;
wchar_t CSettings::apiServer[MAXLEN]=L"api.vkontakte.ru";
wchar_t CSettings::apiReqPath[MAXLEN]=L"/api.php";
int CSettings::apiPort=80;
int CSettings::listMode=1;
bool CSettings::showLists=false;
int CSettings::sendWithCtrlEnter=0;
wchar_t CSettings::soundEventFile[MAXLEN]=L"Sounds\\vk_event.wav";
bool CSettings::soundEvent=true;
bool CSettings::popupEvent=true;
bool CSettings::enableEvents=true;
std::vector<bool> CSettings::notifyForEvents;
int CSettings::eventsRefresh=30000;
bool CSettings::showOffline=true;
bool CSettings::checkUpdates=true;
bool CSettings::checkOfflineMsgs=false;
int CSettings::listX=0;
int CSettings::listY=0;
int CSettings::listW=0;
int CSettings::listH=0;
int CSettings::talkX=0;
int CSettings::talkY=0;
int CSettings::talkW=0;
int CSettings::talkH=0;
int CSettings::notifyOnOff=0;

bool CSettings::needFree=false;

CSettings::CSettings(wchar_t* filename)
{
}


CSettings::~CSettings(void)
{
}


void CSettings::Load(wchar_t* filename)
{
	for(int i=0;i<10;i++)notifyForEvents.push_back(true);
	settings_filename=filename;
	FILE* f=_wfopen(filename, L"r");
	if(!f)return;
	fseek(f,0,SEEK_END);
	int size = ftell(f);
	rewind(f);
	wchar_t* fdata=new wchar_t[size/2+1];
	memset(fdata, 0, size/2+1);
	char* rr=(char*)fdata;
	while(!feof(f)){
		fread(rr, 1, 1, f);
		rr++;
	}
	//fread(fdata, 2, size/2, f);
	fclose(f);
	JSONNode* n=libJSON::Parse(fdata);
	delete fdata;
	if(n){
		if(n->GetNode(L"lang")){
		wcscpy(langFile, n->GetNode(L"lang")->NodeAsString().c_str());
		}
		if(n->GetNode(L"skin")){
		wcscpy(skinFolder, n->GetNode(L"skin")->NodeAsString().c_str());
		}

		if(n->GetNode(L"sound_online"))soundOnline=n->GetNode(L"sound_online")->NodeAsBool();
		if(n->GetNode(L"sound_offline"))soundOffline=n->GetNode(L"sound_offline")->NodeAsBool();
		if(n->GetNode(L"sound_message"))soundMessage=n->GetNode(L"sound_message")->NodeAsBool();
		if(n->GetNode(L"sound_event"))soundEvent=n->GetNode(L"sound_event")->NodeAsBool();
		if(n->GetNode(L"sound_send"))soundSend=n->GetNode(L"sound_send")->NodeAsBool();
		if(n->GetNode(L"popup_online"))popupOnline=n->GetNode(L"popup_online")->NodeAsBool();
		if(n->GetNode(L"popup_offline"))popupOffline=n->GetNode(L"popup_offline")->NodeAsBool();
		if(n->GetNode(L"popup_message"))popupMessage=n->GetNode(L"popup_message")->NodeAsBool();
		if(n->GetNode(L"flash_message"))flashMessage=n->GetNode(L"flash_message")->NodeAsBool();
		if(n->GetNode(L"popup_event"))popupEvent=n->GetNode(L"popup_event")->NodeAsBool();
		if(n->GetNode(L"enable_events"))enableEvents=n->GetNode(L"enable_events")->NodeAsBool();
		if(n->GetNode(L"global_sound_on"))globalSoundOn=n->GetNode(L"global_sound_on")->NodeAsBool();
		if(n->GetNode(L"from_chat"))fromChat=n->GetNode(L"from_chat")->NodeAsBool();
		if(n->GetNode(L"load_history"))loadHistory=n->GetNode(L"load_history")->NodeAsBool();
		if(n->GetNode(L"fl_popup"))flPopup=n->GetNode(L"fl_popup")->NodeAsBool();
		if(n->GetNode(L"num_history"))numHistory=n->GetNode(L"num_history")->NodeAsInt();
		if(n->GetNode(L"notify_on_off"))notifyOnOff=n->GetNode(L"notify_on_off")->NodeAsInt();
		if(n->GetNode(L"ctrl_enter"))sendWithCtrlEnter=n->GetNode(L"ctrl_enter")->NodeAsInt();
		if(n->GetNode(L"events_refresh"))eventsRefresh=n->GetNode(L"events_refresh")->NodeAsInt();

		if(n->GetNode(L"list_x"))listX=n->GetNode(L"list_x")->NodeAsInt();
		if(n->GetNode(L"list_y"))listY=n->GetNode(L"list_y")->NodeAsInt();
		if(n->GetNode(L"list_w"))listW=n->GetNode(L"list_w")->NodeAsInt();
		if(n->GetNode(L"list_h"))listH=n->GetNode(L"list_h")->NodeAsInt();

		if(n->GetNode(L"talk_x"))talkX=n->GetNode(L"talk_x")->NodeAsInt();
		if(n->GetNode(L"talk_y"))talkY=n->GetNode(L"talk_y")->NodeAsInt();
		if(n->GetNode(L"talk_w"))talkW=n->GetNode(L"talk_w")->NodeAsInt();
		if(n->GetNode(L"talk_h"))talkH=n->GetNode(L"talk_h")->NodeAsInt();

		if(n->GetNode(L"show_offline"))showOffline=n->GetNode(L"show_offline")->NodeAsBool();
		if(n->GetNode(L"check_updates"))checkUpdates=n->GetNode(L"check_updates")->NodeAsBool();
		if(n->GetNode(L"check_offline_msgs"))checkOfflineMsgs=n->GetNode(L"check_offline_msgs")->NodeAsBool();

		if(n->GetNode(L"use_proxy"))useProxy=n->GetNode(L"use_proxy")->NodeAsBool();
		if(n->GetNode(L"show_lists"))showLists=n->GetNode(L"show_lists")->NodeAsBool();
		if(n->GetNode(L"list_mode"))listMode=n->GetNode(L"list_mode")->NodeAsInt();
		//if(n->GetNode(L"proxy_port"))proxyPort=n->GetNode(L"proxy_port")->NodeAsInt();
		if(n->GetNode(L"api_port"))apiPort=n->GetNode(L"api_port")->NodeAsInt();
		/*if(n->GetNode(L"api_port")){
		proxyServer=new wchar_t[n->GetNode(L"proxy_server")->NodeAsString().length()+1];
		wcscpy(proxyServer, n->GetNode(L"proxy_server")->NodeAsString().c_str());
		}
		if(n->GetNode(L"proxy_user")){
		proxyUser=new wchar_t[n->GetNode(L"proxy_user")->NodeAsString().length()+1];
		wcscpy(proxyUser, n->GetNode(L"proxy_user")->NodeAsString().c_str());
		}
		if(n->GetNode(L"proxy_pass")){
		proxyPass=new wchar_t[n->GetNode(L"proxy_pass")->NodeAsString().length()+1];
		wcscpy(proxyPass, n->GetNode(L"proxy_pass")->NodeAsString().c_str());
		}*/
		if(n->GetNode(L"api_server")){
		wcscpy(apiServer, n->GetNode(L"api_server")->NodeAsString().c_str());
		}
		if(n->GetNode(L"api_req_path")){
		wcscpy(apiReqPath, n->GetNode(L"api_req_path")->NodeAsString().c_str());
		}
		if(n->GetNode(L"sound_online_file")){
			std::wstring tmp=n->GetNode(L"sound_online_file")->NodeAsString();
			CUtils::ReplaceAll(&tmp, L"|", L"\\");
		wcscpy(soundOnlineFile, tmp.c_str());
		}
		if(n->GetNode(L"sound_offline_file")){
			std::wstring tmp=n->GetNode(L"sound_offline_file")->NodeAsString();
			CUtils::ReplaceAll(&tmp, L"|", L"\\");
		wcscpy(soundOfflineFile, tmp.c_str());
		}
		if(n->GetNode(L"sound_message_file")){
			std::wstring tmp=n->GetNode(L"sound_message_file")->NodeAsString();
			CUtils::ReplaceAll(&tmp, L"|", L"\\");
		wcscpy(soundMessageFile, tmp.c_str());
		}
		if(n->GetNode(L"sound_event_file")){
			std::wstring tmp=n->GetNode(L"sound_event_file")->NodeAsString();
			CUtils::ReplaceAll(&tmp, L"|", L"\\");
		wcscpy(soundEventFile, tmp.c_str());
		}
		if(n->GetNode(L"sound_send_file")){
			std::wstring tmp=n->GetNode(L"sound_send_file")->NodeAsString();
			CUtils::ReplaceAll(&tmp, L"|", L"\\");
		wcscpy(soundSendFile, tmp.c_str());
		}
		if(n->GetNode(L"notify_for_lists")){
		for(int i=0;i<n->GetNode(L"notify_for_lists")->NodeSize();i++){
			notifyForLists.push_back(n->GetNode(L"notify_for_lists")->NodeAt(i)->NodeAsInt()+1);
		}
		}

		if(n->GetNode(L"notify_for_events")){
			notifyForEvents.clear();
		for(int i=0;i<n->GetNode(L"notify_for_events")->NodeSize();i++){
			notifyForEvents.push_back(n->GetNode(L"notify_for_events")->NodeAt(i)->NodeAsBool());
		}
		}

		delete n;
	}
}


#define AddStrParam(a,b) ss+=L"\"";ss+=a;ss+=L"\":\"";ss+=b;ss+=L"\",\r\n";
#define AddParam(a,b) ss+=L"\"";ss+=a;ss+=L"\":";ss+=b;ss+=L",\r\n";


void CSettings::Save()
{
	try{

		std::wstring ss=L"{";
		ss+=L"\r\n//\r\n// Vk.IM Settings File\r\n// Do not edit\r\n//\r\n\r\n";

		AddStrParam(L"lang", langFile);

	AddStrParam(L"skin", skinFolder);

	AddParam(L"sound_online", soundOnline?L"true":L"false");
	AddParam(L"popup_online", popupOnline?L"true":L"false");

	AddParam(L"sound_offline", soundOffline?L"true":L"false");
	AddParam(L"popup_offline", popupOffline?L"true":L"false");

	AddParam(L"sound_message", soundMessage?L"true":L"false");
	AddParam(L"popup_message", popupMessage?L"true":L"false");
	AddParam(L"flash_message", flashMessage?L"true":L"false");

	AddParam(L"sound_event", soundEvent?L"true":L"false");
	AddParam(L"popup_event", popupEvent?L"true":L"false");
	AddParam(L"enable_events", enableEvents?L"true":L"false");

	AddParam(L"sound_send", soundSend?L"true":L"false");
	AddParam(L"global_sound_on", globalSoundOn?L"true":L"false");
	AddParam(L"check_updates", checkUpdates?L"true":L"false");
	AddParam(L"check_offline_msgs", checkOfflineMsgs?L"true":L"false");

	AddParam(L"from_chat", fromChat?L"true":L"false");
	AddParam(L"load_history", loadHistory?L"true":L"false");
	AddParam(L"fl_popup", flPopup?L"true":L"false");
	AddParam(L"ctrl_enter", sendWithCtrlEnter?L"1":L"0");
	wchar_t s[10];
	_itow(numHistory, s, 10);
	AddParam(L"num_history", s);

	AddParam(L"show_offline", showOffline?L"true":L"false");

	std::wstring _soundOnlineFile=soundOnlineFile;
	CUtils::ReplaceAll(&_soundOnlineFile, L"\\", L"|");
	std::wstring _soundOfflineFile=soundOfflineFile;
	CUtils::ReplaceAll(&_soundOfflineFile, L"\\", L"|");
	std::wstring _soundMessageFile=soundMessageFile;
	CUtils::ReplaceAll(&_soundMessageFile, L"\\", L"|");
	std::wstring _soundEventFile=soundEventFile;
	CUtils::ReplaceAll(&_soundEventFile, L"\\", L"|");
	std::wstring _soundSendFile=soundSendFile;
	CUtils::ReplaceAll(&_soundSendFile, L"\\", L"|");
	AddStrParam(L"sound_online_file", _soundOnlineFile);
	AddStrParam(L"sound_offline_file", _soundOfflineFile);
	AddStrParam(L"sound_message_file", _soundMessageFile);
	AddStrParam(L"sound_event_file", _soundEventFile);
	AddStrParam(L"sound_send_file", _soundSendFile);

	ss+=L"\"notify_for_lists\":[";
	for(int i=0;i<notifyForLists.size();i++){
		wchar_t tmp[5];
		swprintf(tmp, L"%i", notifyForLists[i]-1);
		ss+=tmp;
		if(i!=notifyForLists.size()-1)ss+=L", ";
	}
	ss+=L"],\r\n";

	ss+=L"\"notify_for_events\":[";
	for(int i=0;i<notifyForEvents.size();i++){
		ss+=notifyForEvents[i]?L"true":L"false";
		if(i!=notifyForEvents.size()-1)ss+=L", ";
	}
	ss+=L"],\r\n";

	_itow(eventsRefresh, s, 10);
	AddParam(L"events_refresh", s);


	_itow(listX, s, 10);
	AddParam(L"list_x", s);

	_itow(listY, s, 10);
	AddParam(L"list_y", s);

	_itow(listW, s, 10);
	AddParam(L"list_w", s);

	_itow(listH, s, 10);
	AddParam(L"list_h", s);

	_itow(talkX, s, 10);
	AddParam(L"talk_x", s);

	_itow(talkY, s, 10);
	AddParam(L"talk_y", s);

	_itow(talkW, s, 10);
	AddParam(L"talk_w", s);

	_itow(talkH, s, 10);
	AddParam(L"talk_h", s);

	_itow(notifyOnOff, s, 10);
	AddParam(L"notify_on_off", s);


	AddStrParam(L"api_server", apiServer);
	AddStrParam(L"api_req_path", apiReqPath);

	_itow(apiPort, s, 10);
	AddParam(L"api_port", s);

	//AddParam(L"use_proxy", useProxy?L"true":L"false");
	//AddStrParam(L"proxy_server", proxyServer);

	//_itow(proxyPort, s, 10);
	//AddParam(L"proxy_port", s);

	//if(wcslen(proxyUser)>0)AddStrParam(L"proxy_user", proxyUser);
	//if(wcslen(proxyPass)>0)AddStrParam(L"proxy_pass", proxyPass);


	_itow(listMode, s, 10);
	AddParam(L"list_mode", s);

	AddParam(L"show_lists", showLists?L"true":L"false");
	ss+=L"}\r\n";
	FILE* f=_wfopen(settings_filename, L"wb");
	if(!f){
		MessageBox(NULL, L"Невозможно открыть файл настроек для записи.", L"", MB_ICONEXCLAMATION);
		return;
	}
	fwrite(ss.c_str(), 2, ss.length(), f);
	fclose(f);
	needFree=false;
	}catch(...){ // На всякий случай.
		MessageBox(NULL, L"Ошибка при сохранении настроек...", L"", MB_ICONEXCLAMATION);
	}
}
