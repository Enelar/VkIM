#pragma once

typedef HTHEME (WINAPI OPENTHEMEDATA)(HWND, LPCWSTR);
typedef HRESULT (WINAPI CLOSETHEMEDATA)(HTHEME);
typedef HRESULT (WINAPI DRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, RECT*, RECT*);
typedef BOOL (WINAPI ISTHEMEACTIVE)(void);

// CBigButton

class CBigButton : public CButton
{
	DECLARE_DYNAMIC(CBigButton)

public:
	CBigButton();
	virtual ~CBigButton();
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	CFont fntBig, fntSmall;
	HICON arrowN, arrowH;
	CString description;
	bool mouseInside;
	bool hasUxtheme;
	OPENTHEMEDATA* _OpenThemeData;
	CLOSETHEMEDATA* _CloseThemeData;
	DRAWTHEMEBACKGROUND* _DrawThemeBackground;
	ISTHEMEACTIVE* _IsThemeActive;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};


