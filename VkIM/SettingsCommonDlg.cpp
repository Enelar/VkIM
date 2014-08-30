// SettingsCommonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "SettingsCommonDlg.h"
#include "Lang.h"
#include "Settings.h"
#include "Connector.h"
#include <atlbase.h>


// CSettingsCommonDlg dialog
extern CConnector con;

IMPLEMENT_DYNAMIC(CSettingsCommonDlg, CDialog)

CSettingsCommonDlg::CSettingsCommonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsCommonDlg::IDD, pParent)
	, skinLabel(CLang::Get(L"set0_skin"))
	, langLabel(CLang::Get(L"set0_lang"))
	, loadHistoryLabel(CLang::Get(L"set0_load_hist2"))
	, sendWithCtrlEnter(0)
	, sendKeyLabel(CLang::Get(L"set0_send_key"))
	, nextTimeLabel(CLang::Get(L"set0_nexttime"))
{

}

CSettingsCommonDlg::~CSettingsCommonDlg()
{
}

void CSettingsCommonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SKIN_LABEL, skinLabel);
	DDX_Text(pDX, IDC_LANG_LABEL, langLabel);
	DDX_Control(pDX, IDC_COMBO1, skinsList);
	DDX_Control(pDX, IDC_COMBO2, langsList);
	DDX_Control(pDX, IDC_CHECK1, enableAutostart);
	DDX_Control(pDX, IDC_CHECK8, enableFromChat);
	DDX_Control(pDX, IDC_CHECK12, enableLoadHistory);
	DDX_Control(pDX, IDC_SPIN1, numHistoryItems);
	DDX_Text(pDX, IDC_LH2, loadHistoryLabel);
	DDX_Text(pDX, IDC_SK, sendKeyLabel);
	DDX_Radio(pDX, IDC_RADIO1, sendWithCtrlEnter);
	DDV_MinMaxInt(pDX, sendWithCtrlEnter, 0, 1);
	DDX_Text(pDX, IDC_NT, nextTimeLabel);
	DDX_Control(pDX, IDC_CHECK2, enableCheckUpdates);
	DDX_Control(pDX, IDC_CHECK3, enableOfflineMsgs);
	DDX_Control(pDX, IDC_CHECK4, enableFLPopup);
}


BEGIN_MESSAGE_MAP(CSettingsCommonDlg, CDialog)
END_MESSAGE_MAP()


// CSettingsCommonDlg message handlers


BOOL CSettingsCommonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	GetDlgItem(IDC_SK)->SetFont(con.clDlg->userNameLabel.GetFont());
	GetDlgItem(IDC_SKIN_LABEL)->SetFont(con.clDlg->userNameLabel.GetFont());
	GetDlgItem(IDC_LANG_LABEL)->SetFont(con.clDlg->userNameLabel.GetFont());

	sendWithCtrlEnter=CSettings::sendWithCtrlEnter;
	UpdateData(false);
	
	int skinIdx=0;
	int _i=0;
	WIN32_FIND_DATA findData;
	HANDLE hFind=FindFirstFile(L".\\Skins\\*", &findData);
	do{
		if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && findData.cFileName[0]!='.'){
			skinsList.AddString(findData.cFileName);
			if(wcscmp(CSettings::skinFolder, findData.cFileName)==0)skinIdx=_i;
			_i++;
		}
	}while(FindNextFile(hFind, &findData));
	FindClose(hFind);
	skinsList.SetCurSel(skinIdx);

	int lang_idx=0;
	int i=0;
	hFind=FindFirstFile(L".\\Lang\\*", &findData);
	do{
		if(!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			std::wstring fn=L".\\Lang\\";
			wchar_t* fnb=new wchar_t[wcslen(findData.cFileName)+1];
			wcscpy(fnb, findData.cFileName);
			fn+=fnb;
			FILE* f=_wfopen(fn.c_str(), L"r");
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
			if(s){
				if(s->GetNode(L"lang_name")){
					langsList.AddString(s->GetNode(L"lang_name")->NodeAsString().c_str());
					langsList.SetItemDataPtr(langsList.GetCount()-1, fnb);
					if(wcscmp(fnb, CSettings::langFile)==0)lang_idx=i;
				}
				delete s;
			}
			i++;
		}
	}while(FindNextFile(hFind, &findData));
	FindClose(hFind);
	langsList.SetCurSel(lang_idx);

	CRegKey key;
		LONG status = key.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		if (status == ERROR_SUCCESS)
		{
			wchar_t v;
			ULONG n=1;
			status = key.QueryStringValue(L"Vk.IM", &v, &n);
			if(status!=2){
				enableAutostart.SetCheck(true);
				initialAutostart=true;
			}else{
				initialAutostart=false;
			}
		}
	key.Close();

	enableAutostart.SetWindowTextW(CLang::Get(L"set0_autostart"));
	enableFromChat.SetWindowTextW(CLang::Get(L"set0_chat"));
	enableLoadHistory.SetWindowTextW(CLang::Get(L"set0_load_hist1"));
	enableCheckUpdates.SetWindowTextW(CLang::Get(L"set0_check_updates"));
	enableOfflineMsgs.SetWindowTextW(CLang::Get(L"set0_offline_msgs"));
	enableFLPopup.SetWindowTextW(CLang::Get(L"set0_show_info"));
	numHistoryItems.SetPos(CSettings::numHistory);
	enableFromChat.SetCheck(CSettings::fromChat);
	enableLoadHistory.SetCheck(CSettings::loadHistory);
	enableCheckUpdates.SetCheck(CSettings::checkUpdates);
	enableOfflineMsgs.SetCheck(CSettings::checkOfflineMsgs);
	enableFLPopup.SetCheck(CSettings::flPopup);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsCommonDlg::OnOK(){}
void CSettingsCommonDlg::OnCancel(){}