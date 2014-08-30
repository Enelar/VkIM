#pragma once


// CExEdit

class CExEdit : public CEdit
{
	DECLARE_DYNAMIC(CExEdit)

public:
	CExEdit();
	virtual ~CExEdit();
	bool ctrlDown;
	int callbackMsg;
	wchar_t* convTableRU;
	wchar_t* convTableEN;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};


