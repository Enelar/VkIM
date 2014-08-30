// SettingsNotifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "SettingsNotifyDlg.h"
#include "Lang.h"
#include "Settings.h"
#include <MMSystem.h>
#include "Connector.h"

extern CConnector con;
// CSettingsNotifyDlg dialog

IMPLEMENT_DYNAMIC(CSettingsNotifyDlg, CDialog)

CSettingsNotifyDlg::CSettingsNotifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsNotifyDlg::IDD, pParent)
	, statusChangeLabel(CLang::Get(L"set1_status_chg"))
	, onlineLabel(CLang::Get(L"set1_select_events"))
	, offlineLabel(CLang::Get(L"set1_sound"))
	, newMsgsLabel(CLang::Get(L"set1_popup"))
	, friendGroupsLabel(CLang::Get(L"set1_for_groups"))
	, onlineSoundFile(_T(""))
	, offlineSoundFile(_T(""))
	, messageSoundFile(_T(""))
	, eventsSoundFile(_T(""))
	, eventsLabel(CLang::Get(L"set1_events"))
	, updateIntervalLabel(CLang::Get(L"set1_upd_interval"))
	, updateSeconds(CLang::Get(L"seconds"))
	, sendSoundFile(_T(""))
	, notifyOnOff(0)
{

}

CSettingsNotifyDlg::~CSettingsNotifyDlg()
{
}

void CSettingsNotifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_STATUS_CHANGE, statusChangeLabel);
	DDX_Text(pDX, IDC_ST_ONLINE, onlineLabel);
	DDX_Text(pDX, IDC_ST_OFFLINE, offlineLabel);
	DDX_Text(pDX, IDC_NEW_MSGS, newMsgsLabel);
	DDX_Text(pDX, IDC_NTF_GROUPS, friendGroupsLabel);
	DDX_Control(pDX, IDC_CHECK2, enablePpOnline);
	DDX_Control(pDX, IDC_CHECK3, enableSnOnline);
	DDX_Control(pDX, IDC_CHECK4, enablePpOffline);
	DDX_Control(pDX, IDC_CHECK5, enableSnOffline);
	DDX_Control(pDX, IDC_CHECK6, enablePpMsg);
	DDX_Control(pDX, IDC_CHECK7, enableSnMsg);
	DDX_Control(pDX, IDC_HL_TBB, enableHlMsg);
	DDX_Control(pDX, IDC_SEL_ALL, selAllListsBtn);
	DDX_Text(pDX, IDC_EDIT1, onlineSoundFile);
	DDX_Text(pDX, IDC_EDIT3, offlineSoundFile);
	DDX_Control(pDX, IDC_BUTTON1, play1Btn);
	DDX_Control(pDX, IDC_BUTTON3, play2Btn);
	DDX_Control(pDX, IDC_BUTTON7, play3Btn);
	DDX_Control(pDX, IDC_BUTTON9, play4Btn);
	DDX_Text(pDX, IDC_EDIT4, messageSoundFile);
	DDX_Control(pDX, IDC_CHECK11, enableVkEvents);
	DDX_Control(pDX, IDC_CHECK9, enablePpEvents);
	DDX_Control(pDX, IDC_CHECK10, enableSnEvents);
	DDX_Text(pDX, IDC_EDIT5, eventsSoundFile);
	//DDX_Text(pDX, IDC_EVENTS, eventsLabel);
	DDX_Text(pDX, IDC_UPD_I, updateIntervalLabel);
	DDX_Text(pDX, IDC_UPD_S, updateSeconds);
	DDX_Control(pDX, IDC_COMBO1, updIntervalSelect);
	DDX_Control(pDX, IDC_TREE2, eventsList);
	DDX_Text(pDX, IDC_EDIT10, sendSoundFile);
	DDX_Control(pDX, IDC_CHECK13, enableSnSend);
	DDX_Control(pDX, IDC_BUTTON5, play5btn);
	DDX_Radio(pDX, IDC_RADIO1, notifyOnOff);
	DDV_MinMaxInt(pDX, notifyOnOff, 0, 2);
}


BEGIN_MESSAGE_MAP(CSettingsNotifyDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSettingsNotifyDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CSettingsNotifyDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CSettingsNotifyDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CSettingsNotifyDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_SEL_ALL, &CSettingsNotifyDlg::OnBnClickedSelAll)
//	ON_BN_CLICKED(IDC_CLR_ALL, &CSettingsNotifyDlg::OnBnClickedClrAll)
	ON_BN_CLICKED(IDC_BUTTON8, &CSettingsNotifyDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON7, &CSettingsNotifyDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON10, &CSettingsNotifyDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON9, &CSettingsNotifyDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON6, &CSettingsNotifyDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CSettingsNotifyDlg::OnBnClickedButton5)
//	ON_WM_ERASEBKGND()
//	ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_RADIO1, &CSettingsNotifyDlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_RADIO2, &CSettingsNotifyDlg::OnBnClickedRadio2)
ON_BN_CLICKED(IDC_RADIO3, &CSettingsNotifyDlg::OnBnClickedRadio3)
END_MESSAGE_MAP()


// CSettingsNotifyDlg message handlers

BOOL CSettingsNotifyDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	listsSelector=new CSetFListsDlg(this);

	notifyOnOff=CSettings::notifyOnOff;
	if(notifyOnOff!=1){
		selAllListsBtn.ShowWindow(SW_HIDE);
	}
	UpdateData(false);

	enablePpOnline.SetWindowTextW(CLang::Get(L"set1_fonline"));
	enablePpOffline.SetWindowTextW(CLang::Get(L"set1_foffline"));
	enablePpMsg.SetWindowTextW(CLang::Get(L"set1_newmsg"));
	enablePpEvents.SetWindowTextW(CLang::Get(L"set1_events"));
	enableSnOnline.SetWindowTextW(CLang::Get(L"set1_fonline"));
	enableSnOffline.SetWindowTextW(CLang::Get(L"set1_foffline"));
	enableSnMsg.SetWindowTextW(CLang::Get(L"set1_newmsg"));
	enableSnEvents.SetWindowTextW(CLang::Get(L"set1_events"));
	enableSnSend.SetWindowTextW(CLang::Get(L"set1_sound_send"));
	enableHlMsg.SetWindowTextW(CLang::Get(L"set1_hl_taskbtn"));
	selAllListsBtn.SetWindowTextW(CLang::Get(L"set1_select"));
	enableVkEvents.SetWindowTextW(CLang::Get(L"set1_n_events"));

	GetDlgItem(IDC_RADIO1)->SetWindowTextW(CLang::Get(L"set1_all_friends"));
	GetDlgItem(IDC_RADIO2)->SetWindowTextW(CLang::Get(L"set1_some_lists"));
	GetDlgItem(IDC_RADIO3)->SetWindowTextW(CLang::Get(L"set1_nobody"));

	enablePpOnline.SetCheck(CSettings::popupOnline);
	enablePpOffline.SetCheck(CSettings::popupOffline);
	enablePpMsg.SetCheck(CSettings::popupMessage);
	enablePpEvents.SetCheck(CSettings::popupEvent);
	enableSnOnline.SetCheck(CSettings::soundOnline);
	enableSnOffline.SetCheck(CSettings::soundOffline);
	enableSnMsg.SetCheck(CSettings::soundMessage);
	enableSnEvents.SetCheck(CSettings::soundEvent);
	enableSnSend.SetCheck(CSettings::soundSend);
	enableHlMsg.SetCheck(CSettings::flashMessage);
	enableVkEvents.SetCheck(CSettings::enableEvents);

	onlineSoundFile=CSettings::soundOnlineFile;
	offlineSoundFile=CSettings::soundOfflineFile;
	messageSoundFile=CSettings::soundMessageFile;
	eventsSoundFile=CSettings::soundEventFile;
	sendSoundFile=CSettings::soundSendFile;

	updIntervalSelect.AddString(L"15");
	updIntervalSelect.AddString(L"30");
	updIntervalSelect.AddString(L"45");
	updIntervalSelect.AddString(L"60");
	updIntervalSelect.AddString(L"90");
	updIntervalSelect.AddString(L"120");
	updIntervalSelect.AddString(L"180");
	updIntervalSelect.AddString(L"240");

	GetDlgItem(IDC_NTF_GROUPS)->SetFont(con.clDlg->userNameLabel.GetFont());
	GetDlgItem(IDC_ST_OFFLINE)->SetFont(con.clDlg->userNameLabel.GetFont());
	GetDlgItem(IDC_ST_ONLINE)->SetFont(con.clDlg->userNameLabel.GetFont());
	GetDlgItem(IDC_NEW_MSGS)->SetFont(con.clDlg->userNameLabel.GetFont());

	int nSel=1;
	switch(CSettings::eventsRefresh){
	case 15000: nSel=0; break;
	case 30000: nSel=1; break;
	case 45000: nSel=2; break;
	case 60000: nSel=3; break;
	case 90000: nSel=4; break;
	case 120000: nSel=5; break;
	case 180000: nSel=6; break;
	case 240000: nSel=7; break;
	}
	updIntervalSelect.SetCurSel(nSel);
	
	play1Btn.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON, 16, 16, 0));
	play2Btn.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON, 16, 16, 0));
	play3Btn.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON, 16, 16, 0));
	play4Btn.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON, 16, 16, 0));
	play5btn.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON, 16, 16, 0));

	((CButton*)GetDlgItem(IDC_BUTTON2))->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 16, 16, 0));
	((CButton*)GetDlgItem(IDC_BUTTON4))->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 16, 16, 0));
	((CButton*)GetDlgItem(IDC_BUTTON8))->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 16, 16, 0));
	((CButton*)GetDlgItem(IDC_BUTTON10))->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 16, 16, 0));
	((CButton*)GetDlgItem(IDC_BUTTON6))->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 16, 16, 0));

	
	if(con.clDlg->friendList.groupsNames.size()>0){
		updLists=true;
	}else{
		updLists=false;
		selAllListsBtn.EnableWindow(false);
	}

	eventsList.ModifyStyle( TVS_CHECKBOXES, 0 );
	eventsList.ModifyStyle( 0, TVS_CHECKBOXES );

	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_friends"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_photos"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_videos"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_notes"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_gifts"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_events"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_groups"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_opinions"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_offers"), 0, 0));
	eventsItems.push_back(eventsList.InsertItem(CLang::Get(L"menu_vk_questions"), 0, 0));

	for(int i=0;i<10;i++){
		eventsList.SetCheck(eventsItems[i], CSettings::notifyForEvents[i]);
	}
	UpdateData(false);

	return true;
}

void CSettingsNotifyDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	PlaySound(onlineSoundFile, NULL, SND_ASYNC | SND_FILENAME);
}


void CSettingsNotifyDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	PlaySound(offlineSoundFile, NULL, SND_ASYNC | SND_FILENAME);
}


void CSettingsNotifyDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDialog(true, NULL, NULL, 6, L"WAV audio files|*.wav|", GetParent());
	int result = fileDialog.DoModal();
	if (result==IDOK)
	{
		onlineSoundFile=fileDialog.GetPathName();
		UpdateData(false);
	}
}


void CSettingsNotifyDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDialog(true, NULL, NULL, 6, L"WAV audio files|*.wav|", GetParent());
	int result = fileDialog.DoModal();
	if (result==IDOK)
	{
		offlineSoundFile=fileDialog.GetPathName();
		UpdateData(false);
	}
}


void CSettingsNotifyDlg::OnBnClickedSelAll()
{
	// TODO: Add your control notification handler code here
	listsSelector->DoModal();
}


void CSettingsNotifyDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDialog(true, NULL, NULL, 6, L"WAV audio files|*.wav|", GetParent());
	int result = fileDialog.DoModal();
	if (result==IDOK)
	{
		messageSoundFile=fileDialog.GetPathName();
		UpdateData(false);
	}
}


void CSettingsNotifyDlg::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	PlaySound(messageSoundFile, NULL, SND_ASYNC | SND_FILENAME);
}


void CSettingsNotifyDlg::OnBnClickedButton10()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDialog(true, NULL, NULL, 6, L"WAV audio files|*.wav|", GetParent());
	int result = fileDialog.DoModal();
	if (result==IDOK)
	{
		eventsSoundFile=fileDialog.GetPathName();
		UpdateData(false);
	}
}


void CSettingsNotifyDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	PlaySound(eventsSoundFile, NULL, SND_ASYNC | SND_FILENAME);
}


void CSettingsNotifyDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDialog(true, NULL, NULL, 6, L"WAV audio files|*.wav|", GetParent());
	int result = fileDialog.DoModal();
	if (result==IDOK)
	{
		sendSoundFile=fileDialog.GetPathName();
		UpdateData(false);
	}
}


void CSettingsNotifyDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	PlaySound(sendSoundFile, NULL, SND_ASYNC | SND_FILENAME);
}


void CSettingsNotifyDlg::OnOK(){}
void CSettingsNotifyDlg::OnCancel(){}

void CSettingsNotifyDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	selAllListsBtn.ShowWindow(SW_HIDE);
}


void CSettingsNotifyDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	selAllListsBtn.ShowWindow(SW_SHOW);
}


void CSettingsNotifyDlg::OnBnClickedRadio3()
{
	// TODO: Add your control notification handler code here
	selAllListsBtn.ShowWindow(SW_HIDE);
}
