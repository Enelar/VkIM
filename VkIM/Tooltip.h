#pragma once


// CTooltip

class CTooltip : public CWnd
{
	DECLARE_DYNAMIC(CTooltip)

public:
	CTooltip();
	virtual ~CTooltip();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void ShowTooltip(CPoint pnt, wchar_t* text);
	afx_msg void OnPaint();
	void HideTooltip(void);
	void init(void);
};


