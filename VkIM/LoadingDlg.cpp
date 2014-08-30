// LoadingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "LoadingDlg.h"
#include "Lang.h"


// CLoadingDlg dialog

IMPLEMENT_DYNAMIC(CLoadingDlg, CDialog)

CLoadingDlg::CLoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadingDlg::IDD, pParent)
{

}

CLoadingDlg::~CLoadingDlg()
{
}

void CLoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOADING_TXT, loadingLabel);
}


BEGIN_MESSAGE_MAP(CLoadingDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CLoadingDlg message handlers


BOOL CLoadingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	loadingLabel.SetWindowTextW(CLang::Get(L"loading"));
	ModifyStyleEx(0, WS_EX_TOPMOST);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CLoadingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(loadingLabel)loadingLabel.SetWindowPos(NULL, 0, cy/2-10, cx, 20, SWP_NOZORDER);
	// TODO: Add your message handler code here
}
