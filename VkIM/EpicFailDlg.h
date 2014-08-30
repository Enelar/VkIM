#pragma once

#include <string>
// CEpicFailDlg dialog

class CEpicFailDlg : public CDialog
{
	DECLARE_DYNAMIC(CEpicFailDlg)

public:
	CEpicFailDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEpicFailDlg();

// Dialog Data
	enum { IDD = IDD_EPICFAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL OnInitDialog();
	std::wstring errorInfo;
};
