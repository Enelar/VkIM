#pragma once
#include "afxcmn.h"
#include <vector>


// CSetFListsDlg dialog

class CSetFListsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetFListsDlg)

public:
	CSetFListsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetFListsDlg();

// Dialog Data
	enum { IDD = IDD_SETFLISTSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl theList;
	std::vector<HTREEITEM> listsItems;
	std::vector<bool> checked;
	virtual BOOL OnInitDialog();
	bool inited;
	virtual void OnOK();
};
