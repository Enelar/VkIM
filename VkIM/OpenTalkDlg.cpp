// OpenTalkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "OpenTalkDlg.h"
#include "Lang.h"


// COpenTalkDlg dialog

IMPLEMENT_DYNAMIC(COpenTalkDlg, CDialog)

COpenTalkDlg::COpenTalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenTalkDlg::IDD, pParent)
	, inputLinkLabel(CLang::Get(L"enter_url_or_id"))
	, urlOrID(_T(""))
{

}

COpenTalkDlg::~COpenTalkDlg()
{
}

void COpenTalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC1, inputLinkLabel);
	DDX_Control(pDX, IDOK, okBtn);
	DDX_Control(pDX, IDCANCEL, cancelBtn);
	DDX_Text(pDX, IDC_EDIT1, urlOrID);
}


BEGIN_MESSAGE_MAP(COpenTalkDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COpenTalkDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COpenTalkDlg message handlers


BOOL COpenTalkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	okBtn.SetWindowTextW(CLang::Get(L"ok"));
	cancelBtn.SetWindowTextW(CLang::Get(L"cancel"));
	SetWindowTextW(CLang::Get(L"mainmenu_open_talk"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void COpenTalkDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
