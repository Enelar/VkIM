// FixHostsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FixHosts.h"
#include "FixHostsDlg.h"
#include "ListDlg.h"
#include <string>
#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAppDlg dialog




CAppDlg::CAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, option1Btn);
	DDX_Control(pDX, IDC_BUTTON2, option2Btn);
}

BEGIN_MESSAGE_MAP(CAppDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CAppDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAppDlg::OnBnClickedButton2)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CAppDlg message handlers

BOOL CAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon
	SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0), FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//showMoreBtn.SetWindowTextW(L"Ещё варианты \u2193");
	//showMoreBtn.SetWindowTextW(L"Ещё варианты \u2191");

	//option1Btn.SetWindowTextW(L"Автоматическое удаление");
	//option1Btn.description=L"Удалить перенаправления для доменов vkontakte.ru и vk.com.";
	((CButton*)GetDlgItem(IDCANCEL))->SetButtonStyle(((CButton*)GetDlgItem(IDCANCEL))->GetButtonStyle() & ~BS_DEFPUSHBUTTON);

	option1Btn.SetButtonStyle(option1Btn.GetButtonStyle() | BS_DEFPUSHBUTTON);
	option1Btn.SetWindowTextW(L"Очистить файл hosts");
	option1Btn.description=L"Удалить все заданные в файле перенаправления. Если Вы не можете получить доступ к vkontakte.ru, выберите этот вариант.";

	option2Btn.SetWindowTextW(L"Выбрать вручную");
	option2Btn.description=L"Открыть список всех перенаправлений, заданных в файле, с возможностью удаления по выбору.";

	SetDefID(IDC_BUTTON1);
	option1Btn.SetFocus();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CAppDlg::OnOK()
{
}

void CAppDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CRegKey key;
	LONG status = key.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters", KEY_READ);
	if(status!=ERROR_SUCCESS)return;
	wchar_t buf[1024];
	unsigned long len=1023;
	key.QueryStringValue(L"DataBasePath", buf, &len);
	key.Close();
	std::wstring fn=buf;
	fn+=L"\\hosts";
	ExpandEnvironmentStrings(fn.c_str(), buf, 1023);
	FILE* f=_wfopen(buf, L"w");
	fclose(f);
	CDialog::OnOK();
}

void CAppDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CListDlg ldlg;
	ShowWindow(SW_HIDE);
	INT_PTR result=ldlg.DoModal();
	if(result==IDOK){
		CDialog::OnOK();
	}else{
		ShowWindow(SW_SHOW);
	}
}

void CAppDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
	//SetDefID(IDC_BUTTON1);
	//option1Btn.SetFocus();
	// TODO: Add your message handler code here
}
