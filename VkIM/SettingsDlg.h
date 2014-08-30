#pragma once

#include "SettingsNotifyDlg.h"
#include "afxwin.h"
#include "SettingsCommonDlg.h"
#include "SettingsConnDlg.h"
#include "afxcmn.h"
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();
	CSettingsNotifyDlg notify_settings;
	CSettingsCommonDlg common_settings;
	CSettingsConnDlg connect_settings;

	BOOL OnInitDialog();
// Dialog Data
	enum { IDD = IDD_SETTINGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton applyBtn;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CTabCtrl tabs;
};
