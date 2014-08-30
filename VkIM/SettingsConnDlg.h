#pragma once
#include "afxwin.h"


// SettingsConnDlg dialog

class CSettingsConnDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsConnDlg)

public:
	CSettingsConnDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsConnDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGSCONNDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton enableProxyCB;
	CString proxyLabel;
	CString proxyTypeLabel;
	CString proxyAddrLabel;
	CString proxyPortLabel;
	CString proxyUsernameLabel;
	CString proxyPassLabel;
	CString vkApiLabel;
	CString vkApiAddrLabel;
	CString vkApiPortLabel;
	CString vkApiReqLabel;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck1();
	BOOL enableProxy;
	CEdit proxyAddrField;
	CEdit proxyPortField;
	CEdit proxyUserField;
	CEdit proxyPassField;
	void EnableProxyForm(bool state);
	CString apiServer;
	CString apiReqPath;
	int apiPort;
	CString proxyServer;
	int proxyPort;
	CString proxyUser;
	CString proxyPass;
	CButton importIeButton;
	afx_msg void OnBnClickedButton1();
	void OnOK();
	void OnCancel();
};
