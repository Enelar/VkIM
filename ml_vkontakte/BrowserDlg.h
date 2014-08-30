#pragma once
#include "explorer1.h"
#include "resource.h"

// CBrowserDlg dialog

class CBrowserDlg : public CDialog
{
	DECLARE_DYNAMIC(CBrowserDlg)

public:
	CBrowserDlg(CWnd* pParent = NULL, bool _logout=false);   // standard constructor
	virtual ~CBrowserDlg();

// Dialog Data
	enum { IDD = IDD_BROWSERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CExplorer1 ie;
	DECLARE_EVENTSINK_MAP()
	void NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int progressFrame;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool logout;
	bool logoutClicked;
	void DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL);
};
