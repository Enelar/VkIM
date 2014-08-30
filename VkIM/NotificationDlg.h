#pragma once


// CNotificationDlg dialog

class CNotificationDlg : public CDialog
{
	DECLARE_DYNAMIC(CNotificationDlg)

public:
	CNotificationDlg(CWnd* pParent, wchar_t* title, wchar_t* content, int _idx, int _uid);   // standard constructor
	virtual ~CNotificationDlg();

// Dialog Data
	enum { IDD = IDD_NOTIFICATIONDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int idx;
	CDC bdc;
	void StartTransitionIn();
	bool mouseInside;
	bool needClose;
	int uid;
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	static int EaseIn(float t, float b, float c, float d);
	static int EaseOut(float t, float b, float c, float d);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int src_y;
	int dst_y;
	int _i;
	bool cls;
};
