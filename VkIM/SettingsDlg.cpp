// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "SettingsDlg.h"
#include "Lang.h"
#include "Settings.h"
#include <atlbase.h>
#include "Connector.h"

extern CConnector con;
// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, tabs);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//ON_LBN_SELCHANGE(IDC_LIST1, &CSettingsDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDOK, &CSettingsDlg::OnBnClickedOk)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSettingsDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CSettingsDlg message handlers


BOOL CSettingsDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	SetWindowText(CLang::Get(L"settings"));

	tabs.InsertItem(0, CLang::Get(L"settings_common"));
	tabs.InsertItem(1, CLang::Get(L"settings_notifications"));
	tabs.InsertItem(2, CLang::Get(L"settings_connection"));

	int cwx, cwy;
	CRect rect;
	tabs.GetWindowRect(rect);
	cwx=2;//rect.left;
	cwy=rect.Height();//rect.top;

	notify_settings.Create(CSettingsNotifyDlg::IDD, &tabs);
	notify_settings.GetWindowRect(rect);
	cwy-=rect.Height()+4;
	notify_settings.SetWindowPos(NULL, cwx, cwy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	common_settings.Create(CSettingsCommonDlg::IDD, &tabs);
	common_settings.SetWindowPos(NULL, cwx, cwy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	connect_settings.Create(CSettingsConnDlg::IDD, &tabs);
	connect_settings.SetWindowPos(NULL, cwx, cwy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	common_settings.ShowWindow(SW_SHOW);

	/*settingsList.InsertString(0, CLang::Get(L"settings_common"));
	settingsList.InsertString(1, CLang::Get(L"settings_notifications"));
	settingsList.InsertString(2, CLang::Get(L"settings_connection"));
	settingsList.SetItemHeight(0, 30);
	settingsList.SetCurSel(0);*/
	

	SetDlgItemText(IDOK, CLang::Get(L"ok"));
	SetDlgItemText(IDCANCEL, CLang::Get(L"cancel"));

	return true;
}

void CSettingsDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
	common_settings.ShowWindow(SW_HIDE);
	connect_settings.ShowWindow(SW_HIDE);
	notify_settings.ShowWindow(SW_HIDE);

	/*switch(settingsList.GetCurSel()){
	case 0:
		common_settings.ShowWindow(SW_SHOW);
		break;
	case 1:
		notify_settings.ShowWindow(SW_SHOW);
		break;
	case 2:
		connect_settings.ShowWindow(SW_SHOW);
		break;
	}*/
	//Invalidate();
}


void CSettingsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	wcscpy(CSettings::langFile, (wchar_t*)common_settings.langsList.GetItemDataPtr(common_settings.langsList.GetCurSel()));
	common_settings.skinsList.GetLBText(common_settings.skinsList.GetCurSel(), CSettings::skinFolder);
	
	CSettings::popupOffline=notify_settings.enablePpOffline.GetCheck();
	CSettings::popupOnline=notify_settings.enablePpOnline.GetCheck();
	CSettings::popupMessage=notify_settings.enablePpMsg.GetCheck();
	CSettings::soundOffline=notify_settings.enableSnOffline.GetCheck();
	CSettings::soundOnline=notify_settings.enableSnOnline.GetCheck();
	CSettings::soundMessage=notify_settings.enableSnMsg.GetCheck();
	CSettings::soundSend=notify_settings.enableSnSend.GetCheck();
	CSettings::flashMessage=notify_settings.enableHlMsg.GetCheck();
	CSettings::soundEvent=notify_settings.enableSnEvents.GetCheck();
	CSettings::popupEvent=notify_settings.enablePpEvents.GetCheck();
	CSettings::enableEvents=notify_settings.enableVkEvents.GetCheck();

	switch(notify_settings.updIntervalSelect.GetCurSel()){
	case 0: CSettings::eventsRefresh=15000; break;
	case 1: CSettings::eventsRefresh=30000; break;
	case 2: CSettings::eventsRefresh=45000; break;
	case 3: CSettings::eventsRefresh=60000; break;
	case 4: CSettings::eventsRefresh=90000; break;
	case 5: CSettings::eventsRefresh=120000; break;
	case 6: CSettings::eventsRefresh=180000; break;
	case 7: CSettings::eventsRefresh=240000; break;
	}

	con.clDlg->KillTimer(101);
	if(CSettings::enableEvents){
		con.clDlg->SetTimer(101, CSettings::eventsRefresh, NULL);
	}else{
		memset(&con.clDlg->counters, 0, sizeof(Counters));
	}
	con.clDlg->CountersEnableChanged();

	notify_settings.UpdateData();
	connect_settings.UpdateData();
	if(CSettings::needFree){
		delete CSettings::soundMessageFile;
		delete CSettings::soundOfflineFile;
		delete CSettings::soundOnlineFile;
	}
	wcscpy(CSettings::soundOfflineFile, notify_settings.offlineSoundFile.GetBuffer());

	wcscpy(CSettings::soundOnlineFile, notify_settings.onlineSoundFile.GetBuffer());

	wcscpy(CSettings::soundMessageFile, notify_settings.messageSoundFile.GetBuffer());

	wcscpy(CSettings::soundEventFile, notify_settings.eventsSoundFile.GetBuffer());

	wcscpy(CSettings::soundSendFile, notify_settings.sendSoundFile.GetBuffer());

	if(notify_settings.updLists && notify_settings.listsSelector->checked.size()>0){
	CSettings::notifyForLists.clear();

/*	for(int i=0;i<notify_settings.listsItems.size();i++){
		if(notify_settings.ooNotifyLists.GetCheck(notify_settings.listsItems[i])){
			CSettings::notifyForLists.push_back(i-1);
		}
	}*/

	for(int i=0;i<notify_settings.listsSelector->listsItems.size();i++){
		if(notify_settings.listsSelector->checked[i]){
			CSettings::notifyForLists.push_back(i-1);
		}
	}

	}

	CSettings::notifyForEvents.clear();
	for(int i=0;i<notify_settings.eventsItems.size();i++){
		CSettings::notifyForEvents.push_back(notify_settings.eventsList.GetCheck(notify_settings.eventsItems[i]));
	}

	if(common_settings.enableAutostart.GetCheck()!=common_settings.initialAutostart){
	if(common_settings.enableAutostart.GetCheck()){
		CRegKey key;
		LONG status = key.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		if (status == ERROR_SUCCESS)
		{
			int nArgs;
			wchar_t** cmdline=CommandLineToArgvW(GetCommandLineW(), &nArgs);
			std::wstring asCmdLine=L"\"";
			asCmdLine+=cmdline[0];
			asCmdLine+=L"\" /autostart";
			status = key.SetValue(asCmdLine.c_str(), L"Vk.IM");
		}
		key.Close(); 
	}else{
		CRegKey key;
		LONG status = key.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		if (status == ERROR_SUCCESS)
		{
			key.DeleteValue(L"Vk.IM");
		}
		key.Close();
	}
	}

	CSettings::fromChat=common_settings.enableFromChat.GetCheck();
	CSettings::loadHistory=common_settings.enableLoadHistory.GetCheck();
	CSettings::checkUpdates=common_settings.enableCheckUpdates.GetCheck();
	CSettings::checkOfflineMsgs=common_settings.enableOfflineMsgs.GetCheck();
	CSettings::flPopup=common_settings.enableFLPopup.GetCheck();
	CSettings::numHistory=common_settings.numHistoryItems.GetPos();

	CSettings::useProxy=connect_settings.enableProxyCB.GetCheck();
	CSettings::apiPort=connect_settings.apiPort;
	CSettings::proxyPort=connect_settings.proxyPort;
	wcscpy(CSettings::apiServer, connect_settings.apiServer.GetBuffer());
	CSettings::proxyServer=new wchar_t[connect_settings.proxyServer.GetLength()+1];
	wcscpy(CSettings::proxyServer, connect_settings.proxyServer.GetBuffer());
	wcscpy(CSettings::apiReqPath, connect_settings.apiReqPath.GetBuffer());
	CSettings::proxyUser=new wchar_t[connect_settings.proxyUser.GetLength()+1];
	wcscpy(CSettings::proxyUser, connect_settings.proxyUser.GetBuffer());
	CSettings::proxyPass=new wchar_t[connect_settings.proxyPass.GetLength()+1];
	wcscpy(CSettings::proxyPass, connect_settings.proxyPass.GetBuffer());
	common_settings.UpdateData();
	CSettings::sendWithCtrlEnter=common_settings.sendWithCtrlEnter;

	notify_settings.UpdateData();
	CSettings::notifyOnOff=notify_settings.notifyOnOff;


	CSettings::Save();

	CDialog::OnOK();
}


BOOL CSettingsDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	/*CDialog::OnEraseBkgnd(pDC);
	CFont fnt;
	fnt.CreateFontW(-20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	CFont* oldFont=pDC->SelectObject(&fnt);
	pDC->SetBkMode(TRANSPARENT);
	CString txt=L"";
	switch(settingsList.GetCurSel()){
	case 0:
		txt=CLang::Get(L"settings_common");
		break;
	case 1:
		txt=CLang::Get(L"settings_notifications");
		break;
	case 2:
		txt=CLang::Get(L"settings_connection");
		break;
	}
	pDC->SetTextColor(RGB(255, 255, 255));
	pDC->DrawText(txt, CRect(130, 21, 130+456, 26+21), DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SetTextColor(RGB(0, 0, 0));
	pDC->DrawText(txt, CRect(130, 22, 130+456, 26+22), DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SelectObject(oldFont);
	return true;*/
	return CDialog::OnEraseBkgnd(pDC);
}


void CSettingsDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	common_settings.ShowWindow(SW_HIDE);
	connect_settings.ShowWindow(SW_HIDE);
	notify_settings.ShowWindow(SW_HIDE);

	switch(tabs.GetCurSel()){
	case 0:
		common_settings.ShowWindow(SW_SHOW);
		break;
	case 1:
		notify_settings.ShowWindow(SW_SHOW);
		break;
	case 2:
		connect_settings.ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}
