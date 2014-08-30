// Utils.cpp : implementation file
//

#include "stdafx.h"
#include "VkIM.h"
#include "Utils.h"
#include "Connector.h"
#include "EpicFailDlg.h"
#include "MyStackWalker.h"
#include <math.h>


// CUtils
extern CConnector con;

DRAWTHEMETEXTEX* CUtils::_DrawThemeTextEx=NULL;
/*CUtils::CUtils()
{
}

CUtils::~CUtils()
{
}*/

char* CUtils::WcharToUtf8(wchar_t* str){
unsigned int utf8_length;
utf8_length = WideCharToMultiByte(CP_UTF8,0,str,-1,NULL,0,NULL,NULL);
char* utf8str = new char[utf8_length];
WideCharToMultiByte(CP_UTF8,0,str,-1,utf8str,utf8_length,NULL,NULL);
return utf8str;
}

wchar_t* CUtils::Utf8ToWchar(char* utf8data){
int nLen = MultiByteToWideChar(CP_UTF8, 0, utf8data, -1, NULL, NULL);
wchar_t* res=new wchar_t[nLen+1];
memset(res, 0, (nLen+1)*sizeof(wchar_t));
MultiByteToWideChar(CP_UTF8, 0, utf8data, -1, (LPWSTR)res, nLen);
return res;
}

char* CUtils::wtoc(const wchar_t* w, size_t max)
{
  char* c = new char[max];
  wcstombs(c,w,max);
  return c;
}

wchar_t* CUtils::ctow(const char* c, size_t max)
{
  wchar_t* w = new wchar_t[max];
  mbstowcs(w,c,max);
  return w;
}

wchar_t* CUtils::urlDecode(wchar_t* text)
{
	wchar_t* decoded = new wchar_t[wcslen(text)+2];
	wchar_t* tmp=new wchar_t[2];
	memset(decoded, 0, wcslen(text)+2);
	int c=0;
	int dc=0;
	for(int i=0;i<wcslen(text);i++){
		if(text[i]=='%'){
			c=1;
		}else if(c==1){
			tmp[0]=text[i];
			c++;
		}else if(c==2){
			tmp[1]=text[i];
			c=0;
			swscanf(tmp, L"%x", decoded+dc);
			dc++;
		}else if(text[i]=='+'){
			decoded[dc]=' ';
			dc++;
		}else{
			decoded[dc]=text[i];
			dc++;
		}
	}
	delete [] tmp;
	return decoded;
}

char* CUtils::urlEncode(wchar_t* text){
	std::string enc="";
	char* utf=CUtils::WcharToUtf8(text);
	for(int i=0;i<strlen(utf);i++){
		if((utf[i]<48)||
			(utf[i]>57 && utf[i]<65)||
			(utf[i]>90 && utf[i]<97)||
			(utf[i]>122)){
			char* buf=new char[3];
			memset(buf, 0, 3);
			sprintf(buf, "%02x", (unsigned char)utf[i]);
			enc+="%";
			enc+=buf;
			delete buf;
		}
		else{
		enc+=utf[i];
		}
	}
	char* r=new char[enc.length()+1];
	memset(r, 0, enc.length()+1);
	memcpy(r, (char*)enc.c_str(), enc.length());
	delete [] utf;
	return r;
}

void CUtils::Split(std::string str, std::string separator, std::vector<std::string>* results){
    int found;
    found = str.find_first_of(separator);
	while(found != std::string::npos){
        if(found > 0){
            results->push_back(str.substr(0,found));
        }
        str = str.substr(found+1);
        found = str.find_first_of(separator);
    }
    if(str.length() > 0){
        results->push_back(str);
    }
}


HBITMAP CUtils::ResizeBitmap(HBITMAP bmp, int w, int h, int new_w, int new_h)
{
	CDC* sdc=CDC::FromHandle(CreateCompatibleDC(NULL));
	HBITMAP oldBmpS=(HBITMAP)sdc->SelectObject(bmp);
	BITMAPINFOHEADER bi;
    memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = w;    
    bi.biHeight = h;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;
	unsigned int* bits=(unsigned int*)calloc(w*h, sizeof(unsigned int));
	unsigned int* dbits=(unsigned int*)calloc(new_w*new_h, sizeof(unsigned int));
	GetDIBits(sdc->m_hDC, bmp, 0, h, bits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	HBITMAP dbmp=CreateCompatibleBitmap(sdc->m_hDC, new_w, new_h);
	CDC* ddc=CDC::FromHandle(CreateCompatibleDC(NULL));
	HBITMAP oldBmp=(HBITMAP)ddc->SelectObject(dbmp);
	for(int i=0;i<new_h;i++){
		for(int j=0;j<new_w;j++){
			float x=(float)j/(float)new_w*(float)w;
			float y=(float)i/(float)new_h*(float)h;
			int x1=floor(x);
			int y1=floor(y);
			int x2=ceil(x);
			int y2=ceil(y);
			if(x1<0)x1=0;
			if(y1<0)y1=0;
			if(x1==x2){
				x2++;
			}
			if(y1==y2){
				y2++;
			}
			unsigned int px11=bits[x1+y1*w];
			unsigned int px12=bits[x1+y2*w];
			unsigned int px21=bits[x2+y1*w];
			unsigned int px22=bits[x2+y2*w];

			float r11=(px11>>16)&0xFF;
			float r12=(px12>>16)&0xFF;
			float r21=(px21>>16)&0xFF;
			float r22=(px22>>16)&0xFF;

			float r=(r11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(r21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(r12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(r22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);

			float g11=(px11>>8)&0xFF;
			float g12=(px12>>8)&0xFF;
			float g21=(px21>>8)&0xFF;
			float g22=(px22>>8)&0xFF;

			float g=(g11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(g21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(g12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(g22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);

			float b11=(px11)&0xFF;
			float b12=(px12)&0xFF;
			float b21=(px21)&0xFF;
			float b22=(px22)&0xFF;

			float b=(b11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(b21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(b12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(b22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);
			dbits[j+i*new_w]=(((int)r)<<16)|(((int)g)<<8)|((int)b);
		}
	}
	delete bits;
    memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = new_w;    
    bi.biHeight = new_h;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;
	SetDIBits(ddc->m_hDC, dbmp, 0, new_h, dbits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	delete dbits;
	sdc->SelectObject(oldBmpS);
	return (HBITMAP)ddc->SelectObject(oldBmp);
}


HBITMAP CUtils::ResizeBitmapEx(HBITMAP bmp, int w, int h, int new_w, int new_h, int srcx, int srcy, int srcw, int srch)
{
	CDC* sdc=CDC::FromHandle(CreateCompatibleDC(NULL));
	HBITMAP oldBmpS=(HBITMAP)sdc->SelectObject(bmp);
	BITMAPINFOHEADER bi;
    memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = w;    
    bi.biHeight = h;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;
	unsigned int* bits=(unsigned int*)calloc(w*h, sizeof(unsigned int));
	unsigned int* dbits=(unsigned int*)calloc(new_w*new_h, sizeof(unsigned int));
	GetDIBits(sdc->m_hDC, bmp, srcy, srch, bits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	HBITMAP dbmp=CreateCompatibleBitmap(sdc->m_hDC, new_w, new_h);
	CDC* ddc=CDC::FromHandle(CreateCompatibleDC(NULL));
	HBITMAP oldBmp=(HBITMAP)ddc->SelectObject(dbmp);
	for(int i=0;i<new_h;i++){
		for(int j=0;j<new_w;j++){
			float x=(float)j/(float)new_w*(float)srcw+srcx;
			float y=(float)i/(float)new_h*(float)srch;
			int x1=floor(x);
			int y1=floor(y);
			int x2=ceil(x);
			int y2=ceil(y);
			if(x1<0)x1=0;
			if(y1<0)y1=0;
			if(x1==x2){
				x2++;
			}
			if(y1==y2){
				y2++;
			}
			unsigned int px11=bits[x1+y1*w];
			unsigned int px12=bits[x1+y2*w];
			unsigned int px21=bits[x2+y1*w];
			unsigned int px22=bits[x2+y2*w];

			float r11=(px11>>16)&0xFF;
			float r12=(px12>>16)&0xFF;
			float r21=(px21>>16)&0xFF;
			float r22=(px22>>16)&0xFF;

			float r=(r11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(r21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(r12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(r22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);

			float g11=(px11>>8)&0xFF;
			float g12=(px12>>8)&0xFF;
			float g21=(px21>>8)&0xFF;
			float g22=(px22>>8)&0xFF;

			float g=(g11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(g21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(g12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(g22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);

			float b11=(px11)&0xFF;
			float b12=(px12)&0xFF;
			float b21=(px21)&0xFF;
			float b22=(px22)&0xFF;

			float b=(b11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(b21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(b12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(b22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);

			float a11=(px11>>24)&0xFF;
			float a12=(px12>>24)&0xFF;
			float a21=(px21>>24)&0xFF;
			float a22=(px22>>24)&0xFF;

			float a=(a11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(a21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(a12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(a22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);
			dbits[j+i*new_w]=(((int)a)<<24)|(((int)r)<<16)|(((int)g)<<8)|((int)b);
		}
	}
	delete bits;
    memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = new_w;    
    bi.biHeight = new_h;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;
	SetDIBits(ddc->m_hDC, dbmp, 0, new_h, dbits, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	delete dbits;
	sdc->SelectObject(oldBmpS);
	return (HBITMAP)ddc->SelectObject(oldBmp);
}


void CUtils::ReplaceAll(std::wstring* sbj, std::wstring from, std::wstring to)
{
	int pos;
	while((pos=sbj->find(from))!=-1){
		sbj->replace(pos, from.length(), to);
	}
}



void CUtils::DrawAeroText(CRect rect, CDC* dc, int glowSize, int flags, CFont* font, wchar_t* text)
{
	if(!_DrawThemeTextEx){
		HMODULE dllka=LoadLibrary(L"uxtheme.dll");
		if(dllka){
			_DrawThemeTextEx=(DRAWTHEMETEXTEX*)GetProcAddress(dllka, "DrawThemeTextEx");
			FreeLibrary(dllka);
		}
		if(!_DrawThemeTextEx){
			return;
		}
	}
	HTHEME hTheme = con.clDlg->_OpenThemeData(NULL, L"CompositedWindow::Window");
    if (hTheme)
    {
        HDC hdcPaint = CreateCompatibleDC(dc->GetSafeHdc());
        if (hdcPaint)
        {
            int cx = rect.right-rect.left;
			int cy=rect.Height();

            // Define the BITMAPINFO structure used to draw text.
            // Note that biHeight is negative. This is done because
            // DrawThemeTextEx() needs the bitmap to be in top-to-bottom
            // order.
            BITMAPINFO dib = { 0 };
            dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
            dib.bmiHeader.biWidth           = cx;
            dib.bmiHeader.biHeight          = -cy;
            dib.bmiHeader.biPlanes          = 1;
            dib.bmiHeader.biBitCount        = 32;
            dib.bmiHeader.biCompression     = BI_RGB;

            HBITMAP hbm = CreateDIBSection(dc->GetSafeHdc(), &dib, DIB_RGB_COLORS, NULL, NULL, 0);
            if (hbm)
            {
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcPaint, hbm);

                // Setup the theme drawing options.
                DTTOPTS DttOpts = {sizeof(DTTOPTS)};
                DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE;
                DttOpts.iGlowSize = 15;

                // Select a font.
                LOGFONT lgFont;
                HFONT hFontOld = NULL;
                /*if (SUCCEEDED(GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont)))
                {
                    HFONT hFont = CreateFontIndirect(&lgFont);
                    hFontOld = (HFONT) SelectObject(hdcPaint, hFont);
                }*/
				hFontOld = (HFONT) SelectObject(hdcPaint, font->m_hObject);
                // Draw the title.
                RECT rcPaint = rect;
				OffsetRect(&rcPaint, -rect.left, -rect.top);
				::SetTextColor(hdcPaint, RGB(255,255,255));
                //rcPaint.top += 8;
                //rcPaint.left += 8;
                _DrawThemeTextEx(hTheme, 
                                hdcPaint, 
                                0, 0, 
                                text, 
                                -1, 
                                flags, 
                                &rcPaint, 
                                &DttOpts);

                // Blit text to the frame.
				BLENDFUNCTION func;
				func.BlendOp=AC_SRC_OVER;
				func.AlphaFormat=AC_SRC_ALPHA;
				func.SourceConstantAlpha=255;
				func.BlendFlags=0;
				AlphaBlend(dc->m_hDC, rect.left, rect.top, cx, cy, hdcPaint, 0, 0, cx, cy, func);
               // BitBlt(dc->GetSafeHdc(), rect.left, rect.top, cx, cy, hdcPaint, 0, 0, SRCCOPY);

                SelectObject(hdcPaint, hbmOld);
                if (hFontOld)
                {
                    SelectObject(hdcPaint, hFontOld);
                }
                DeleteObject(hbm);
            }
            DeleteDC(hdcPaint);
        }
        con.clDlg->_CloseThemeData(hTheme);
	}
}

wchar_t* GetExceptionString(unsigned int code){
	switch(code){
case EXCEPTION_ACCESS_VIOLATION: return L"EXCEPTION_ACCESS_VIOLATION";
case EXCEPTION_DATATYPE_MISALIGNMENT: return L"EXCEPTION_DATATYPE_MISALIGNMENT";
case EXCEPTION_BREAKPOINT: return L"EXCEPTION_BREAKPOINT";
case EXCEPTION_SINGLE_STEP: return L"EXCEPTION_SINGLE_STEP";
case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
case EXCEPTION_FLT_DENORMAL_OPERAND: return L"EXCEPTION_FLT_DENORMAL_OPERAND";
case EXCEPTION_FLT_DIVIDE_BY_ZERO: return L"EXCEPTION_FLT_DIVIDE_BY_ZERO";
case EXCEPTION_FLT_INEXACT_RESULT: return L"EXCEPTION_FLT_INEXACT_RESULT";
case EXCEPTION_FLT_INVALID_OPERATION: return L"EXCEPTION_FLT_INVALID_OPERATION";
case EXCEPTION_FLT_OVERFLOW: return L"EXCEPTION_FLT_OVERFLOW";
case EXCEPTION_FLT_STACK_CHECK: return L"EXCEPTION_FLT_STACK_CHECK";
case EXCEPTION_FLT_UNDERFLOW: return L"EXCEPTION_FLT_UNDERFLOW";
case EXCEPTION_INT_DIVIDE_BY_ZERO: return L"EXCEPTION_INT_DIVIDE_BY_ZERO";
case EXCEPTION_INT_OVERFLOW: return L"EXCEPTION_INT_OVERFLOW";
case EXCEPTION_PRIV_INSTRUCTION: return L"EXCEPTION_PRIV_INSTRUCTION";
case EXCEPTION_IN_PAGE_ERROR: return L"EXCEPTION_IN_PAGE_ERROR";
case EXCEPTION_ILLEGAL_INSTRUCTION: return L"EXCEPTION_ILLEGAL_INSTRUCTION";
case EXCEPTION_NONCONTINUABLE_EXCEPTION: return L"EXCEPTION_NONCONTINUABLE_EXCEPTION";
case EXCEPTION_STACK_OVERFLOW: return L"EXCEPTION_STACK_OVERFLOW";
case EXCEPTION_INVALID_DISPOSITION: return L"EXCEPTION_INVALID_DISPOSITION";
case EXCEPTION_GUARD_PAGE: return L"EXCEPTION_GUARD_PAGE";
case EXCEPTION_INVALID_HANDLE: return L"EXCEPTION_INVALID_HANDLE";
}
	return L"UNKNOWN";
}

int CUtils::HandleException(unsigned int code, struct _EXCEPTION_POINTERS* ep, wchar_t* thrdName)
{
	if(code==EXCEPTION_BREAKPOINT)return EXCEPTION_CONTINUE_EXECUTION;
	MyStackWalker sw;
	sw.ShowCallstack();
	wchar_t* strace=ctow(sw.buffer.c_str(), sw.buffer.length()+1);

	CEpicFailDlg fail;
	fail.errorInfo=L"Код исключения: "+std::wstring(GetExceptionString(code))+L"\r\nПоток: "+wstring(thrdName)
		+L"\r\n"+std::wstring(strace);

	delete strace;

	fail.DoModal();

	return EXCEPTION_EXECUTE_HANDLER;
}
