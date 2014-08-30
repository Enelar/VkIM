#pragma once
#include "explorer1.h"
#include <MsHTML.h>
#include <hash_map>
#include "IEUIHandler.h"


// CMessageBoard dialog
typedef struct
{
	IHTMLElement* elem;
	std::wstring msg;
}BMSG;

class CMessageBoard : public CDialog
{
	DECLARE_DYNAMIC(CMessageBoard)

public:
	CMessageBoard(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMessageBoard();
	void AddMessage(const wchar_t* sender, const wchar_t* msg, int senderID, bool my, const wchar_t* time, int local_id, bool readState=true, bool prepend=false, int tmp_id=-1);
	void AddDivider(void);
	bool ready;
	stdext::hash_map<int, IHTMLElement*> msgs;
	//std::vector<BMSG> msgs_no_id;
	stdext::hash_map<int, IHTMLElement*> msgs_no_id;
	CIEUIHandler* uiHandler;

// Dialog Data
	enum { IDD = IDD_MESSAGEBOARD };
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CExplorer1 ie;
	DECLARE_EVENTSINK_MAP()
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	void NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL);
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void SetReadState(int local_id);
	void SetMsgID(int id, int tmp_id);
	virtual void OnOK();
	virtual void OnCancel();
	void ScrollToBottom(void);
	IHTMLElement* loadMore;
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	long scrollPos;
};
