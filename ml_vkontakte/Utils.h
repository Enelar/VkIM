#pragma once

// CUtils command target

#include <vector>
#include <string>
#include "stdafx.h"

typedef HRESULT (WINAPI DRAWTHEMETEXTEX)(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,LPRECT,DTTOPTS*);

class CUtils// : public CObject
{
public:
	//CUtils();
	//virtual ~CUtils();

	//static void UrlDecode(wchar_t *st);
	static wchar_t* urlDecode(wchar_t *text);
	static void Split(std::string str, std::string separator, std::vector<std::string>* results);
	static char* WcharToUtf8(wchar_t* str);
	static wchar_t* Utf8ToWchar(char* utf8data);
	static char* wtoc(const wchar_t* w, size_t max);
	static wchar_t* ctow(const char* c, size_t max);
	static char* urlEncode(wchar_t* text);
	static HBITMAP ResizeBitmap(HBITMAP bmp, int w, int h, int new_w, int new_h);
	static void ReplaceAll(std::wstring* sbj, std::wstring from, std::wstring to);
	static void DrawAeroText(CRect rect, CDC* dc, int glowSize, int flags, CFont* font, wchar_t* text);

	static DRAWTHEMETEXTEX* _DrawThemeTextEx;
	static int HandleException(unsigned int code, struct _EXCEPTION_POINTERS* ep, wchar_t* thrdName);
};


