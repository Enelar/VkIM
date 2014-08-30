// Tooltip.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "Tooltip.h"
#include "Connector.h"

// CTooltip
extern CConnector con;

IMPLEMENT_DYNAMIC(CTooltip, CWnd)

CTooltip::CTooltip()
{
}

CTooltip::~CTooltip()
{
}


BEGIN_MESSAGE_MAP(CTooltip, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CTooltip message handlers




void CTooltip::ShowTooltip(CPoint pnt, wchar_t* text)
{
	if(IsWindowVisible()){
		ShowWindow(SW_HIDE);
	}
	SetWindowText(text);
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CFont fnt;
	fnt.CreateFontW(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	CFont* oldFont=dc.SelectObject(&fnt);
	CSize sz=dc.GetTextExtent(text, wcslen(text));
	dc.SelectObject(oldFont);
	SetWindowPos(CWnd::FromHandle(HWND_TOPMOST), pnt.x, pnt.y, sz.cx+6, 18, SWP_NOACTIVATE);
	ShowWindow(SW_SHOWNA);
	RedrawWindow();
}

void CTooltip::HideTooltip(void)
{
	if(IsWindowVisible()){
		ShowWindow(SW_HIDE);
	}
}

void CTooltip::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	COLORREF bordercolor=GetSysColor(COLOR_INFOTEXT);
	COLORREF bgcolor=GetSysColor(COLOR_INFOBK);
	CRect rect;
	GetWindowRect(rect);
	dc.FillSolidRect(0, 0, rect.Width(), rect.Height(), bordercolor);
	dc.FillSolidRect(1, 1, rect.Width()-2, rect.Height()-2, bgcolor);
	CFont fnt;
	fnt.CreateFontW(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	CFont* oldFont=dc.SelectObject(&fnt);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(bordercolor);
	CString txt;
	GetWindowText(txt);
	rect.OffsetRect(-rect.left, -rect.top);
	dc.DrawText(txt, rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	dc.SelectObject(oldFont);
}


void CTooltip::init(void)
{
	CreateEx(0, L"Static", L"", WS_POPUP, CRect(0,0,10,10), con.clDlg->GetParent(), 0);
	SetClassLong(GetSafeHwnd(), GCL_STYLE, GetClassLong(GetSafeHwnd(), GCL_STYLE)|CS_DROPSHADOW);
}
