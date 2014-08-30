#pragma once

#include <string>
// CUserInfoTip
using namespace std;

class CUserInfoTip : public CWnd
{
	DECLARE_DYNAMIC(CUserInfoTip)

public:
	bool exists;
	CUserInfoTip();
	virtual ~CUserInfoTip();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	int item;
	HRGN rgn;
	POINT* GetPolygon(bool draw=false);
	POINT pos;
	int t;
	bool flip;
	wstring username;
	CFont headerFnt;
	CFont fnt;
	CFont smallFnt;
	bool hasInfo;
	int ldrFrame;
	HBITMAP ava;
	wstring unv;
	wstring bdate;
	wstring city;
	wstring act;
	void StartAnim(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void DataLoaded(void);
};


