#pragma once
#include "skinbutton.h"
#include "afxwin.h"
#include "TransparentStatic.h"
#include <string>

using namespace std;

// CStatusDlg dialog


typedef struct
{
	wstring filename;
	int type;
	bool upload;
}StatusAttach;

typedef struct
{
	wstring text;
	StatusAttach* att;
}StatusUpdateReq;

class CStatusDlg : public CDialog
{
	DECLARE_DYNAMIC(CStatusDlg)

public:
	CStatusDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatusDlg();

// Dialog Data
	enum { IDD = IDD_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSkinButton btnCancel;
	CSkinButton btnOk;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	CEdit textbox;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	CTransparentStatic btnAttach;
	CMenu attMenu;
	CMenu attPhotoMenu;
	CMenu attVideoMenu;
	CMenu attAudioMenu;
	afx_msg void OnStnClickedSt();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	StatusAttach att;
	bool hasAtt;
	CString statusText;
};
