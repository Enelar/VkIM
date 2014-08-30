// SearchEdit.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "SearchEdit.h"
#include "Skin.h"
#include "Lang.h"

extern CSkin skin;
// CSearchEdit

IMPLEMENT_DYNAMIC(CSearchEdit, CEdit)

CSearchEdit::CSearchEdit()
{
	hasFocus=false;
}

CSearchEdit::~CSearchEdit()
{
}


BEGIN_MESSAGE_MAP(CSearchEdit, CEdit)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CSearchEdit message handlers




BOOL CSearchEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetWindowRect(rect);
	skin.searchBG.bmp.DrawStretched(pDC, 0, 0, rect.Width(), rect.Height(), 0, 0, 0, 0);
	skin.searchIcon.DrawUnscaled(pDC, 2, rect.Height()/2-8, 16, 16, 0, 0);
	if(!hasFocus){
		rect.DeflateRect(20, 0);
		rect.MoveToXY(20, 0);
		pDC->SetTextColor(skin.searchPlaceholderColor);
		pDC->SetBkMode(TRANSPARENT);
		CFont* fnt=pDC->SelectObject(GetFont());
		pDC->DrawText(CLang::Get(L"search"), -1, rect, DT_VCENTER|DT_SINGLELINE);
		pDC->SelectObject(fnt);
	}
	if(GetWindowTextLength()>0){
		skin.searchClear.DrawUnscaled(pDC, rect.Width()-18, rect.Height()/2-8, 16, 16, 0, 0);
	}
	return true;
}


void CSearchEdit::Resize(void)
{
	if(hasFocus){
		CRect rect;
		GetRect(rect);
		if(rect.left<15){
			rect.left=0;
		rect.DeflateRect(20, 0);
		}
		CRect wrect;
		GetWindowRect(wrect);
		rect.OffsetRect(0, -rect.top);
		rect.OffsetRect(0, wrect.Height()/2-rect.Height()/2);
		SetRect(rect);
		RedrawWindow();
	}else{
		CRect wrect;
		GetWindowRect(wrect);
		CRect rect;
		GetRect(rect);
		rect.OffsetRect(0, wrect.Height());
		SetRect(rect);
		Invalidate();
	}
}


void CSearchEdit::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);
	Resize();
	// TODO: Add your message handler code here
}


int CSearchEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	Resize();
	return 0;
}


void CSearchEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEdit::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	Resize();
}


void CSearchEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);
	hasFocus=true;
	Resize();
	// TODO: Add your message handler code here
}


void CSearchEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	if(GetWindowTextLength()==0){
	hasFocus=false;
	Resize();
	}
	// TODO: Add your message handler code here
}


HBRUSH CSearchEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  Change any attributes of the DC here
	//pDC->FillSolidRect(0, 0, 10, 10, RGB(255,0,255));
	// TODO:  Return a non-NULL brush if the parent's handler should not be called
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}


void CSearchEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	Invalidate();
}


void CSearchEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetWindowRect(rect);
	if(point.x>rect.Width()-20){
		SetSel(0, GetWindowTextLength());
		Clear();
		Invalidate();
		GetParent()->SetFocus();
	}else{
		CEdit::OnLButtonDown(nFlags, point);
	}
}
