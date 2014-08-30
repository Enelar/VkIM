// NotificationWnd.cpp : implementation file
//
#include "stdafx.h"
#include "VkIM.h"
#include "NotificationWnd.h"
#include "Skin.h"
#include "Connector.h"
#include <GdiPlus.h>

extern CSkin skin;
extern CConnector con;
// CNotificationWnd

typedef enum _DWMWINDOWATTRIBUTE {
  DWMWA_NCRENDERING_ENABLED           = 1,
  DWMWA_NCRENDERING_POLICY,
  DWMWA_TRANSITIONS_FORCEDISABLED,
  DWMWA_ALLOW_NCPAINT,
  DWMWA_CAPTION_BUTTON_BOUNDS,
  DWMWA_NONCLIENT_RTL_LAYOUT,
  DWMWA_FORCE_ICONIC_REPRESENTATION,
  DWMWA_FLIP3D_POLICY,
  DWMWA_EXTENDED_FRAME_BOUNDS,
  DWMWA_HAS_ICONIC_BITMAP,
  DWMWA_DISALLOW_PEEK,
  DWMWA_EXCLUDED_FROM_PEEK,
  DWMWA_LAST 
} DWMWINDOWATTRIBUTE;
typedef UINT (CALLBACK* DSWA)(HWND,DWORD,LPCVOID,DWORD);

IMPLEMENT_DYNAMIC(CNotificationWnd, CWnd)

CNotificationWnd::CNotificationWnd(wchar_t* title, wchar_t* content, int idx)
{
	//RECT r=CRect(0, 0, 200, 95);
	//CreateEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE, L"MDIClient", L"", WS_POPUP, 0, 0, 200, 95, ::GetDesktopWindow(), 0);
	//SetParent(CWnd::GetDesktopWindow());
	ix=idx;
	//ShowWindow(SW_SHOWNA);

	/*RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect, NULL);
	SetWindowPos(NULL, rect.right-200, rect.bottom-(95*idx)+30, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	CDC scrdc;
	scrdc.Attach(::GetDC(0));
	bdc.CreateCompatibleDC(&scrdc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&scrdc, 200, 95);
	CBitmap* obmp=bdc.SelectObject(&bmp);
	skin.notifierBG.DrawUnscaled(&bdc, 0, 0, 200, 95, 0, 0);
	skin.tabCloseInact.DrawUnscaled(&bdc, 168, 15, 15, 15, 0, 0);

	Gdiplus::Graphics g(bdc.GetSafeHdc());
    Gdiplus::FontFamily ff(_T("Tahoma"));
    Gdiplus::Font font(&ff, 11, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::Font font2(&ff, 11, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::PointF pt(15, 40);
	Gdiplus::RectF rc(15,55, 170, 27);
    Gdiplus::SolidBrush brush(Gdiplus::Color(254, 0, 0, 0));
    g.DrawString(title, wcslen(title), &font, pt, &brush);
	g.DrawString(content, wcslen(content), &font2, rc, NULL, &brush);


	BLENDFUNCTION func;
	func.BlendOp=AC_SRC_OVER;
	func.AlphaFormat=AC_SRC_ALPHA;
	func.SourceConstantAlpha=0;
	func.BlendFlags=0;
	UpdateLayeredWindow(&scrdc, &CPoint(rect.right-200, rect.bottom-(95*idx)+30), &CSize(200, 95), &bdc, &CPoint(0,0), 0, &func, ULW_ALPHA);
	::ReleaseDC(0, scrdc);

	mouseInside=true;
	needClose=false;
	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);*/
}

CNotificationWnd::~CNotificationWnd()
{
}

int EaseOut(float t, float b, float c, float d){
	float ts=(t/=d)*t;
	float tc=ts*t;
	return b+c*(tc*ts + -5*ts*ts + 10*tc + -10*ts + 5*t);
}

int EaseIn(float t, float b, float c, float d){
	float ts=(t/=d)*t;
	float tc=ts*t;
	return b+c*(tc*ts);
}

void CNotificationWnd::DoTransitionIn(){
	RECT rect;
	GetWindowRect(&rect);
	int src_y=rect.top;
	int dst_y=-30;
	for(int i=0;i<50;i++){
		int y=EaseOut(i, src_y, dst_y+1, 50);
		SetWindowPos(NULL, rect.left, y, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		CDC scrdc;
	scrdc.Attach(::GetDC(0));
		BLENDFUNCTION func;
	func.BlendOp=AC_SRC_OVER;
	func.AlphaFormat=AC_SRC_ALPHA;
	func.SourceConstantAlpha=EaseOut(i, 0, 255, 50);
	func.BlendFlags=0;
	UpdateLayeredWindow(&scrdc, &CPoint(rect.left, y), &CSize(200, 95), &bdc, &CPoint(0,0), 0, &func, ULW_ALPHA);
	::ReleaseDC(0, scrdc);

		_sleep(10);
	}

	for(int i=0;i<300;i++){
		_sleep(10);
		if(needClose)break;
	}

	while(mouseInside && !needClose){
		_sleep(10);
	}
	if(!this)return;

	for(int i=0;i<50;i++){
		CDC scrdc;
	scrdc.Attach(::GetDC(0));
		BLENDFUNCTION func;
	func.BlendOp=AC_SRC_OVER;
	func.AlphaFormat=AC_SRC_ALPHA;
	func.SourceConstantAlpha=EaseIn(i, 255, -255, 50);
	func.BlendFlags=0;
	UpdateLayeredWindow(&scrdc, &CPoint(rect.left, src_y+dst_y), &CSize(200, 95), &bdc, &CPoint(0,0), 0, &func, ULW_ALPHA);
	::ReleaseDC(0, scrdc);

		_sleep(10);
	}
	SendMessage(WM_CLOSE);
}

UINT CNotificationWnd::DoStartTransitionIn(CNotificationWnd* wnd){
	wnd->DoTransitionIn();
	return 0;
}

void CNotificationWnd::StartTransitionIn(){
	AfxBeginThread((AFX_THREADPROC)&DoStartTransitionIn, this);
}

BEGIN_MESSAGE_MAP(CNotificationWnd, CWnd)
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CNotificationWnd message handlers



void CNotificationWnd::OnMouseMove(UINT nFlags, CPoint point){
	if(!mouseInside){
	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);
	mouseInside=true;
	}
}

void CNotificationWnd::OnLButtonDown(UINT nFlags, CPoint point){
	if(point.x>168 && point.y>15 && point.x<183 && point.y<30){
		needClose=true;
	}
}

void CNotificationWnd::OnMouseLeave(){
	mouseInside=false;
}

void CNotificationWnd::OnClose(){
	con.clDlg->SendMessage(WM_HIDE_NOTIFICATION, ix);
	CWnd::OnClose();
}

void CNotificationWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	//CWnd::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
}


void CNotificationWnd::OnSetFocus(CWnd* pOldWnd)
{
	//CWnd::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
}


int CNotificationWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}
