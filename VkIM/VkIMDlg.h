// VkIMDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "CLTree.h"
#include "NotificationDlg.h"
#include "TransparentStatic.h"
#include "SettingsDlg.h"
#include "LoadingDlg.h"
#include <hash_map>
#include "ExEdit.h"
#include "SearchEdit.h"
#include "Tooltip.h"
#include "ContactListCtrl.h"
#include "StatusDlg.h"

#define WM_TRAY_ICON WM_USER+10
#define WM_SHOW_NOTIFICATION WM_USER+11
#define WM_HIDE_NOTIFICATION WM_USER+12
#define WM_HIDE_LOADING WM_USER+13
#define WM_STATUS_ENTER WM_USER+14
#define WM_UPDATE_COUNTERS WM_USER+15
#define WM_HISTORY_LOADED WM_USER+16
#define WM_ADD_MESSAGE WM_USER+17
#define WM_SET_READ WM_USER+18

typedef HTHEME (WINAPI OPENTHEMEDATA)(HWND, LPCWSTR);
typedef HRESULT (WINAPI CLOSETHEMEDATA)(HTHEME);
typedef HRESULT (WINAPI DRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, RECT*, RECT*);
typedef BOOL (WINAPI ISTHEMEACTIVE)(void);

typedef HRESULT (WINAPI DWMISCOMPOSITIONENABLED)(BOOL*);
typedef HRESULT (WINAPI DWMSETWINDOWATTRIBUTE)(HWND, DWORD, LPCVOID, DWORD);

typedef struct
{
	int friends;
	int photos;
	int videos;
	int notes;
	int gifts;
	int events;
	int groups;
	int opinions;
	int offers;
	int questions;
}Counters;

typedef struct
{
	wchar_t* title;
	wchar_t* content;
	int uid;
}NotificationStruct;

// CVkIMDlg dialog
class CVkIMDlg : public CDialog
{
// Construction
public:
	CVkIMDlg(CWnd* pParent = NULL);	// standard constructor
	~CVkIMDlg();

// Dialog Data
	enum { IDD = IDD_VKIM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//CLTree friendList;
	CContactListCtrl friendList;
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CTransparentStatic userNameLabel;
	CExEdit userStatusField;
	afx_msg void OnStnClickedUserName();
	afx_msg void OnEnSetfocusUserStatus();
	CStatic userAvatar;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnOpenTab(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowNotification(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHideNotification(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedSettingsBtn();
	afx_msg void OnClose();
	CMenu trayMenu;
	CMenu servMenu;
	CMenu mainMenu;
	CMenu listModeMenu;
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnStnClickedAva();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	stdext::hash_map<int, CNotificationDlg*> notifications;

	CSettingsDlg* settings;
	CSearchEdit searchField;
	CStatusDlg* sdlg;
protected:
	afx_msg LRESULT OnHideLoading(WPARAM wParam, LPARAM lParam);
public:
//	CLoadingDlg loadingDlg;
	//CLoadingDlg* loadingDlg;
	CTransparentStatic userStatusLabel;
	afx_msg void OnStnClickedUserStatusS();
	bool userHasStatus;
protected:
	afx_msg LRESULT OnStatusEnter(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnKillfocusUserStatus();
	afx_msg void OnBnClickedMfcbutton1();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnEnChangeClSearch();
	CString searchValue;
	afx_msg void OnNcPaint();
	int menuBtnState;
	int settingsBtnState;
	int soundBtnState;
	int closeBtnState;
	bool hasUxtheme;
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	OPENTHEMEDATA* _OpenThemeData;
	CLOSETHEMEDATA* _CloseThemeData;
	DRAWTHEMEBACKGROUND* _DrawThemeBackground;
	ISTHEMEACTIVE* _IsThemeActive;
	bool isWindowActive;
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int progressFrame;
	int sProgressFrame;
	afx_msg void OnNcMouseLeave();
	bool mouseInNC;
	HWND pwnd;
protected:
	afx_msg LRESULT OnUpdateCounters(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	bool startHidden;
	Counters counters;
	std::vector<int> countersWidth;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	bool onCounters;
	CTooltip tooltip;
	int onCounter;
	afx_msg void OnMouseLeave();
	bool mouseInside;
	void CountersEnableChanged(void);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);


	POINT snap_cur_pos;
    RECT snap_rcWindow, snap_wa, *snap_prc;
    int snap_x, snap_y;
	afx_msg void OnEnterSizeMove();
	afx_msg void OnExitSizeMove();
protected:
	afx_msg LRESULT OnTaskbarcreated(WPARAM wParam, LPARAM lParam);
public:
	void StartStatusProgress(void);
	void StopStatusProgress(void);
	wstring newStatusText;
	bool logout;
};
