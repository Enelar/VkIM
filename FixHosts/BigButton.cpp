// BigButton.cpp : implementation file
//

#include "stdafx.h"
#include "FixHosts.h"
#include "BigButton.h"
#include <uxtheme.h>


// CBigButton

IMPLEMENT_DYNAMIC(CBigButton, CButton)

CBigButton::CBigButton()
{
	fntBig.CreateFontW(-13, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	fntSmall.CreateFontW(-11, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Tahoma");
	arrowN=(HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ARROW_N), IMAGE_ICON, 32, 32, 0);
	arrowH=(HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ARROW_H), IMAGE_ICON, 32, 32, 0);

	HMODULE hm=LoadLibrary(L"uxtheme.dll");
	if(hm){
		hasUxtheme=true;
		_IsThemeActive=(ISTHEMEACTIVE*)GetProcAddress(hm, "IsThemeActive");
		_OpenThemeData=(OPENTHEMEDATA*)GetProcAddress(hm, "OpenThemeData");
		_CloseThemeData=(CLOSETHEMEDATA*)GetProcAddress(hm, "CloseThemeData");
		_DrawThemeBackground=(DRAWTHEMEBACKGROUND*)GetProcAddress(hm, "DrawThemeBackground");
		FreeLibrary(hm);
	}else hasUxtheme=false;

	mouseInside=false;
	TRACKMOUSEEVENT tme;
	memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_LEAVE;
	tme.hwndTrack=GetSafeHwnd();
	::TrackMouseEvent(&tme);
}

CBigButton::~CBigButton()
{
}


BEGIN_MESSAGE_MAP(CBigButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


void CBigButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   UINT uStyle = DFCS_BUTTONPUSH;

   // This code only works with buttons.
   ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

   // If drawing selected, add the pushed style to DrawFrameControl.
   if (lpDrawItemStruct->itemState & ODS_SELECTED)
      uStyle |= DFCS_PUSHED;

   CDC* dc=CDC::FromHandle(lpDrawItemStruct->hDC);
   dc->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_3DFACE));

   if(mouseInside){
   if(hasUxtheme && _IsThemeActive()){
	   HANDLE hTheme=_OpenThemeData(NULL, L"TOOLBAR");
	   if(lpDrawItemStruct->itemState & ODS_SELECTED)
		_DrawThemeBackground(hTheme, lpDrawItemStruct->hDC, TP_BUTTON, TS_PRESSED, &lpDrawItemStruct->rcItem, NULL);
	   else
		   _DrawThemeBackground(hTheme, lpDrawItemStruct->hDC, TP_BUTTON, TS_HOT, &lpDrawItemStruct->rcItem, NULL);
	   _CloseThemeData(hTheme);
   }else{
   ::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
      DFC_BUTTON, uStyle);
   CRect rr=lpDrawItemStruct->rcItem;
   rr.DeflateRect(1, 1);
   dc->FillSolidRect(rr, GetSysColor(COLOR_3DFACE));
   }
   }

   /*if(lpDrawItemStruct->itemState & ODS_FOCUS){
		CRect frect=lpDrawItemStruct->rcItem;
		frect.DeflateRect(3, 3);
		dc->DrawFocusRect(frect);
   }*/

   // Get the button's text.
   CString strText;
   GetWindowText(strText);

   dc->DrawIcon(0,0,mouseInside?arrowH:arrowN);
   dc->SetBkMode(TRANSPARENT);
   // Draw the button text using the text color red.
   dc->SetTextColor(0);
   CFont* oldFont=dc->SelectObject(&fntBig);
   RECT l1rect=lpDrawItemStruct->rcItem;
   l1rect.left+=32;
   l1rect.bottom=l1rect.top+32;
   dc->DrawText(strText, strText.GetLength(), &l1rect, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

   dc->SelectObject(fntSmall);
	RECT l2rect=lpDrawItemStruct->rcItem;
   l2rect.top+=32;
   l2rect.left+=5;
	l2rect.right-=5;
	dc->DrawText(description, description.GetLength(), &l2rect, DT_TOP|DT_LEFT|DT_WORDBREAK);
   //::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
   dc->SelectObject(oldFont);
}


// CBigButton message handlers

void CBigButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(!mouseInside){
		mouseInside=true;
		TRACKMOUSEEVENT tme;
		memset(&tme, 0, sizeof(TRACKMOUSEEVENT));
		tme.cbSize=sizeof(TRACKMOUSEEVENT);
		tme.dwFlags=TME_LEAVE;
		tme.hwndTrack=GetSafeHwnd();
		::TrackMouseEvent(&tme);
		RedrawWindow();
	}
	CButton::OnMouseMove(nFlags, point);
}

void CBigButton::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	mouseInside=false;
	RedrawWindow();
	CButton::OnMouseLeave();
}
