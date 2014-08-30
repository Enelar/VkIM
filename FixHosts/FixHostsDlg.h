// FixHostsDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "bigbutton.h"


// CAppDlg dialog
class CAppDlg : public CDialog
{
// Construction
public:
	CAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FIXHOSTS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton showMoreBtn;
	CBigButton option1Btn;
	CBigButton option2Btn;
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
