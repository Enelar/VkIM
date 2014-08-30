// ExEdit.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "ExEdit.h"
#include "VkIMDlg.h"
#include "Settings.h"
#include "TalkDlg.h"

extern CTalkDlg* talks;
// CExEdit

IMPLEMENT_DYNAMIC(CExEdit, CEdit)

CExEdit::CExEdit()
{
	ctrlDown=false;
	callbackMsg=WM_KEYDOWN;
	convTableRU=L"ÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞ,éöóêåíãøùçõúôûâàïğîëäæıÿ÷ñìèòüáş.";
	convTableEN=L"QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?qwertyuiop[]asdfghjkl;'zxcvbnm,./";
}

CExEdit::~CExEdit()
{
}


BEGIN_MESSAGE_MAP(CExEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()



// CExEdit message handlers




void CExEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//TRACE("OnKeyDown(%i[%c], %i, %i)\n", nChar, nChar, nRepCnt, nFlags);
	if(callbackMsg==WM_KEYDOWN){
		if(CSettings::sendWithCtrlEnter){
			if(nChar==VK_RETURN && ctrlDown && nRepCnt==1){
				GetParent()->SendMessage(callbackMsg, VK_RETURN);
				PostMessage(EM_SETSEL, 0, -1);
				PostMessage(WM_CLEAR);
				return;
			}
			if(nChar==VK_RETURN && !ctrlDown && nRepCnt==1){
				ReplaceSel(L"\r\n", 1);
				return;
			}
		}else{
			if(nChar==VK_RETURN && !ctrlDown && nRepCnt==1){
				GetParent()->SendMessage(callbackMsg, VK_RETURN);
				return;
			}
		}
	}else{
	if(nChar==VK_RETURN && !ctrlDown && nRepCnt==1){
		GetParent()->SendMessage(callbackMsg, VK_RETURN);
		return;
	}
	}
	if(nChar=='R' && ctrlDown){
		wchar_t* tbuf=new wchar_t[GetWindowTextLength()+1];
		GetWindowText(tbuf, -1);
		for(int i=0;i<wcslen(tbuf);i++){
			bool repl=false;
			for(int j=0;j<wcslen(convTableEN);j++){
				if(convTableEN[j]==tbuf[i]){
					tbuf[i]=convTableRU[j];
					repl=true;
					break;
				}
			}
			if(!repl){
				for(int j=0;j<wcslen(convTableRU);j++){
					if(convTableRU[j]==tbuf[i]){
						tbuf[i]=convTableEN[j];
						repl=true;
						break;
					}
				}
			}
		}
		SetWindowText(tbuf);
		delete tbuf;
		return;
	}
	if(callbackMsg==WM_KEYDOWN && ctrlDown && nChar=='W'){
		CWnd* par=GetParent();
		if(par)par->SendMessage(WM_CLOSE);
	}
	if(nChar==VK_CONTROL && nRepCnt==1){
		ctrlDown=true;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CExEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if(nChar==VK_CONTROL){
		ctrlDown=false;
	}
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CExEdit::OnSetFocus(CWnd* pOldWnd)
{
	TRACE("Setfocus\n");
	CEdit::OnSetFocus(pOldWnd);
	//if(callbackMsg==WM_KEYDOWN){
		//con.clDlg->SetActiveWindow();
		//talks->SetActiveWindow();
	//}

	
	// TODO: Add your message handler code here
}


void CExEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	//DestroyCaret();
}


void CExEdit::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CEdit::OnActivate(nState, pWndOther, bMinimized);
	TRACE("Activate\n");
	// TODO: Add your message handler code here
}
