// MessageBoard.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "MessageBoard.h"
#include <string>
#include "Settings.h"
#include "Utils.h"
#include "Skin.h"
#include "Lang.h"
#include <MsHtmHst.h>
#include "Connector.h"


// CMessageBoard dialog
extern CSkin skin;
extern CConnector con;

IMPLEMENT_DYNAMIC(CMessageBoard, CDialog)

CMessageBoard::CMessageBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageBoard::IDD, pParent)
{
	ready=false;
	uiHandler=NULL;
}

CMessageBoard::~CMessageBoard()
{
	if(uiHandler)delete uiHandler;
}

void CMessageBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, ie);
}


BEGIN_MESSAGE_MAP(CMessageBoard, CDialog)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CMessageBoard message handlers


void CMessageBoard::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(ie)ie.SetWindowPos(NULL, -2, -2, cx+4, cy+4, SWP_NOMOVE|SWP_NOZORDER);
}


BOOL CMessageBoard::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect wr;
	GetClientRect(wr);
	
	ie.SetWindowPos(NULL, -2, -2, wr.Width()+4, wr.Height()+4, SWP_NOZORDER);
	ie.Navigate(L"about:blank", NULL, NULL, NULL, NULL);

	return false;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
BEGIN_EVENTSINK_MAP(CMessageBoard, CDialog)
	ON_EVENT(CMessageBoard, IDC_EXPLORER1, 250, CMessageBoard::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CMessageBoard, IDC_EXPLORER1, 252, CMessageBoard::NavigateComplete2Explorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()


void CMessageBoard::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	// TODO: Add your message handler code here
	TRACE("Before navigate %ws\n", URL->bstrVal);

	if(wcscmp(URL->bstrVal, L"vkim:load_history")==0){
		*Cancel=true;
		loadMore->put_className(L"loadMoreLoading");
		con.GetHistory((CTalkTab*)GetParent());
		return;
	}

	if(wcscmp(URL->bstrVal, L"about:blank")!=0){
		*Cancel=true;
		ShellExecute(GetSafeHwnd(), L"open", URL->bstrVal, NULL, NULL, SW_SHOWNORMAL);
	}
}


void CMessageBoard::NavigateComplete2Explorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	// TODO: Add your message handler code here
	TRACE("Navigate complete\n");
	IHTMLDocument2 *pHtmlDoc=(IHTMLDocument2*)ie.get_Document();
	IHTMLStyleSheet* style;
	pHtmlDoc->createStyleSheet(NULL, -1, &style);
	style->put_cssText(skin.messageStyle);
	ready=true;
	IHTMLElement* elem;
	pHtmlDoc->get_body(&elem);
	elem->put_innerHTML(L"<!-- qweqwe -->"); // Без этого не работает...
	if(CSettings::loadHistory){
		std::wstring loadLinkCode=L"<div class='loadMoreLink'><a href='vkim:load_history'>"+std::wstring(CLang::Get(L"load_more"))+L" &uarr;</a></div>";
		elem->put_innerHTML((BSTR)loadLinkCode.c_str());
		IHTMLDOMNode* node;
		elem->QueryInterface(__uuidof(IHTMLDOMNode), (void**)&node);
		IHTMLDOMNode* n;
		node->get_firstChild(&n);
		n->QueryInterface(__uuidof(IHTMLElement), (void**)&loadMore);
	}
	ICustomDoc* cdoc;
	HRESULT r=pHtmlDoc->QueryInterface(__uuidof(ICustomDoc),(void**)&cdoc);
	if(SUCCEEDED(r)){
		uiHandler=new CIEUIHandler();
		cdoc->SetUIHandler(uiHandler);
	}
}

void CMessageBoard::AddMessage(const wchar_t* sender, const wchar_t* msg, int senderID, bool my, const wchar_t* time, int local_id, bool readState, bool prepend, int tmp_id)
{
	TRACE("Add msg [%i]\n", my);
	IHTMLDocument2 *doc=(IHTMLDocument2*)ie.get_Document();
	IHTMLElement* elem;
	doc->get_body(&elem);
	IHTMLElement* elem2;

	std::wstring _msg=msg;
	int nPos;
	int off=0;
	if(my && !readState){
	while((nPos=_msg.find(L"<"))!=-1){
		_msg.replace(nPos, 1, L"&lt;");
	}
	while((nPos=_msg.find(L">"))!=-1){
		_msg.replace(nPos, 1, L"&gt;");
	}
	while((nPos=_msg.find(L"\n"))!=-1){
		_msg.replace(nPos, 1, L"<br>");
	}
	}
	while((nPos=_msg.find(L"http://", off))!=-1){
		std::wstring link=_msg.substr(nPos, _msg.find_first_of(L" ,!(){}[];'<>", nPos)-nPos);
		std::wstring rpl=L"<a href='"+link+L"'>"+link+L"</a>";
		_msg.replace(nPos, link.length(), rpl);
		off=nPos+rpl.length()+1;
	}
	off=0;
	while((nPos=_msg.find(L"https://", off))!=-1){
		std::wstring link=_msg.substr(nPos, _msg.find_first_of(L" ,!(){}[];'<>", nPos)-nPos);
		std::wstring rpl=L"<a href='"+link+L"'>"+link+L"</a>";
		_msg.replace(nPos, link.length(), rpl);
		off=nPos+rpl.length()+1;
	}

	if(_msg.find(L"<*>")!=-1){
		std::wstring media_tag=_msg.substr(_msg.find(L"<*>")+3);
		_msg=_msg.substr(0, _msg.find(L"<*>"));
		media_tag=media_tag.substr(2, media_tag.length()-4);
		std::wstring media_url=L"";
		if(media_tag.substr(0,5).compare(L"audio")==0){
			media_url=L"http://vkontakte.ru/audio.php?id=";
			media_url+=media_tag.substr(5, media_tag.find(L"_")-5);
			media_url+=L"&audio_id=";
			media_url+=media_tag.substr(media_tag.find(L"_")+1);
		}else{
			media_url=L"http://vkontakte.ru/";
			media_url+=media_tag;
		}

		_msg+=L"<br><a href='";
		_msg+=media_url;
		_msg+=L"'>";
		if(media_tag.substr(0,5).compare(L"audio")==0){
			_msg+=CLang::Get(L"att_audio");
		}
		if(media_tag.substr(0,5).compare(L"video")==0){
			_msg+=CLang::Get(L"att_video");
		}
		if(media_tag.substr(0,5).compare(L"photo")==0){
			_msg+=CLang::Get(L"att_photo");
		}
		_msg+=L"</a>";
	}

	doc->createElement(L"div", &elem2);
	if(readState){
		elem2->put_className(my?L"msgMy":L"msg");
	}else{
		elem2->put_className(my?L"msgMyNew":L"msg");
	}
	if(local_id>0){msgs[local_id]=elem2;}
	else{
		if(tmp_id!=-1){
			msgs_no_id[tmp_id]=elem2;
		}
	}
	std::wstring msgHtml=skin.messageTemplate;
	
	
	while((nPos=msgHtml.find(L"{SENDER_NAME}"))!=-1){
		msgHtml.replace(nPos, 13, sender);
	}
	while((nPos=msgHtml.find(L"{MSG}"))!=-1){
		msgHtml.replace(nPos, 5, _msg);
	}
	wchar_t* s=new wchar_t[10];
	_itow(senderID, s, 10);
	while((nPos=msgHtml.find(L"{SENDER_ID}"))!=-1){
		msgHtml.replace(nPos, 11, s);
	}
	while((nPos=msgHtml.find(L"{TIME}"))!=-1){
		msgHtml.replace(nPos, 6, time);
	}
	delete s;
	elem2->put_innerHTML((wchar_t*)msgHtml.c_str());
	IHTMLDOMNode* node;
	elem->QueryInterface(__uuidof(IHTMLDOMNode), (void**)&node);
	IHTMLDOMNode* node2;
	elem2->QueryInterface(__uuidof(IHTMLDOMNode), (void**)&node2);

	if(prepend){
		VARIANT_BOOL h;
		node->hasChildNodes(&h);
		if(h){
			IHTMLDOMNode* node2_;
			IHTMLDOMNode* fnode;
			node->get_firstChild(&fnode);
			VARIANT rn;
			rn.vt=VT_DISPATCH;
			rn.pdispVal=fnode;
			node->insertBefore(node2, rn, &node2_);
			fnode->swapNode(node2, &node2_);
		}else{
			IHTMLDOMNode* node2_;
			node->appendChild(node2, &node2_);
		}
	}else{
		IHTMLDOMNode* node2_;
		node->appendChild(node2, &node2_);
	}

	if(!prepend){
	IHTMLElement2 *pElement;
    elem->QueryInterface(__uuidof(IHTMLElement2),(void**)&pElement);
	pElement->put_scrollTop(20000000);
	}
}


void CMessageBoard::AddDivider(void)
{
	IHTMLDocument2 *doc=(IHTMLDocument2*)ie.get_Document();
	IHTMLElement* elem;
	doc->get_body(&elem);
	IHTMLElement* elem2;
	doc->createElement(L"div", &elem2);
	elem2->put_className(L"divider");
	IHTMLDOMNode* node;
	elem->QueryInterface(__uuidof(IHTMLDOMNode), (void**)&node);
	IHTMLDOMNode* node2;
	elem2->QueryInterface(__uuidof(IHTMLDOMNode), (void**)&node2);
	IHTMLDOMNode* node2_;
	node->appendChild(node2, &node2_);
}


void CMessageBoard::SetReadState(int local_id)
{
	TRACE("Set read state %i\n", local_id);
	if(msgs[local_id]){
		msgs[local_id]->put_className(L"msgMy");
	}
}



void CMessageBoard::SetMsgID(int id, int tmp_id)
{
	msgs[id]=msgs_no_id[tmp_id];
	msgs_no_id.erase(msgs_no_id.find(tmp_id));
}


void CMessageBoard::OnOK()
{
}


void CMessageBoard::OnCancel()
{
}


void CMessageBoard::ScrollToBottom(void)
{
	IHTMLDocument2 *doc=(IHTMLDocument2*)ie.get_Document();
	IHTMLElement* elem;
	doc->get_body(&elem);
	IHTMLElement2 *pElement;
	elem->QueryInterface(__uuidof(IHTMLElement2),(void**)&pElement);
	pElement->put_scrollTop(20000000);
	long s;
	pElement->get_scrollTop(&s);
	scrollPos=s;
}

void CMessageBoard::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanged(lpwndpos);
	IHTMLDocument2 *doc=(IHTMLDocument2*)ie.get_Document();
	IHTMLElement* elem;
	doc->get_body(&elem);
	if(!elem)return;
	IHTMLElement2 *pElement;
	elem->QueryInterface(__uuidof(IHTMLElement2),(void**)&pElement);
	long s;
	pElement->get_scrollTop(&s);
	if(s<100){
		pElement->put_scrollTop(scrollPos);
	}else{
		scrollPos=s;
	}
	TRACE("s=%i\n", s);
	// TODO: Add your message handler code here
}


BOOL CMessageBoard::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return 1;
}
