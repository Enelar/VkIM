// TalkTab.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "TalkTab.h"
#include "Connector.h"
#include "Lang.h"
#include "TalkDlg.h"
#include "Skin.h"
#include "Settings.h"

extern CConnector con;
extern CSkin skin;
extern CTalkDlg* talks;

// CTalkTab dialog
extern unsigned int uid;

IMPLEMENT_DYNAMIC(CTalkTab, CDialog)

CTalkTab::CTalkTab(int _uid, CWnd* pParent, bool newMsg, bool _hidden, wstring _previewURL)
	: CDialog(CTalkTab::IDD, pParent)
{
	progressFrame=0;
	uid=_uid;
	hasNewMsg=newMsg;
	hidden=_hidden;
	msgCount=0;
#ifdef _RLS2000
	InitControlContainer();
#endif
#if _MSC_VER==1500
	InitControlContainer();
#endif
	previewURL=_previewURL;
	if(talks->tb)tabProxy=new CTabProxy(this);
	else tabProxy=NULL;
}

CTalkTab::~CTalkTab()
{
	//delete user_name;
	if(tabProxy)delete tabProxy;
}

void CTalkTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_FIELD, msgField);
	DDX_Control(pDX, IDC_SEND_BTN, sendBtn);
}


BEGIN_MESSAGE_MAP(CTalkTab, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SEND_BTN, &CTalkTab::OnBnClickedSendBtn)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_EN_UPDATE(IDC_MSG_FIELD, &CTalkTab::OnEnUpdateMsgField)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_MESSAGE(WM_HISTORY_LOADED, &CTalkTab::OnHistoryLoaded)
	ON_MESSAGE(WM_ADD_MESSAGE, &CTalkTab::OnAddMessage)
	ON_MESSAGE(WM_SET_READ, &CTalkTab::OnSetRead)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTalkTab message handlers


void CTalkTab::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if(msgField){
		RECT rect;
		msgField.GetWindowRect(&rect);
		msgField.SetWindowPos(NULL, 0, cy-105, cx, rect.bottom-rect.top, SWP_NOZORDER);
		sendBtn.SetWindowPos(NULL, cx-106, cy-34, 100, 28, SWP_NOZORDER);
		msgBoard.SetWindowPos(NULL, 0, 0, cx, cy-110, SWP_NOZORDER);
	}
}


void CTalkTab::AddIncomingMessage(wchar_t* msg, int* md){
	if(IsWindowVisible() && CWnd::GetActiveWindow()==talks){
		MarkMsgsAsRead();
	}
	if(!IsWindowVisible() || CWnd::GetActiveWindow()!=talks){
		hasNewMsg=true;
		talks->tabs.RedrawWindow();
	}
	int mid=md[0];
	unsigned long mt=md[1];
		char* buf=new char[128];
		time_t _t=time(NULL);
		if(mt!=0)_t=mt;
		tm* t=localtime(&_t);
		wchar_t* wbuf;
		if(mt==0){
			strftime(buf, 128, "%H:%M:%S", t);
			wbuf=CUtils::ctow(buf, 128);
			msgBoard.AddMessage(user_name.c_str(), msg, uid, false, wbuf, mid);
		}else{
			strftime(buf, 128, "%d [%m] %Y, %H:%M", t);
			wbuf=CUtils::ctow(buf, 128);
			std::wstring wb=wbuf;
			int nPos;
			if((nPos=wb.find(L"[01]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month1"));
			if((nPos=wb.find(L"[02]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month2"));
			if((nPos=wb.find(L"[03]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month3"));
			if((nPos=wb.find(L"[04]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month4"));
			if((nPos=wb.find(L"[05]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month5"));
			if((nPos=wb.find(L"[06]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month6"));
			if((nPos=wb.find(L"[07]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month7"));
			if((nPos=wb.find(L"[08]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month8"));
			if((nPos=wb.find(L"[09]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month9"));
			if((nPos=wb.find(L"[10]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month10"));
			if((nPos=wb.find(L"[11]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month11"));
			if((nPos=wb.find(L"[12]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month12"));
			msgBoard.AddMessage(user_name.c_str(), msg, uid, false, (wchar_t*)wb.c_str(), mid);
		}

		
		msgCount++;
		delete buf;
		delete wbuf;
}


void CTalkTab::OnBnClickedSendBtn()
{
	// TODO: Add your control notification handler code here
	DoSend();
}


void CTalkTab::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if(nChar==VK_RETURN){
		DoSend();
		return;
	}
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CTalkTab::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//TRACE("%c\n", nChar);
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}


void CTalkTab::OnEnUpdateMsgField()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
}


BOOL CTalkTab::OnInitDialog(){
	CDialog::OnInitDialog();
	sendBtn.SetWindowTextW(CLang::Get(L"send"));

	msgBoard.Create(CMessageBoard::IDD, this);

	CRect rect;
	GetClientRect(rect);

	msgField.SetWindowPos(NULL, 0, 0, rect.Width(), 65, SWP_NOMOVE|SWP_NOZORDER);

	if(CSettings::loadHistory){
		SetTimer(100, 250, NULL);
		con.GetHistory(this);
		historyLoaded=false;
		msgBoard.ShowWindow(SW_HIDE);
	}else{
		historyLoaded=true;
		msgBoard.ShowWindow(SW_SHOW);
		msgBoard.ie.SetParent(this);
	}

	return true;
}

void CTalkTab::DoSend(void)
{
	if(msgField.GetWindowTextLengthW()<2)return;
	wchar_t* buf=new wchar_t[msgField.GetWindowTextLengthW()+2];
	memset(buf, 0, msgField.GetWindowTextLengthW()+2);
	msgField.GetWindowTextW(buf, msgField.GetWindowTextLengthW()+1);
	msgField.SetWindowTextW(L"");
	int tmpid=con.SendMsg(buf, uid);

		char* _buf=new char[128];
		time_t _t=time(NULL);
		tm* t=localtime(&_t);
		strftime(_buf, 128, "%H:%M:%S", t);
		wchar_t* wbuf=CUtils::ctow(_buf, 128);

		msgBoard.AddMessage(con.my_name, buf, ::uid, true, wbuf, 0, false, false, tmpid);
		msgCount++;
		delete _buf;
		delete wbuf;
}


void CTalkTab::MarkMsgsAsRead(void)
{
	if(!this)return;
	if(hidden)return;
	if(unreadIDs.size()==0)return;
	if(!IsWindowVisible())return;
	hasNewMsg=false;
	talks->tabs.RedrawWindow();
	std::wstring ids=L"";
		for(int i=0;i<unreadIDs.size();i++){
			wchar_t* buf=new wchar_t[15];
			_itow(unreadIDs[i], buf, 10);
			ids+=buf;
			if(i!=unreadIDs.size()-1)ids+=L",";
		}
		wchar_t* ib=new wchar_t[ids.length()+1];
		wcscpy(ib, ids.c_str());
		con.MarkAsRead(ib);
		unreadIDs.clear();
}


BOOL CTalkTab::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	RECT rect;
	GetWindowRect(&rect);
	int w=rect.right-rect.left;
	int h=rect.bottom-rect.top;

	CRect wrect;
	msgBoard.GetWindowRect(wrect);
	wrect.OffsetRect(-wrect.left, -wrect.top);
	CPoint center=wrect.CenterPoint();

	if(!historyLoaded){
	pDC->FillSolidRect(0, 0, w, h, RGB(255,255,255));
	skin.progress.bmp.DrawUnscaled(pDC, center.x-skin.progress.w/2, center.y-skin.progress.h/2, skin.progress.w, skin.progress.h, skin.progress.w*progressFrame, 0);
	}
	skin.talkBottomBar.bmp.DrawStretched(pDC, 0, h-40, w, 40, 0, 0, 0, 0);
	skin.talkDivider.bmp.DrawStretched(pDC, 0, h-110, w, 5, 0, 0, 0, 0);

	

	return true;//CDialog::OnEraseBkgnd(pDC);
}


void CTalkTab::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
	TRACE("showWindow %i\n", bShow);
	//Invalidate();
	//RedrawWindow();
}


void CTalkTab::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}


void CTalkTab::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
}


void CTalkTab::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==100){
		CRect rect;
		GetClientRect(rect);
		Invalidate();
		progressFrame++;
		progressFrame%=skin.progress.n;
	}
	CDialog::OnTimer(nIDEvent);
}


afx_msg LRESULT CTalkTab::OnHistoryLoaded(WPARAM wParam, LPARAM lParam)
{
	HistoryEntry* entries=(HistoryEntry*)wParam;
	if(!historyLoaded){
	for(int i=lParam-1;i>=0;i--){
		msgCount++;
		const wchar_t* name=entries[i].from==uid?user_name.c_str():con.my_name;
		char* buf=new char[128];
		tm* t=localtime(&entries[i].time);
		strftime(buf, 128, "%d [%m] %Y, %H:%M", t);
		wchar_t* wbuf=CUtils::ctow(buf, 128);
		std::wstring wb=wbuf;
		int nPos;
		if((nPos=wb.find(L"[01]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month1"));
		if((nPos=wb.find(L"[02]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month2"));
		if((nPos=wb.find(L"[03]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month3"));
		if((nPos=wb.find(L"[04]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month4"));
		if((nPos=wb.find(L"[05]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month5"));
		if((nPos=wb.find(L"[06]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month6"));
		if((nPos=wb.find(L"[07]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month7"));
		if((nPos=wb.find(L"[08]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month8"));
		if((nPos=wb.find(L"[09]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month9"));
		if((nPos=wb.find(L"[10]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month10"));
		if((nPos=wb.find(L"[11]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month11"));
		if((nPos=wb.find(L"[12]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month12"));

		msgBoard.AddMessage(entries[i].from==uid?user_name.c_str():con.my_name, entries[i].text, entries[i].from, !(entries[i].from==uid), (wchar_t*)wb.c_str(), 0);

		delete buf;
		delete wbuf;
		delete entries[i].text;
	}
	msgBoard.AddDivider();

	KillTimer(100);
	//msgBoard.ShowWindow(SW_SHOW);
	msgBoard.ie.SetParent(this);
	historyLoaded=true;
	}else{
		for(int i=0;i<lParam;i++){
			msgCount++;
		const wchar_t* name=entries[i].from==uid?user_name.c_str():con.my_name;
		char* buf=new char[128];
		tm* t=localtime(&entries[i].time);
		strftime(buf, 128, "%d [%m] %Y, %H:%M", t);
		wchar_t* wbuf=CUtils::ctow(buf, 128);
		std::wstring wb=wbuf;
		int nPos;
		if((nPos=wb.find(L"[01]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month1"));
		if((nPos=wb.find(L"[02]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month2"));
		if((nPos=wb.find(L"[03]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month3"));
		if((nPos=wb.find(L"[04]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month4"));
		if((nPos=wb.find(L"[05]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month5"));
		if((nPos=wb.find(L"[06]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month6"));
		if((nPos=wb.find(L"[07]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month7"));
		if((nPos=wb.find(L"[08]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month8"));
		if((nPos=wb.find(L"[09]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month9"));
		if((nPos=wb.find(L"[10]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month10"));
		if((nPos=wb.find(L"[11]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month11"));
		if((nPos=wb.find(L"[12]"))!=-1)wb.replace(nPos, 4, CLang::Get(L"month12"));

		msgBoard.AddMessage(entries[i].from==uid?user_name.c_str():con.my_name, entries[i].text, entries[i].from, !(entries[i].from==uid), (wchar_t*)wb.c_str(), 0, true, true);

		delete buf;
		delete wbuf;
		delete entries[i].text;
	}
		msgBoard.loadMore->put_className(L"loadMoreLink");
	}
	return 0;
}


afx_msg LRESULT CTalkTab::OnAddMessage(WPARAM wParam, LPARAM lParam)
{
	AddIncomingMessage((wchar_t*)wParam, (int*)lParam);
	return 0;
}


afx_msg LRESULT CTalkTab::OnSetRead(WPARAM wParam, LPARAM lParam)
{
	msgBoard.SetReadState(wParam);
	return 0;
}


void CTalkTab::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if(hidden){
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	CDialog::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
}


void CTalkTab::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
	if(talks->tb){
	int idx=0;
	for(int i=0;i<talks->tabs.GetItemCount();i++){
		TC_ITEM tci;
		tci.mask = TCIF_PARAM;
		talks->tabs.GetItem(i, &tci);
		CWnd* pWnd = (CWnd *)tci.lParam;
		if(pWnd==this){
			idx=i;
			break;
		}
	}
	LRESULT r;
	talks->OnTcnSelchangingTab1(NULL, &r);
	talks->tabs.SetCurSel(idx);
	talks->OnTcnSelchangeTab1(NULL, &r);
	talks->ShowWindow(SW_RESTORE);
	talks->SetActiveWindow();
	msgBoard.ScrollToBottom();
	}
}


void CTalkTab::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(talks->tb){
	int idx=0;
	for(int i=0;i<talks->tabs.GetItemCount();i++){
		TC_ITEM tci;
		tci.mask = TCIF_PARAM;
		talks->tabs.GetItem(i, &tci);
		CWnd* pWnd = (CWnd *)tci.lParam;
		if(pWnd==this){
			idx=i;
			break;
		}
	}
	talks->tabs.CloseTab(idx);
	}
}


////////////////////////////////////////////
// CTabProxy
////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CTabProxy, CWnd)

CTabProxy::CTabProxy(CTalkTab* pr){
	pwnd=pr;
	CreateEx(WS_EX_NOACTIVATE, ::AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW), L"", WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION, CRect(-32000, -32000, -32000, -32000), talks, 0);
	SetAttributes();
	AfxBeginThread((AFX_THREADPROC)&LoadPreviewBitmap, this);
}

void CTabProxy::SetAttributes(){
	BOOL fForceIconic = 1;
    BOOL fHasIconicBitmap = 1;
	BOOL isEnabled;
	talks->_DwmIsCompositionEnabled(&isEnabled);
	
	if(isEnabled){
            talks->_DwmSetWindowAttribute(
                m_hWnd,
                DWMWA_FORCE_ICONIC_REPRESENTATION,
                &fForceIconic,
                sizeof(BOOL));
			talks->_DwmSetWindowAttribute(
                m_hWnd,
                DWMWA_HAS_ICONIC_BITMAP,
                &fHasIconicBitmap,
                sizeof(BOOL));
			HRGN hRgn=CreateRectRgn(1, 1, 500, 500);
			SetWindowRgn(hRgn, false);
	}
}

CTabProxy::~CTabProxy(){

}

BEGIN_MESSAGE_MAP(CTabProxy, CWnd)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_DWMSENDICONICTHUMBNAIL, &CTabProxy::OnDwmsendiconicthumbnail)
	ON_MESSAGE(WM_DWMSENDICONICLIVEPREVIEWBITMAP, &CTabProxy::OnDwmsendiconiclivepreviewbitmap)
END_MESSAGE_MAP()


void CTabProxy::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	// TODO: Add your message handler code here
	pwnd->SendMessage(WM_ACTIVATE);
}


void CTabProxy::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	pwnd->SendMessage(WM_CLOSE);
}

HBITMAP CTabProxy::CreateDIB(int nWidth, int nHeight, int bgcolor)
{
	HBITMAP bmp2=pwnd->previewImage;
	if(!bmp2)return NULL;

	CDC* sdc=CDC::FromHandle(CreateCompatibleDC(NULL));
	HBITMAP oldBmpS=(HBITMAP)sdc->SelectObject(bmp2);
	BITMAPINFO bi;
    memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    
    bi.bmiHeader.biWidth = 100;    
    bi.bmiHeader.biHeight = 100;  
    bi.bmiHeader.biPlanes = 1;    
    bi.bmiHeader.biBitCount = 32;    
	unsigned int* bits=(unsigned int*)calloc(10000, sizeof(unsigned int));
	GetDIBits(sdc->m_hDC, bmp2, 0, 100, bits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    
    bi.bmiHeader.biWidth = 100;    
    bi.bmiHeader.biHeight = -100;  
    bi.bmiHeader.biPlanes = 1;    
    bi.bmiHeader.biBitCount = 32;
	sdc->SelectObject(oldBmpS);
	unsigned int* nbits;
	HBITMAP bmp=CreateDIBSection(sdc->m_hDC, &bi, DIB_RGB_COLORS, (void**)&nbits, 0, 0);
	for(int i=0;i<100;i++)memcpy(nbits+i*100, bits+(99-i)*100, 400);
	delete bits;
	return bmp;
}

HBITMAP CTabProxy::CreateDIBPreview(int _w, int _h)
{
    /*HBITMAP hbm = NULL;
    HDC hdcMem = CreateCompatibleDC(NULL);
    LPBYTE   pbDS;
	HBITMAP bmp2=NULL;
	for(int i=0;i<con.clDlg->friendList.items.size();i++){
		if(con.clDlg->friendList.items[i].uid==pwnd->uid){
			bmp2=con.clDlg->friendList.items[i].userPhoto;
			break;
		}
	}
	if(!bmp2)return NULL;
	CDC* mdc=pwnd->GetDC();

    if (hdcMem != NULL)
    {
        BITMAPINFO bmi;
        ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = nWidth;
        bmi.bmiHeader.biHeight = -nHeight;  // Use a top-down DIB
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;

        pbDS = NULL;
        hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, (LPVOID *)&pbDS, NULL, NULL);
        if (hbm != NULL)
        {
            int nGreen, nRed, nBlue;
            nRed = 0; nGreen= 185; nBlue = 242;

            // Fill in the pixels of the bitmap
            for (int y = 0; y < nHeight; y++)
            {
                for (int x = 0; x < nWidth; x++)
                {
                    pbDS[3] = 255;
					COLORREF px=mdc->GetPixel(x, y);
                    pbDS[2] = GetRValue(px);
                    pbDS[1] = GetGValue(px);
                    pbDS[0] = GetBValue(px);
					pbDS += 4;
                }
            }
        }
		pwnd->ReleaseDC(mdc);
        DeleteDC(hdcMem);
    }
    return hbm;*/
	CDC* wdc=pwnd->GetDC();
	CDC* sdc=CDC::FromHandle(::CreateCompatibleDC(wdc->m_hDC));
	CRect rect;
	wdc->GetClipBox(rect);
	int w=rect.Width();
	int h=rect.Height();
	HBITMAP bmp2=CreateCompatibleBitmap(wdc->m_hDC, w, h);
	HBITMAP oldBmp=(HBITMAP)sdc->SelectObject(bmp2);
	sdc->BitBlt(0, 0, w, h, wdc, 0, 0, SRCCOPY);
	BITMAPINFO bi;
    memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    
    bi.bmiHeader.biWidth = w;    
    bi.bmiHeader.biHeight = h;  
    bi.bmiHeader.biPlanes = 1;    
    bi.bmiHeader.biBitCount = 32;    
	unsigned int* bits=(unsigned int*)calloc(w*h, sizeof(unsigned int));
	GetDIBits(sdc->m_hDC, bmp2, 0, w, bits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    
    bi.bmiHeader.biWidth = w;    
    bi.bmiHeader.biHeight = -h;  
    bi.bmiHeader.biPlanes = 1;    
    bi.bmiHeader.biBitCount = 32;
	unsigned int* nbits;
	HBITMAP bmp=CreateDIBSection(sdc->m_hDC, &bi, DIB_RGB_COLORS, (void**)&nbits, 0, 0);
	//for(int i=0;i<h;i++)memcpy(nbits+i*w, bits+((h-1)-i)*w, w*4);
	memcpy(nbits, bits, w*h*4);
	delete bits;
	pwnd->ReleaseDC(wdc);
	sdc->SelectObject(oldBmp);
	sdc->DeleteDC();
	return bmp;
}

afx_msg LRESULT CTabProxy::OnDwmsendiconicthumbnail(WPARAM wParam, LPARAM lParam)
{
	TRACE("send thumbnail\n");
	HBITMAP bmp=CreateDIB(100, 100);
	HRESULT r=talks->_DwmSetIconicThumbnail(m_hWnd, bmp, 0);
	TRACE("HRESULT=%i\n", r);
	DeleteObject(bmp);
	return 0;
}

afx_msg LRESULT CTabProxy::OnDwmsendiconiclivepreviewbitmap(WPARAM wParam, LPARAM lParam)
{
	TRACE("send preview\n");
	CRect rect;
	pwnd->GetWindowRect(rect);
	//HBITMAP bmp=CreateDIBPreview(rect.Width(), rect.Height());
	HBITMAP bmp=CreateDIB(rect.Width(), rect.Height());
	POINT pt={rect.Width()/2-50, 25+rect.Height()/2-50};
	HRESULT r=talks->_DwmSetIconicLivePreviewBitmap(m_hWnd, bmp, &pt, 0);
	TRACE("HRESULT=%i\n", r);
	DeleteObject(bmp);
	
	return 0;
}


UINT CTabProxy::LoadPreviewBitmap(CTabProxy* px)
{
	wchar_t* fnm=con.DownloadImage((wchar_t*)px->pwnd->previewURL.c_str());
	px->pwnd->previewImage=CEnBitmap::LoadImageFile(fnm);
	talks->_DwmInvalidateIconicBitmaps(px->m_hWnd);
	delete fnm;
	return 0;
}
