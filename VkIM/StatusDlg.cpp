// StatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "StatusDlg.h"
#include "lang.h"
#include "Connector.h"
#include "Skin.h"

extern CConnector con;
extern CSkin skin;

enum{
	M_ATT_PHOTO=100,
	M_ATT_VIDEO,
	M_ATT_AUDIO,
	MP_FILE,
	MP_ALBUM,
	MP_WEBCAM,
	MV_LOADED,
	MA_FILE,
	MA_LOADED
};

// CStatusDlg dialog

IMPLEMENT_DYNAMIC(CStatusDlg, CDialog)

CStatusDlg::CStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusDlg::IDD, pParent)
	, statusText(_T(""))
{
	hasAtt=false;
}

CStatusDlg::~CStatusDlg()
{
}

void CStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, btnCancel);
	DDX_Control(pDX, IDC_BUTTON2, btnOk);
	DDX_Control(pDX, IDC_EDIT1, textbox);
	DDX_Control(pDX, IDC_ST, btnAttach);
	DDX_Text(pDX, IDC_EDIT1, statusText);
}


BEGIN_MESSAGE_MAP(CStatusDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON2, &CStatusDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CStatusDlg::OnBnClickedButton1)
	ON_STN_CLICKED(IDC_ST, &CStatusDlg::OnStnClickedSt)
END_MESSAGE_MAP()


// CStatusDlg message handlers


BOOL CStatusDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetWindowRect(rect);
	pDC->FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(0xcc, 0xcc, 0xcc));
	pDC->FillSolidRect(1, 1, rect.Width()-2, rect.Height()-2, RGB(0xf7, 0xf7, 0xf7));
	textbox.GetWindowRect(rect);
	ScreenToClient(rect);
	pDC->FillSolidRect(rect.left-1, rect.top-1, rect.Width()+2, rect.Height()+2, RGB(0xc0, 0xca, 0xd5));
	btnAttach.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.top+=rect.Height()/2-4;
	if(hasAtt){
	CPen pen(PS_SOLID, 1, RGB(204, 213, 221));
	CPen* op=pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left+1, rect.top-1);
	pDC->LineTo(rect.left+4, rect.top+2);
	pDC->LineTo(rect.left+7, rect.top-1);
	pDC->LineTo(rect.left+9, rect.top+1);
	pDC->LineTo(rect.left+6, rect.top+4);
	pDC->LineTo(rect.left+9, rect.top+7);
	pDC->LineTo(rect.left+7, rect.top+9);
	pDC->LineTo(rect.left+4, rect.top+6);
	pDC->LineTo(rect.left+1, rect.top+9);
	pDC->LineTo(rect.left-1, rect.top+7);
	pDC->LineTo(rect.left+2, rect.top+4);
	pDC->LineTo(rect.left-1, rect.top+1);
	pDC->LineTo(rect.left+1, rect.top-1);
	pDC->SelectObject(op);
	CBrush brush(RGB(204, 213, 221));
	CBrush* ob=pDC->SelectObject(&brush);
	pDC->FloodFill(rect.left+4, rect.top+4, RGB(204, 213, 221));
	pDC->SelectObject(ob);
	}
	return true;
}


BOOL CStatusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rect;
	textbox.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.DeflateRect(1, 1);
	textbox.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
	btnOk.SetWindowTextW(CLang::Get(L"ok"));
	btnCancel.SetWindowTextW(CLang::Get(L"cancel"));
	btnAttach.SetWindowTextW(CLang::Get(L"status_add"));
	btnCancel.gray=true;
	btnAttach.color=RGB(43, 88, 122);

	attMenu.CreatePopupMenu();
	attPhotoMenu.CreatePopupMenu();
	attVideoMenu.CreatePopupMenu();
	attAudioMenu.CreatePopupMenu();
	MENUITEMINFO ii;
	ii.cbSize=sizeof(MENUITEMINFO);
	ii.fType=MFT_STRING;
	ii.fMask=MIIM_TYPE|MIIM_ID|MIIM_CHECKMARKS;//|MIIM_SUBMENU;

	ii.dwTypeData=CLang::Get(L"st_add_photo");
	ii.wID=M_ATT_PHOTO;
	//ii.hSubMenu=attPhotoMenu.m_hMenu;
	ii.hbmpUnchecked=ii.hbmpChecked=::LoadBitmapW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ATT_PHOTO));
	attMenu.InsertMenuItemW(M_ATT_PHOTO, &ii, false);

	ii.dwTypeData=CLang::Get(L"st_add_video");
	ii.wID=M_ATT_VIDEO;
	//ii.hSubMenu=attVideoMenu.m_hMenu;
	ii.hbmpUnchecked=ii.hbmpChecked=::LoadBitmapW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ATT_VIDEO));
	attMenu.InsertMenuItemW(M_ATT_VIDEO, &ii, false);

	ii.dwTypeData=CLang::Get(L"st_add_audio");
	ii.wID=M_ATT_AUDIO;
	//ii.hSubMenu=attAudioMenu.m_hMenu;
	ii.hbmpUnchecked=ii.hbmpChecked=::LoadBitmapW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ATT_AUDIO));
	attMenu.InsertMenuItemW(M_ATT_AUDIO, &ii, false);

	ii.fMask=MIIM_TYPE|MIIM_ID;

	ii.dwTypeData=CLang::Get(L"st_file");
	ii.wID=MP_FILE;
	attPhotoMenu.InsertMenuItemW(MP_FILE, &ii, false);

	ii.dwTypeData=CLang::Get(L"st_ap_album");
	ii.wID=MP_ALBUM;
	attPhotoMenu.InsertMenuItemW(MP_ALBUM, &ii, false);

	ii.dwTypeData=CLang::Get(L"st_ap_webcam");
	ii.wID=MP_WEBCAM;
	attPhotoMenu.InsertMenuItemW(MP_WEBCAM, &ii, false);

	/*ii.dwTypeData=CLang::Get(L"st_file");
	ii.wID=MV_FILE;
	attVideoMenu.InsertMenuItemW(MV_FILE, &ii, false);*/

	ii.dwTypeData=CLang::Get(L"st_loaded");
	ii.wID=MV_LOADED;
	attVideoMenu.InsertMenuItemW(MV_LOADED, &ii, false);

	ii.dwTypeData=CLang::Get(L"st_file");
	ii.wID=MA_FILE;
	attAudioMenu.InsertMenuItemW(MA_FILE, &ii, false);

	ii.dwTypeData=CLang::Get(L"st_loaded");
	ii.wID=MA_LOADED;
	attAudioMenu.InsertMenuItemW(MA_LOADED, &ii, false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CStatusDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(!hasAtt && statusText.IsEmpty())return;
	StatusUpdateReq* rq=new StatusUpdateReq;
	rq->text=statusText;
	if(hasAtt){
		rq->att=new StatusAttach;
		memcpy(rq->att, &att, sizeof(StatusAttach));
	}else{
		rq->att=NULL;
	}
	con.SetStatus(rq);
	con.clDlg->sdlg=NULL;
	CDialog::OnOK();
	delete this;
}


void CStatusDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	con.clDlg->sdlg=NULL;
	CDialog::OnOK();
	delete this;
}


void CStatusDlg::OnStnClickedSt()
{
	// TODO: Add your control notification handler code here
	if(!hasAtt){
		CRect rect;
		btnAttach.GetWindowRect(rect);
		attMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.bottom, this);
	}else{
		hasAtt=false;
		btnAttach.SetWindowTextW(CLang::Get(L"status_add"));
		btnAttach.color=RGB(43, 88, 122);
		Invalidate();
	}
}


BOOL CStatusDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if(wParam==M_ATT_PHOTO){//MP_FILE){
		CFileDialog fileDialog(true, NULL, NULL, 6, L"Изображения|*.jpg; *.jpeg; *.png; *.gif|", GetParent());
		int result = fileDialog.DoModal();
		if (result==IDOK)
		{
			att.filename=fileDialog.GetPathName();
			att.upload=true;
			att.type=ATT_PHOTO;
			hasAtt=true;
			wstring ws=L"     "+wstring(CLang::Get(L"att_photo"));
			btnAttach.SetWindowTextW(ws.c_str());
			Invalidate();
		}
	}

	if(wParam==M_ATT_AUDIO){//MA_FILE){
		CFileDialog fileDialog(true, NULL, NULL, 6, L"Аудиофайлы|*.mp3|", GetParent());
		int result = fileDialog.DoModal();
		if (result==IDOK)
		{
			att.filename=fileDialog.GetPathName();
			att.upload=true;
			att.type=ATT_AUDIO;
			hasAtt=true;
			wstring ws=L"     "+wstring(CLang::Get(L"att_audio"));
			btnAttach.SetWindowTextW(ws.c_str());
			Invalidate();
		}
	}

	if(wParam==M_ATT_VIDEO){
		CFileDialog fileDialog(true, NULL, NULL, 6, L"Видеофайлы|*.mp4; *.avi; *.3gp; *.mpg; *.mpeg; *.mov; *.flv; *.wmv|", GetParent());
		int result = fileDialog.DoModal();
		if (result==IDOK)
		{
			att.filename=fileDialog.GetPathName();
			att.upload=true;
			att.type=ATT_VIDEO;
			hasAtt=true;
			wstring ws=L"     "+wstring(CLang::Get(L"att_video"));
			btnAttach.SetWindowTextW(ws.c_str());
			Invalidate();
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}
