// SetFListsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "SetFListsDlg.h"
#include "Lang.h"
#include "Connector.h"
#include "Settings.h"

// CSetFListsDlg dialog
extern CConnector con;

IMPLEMENT_DYNAMIC(CSetFListsDlg, CDialog)

CSetFListsDlg::CSetFListsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetFListsDlg::IDD, pParent)
{
	inited=false;
}

CSetFListsDlg::~CSetFListsDlg()
{
}

void CSetFListsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, theList);
}


BEGIN_MESSAGE_MAP(CSetFListsDlg, CDialog)
END_MESSAGE_MAP()


// CSetFListsDlg message handlers


BOOL CSetFListsDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();

	SetWindowTextW(CLang::Get(L"set1_select_lists"));
	GetDlgItem(IDOK)->SetWindowTextW(CLang::Get(L"ok"));
	GetDlgItem(IDCANCEL)->SetWindowTextW(CLang::Get(L"cancel"));

	listsItems.clear();

	// TODO:  Add extra initialization here
	theList.ModifyStyle( TVS_CHECKBOXES, 0 );
	theList.ModifyStyle( 0, TVS_CHECKBOXES );

	if(con.clDlg->friendList.groupsNames.size()>0){
	listsItems.push_back(theList.InsertItem(CLang::Get(L"ungrouped")));

	for(int i=0;i<con.clDlg->friendList.groupsNames.size()-1;i++){
		listsItems.push_back(theList.InsertItem(con.clDlg->friendList.groupsNames[i].c_str(), 0, 0));
	}

	if(!inited || checked.size()==0){
		for(int i=0;i<CSettings::notifyForLists.size();i++){
			if(listsItems.size()>CSettings::notifyForLists[i]+1){
				theList.SetCheck(listsItems[CSettings::notifyForLists[i]+1]);
			}
		}
	}else{
		for(int i=0;i<listsItems.size();i++){
			theList.SetCheck(listsItems[i], checked[i]);
		}
	}
	//updLists=true;
	}

	inited=true;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSetFListsDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	checked.clear();

	for(int i=0;i<listsItems.size();i++){
		checked.push_back(theList.GetCheck(listsItems[i]));
	}

	CDialog::OnOK();
}
