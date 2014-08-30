#pragma once

#include <vector>
#include <string>
#include <map>
#include "UserInfoTip.h"

#define BASE_ITEM_H 18
#define BIG_ITEM_H 33
// CContactListCtrl

using namespace std;

typedef struct			// ��������� ��� �������� ������ ������
{
	unsigned int uid;			// ��������
	wstring userName;			// ���
	wstring userStatus;			// ������
	wstring userPhotoURL;		// ����� ���
	wstring userBigPhotoURL;	// ����� ������� ���
	HBITMAP userPhoto;			// ��� 50�50
	HBITMAP userPhoto30;		// ��� 30�30
	HBITMAP userPhoto100;		// ��� 100�100 (������ � win7)
	int rating;					// �������
	bool online;				// ������ ��� ���
	vector<int> groups;			// ������ � ������� �������
}ListEntry;

class CContactListCtrl : public CWnd
{
	DECLARE_DYNAMIC(CContactListCtrl)

public:
	CContactListCtrl();
	virtual ~CContactListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CFont fnt;


	int scrollOffset;
	HTREEITEM AddItem(ListEntry item, int group);

	vector<ListEntry> items;
	map<int,vector<int>> sortedItems;
	vector<wstring> groupsNames;
	vector<int> groupsIDs;
	vector<bool> groupsExpanded;
	vector<int> searchItems;
	bool groupsMode;
	int viewMode;
	int selectedItem;
	int hoverItem;
	int totalHeight;
	int itemHeight;
	int width;
	bool showOffline;
	bool searching;
	bool mouseInside;
	CBitmap bmpClosed;
	CBitmap bmpOpened;

	CMenu contactContextMenu;
	int menuUid;

	int DrawGroupHeader(CDC* dc, int num, int y);
	int DrawItem(CDC* dc, int item, int y);
	void SetOnlineStatus(int uid, bool online);
	void Sort(int group, bool redraw=true, bool idx=false);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void UpdateScroll(void);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SetGroupsMode(bool gm);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg UINT OnGetDlgCode();
	int HitTest(CPoint point);
	void StartSearch(void);
	void UpdateSearch(wchar_t* q);
	void StopSearch(void);
	int GetItemByUID(int uid);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	bool IsItemVisible(int item);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseLeave();
	CUserInfoTip* userTip;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int tipItem;
	bool tipWaiting;
	bool noDraw;
};


