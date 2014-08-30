#include "Skin.h"
#include "Utils.h"
#include "resource.h"


CSkin::CSkin(void)
{
	useWindowsStyle=true;
}


CSkin::~CSkin(void)
{
	delete messageTemplate;
	delete messageStyle;
}


void CSkin::Load(wchar_t* skinFolder){
	std::wstring fn=skinFolder;
	fn+=L"skinProps.txt";
	FILE* f=_wfopen(fn.c_str(), L"r");
	if(!f){
		std::wstring e=L"Невозможно открыть файл для чтения:\n\n";
		e+=fn;
		MessageBox(NULL, e.c_str(), NULL, MB_ICONEXCLAMATION|MB_OK);
	}
	fseek(f,0,SEEK_END);
	int size = ftell(f);
	rewind(f);
	char* fdata=new char[size+1];
	memset(fdata, 0, size+1);
	char* rr=fdata;
	while(!feof(f)){
		fread(rr, 1, 1, f);
		rr++;
	}
	//fread(fdata, 1, size, f);
	fclose(f);
	wchar_t* d=CUtils::Utf8ToWchar(fdata);
	delete fdata;
	JSONNode* s=libJSON::Parse(d);
	delete d;
	if(!s){
		wchar_t* errmsg=new wchar_t[200];
		wsprintf(errmsg, L"Error loading skin \"%s\"; using default.", skinFolder);
		MessageBox(NULL, errmsg, L"Vk.IM", MB_ICONEXCLAMATION|MB_OK);
		delete errmsg;
		Load(L".\\Skins\\VK Style\\");
		return;
	}

	wbuttons.LoadImageW(IDB_WBUTTONS, NULL, AfxGetInstanceHandle(), -1);
	groupsBG.bmp.LoadImage((skinFolder+(s->GetNode(L"groups_bg")->GetNode(L"file")->NodeAsString())).c_str(), -1);
	groupsBG.w=s->GetNode(L"groups_bg")->GetNode(L"size")->NodeAt(0)->NodeAsInt();
	groupsBG.h=s->GetNode(L"groups_bg")->GetNode(L"size")->NodeAt(1)->NodeAsInt();
	groupsBG.n=s->GetNode(L"groups_bg")->GetNode(L"num_frames")->NodeAsInt();

	progress.bmp.LoadImage((skinFolder+(s->GetNode(L"progress")->GetNode(L"file")->NodeAsString())).c_str(), -1);
	progress.w=s->GetNode(L"progress")->GetNode(L"size")->NodeAt(0)->NodeAsInt();
	progress.h=s->GetNode(L"progress")->GetNode(L"size")->NodeAt(1)->NodeAsInt();
	progress.n=s->GetNode(L"progress")->GetNode(L"num_frames")->NodeAsInt();

	progressSmall.bmp.LoadImage((skinFolder+(s->GetNode(L"progress_small")->GetNode(L"file")->NodeAsString())).c_str(), -1);
	progressSmall.w=s->GetNode(L"progress_small")->GetNode(L"size")->NodeAt(0)->NodeAsInt();
	progressSmall.h=s->GetNode(L"progress_small")->GetNode(L"size")->NodeAt(1)->NodeAsInt();
	progressSmall.n=s->GetNode(L"progress_small")->GetNode(L"num_frames")->NodeAsInt();

	iconOnline=(HICON)LoadImage(NULL, (skinFolder+s->GetNode(L"icons")->GetNode(L"user_online")->NodeAsString()).c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	iconOffline=(HICON)LoadImage(NULL, (skinFolder+s->GetNode(L"icons")->GetNode(L"user_offline")->NodeAsString()).c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	iconUnknown=(HICON)LoadImage(NULL, (skinFolder+s->GetNode(L"icons")->GetNode(L"user_unknown")->NodeAsString()).c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	iconNewMsg=(HICON)LoadImage(NULL, (skinFolder+s->GetNode(L"icons")->GetNode(L"new_msg")->NodeAsString()).c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

	LoadImg(s->GetNode(L"list_highlight"), &listHl, skinFolder);
	LoadImg(s->GetNode(L"list_hover"), &listHlHover, skinFolder);
	LoadImg(s->GetNode(L"list_item"), &listItem, skinFolder);
	LoadImg(s->GetNode(L"tab_active"), &tabActive, skinFolder);
	LoadImg(s->GetNode(L"tab_inactive"), &tabInactive, skinFolder);
	LoadImg(s->GetNode(L"tab_over"), &tabOver, skinFolder);
	LoadImg(s->GetNode(L"tabbar_bg"), &tabbarBG, skinFolder);
	LoadImg(s->GetNode(L"user_info_bg"), &userInfoBG, skinFolder);
	LoadImg(s->GetNode(L"button_up"), &btnUp, skinFolder);
	LoadImg(s->GetNode(L"button_over"), &btnOver, skinFolder);
	LoadImg(s->GetNode(L"button_down"), &btnDown, skinFolder);
	LoadImg(s->GetNode(L"button_gray_up"), &btnGUp, skinFolder);
	LoadImg(s->GetNode(L"button_gray_over"), &btnGOver, skinFolder);
	LoadImg(s->GetNode(L"button_gray_down"), &btnGDown, skinFolder);
	LoadImg(s->GetNode(L"talk_bottom_bar"), &talkBottomBar, skinFolder);
	LoadImg(s->GetNode(L"talk_divider"), &talkDivider, skinFolder);
	LoadImg(s->GetNode(L"search_bg"), &searchBG, skinFolder);
	LoadImg(s->GetNode(L"list_bottom_bar"), &listBottomBar, skinFolder);
	LoadImg(s->GetNode(L"shadow1"), &shadow1, skinFolder);
	LoadImg(s->GetNode(L"shadow2"), &shadow2, skinFolder);

	//listItem.bmp.useAlpha=listHl.bmp.useAlpha=listHlHover.bmp.useAlpha=false;

	tabCloseInact.LoadImage((skinFolder+(s->GetNode(L"tab_close_inactive")->NodeAsString())).c_str(), -1);
	tabCloseAct.LoadImage((skinFolder+(s->GetNode(L"tab_close_active")->NodeAsString())).c_str(), -1);
	notifierBG.LoadImage((skinFolder+(s->GetNode(L"notifier_bg")->NodeAsString())).c_str(), -1);
	searchIcon.LoadImage((skinFolder+(s->GetNode(L"search_icon")->NodeAsString())).c_str(), -1);
	searchClear.LoadImage((skinFolder+(s->GetNode(L"search_clear")->NodeAsString())).c_str(), -1);

	for(int i=0;i<s->GetNode(L"vk_services_icons")->NodeSize();i++){
		vkServicesIcons.push_back((HICON)LoadImage(NULL, (skinFolder+s->GetNode(L"vk_services_icons")->NodeAt(i)->NodeAsString()).c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
	}

	//myMsgColor=LoadColor(s->GetNode(L"my_msg_color"));
	//otherMsgColor=LoadColor(s->GetNode(L"other_msg_color"));
	nameColor=LoadColor(s->GetNode(L"name_color"));
	statusColor=LoadColor(s->GetNode(L"status_color"));
	noNewEventsColor=LoadColor(s->GetNode(L"no_new_events_color"));
	eventsCountColor=LoadColor(s->GetNode(L"events_count_color"));
	btnTextColor=LoadColor(s->GetNode(L"btn_text_color"));
	btnGTextColor=LoadColor(s->GetNode(L"btn_gray_text_color"));
	btnTextSColor=LoadColor(s->GetNode(L"btn_shadow_text_color"));
	btnGTextSColor=LoadColor(s->GetNode(L"btn_shadow_gray_text_color"));
	groupTitleColor=LoadColor(s->GetNode(L"group_title_color"));
	contactOnlineColor=LoadColor(s->GetNode(L"contact_online_color"));
	contactOfflineColor=LoadColor(s->GetNode(L"contact_offline_color"));
	contactOnlineSelColor=LoadColor(s->GetNode(L"contact_online_sel_color"));
	contactOfflineSelColor=LoadColor(s->GetNode(L"contact_offline_sel_color"));
	notifyTitleColor=LoadColor(s->GetNode(L"notify_title_color"));
	notifyTextColor=LoadColor(s->GetNode(L"notify_text_color"));
	tabActiveTextColor=LoadColor(s->GetNode(L"tab_active_text_color"));
	tabOverTextColor=LoadColor(s->GetNode(L"tab_over_text_color"));
	tabIncativeTextColor=LoadColor(s->GetNode(L"tab_inactive_text_color"));
	searchPlaceholderColor=LoadColor(s->GetNode(L"search_placeholder_color"));
	listLinesColor=LoadColor(s->GetNode(L"list_lines_color"));

	std::wstring styleFile=skinFolder;
	styleFile+=L"messageTemplate.html";
	f=_wfopen(styleFile.c_str(), L"r");
	fseek(f,0,SEEK_END);
	size = ftell(f);
	rewind(f);
	fdata=new char[size+1];
	memset(fdata, 0, size+1);
	fread(fdata, 1, size, f);
	fclose(f);
	messageTemplate=CUtils::Utf8ToWchar(fdata);
	delete fdata;

	styleFile=skinFolder;
	styleFile+=L"messages.css";
	f=_wfopen(styleFile.c_str(), L"r");
	fseek(f,0,SEEK_END);
	size = ftell(f);
	rewind(f);
	fdata=new char[size+1];
	memset(fdata, 0, size+1);
	fread(fdata, 1, size, f);
	fclose(f);
	messageStyle=CUtils::Utf8ToWchar(fdata);
	delete fdata;

	delete s;
}

void CSkin::LoadImg(JSONNode* s, SkinElement* el, wchar_t* skinFolder){
	el->bmp.LoadImage((skinFolder+(s->GetNode(L"file")->NodeAsString())).c_str(), -1);
	if(s->GetNode(L"scale9")){
		el->bmp.scaleType=1;
		el->bmp.rcImg.right=s->GetNode(L"size")->NodeAt(0)->NodeAsInt();
		el->bmp.rcImg.bottom=s->GetNode(L"size")->NodeAt(1)->NodeAsInt();

		el->bmp.scale9.top=s->GetNode(L"scale9")->NodeAt(0)->NodeAsInt();
		el->bmp.scale9.right=s->GetNode(L"scale9")->NodeAt(1)->NodeAsInt();
		el->bmp.scale9.bottom=s->GetNode(L"scale9")->NodeAt(2)->NodeAsInt();
		el->bmp.scale9.left=s->GetNode(L"scale9")->NodeAt(3)->NodeAsInt();
	}else{
		el->bmp.scaleType=0;
		if(s->GetNode(L"size"))el->bmp.rcImg.right=s->GetNode(L"size")->NodeAt(0)->NodeAsInt();
		if(s->GetNode(L"size"))el->bmp.rcImg.bottom=s->GetNode(L"size")->NodeAt(1)->NodeAsInt();
	}
}

COLORREF CSkin::LoadColor(JSONNode* s){
	if(s && s->NodeSize()>=3)return RGB(s->NodeAt(0)->NodeAsInt(), s->NodeAt(1)->NodeAsInt(), s->NodeAt(2)->NodeAsInt());
	else return 0;
}