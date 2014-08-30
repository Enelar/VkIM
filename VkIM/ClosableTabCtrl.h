#pragma once


// CClosableTabCtrl

#include <vector>

#define TAB_HEIGHT 24
#define TAB_TOP_MARGIN 1
#define TAB_SPACING 1

class CClosableTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CClosableTabCtrl)

public:
	CClosableTabCtrl();
	virtual ~CClosableTabCtrl();
	void DrawItem(LPDRAWITEMSTRUCT drawItemStruct);
	virtual bool GetItemRect(int item, LPRECT rect);
	afx_msg void OnPaint();
	afx_msg LRESULT HitTest(WPARAM wParam, LPARAM lParam);
	std::vector<int> itemWidths;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	int over_idx;
	bool mouseInside;
	CSpinButtonCtrl * m_pSpinCtrl;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void CloseTab(int idx);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
};


