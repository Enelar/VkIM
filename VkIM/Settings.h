#pragma once

#include "jsonmain.h"
#include "JSONNode.h"
#include "stdafx.h"
#include <vector>

#define MAXLEN 512

class CSettings
{
public:
	CSettings(wchar_t* filename);
	~CSettings(void);
	static void Load(wchar_t* filename);
	static void Save();

	static wchar_t langFile[MAXLEN];
	static wchar_t skinFolder[MAXLEN];
	static wchar_t* settings_filename;

	static bool soundOnline;
	static bool popupOnline;
	static wchar_t soundOnlineFile[MAXLEN];

	static bool soundSend;
	static wchar_t soundSendFile[MAXLEN];

	static bool soundOffline;
	static bool popupOffline;
	static wchar_t soundOfflineFile[MAXLEN];

	static bool soundMessage;
	static bool popupMessage;
	static bool flashMessage;
	static wchar_t soundMessageFile[MAXLEN];

	static std::vector<int> notifyForLists;

	static bool needFree;
	static bool globalSoundOn;
	static bool fromChat;
	static bool loadHistory;
	static int numHistory;

	static wchar_t apiServer[MAXLEN];
	static wchar_t apiReqPath[MAXLEN];
	static int apiPort;

	static bool useProxy;
	static wchar_t* proxyServer;
	static wchar_t* proxyUser;
	static wchar_t* proxyPass;
	static int proxyPort;

	static bool showLists;
	static int listMode;
	static int sendWithCtrlEnter;

	static wchar_t soundEventFile[MAXLEN];
	static bool soundEvent;
	static bool enableEvents;
	static std::vector<bool> notifyForEvents;
	static bool popupEvent;
	static int eventsRefresh;
	static bool showOffline;
	static bool checkUpdates;
	static bool checkOfflineMsgs;
	static bool flPopup;

	static int listX;
	static int listY;
	static int listW;
	static int listH;
	static int talkX;
	static int talkY;
	static int talkW;
	static int talkH;

	static int notifyOnOff;
};

