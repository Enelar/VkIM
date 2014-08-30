// SettingsConnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "SettingsConnDlg.h"
#include "Lang.h"
#include "Settings.h"
#include <WinInet.h>
#include "Connector.h"

extern CConnector con;
// CSettingsConnDlg dialog

IMPLEMENT_DYNAMIC(CSettingsConnDlg, CDialog)

CSettingsConnDlg::CSettingsConnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsConnDlg::IDD, pParent)
	, proxyLabel(CLang::Get(L"set2_proxy"))
	, proxyTypeLabel(CLang::Get(L"set2_proxy_type"))
	, proxyAddrLabel(CLang::Get(L"set2_server_addr"))
	, proxyPortLabel(CLang::Get(L"port"))
	, proxyUsernameLabel(CLang::Get(L"set2_username"))
	, proxyPassLabel(CLang::Get(L"set2_password"))
	, vkApiLabel(CLang::Get(L"set2_vkapi"))
	, vkApiAddrLabel(CLang::Get(L"set2_server_addr"))
	, vkApiPortLabel(CLang::Get(L"port"))
	, vkApiReqLabel(CLang::Get(L"set2_reqpath"))
	, enableProxy(FALSE)
	, apiServer(_T(""))
	, apiReqPath(_T(""))
	, apiPort(0)
	, proxyServer(_T(""))
	, proxyPort(0)
	, proxyUser(_T(""))
	, proxyPass(_T(""))
{

}

CSettingsConnDlg::~CSettingsConnDlg()
{
}

void CSettingsConnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, enableProxyCB);
	DDX_Text(pDX, IDC_PROXY_LABEL, proxyLabel);
	DDX_Text(pDX, IDC_PADDR_LABEL, proxyAddrLabel);
	DDX_Text(pDX, IDC_PPORT_LABEL, proxyPortLabel);
	DDX_Text(pDX, IDC_PUNAME_LABEL, proxyUsernameLabel);
	DDX_Text(pDX, IDC_PPASS_LABEL, proxyPassLabel);
	DDX_Text(pDX, IDC_VKAPI_LABEL, vkApiLabel);
	DDX_Text(pDX, IDC_API_ADDR, vkApiAddrLabel);
	DDX_Text(pDX, IDC_API_PORT, vkApiPortLabel);
	DDX_Text(pDX, IDC_API_REQ, vkApiReqLabel);
	//DDX_Check(pDX, IDC_CHECK1, enableProxy);
	DDX_Control(pDX, IDC_EDIT1, proxyAddrField);
	DDX_Control(pDX, IDC_EDIT2, proxyPortField);
	DDX_Control(pDX, IDC_EDIT3, proxyUserField);
	DDX_Control(pDX, IDC_EDIT6, proxyPassField);
	DDX_Text(pDX, IDC_EDIT7, apiServer);
	DDX_Text(pDX, IDC_EDIT9, apiReqPath);
	DDX_Text(pDX, IDC_EDIT8, apiPort);
	DDX_Text(pDX, IDC_EDIT1, proxyServer);
	DDX_Text(pDX, IDC_EDIT2, proxyPort);
	DDX_Text(pDX, IDC_EDIT3, proxyUser);
	DDX_Text(pDX, IDC_EDIT6, proxyPass);
	DDX_Control(pDX, IDC_BUTTON1, importIeButton);
}


BEGIN_MESSAGE_MAP(CSettingsConnDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, &CSettingsConnDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON1, &CSettingsConnDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// SettingsConnDlg message handlers


BOOL CSettingsConnDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	enableProxyCB.SetWindowTextW(CLang::Get(L"enable"));
	importIeButton.SetWindowTextW(CLang::Get(L"set2_use_ie"));

	GetDlgItem(IDC_PROXY_LABEL)->SetFont(con.clDlg->userNameLabel.GetFont());
	GetDlgItem(IDC_VKAPI_LABEL)->SetFont(con.clDlg->userNameLabel.GetFont());

	apiServer=CSettings::apiServer;
	apiReqPath=CSettings::apiReqPath;
	apiPort=CSettings::apiPort;
	proxyServer=CSettings::proxyServer;
	proxyPort=CSettings::proxyPort;
	proxyUser=CSettings::proxyUser;
	proxyPass=CSettings::proxyPass;
	UpdateData(false);
	enableProxyCB.SetCheck(CSettings::useProxy);
	EnableProxyForm(enableProxyCB.GetCheck());	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSettingsConnDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	EnableProxyForm(enableProxyCB.GetCheck());
}


void CSettingsConnDlg::EnableProxyForm(bool state)
{
	GetDlgItem(IDC_PADDR_LABEL)->EnableWindow(state);
	GetDlgItem(IDC_PUNAME_LABEL)->EnableWindow(state);
	GetDlgItem(IDC_PPASS_LABEL)->EnableWindow(state);
	GetDlgItem(IDC_PPORT_LABEL)->EnableWindow(state);
	proxyAddrField.EnableWindow(state);
	proxyPortField.EnableWindow(state);
	proxyUserField.EnableWindow(state);
	proxyPassField.EnableWindow(state);
	//importIeButton.EnableWindow(state);
}

void CSettingsConnDlg::OnBnClickedButton1()
{
	/*// TODO: Add your control notification handler code here
	INTERNET_PER_CONN_OPTION_LIST    List;
INTERNET_PER_CONN_OPTION         Option[1];
unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

Option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;

List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
List.pszConnection = NULL;
List.dwOptionCount = 1;
List.dwOptionError = 0;
List.pOptions = Option;

if(!InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize)){
	TRACE("Option error\n");
	return;
}
if(Option[0].Value.pszValue){
	std::wstring px=Option[0].Value.pszValue;
	std::wstring pxaddr=px.substr(0, px.find(L":"));
	std::wstring pxport=px.substr(px.find(L":")+1);
	proxyServer=pxaddr.c_str();
	//proxyUser=CSettings::proxyUser;
	//proxyPass=CSettings::proxyPass;
	UpdateData(false);
	proxyPortField.SetWindowTextW(pxport.c_str());
}*/
	ShellExecute(0, L"open", L"rundll32.exe",L"shell32.dll,Control_RunDLL inetcpl.cpl,Internet,4", L"", SW_SHOW);

}

void CSettingsConnDlg::OnOK(){}
void CSettingsConnDlg::OnCancel(){}