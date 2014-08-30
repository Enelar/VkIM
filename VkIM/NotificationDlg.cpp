// NotificationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "NotificationDlg.h"
#include "Skin.h"
#include "Connector.h"
#include <GdiPlus.h>
#include "TalkDlg.h"

extern CSkin skin;
extern CConnector con;
extern CTalkDlg* talks;

// CNotificationDlg dialog

IMPLEMENT_DYNAMIC(CNotificationDlg, CDialog)

CNotificationDlg::CNotificationDlg(CWnd* pParent /*=NULL*/, wchar_t* title, wchar_t* content, int _idx, int _uid)
	: CDialog(CNotificationDlg::IDD, pParent)
{
	idx=_idx;
	uid=_uid;
	CDC scrdc;
	scrdc.Attach(::GetDC(0));
	bdc.CreateCompatibleDC(&scrdc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&scrdc, 200, 95);
	CBitmap* obmp=bdc.SelectObject(&bmp);
	skin.notifierBG.DrawUnscaled(&bdc, 0, 0, 200, 95, 0, 0);
	skin.tabCloseInact.DrawUnscaled(&bdc, 168, 15, 15, 15, 0, 0);
	std::wstring _content=content;
	int cl=_content.length();
	int nPos;
	while((nPos=_content.find(L"<br>"))!=-1){
		_content.replace(nPos, 4, L" ");
		cl-=3;
	}
	while((nPos=_content.find(L"&lt;"))!=-1){
		_content.replace(nPos, 4, L"<");
		cl-=3;
	}
	while((nPos=_content.find(L"&gt;"))!=-1){
		_content.replace(nPos, 4, L">");
		cl-=3;
	}
	while((nPos=_content.find(L"&quot;"))!=-1){
		_content.replace(nPos, 6, L"\"");
		cl-=5;
	}
	while((nPos=_content.find(L"&amp;"))!=-1){
		_content.replace(nPos, 5, L"&");
		cl-=4;
	}

	Gdiplus::Graphics g(bdc.GetSafeHdc());
    Gdiplus::FontFamily ff(_T("Tahoma"));
    Gdiplus::Font font(&ff, 11, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::Font font2(&ff, 11, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::PointF pt(15, 40);
	Gdiplus::RectF rc(15,55, 170, 27);
	Gdiplus::SolidBrush brush1(Gdiplus::Color(254, GetRValue(skin.notifyTitleColor), GetGValue(skin.notifyTitleColor), GetBValue(skin.notifyTitleColor)));
	Gdiplus::SolidBrush brush2(Gdiplus::Color(254, GetRValue(skin.notifyTextColor), GetGValue(skin.notifyTextColor), GetBValue(skin.notifyTextColor)));
    g.DrawString(title, wcslen(title), &font, pt, &brush1);
	g.DrawString(_content.c_str(), wcslen(content), &font2, rc, NULL, &brush2);

	::ReleaseDC(0, scrdc);

	mouseInside=true;
	needClose=false;
	_i=0;

	Create(IDD, pParent);
}

CNotificationDlg::~CNotificationDlg()
{
}

void CNotificationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNotificationDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SETFOCUS()
	ON_WM_ACTIVATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CNotificationDlg message handlers


BOOL CNotificationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE);
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, WS_POPUP);
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect, NULL);
	SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), rect.right-200, rect.bottom-(95*idx)+30, 0, 0, /*SWP_NOZORDER|*/SWP_NOSIZE|SWP_NOACTIVATE);
	ShowWindow(SW_SHOWNA);

		CDC scrdc;
	scrdc.Attach(::GetDC(0));
	BLENDFUNCTION func;
	func.BlendOp=AC_SRC_OVER;
	func.AlphaFormat=AC_SRC_ALPHA;
	func.SourceConstantAlpha=0;
	func.BlendFlags=0;
	UpdateLayeredWindow(&scrdc, &CPoint(rect.right-200, rect.bottom-(95*idx)+30), &CSize(200, 95), &bdc, &CPoint(0,0), 0, &func, ULW_ALPHA);
	::ReleaseDC(0, scrdc);

	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);
	cls=false;
	StartTransitionIn();

	//return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	return FALSE;
}


int CNotificationDlg::EaseOut(float t, float b, float c, float d){
	float ts=(t/=d)*t;
	float tc=ts*t;
	return b+c*(tc*ts + -5*ts*ts + 10*tc + -10*ts + 5*t);
}

int CNotificationDlg::EaseIn(float t, float b, float c, float d){
	float ts=(t/=d)*t;
	float tc=ts*t;
	return b+c*(tc*ts);
}

void CNotificationDlg::StartTransitionIn(){
	RECT rect;
	GetWindowRect(&rect);
	src_y=rect.top;
	dst_y=-30;
	SetTimer(100, 10, NULL);
}

void CNotificationDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
	con.clDlg->SendMessage(WM_HIDE_NOTIFICATION, idx);
}


void CNotificationDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(point.x>168 && point.y>15 && point.x<183 && point.y<30){
		//needClose=true;
		KillTimer(101);
		SetTimer(102, 10, NULL);
	}
	if(point.y>40 && uid>0){
		if(talks->openedTabs[uid]){
			for(int i=0;i<talks->tabs.GetItemCount();i++){
				TC_ITEM tci;
				tci.mask = TCIF_PARAM;
				talks->tabs.GetItem(i, &tci);
				CTalkTab* pWnd = (CTalkTab *)tci.lParam;
				if(pWnd)pWnd->ShowWindow(SW_HIDE);
				if(pWnd && pWnd->uid==uid){
					pWnd->hidden=false;
					pWnd->hasNewMsg=false;
					pWnd->ShowWindow(SW_SHOW);
					talks->tabs.SetCurSel(i);
				}
			}
		}else{
			if(con.clDlg->friendList.GetItemByUID(uid)){
				if(!talks->isCreated){
					con.clDlg->SendMessage(WM_OPENTAB);
				}
				talks->OpenNewTab(&con.clDlg->friendList.items[con.clDlg->friendList.GetItemByUID(uid)]);
			}
		}
		talks->ShowWindow(SW_RESTORE);
		talks->ShowWindow(SW_SHOW);
		talks->SetForegroundWindow();
		//needClose=true;
		KillTimer(101);
		SetTimer(102, 10, NULL);
	}
	if(point.y>40 && uid<0){
		wchar_t* url=NULL;
		switch((-uid)-1){
		case 0:
			url=L"http://vkontakte.ru/friends.php?filter=requests";
			break;
		case 1:
			url=L"http://vkontakte.ru/photos.php?act=added";
			break;
		case 2:
			url=L"http://vkontakte.ru/video.php?act=tagview";
			break;
		case 3:
			url=L"http://vkontakte.ru/notes.php?act=comms";
			break;
		case 4:
			url=L"http://vkontakte.ru/gifts.php";
			break;
		case 5:
			url=L"http://vkontakte.ru/events.php";
			break;
		case 6:
			url=L"http://vkontakte.ru/groups.php?filter=invitations";
			break;
		case 7:
			url=L"http://vkontakte.ru/opinions.php";
			break;
		case 8:
			url=L"http://vkontakte.ru/matches.php";
			break;
		case 9:
			url=L"http://vkontakte.ru/questions.php";
			break;
		}
		if(url)ShellExecute(GetSafeHwnd(), L"open", url, NULL, NULL, SW_SHOWNORMAL);
	}
	CDialog::OnLButtonDown(nFlags, point);
}


void CNotificationDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(!mouseInside){
	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);
	mouseInside=true;
	}
	CDialog::OnMouseMove(nFlags, point);
}


void CNotificationDlg::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	mouseInside=false;
	if(cls){
		SetTimer(102, 10, NULL);
	}
	CDialog::OnMouseLeave();
}


void CNotificationDlg::OnSetFocus(CWnd* pOldWnd)
{
	//CDialog::OnSetFocus(pOldWnd);
	//if(pOldWnd)pOldWnd->SetFocus();
	// TODO: Add your message handler code here
}


void CNotificationDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	//CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
}


void CNotificationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100){
		CRect rect;
		GetWindowRect(rect);
		int y=EaseOut(_i, src_y, dst_y+1, 50);
		SetWindowPos(NULL, rect.left, y, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		CDC scrdc;
		scrdc.Attach(::GetDC(0));
		BLENDFUNCTION func;
		func.BlendOp=AC_SRC_OVER;
		func.AlphaFormat=AC_SRC_ALPHA;
		func.SourceConstantAlpha=EaseOut(_i, 0, 255, 50);
		func.BlendFlags=0;
		UpdateLayeredWindow(&scrdc, &CPoint(rect.left, y), &CSize(200, 95), &bdc, &CPoint(0,0), 0, &func, ULW_ALPHA);
		::ReleaseDC(0, scrdc);
		_i++;
		if(_i==50){
			KillTimer(100);
			SetTimer(101, 3000, NULL);
			_i=0;
		}
	}
	if(nIDEvent==101){
		KillTimer(101);
		if(!mouseInside){
			SetTimer(102, 10, NULL);
		}else{
			cls=true;
		}
	}
	if(nIDEvent==102){
		CRect rect;
		GetWindowRect(rect);
		CDC scrdc;
		scrdc.Attach(::GetDC(0));
		BLENDFUNCTION func;
		func.BlendOp=AC_SRC_OVER;
		func.AlphaFormat=AC_SRC_ALPHA;
		func.SourceConstantAlpha=EaseIn(_i, 255, -255, 50);
		func.BlendFlags=0;
		UpdateLayeredWindow(&scrdc, &CPoint(rect.left, src_y+dst_y), &CSize(200, 95), &bdc, &CPoint(0,0), 0, &func, ULW_ALPHA);
		::ReleaseDC(0, scrdc);
		_i++;
		if(_i==50){
			KillTimer(102);
			PostMessage(WM_CLOSE);
		}
	}
	CDialog::OnTimer(nIDEvent);
}
