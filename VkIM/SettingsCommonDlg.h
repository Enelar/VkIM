#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSettingsCommonDlg dialog

class CSettingsCommonDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsCommonDlg)

public:
	CSettingsCommonDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsCommonDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGSCOMMONDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString skinLabel;
	CString langLabel;
	virtual BOOL OnInitDialog();
	CComboBox skinsList;
	CComboBox langsList;
	CButton enableAutostart;
	bool initialAutostart;
	CButton enableFromChat;
	CButton enableLoadHistory;
	CSpinButtonCtrl numHistoryItems;
	CString loadHistoryLabel;
	int sendWithCtrlEnter;
	CString sendKeyLabel;
	CString nextTimeLabel;
	void OnOK();
	void OnCancel();
	CButton enableCheckUpdates;
	CButton enableOfflineMsgs;
	CButton enableFLPopup;
};
