// CaptchaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "CaptchaDlg.h"
#include "Lang.h"


// CCaptchaDlg dialog

IMPLEMENT_DYNAMIC(CCaptchaDlg, CDialog)

CCaptchaDlg::CCaptchaDlg(HBITMAP _img, CWnd* pParent /*=NULL*/)
	: CDialog(CCaptchaDlg::IDD, pParent)
	, inputValue(_T(""))
	, titleLabel(CLang::Get(L"captcha"))
{
	img=_img;
}

CCaptchaDlg::~CCaptchaDlg()
{
}

void CCaptchaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMG, imgControl);
	DDX_Text(pDX, IDC_EDIT1, inputValue);
	DDX_Text(pDX, IDC_TITLE, titleLabel);
}


BEGIN_MESSAGE_MAP(CCaptchaDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCaptchaDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCaptchaDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CCaptchaDlg message handlers


void CCaptchaDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void CCaptchaDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


BOOL CCaptchaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	imgControl.SetBitmap(img);
	SetDlgItemText(IDOK, CLang::Get(L"ok"));
	SetDlgItemText(IDCANCEL, CLang::Get(L"cancel"));

	GetDlgItem(IDC_EDIT1)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
