#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "JSON_Defs.h"
#include "jsonmain.h"
#include "VkAPIRequest.h"
#include "Utils.h"
#include <vector>
#include <string>

using namespace std;
// CPluginViewDlg dialog

typedef struct{
	wstring artist;
	wstring title;
	wstring url;
	int duration;
	wstring aid;
	wstring oid;
}Song;

typedef struct{
	wstring name;
	int id;
}Friend;

class CPluginViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CPluginViewDlg)

public:
	CPluginViewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPluginViewDlg();

// Dialog Data
	enum { IDD = IDD_PLUGINVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit searchEdit;
	CButton searchBtn;
	CListCtrl list;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CListBox friendList;
	void AddItem(int number, wchar_t* performer, wchar_t* title, int duration);
	JSONNode* SendRequest(CVkAPIRequest* req);
	CString searchValue;
	vector<Song> slist;
	afx_msg void OnNMDblclkList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList3(NMHDR *pNMHDR, LRESULT *pResult);
	static UINT LoadUserThread(void* param);
	int currentUser;
	static UINT LoadFriendsThread(void* param);
	static UINT LoadSearchThread(void* param);
	afx_msg void OnLbnSelchangeList1();
	void AddToPlaylist(int idx);
	void Play(int idx);

	CMenu contextMenu;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	CButton removeRepeatCheck;
	CStatic progrLabel;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ShowProgress(void);
	void HideProgress(void);
	afx_msg void OnEnSetfocusEdit1();
	afx_msg void OnStnClickedRrptLabel();
};
