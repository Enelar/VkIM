#pragma once
#include "afxcmn.h"
#include <hash_map>
#include <string>
#include "TalkTab.h"
#include "ClosableTabCtrl.h"
#include "VkIMDlg.h"
#include <dwmapi.h>


// CTalkDlg dialog
#define WM_OPENTAB WM_USER+1
#define WM_DWMSENDICONICTHUMBNAIL 0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP 0x0326
typedef HRESULT (WINAPI DWMSETICONICTHUMBNAIL)(HWND, HBITMAP, DWORD);
typedef HRESULT (WINAPI DWMINVIB)(HWND);
typedef HRESULT (WINAPI DWMSETLPBITMAP)(HWND, HBITMAP, POINT*, DWORD);
typedef HRESULT (WINAPI DWMEXTENDFRAMEINTOCLIENTAREA)(HWND,MARGINS*);

using namespace std;

class CTalkDlg : public CDialog
{
	DECLARE_DYNAMIC(CTalkDlg)

public:
	CTalkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTalkDlg();
	stdext::hash_map<unsigned int,CTalkTab*> openedTabs;
	void OpenNewTab(ListEntry* info, bool newMsg=false, bool setSel=true);
	HWND hwndTabs;
	bool isCreated;

// Dialog Data
	enum { IDD = IDD_TALKDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CClosableTabCtrl tabs;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnTcnSelchangingTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcMouseHover(UINT nFlags, CPoint point);
	afx_msg LRESULT OnOpenTab(WPARAM wParam, LPARAM lParam);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual void OnOK();
	virtual void OnCancel();
	DWMSETWINDOWATTRIBUTE* _DwmSetWindowAttribute;
	DWMSETICONICTHUMBNAIL* _DwmSetIconicThumbnail;
	DWMINVIB* _DwmInvalidateIconicBitmaps;
	DWMSETLPBITMAP* _DwmSetIconicLivePreviewBitmap;
	DWMEXTENDFRAMEINTOCLIENTAREA* _DwmExtendFrameIntoClientArea;
	DWMISCOMPOSITIONENABLED* _DwmIsCompositionEnabled;
	ITaskbarList3* tb;
	afx_msg LRESULT OnCompositionChanged(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcActivate(BOOL bActive);

	bool noFocus;
	afx_msg void OnExitSizeMove();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
