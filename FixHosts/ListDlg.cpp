// ListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FixHosts.h"
#include "ListDlg.h"

#include <atlbase.h>

// CListDlg dialog

IMPLEMENT_DYNAMIC(CListDlg, CDialog)

CListDlg::CListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListDlg::IDD, pParent)
{

}

CListDlg::~CListDlg()
{
}

void CListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, list);
}


BEGIN_MESSAGE_MAP(CListDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CListDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CListDlg message handlers

BOOL CListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0), FALSE);		// Set small icon
	SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, 0), TRUE);		// Set small icon

	// TODO:  Add extra initialization here
	list.InsertColumn(0, L"IP-адрес");
	list.InsertColumn(1, L"Домен");
	list.SetColumnWidth(0, 150);
	list.SetColumnWidth(1, 300);
	list.SetExtendedStyle(LVS_EX_CHECKBOXES);

	CRegKey key;
	LONG status = key.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters", KEY_READ);
	if(status!=ERROR_SUCCESS)return TRUE;
	wchar_t buf[1024];
	unsigned long len=1023;
	key.QueryStringValue(L"DataBasePath", buf, &len);
	key.Close();
	std::wstring fn=buf;
	fn+=L"\\hosts";
	ExpandEnvironmentStrings(fn.c_str(), buf, 1023);
	FILE* f=_wfopen(buf, L"r");
	if(!f)return TRUE;
	fseek(f,0,SEEK_END);
	int size = ftell(f);
	rewind(f);
	char* fdata=new char[size+1];
	memset(fdata, 0, size+1);
	char* rr=fdata;
	while(!feof(f)){
		fread(rr, 1, 1, f);
		rr++;
	}
	fclose(f);
	std::vector<char*> lines;
	char* part = strtok (fdata,"\r\n");
	if(strlen(part)>0){
		lines.push_back(part);
		_lines.push_back(part);
	}
	while (part != NULL)
	{
		part = strtok (NULL, "\r\n");
		if(part && strlen(part)>0){
			lines.push_back(part);
			_lines.push_back(part);
		}
	}
	bool found=false;
	for(int i=0;i<lines.size();i++){
		if(lines[i][0]=='#')continue;
		char* ip = strtok (lines[i]," \t#");
		char* host = strtok (NULL, " \t#");
		if(ip && host){
			int idx=list.InsertItem(i, CString(ip));
			list.SetItemText(idx, 1, CString(host));
			list.SetItemData(idx, i);
			list.SetItemState(idx, UINT((int(true) + 1) << 12),  LVIS_STATEIMAGEMASK);
		}
	}
	delete fdata;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CListDlg::OnBnClickedOk()
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
	for(int i=0;i<_lines.size();i++){
		bool w=true;
		for(int j=0;j<list.GetItemCount();j++){
			if(list.GetItemData(j)==i && !((list.GetItemState(j, LVIS_STATEIMAGEMASK)>>12)-1))w=false;
		}
		if(w)fprintf(f, "%s\n", _lines[i].c_str());
	}
	fclose(f);
	OnOK();
}
