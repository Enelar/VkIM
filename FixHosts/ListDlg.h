#pragma once
#include "afxcmn.h"

#include <vector>
#include <string>

using namespace std;
// CListDlg dialog

class CListDlg : public CDialog
{
	DECLARE_DYNAMIC(CListDlg)

public:
	CListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CListDlg();

// Dialog Data
	enum { IDD = IDD_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl list;
	virtual BOOL OnInitDialog();
	vector<string> _lines;

	afx_msg void OnBnClickedOk();
};
