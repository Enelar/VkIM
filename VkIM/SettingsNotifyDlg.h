#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "SetFListsDlg.h"
#include <vector>


// CSettingsNotifyDlg dialog

class CSettingsNotifyDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsNotifyDlg)

public:
	CSettingsNotifyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsNotifyDlg();

// Dialog Data
	enum { IDD = IDD_SETTINGSNOTIFYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	BOOL OnInitDialog();
	CString statusChangeLabel;
	CString onlineLabel;
	CString offlineLabel;
	CString newMsgsLabel;
	CString friendGroupsLabel;
	CButton enablePpOnline;
	CButton enableSnOnline;
	CButton enablePpOffline;
	CButton enableSnOffline;
	CButton enablePpMsg;
	CButton enableSnMsg;
	CButton enableHlMsg;
	CButton selAllListsBtn;
	CString onlineSoundFile;
	CString offlineSoundFile;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	CButton play1Btn;
	CButton play2Btn;
	CButton play3Btn;
	CButton play4Btn;
	afx_msg void OnBnClickedButton4();
	
	std::vector<HTREEITEM> eventsItems;
	afx_msg void OnBnClickedSelAll();
	CString messageSoundFile;
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton7();
	CButton enableVkEvents;
	CButton enablePpEvents;
	CButton enableSnEvents;
	CString eventsSoundFile;
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton9();
	CString eventsLabel;
	CString updateIntervalLabel;
	CString updateSeconds;
	CComboBox updIntervalSelect;
	CTreeCtrl eventsList;
	afx_msg void OnBnClickedButton6();
	CString sendSoundFile;
	CButton enableSnSend;
	afx_msg void OnBnClickedButton5();
	CButton play5btn;

	CSetFListsDlg* listsSelector;

	void OnOK();
	void OnCancel();
	bool updLists;
	int notifyOnOff;
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
};
