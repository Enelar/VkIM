// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "AboutDlg.h"
#include "Lang.h"
#include "VkAPIRequest.h"
#include "Connector.h"

extern CConnector con;
// CAboutDlg dialog

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	nclicks=0;
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK2, &CAboutDlg::OnNMClickSyslink2)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()


// CAboutDlg message handlers


void CAboutDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/id1708231", NULL, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetWindowText(CLang::Get(L"mainmenu_about"));
	::SetWindowTextA(GetDlgItem(IDC_STATIC_TXT)->m_hWnd, "Vk.IM " APP_VER_STR "\nVKontakte Instant Messenger");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CAboutDlg::OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	ShellExecute(GetSafeHwnd(), L"open", L"http://vkontakte.ru/club20296424", NULL, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}


void CAboutDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	nclicks++;
	if(nclicks==5){
		CVkAPIRequest req(L"activity.get");
		req.AddParameter(L"uid", L"1");
		JSONNode* jn=con.SendAPIRequest(&req);
		if(jn && jn->GetNode(L"response") && jn->GetNode(L"response")->GetNode(L"activity"))
			MessageBox(jn->GetNode(L"response")->GetNode(L"activity")->NodeAsString().c_str(), 0, MB_ICONINFORMATION);
		if(jn)delete jn;
		nclicks=0;
	}
	CDialog::OnNcRButtonDown(nHitTest, point);
}


void CAboutDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	OnNcLButtonDown(nHitTest, point);
	CDialog::OnNcLButtonDblClk(nHitTest, point);
}
