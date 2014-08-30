#pragma once

#include <vector>
#include "EnBitmap.h"
#include "JSONNode.h"
#include "jsonmain.h"

typedef struct
{
	CEnBitmap bmp;
	int w;
	int h;
	int n;
}SkinElement;

class CSkin
{
public:
	CSkin(void);
	~CSkin(void);
	void Load(wchar_t* skinFolder);
	void LoadImg(JSONNode* s, SkinElement* el, wchar_t* skinFolder);
	COLORREF LoadColor(JSONNode* s);

	SkinElement groupsBG;
	SkinElement progress;
	SkinElement progressSmall;
	SkinElement listHl;
	SkinElement listHlHover;
	SkinElement listItem;
	SkinElement tabActive;
	SkinElement tabInactive;
	SkinElement tabOver;
	SkinElement tabbarBG;
	SkinElement userInfoBG;
	HICON iconOnline;
	HICON iconOffline;
	HICON iconUnknown;
	HICON iconNewMsg;
	CEnBitmap tabCloseInact;
	CEnBitmap tabCloseAct;
	CEnBitmap notifierBG;
	CEnBitmap searchClear;
	CEnBitmap searchIcon;
	SkinElement btnUp;
	SkinElement btnDown;
	SkinElement btnOver;
	SkinElement btnGUp;
	SkinElement btnGDown;
	SkinElement btnGOver;
	SkinElement talkBottomBar;
	SkinElement listBottomBar;
	SkinElement talkDivider;
	SkinElement searchBG;
	SkinElement shadow1;
	SkinElement shadow2;

	//COLORREF myMsgColor;
	//COLORREF otherMsgColor;
	COLORREF nameColor;
	COLORREF statusColor;
	COLORREF noNewEventsColor;
	COLORREF eventsCountColor;
	COLORREF btnTextColor;
	COLORREF btnGTextColor;
	COLORREF btnTextSColor;
	COLORREF btnGTextSColor;
	COLORREF groupTitleColor;
	COLORREF contactOnlineColor;
	COLORREF contactOfflineColor;
	COLORREF contactOnlineSelColor;
	COLORREF contactOfflineSelColor;
	COLORREF notifyTitleColor;
	COLORREF notifyTextColor;
	COLORREF tabActiveTextColor;
	COLORREF tabOverTextColor;
	COLORREF tabIncativeTextColor;
	COLORREF searchPlaceholderColor;
	COLORREF listLinesColor;

	std::vector<HICON> vkServicesIcons;
	bool useWindowsStyle;
	CEnBitmap wbuttons;
	wchar_t* messageTemplate;
	wchar_t* messageStyle;
};

