#pragma once
#include "afxwin.h"


// COpenTalkDlg dialog

class COpenTalkDlg : public CDialog
{
	DECLARE_DYNAMIC(COpenTalkDlg)

public:
	COpenTalkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COpenTalkDlg();

// Dialog Data
	enum { IDD = IDD_OPENTALKDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString inputLinkLabel;
	CButton okBtn;
	CButton cancelBtn;
	CString urlOrID;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
