#pragma once
#include "afxwin.h"


// CCaptchaDlg dialog

class CCaptchaDlg : public CDialog
{
	DECLARE_DYNAMIC(CCaptchaDlg)

public:
	CCaptchaDlg(HBITMAP _img, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCaptchaDlg();

// Dialog Data
	enum { IDD = IDD_CAPTCHADLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	HBITMAP img;
	virtual BOOL OnInitDialog();
	CStatic imgControl;
	CString inputValue;
	CString titleLabel;
};
