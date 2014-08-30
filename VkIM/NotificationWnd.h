#pragma once


// CNotificationWnd

class CNotificationWnd : public CWnd
{
	DECLARE_DYNAMIC(CNotificationWnd)

public:
	CNotificationWnd(wchar_t* title, wchar_t* content, int idx=1);
	virtual ~CNotificationWnd();
	void DoTransitionIn();
	static UINT DoStartTransitionIn(CNotificationWnd* wnd);
	void StartTransitionIn();
	CDC bdc;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	bool mouseInside;
	bool needClose;
	void OnClose();
	int ix;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


