// UserInfoTip.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "UserInfoTip.h"
#include "Connector.h"
#include "NotificationDlg.h"
#include "Skin.h"
#include "Lang.h"

extern CConnector con;
extern CSkin skin;
// CUserInfoTip

IMPLEMENT_DYNAMIC(CUserInfoTip, CWnd)

CUserInfoTip::CUserInfoTip()
{
	exists=false;
	hasInfo=false;
	bdate=L"";
	unv=L"";
	city=L"";
	act=L"";
	ava=0;
	headerFnt.CreateFontW(-11, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	fnt.CreateFontW(-11, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	smallFnt.CreateFontW(-9, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	CreateEx(WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_LAYERED, AfxRegisterWndClass(CS_VREDRAW|CS_HREDRAW/*|CS_DROPSHADOW*/), L"", WS_POPUP, CRect(0,0,250,200), con.clDlg, 0);
	ShowWindow(SW_SHOWNA);
}

CUserInfoTip::~CUserInfoTip()
{
	exists=false;
	if(ava)DeleteObject(ava);
	headerFnt.DeleteObject();
	fnt.DeleteObject();
	smallFnt.DeleteObject();
}


BEGIN_MESSAGE_MAP(CUserInfoTip, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CUserInfoTip message handlers




void CUserInfoTip::OnPaint()
{
	CRect r;
	GetClientRect(r);
	int h=r.Height();
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	dc.FillSolidRect(0, 0, 250, h, RGB(255,255,255));
	CPen pen(PS_SOLID, 1, RGB(174,189,204));
	CPen* op=dc.SelectObject(&pen);
	CFont* of=dc.SelectObject(&headerFnt);
	dc.UpdateColors();
	dc.MoveTo(0, 0);
	POINT* pts=GetPolygon(true);
	for(int i=1;i<7;i++){
		dc.LineTo(pts[i].x, pts[i].y);
	}
	dc.LineTo(flip?6:0,0);
	delete pts;
	dc.SelectObject(op);
	dc.FillSolidRect(flip?6:0, 0, 244, 22, RGB(59,103,152));
	dc.FillSolidRect(flip?7:1, 2, 242, 19, RGB(92,130,171));
	dc.FillSolidRect(flip?7:1, 1, 242, 1, RGB(126,156,188));
	dc.FillSolidRect(flip?8:2, 2, 240, 18, RGB(109,143,179));
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(59,103,152));
	dc.DrawText(username.c_str(), -1, CRect(flip?16:10, 1, 244, 23), DT_VCENTER|DT_SINGLELINE);
	dc.SetTextColor(RGB(255,255,255));
	dc.DrawText(username.c_str(), -1, CRect(flip?16:10, 0, 244, 22), DT_VCENTER|DT_SINGLELINE);
	
	if(!hasInfo){
		int pgx=(flip?6:0)+122-skin.progress.w/2;
		int pgy=100-skin.progress.w/2+11;
		skin.progress.bmp.DrawUnscaled(&dc, pgx, pgy, skin.progress.w, skin.progress.h, skin.progress.w*ldrFrame, 0);
	}else{
		CDC mdc;
		mdc.CreateCompatibleDC(0);
		HGDIOBJ ob=mdc.SelectObject(ava);
		CRect ar;
		mdc.GetClipBox(ar);
		TRACE("%i %i\n", ar.Width(), ar.Height());
		dc.BitBlt(flip?13:7, 29, ar.Width(), ar.Height(), &mdc, 0, 0, SRCCOPY);
		mdc.SelectObject(ob);
		mdc.DeleteDC();
		dc.SelectObject(&fnt);
		dc.SetTextColor(RGB(120,120,120));
		dc.DrawText(CLang::Get(L"info_bdate"), -1, CRect(flip?120:114, 29, flip?243:237, 44), DT_VCENTER|DT_SINGLELINE);
		dc.DrawText(CLang::Get(L"info_city"), -1, CRect(flip?120:114, 59, flip?243:237, 74), DT_VCENTER|DT_SINGLELINE);
		dc.DrawText(CLang::Get(L"info_university"), -1, CRect(flip?120:114, 89, flip?243:237, 104), DT_VCENTER|DT_SINGLELINE);
		dc.SetTextColor(RGB(0,0,0));
		dc.DrawText(bdate.c_str(), -1, CRect(flip?120:114, 44, flip?243:237, 59), DT_VCENTER|DT_SINGLELINE);
		dc.DrawText(city.c_str(), -1, CRect(flip?120:114, 74, flip?243:237, 89), DT_VCENTER|DT_SINGLELINE);
		dc.DrawText(unv.c_str(), -1, CRect(flip?120:114, 104, flip?243:237, 119), DT_VCENTER|DT_SINGLELINE);
		if(act.length()>0){
			dc.FillSolidRect(flip?7:1, (ar.Height()>97?ar.Height():97)+36, 242, 1, RGB(204,211,220));
			dc.FillSolidRect(flip?7:1, (ar.Height()>97?ar.Height():97)+37, 242, h-((ar.Height()>97?ar.Height():97)+37)-1, RGB(228,242,246));
			dc.SelectObject(&smallFnt);
			dc.DrawText(act.c_str(), -1, CRect(flip?13:7, (ar.Height()>97?ar.Height():97)+43, flip?243:237, (ar.Height()>97?ar.Height():97)+136), DT_LEFT|DT_WORDBREAK);
		}
	}
	dc.SelectObject(of);
}


BOOL CUserInfoTip::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return true;//CWnd::OnEraseBkgnd(pDC);
}


void CUserInfoTip::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//lpwndpos->flags
	CWnd::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}


POINT* CUserInfoTip::GetPolygon(bool draw)
{
	CRect r;
	GetClientRect(r);
	int h=r.Height();
	if(!flip){
	POINT rgnPts[]={
		{0,0},
		{244-(draw?1:0),0},
		{244-(draw?1:0),30},
		{250-(draw?1:0),36},
		{244-(draw?1:0),42},
		{244-(draw?1:0),h-(draw?1:0)},
		{0,h-(draw?1:0)}
	};
	POINT* pts=new POINT[7];
	memcpy(pts, rgnPts, sizeof(POINT)*7);
	return pts;
	}else{
	POINT rgnPts[]={
		{6,0},
		{250-(draw?1:0),0},
		{250-(draw?1:0),h-(draw?1:0)},
		{6,h-(draw?1:0)},
		{6,42},
		{0,36},
		{6,30}
	};
	POINT* pts=new POINT[7];
	memcpy(pts, rgnPts, sizeof(POINT)*7);
	return pts;
	}
}


void CUserInfoTip::StartAnim(void)
{
	POINT* rp=GetPolygon();
	rgn=::CreatePolygonRgn(rp, 7, ALTERNATE);
	delete rp;
	SetWindowRgn(rgn, true);
	username=con.clDlg->friendList.items[item].userName;
	t=0;
	ldrFrame=0;
	SetTimer(100, 10, NULL);
	SetTimer(101, 250, NULL);
	exists=true;
	con.LoadUserInfo(this);
}


void CUserInfoTip::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100){
		if(t==26){
			KillTimer(100);
			return;
		}
		int op=CNotificationDlg::EaseOut(t, 0, 255, 25);
		SetLayeredWindowAttributes(0, op, LWA_ALPHA);
		int sy=pos.y-25;
		int y=CNotificationDlg::EaseOut(t, 0, 25, 25);
		SetWindowPos(NULL, pos.x, sy+y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		t++;
	}
	if(nIDEvent==101){
		ldrFrame++;
		ldrFrame%=skin.progress.n;
		RedrawWindow();
	}
	CWnd::OnTimer(nIDEvent);
}


void CUserInfoTip::DataLoaded(void)
{
	::EnterCriticalSection(&con.tipCs);
	if(!this || !::IsWindow(m_hWnd)){
		::LeaveCriticalSection(&con.tipCs);
		return;
	}
	CDC mdc;
	mdc.CreateCompatibleDC(0);
	HGDIOBJ ob=mdc.SelectObject(ava);
	CFont* of=mdc.SelectObject(&smallFnt);
	SIZE sz2;
	CRect trect(flip?13:7, 0, flip?243:237, 136);
	mdc.DrawText(act.c_str(), -1, trect, DT_LEFT|DT_WORDBREAK|DT_CALCRECT);
	CRect r;
	mdc.GetClipBox(r);
	int h=22+7+(r.Height()>97?r.Height():97)+7+trect.Height()+(act.length()>0?14:0);
	SetWindowPos(NULL, 0, 0, 250, h, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
	mdc.SelectObject(of);
	mdc.SelectObject(ob);
	mdc.DeleteDC();
	if(bdate.compare(L"null")==0)bdate=CLang::Get(L"info_no_info");
	if(city.compare(L"null")==0)city=CLang::Get(L"info_no_info");
	if(unv.compare(L"'")==0)unv=CLang::Get(L"info_no_info");
	if(unv[unv.length()-1]=='\'')unv=unv.substr(0, unv.length()-1);
	POINT* rp=GetPolygon();
	rgn=::CreatePolygonRgn(rp, 7, ALTERNATE);
	delete rp;
	SetWindowRgn(rgn, true);
	KillTimer(101);
	hasInfo=true;
	RedrawWindow();
	::LeaveCriticalSection(&con.tipCs);
}
