#pragma once
#include "afxcmn.h"
#include <string>
#include <vector>
#include "afxwin.h"
#include "ExEdit.h"
#include "SkinButton.h"
#include "stdafx.h"
#include "MessageBoard.h"

class CTalkTab;

using namespace std;

class CTabProxy: public CWnd
{
	DECLARE_DYNAMIC(CTabProxy)
public:
	CTabProxy(CTalkTab* pr);
	~CTabProxy();
	void SetAttributes();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	CTalkTab* pwnd;
	HBITMAP CreateDIB(int nWidth, int nHeight, int bgcolor=0);
	HBITMAP CreateDIBPreview(int nWidth, int nHeight);
	afx_msg LRESULT OnDwmsendiconicthumbnail(WPARAM wParam, LPARAM lParam);
	
protected:
	afx_msg LRESULT OnDwmsendiconiclivepreviewbitmap(WPARAM wParam, LPARAM lParam);
public:
	static UINT LoadPreviewBitmap(CTabProxy* px);
};

// CTalkTab dialog

class CTalkTab : public CDialog
{
	DECLARE_DYNAMIC(CTalkTab)

public:
	CTalkTab(int _uid, CWnd* pParent = NULL, bool newMsg=false, bool _hidden=false, wstring _previewURL=L"");   // standard constructor
	virtual ~CTalkTab();

// Dialog Data
	enum { IDD = IDD_TALKTAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CExEdit msgField;
	CSkinButton sendBtn;

	int uid;
	afx_msg void OnBnClickedSendBtn();
	void AddIncomingMessage(wchar_t* msg, int* md);
	wstring user_name;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnUpdateMsgField();
	BOOL OnInitDialog();
	void DoSend(void);
	std::vector<int> unreadIDs;
	std::vector<int> sentIDs;
	void MarkMsgsAsRead(void);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int progressFrame;
	bool historyLoaded;
	bool hasNewMsg;
	CMessageBoard msgBoard;
	bool hidden;
	HBITMAP previewImage;
	std::wstring previewURL;
protected:
	afx_msg LRESULT OnHistoryLoaded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetRead(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	CTabProxy* tabProxy;
	int msgCount;
protected:
};