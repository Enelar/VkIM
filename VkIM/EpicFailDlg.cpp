// EpicFailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "EpicFailDlg.h"


// CEpicFailDlg dialog

IMPLEMENT_DYNAMIC(CEpicFailDlg, CDialog)

CEpicFailDlg::CEpicFailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEpicFailDlg::IDD, pParent)
{

}

CEpicFailDlg::~CEpicFailDlg()
{
}

void CEpicFailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEpicFailDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CEpicFailDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CEpicFailDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CEpicFailDlg message handlers


void CEpicFailDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	ExitProcess(0);
}


void CEpicFailDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL, L"open", L"VkIM.exe", NULL, NULL, SW_SHOWNORMAL);
	ExitProcess(0);
}


BOOL CEpicFailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	((CStatic*)GetDlgItem(IDC_ICON))->SetIcon(LoadIcon(NULL, IDI_ERROR));
	GetDlgItem(IDC_EDIT1)->SetWindowTextW(errorInfo.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
