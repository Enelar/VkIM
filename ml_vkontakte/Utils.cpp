// Utils.cpp : implementation file
//

#include "stdafx.h"
#include "Utils.h"
#include <math.h>


// CUtils

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
	HBITMAP dbmp=CreateCompatibleBitmap(sdc->m_hDC, new_w, new_h);
	CDC* ddc=CDC::FromHandle(CreateCompatibleDC(NULL));
	HBITMAP oldBmp=(HBITMAP)ddc->SelectObject(dbmp);
	for(int i=0;i<new_h;i++){
		for(int j=0;j<new_w;j++){
			double x=(double)j/(double)new_w*(double)w;
			double y=(double)i/(double)new_h*(double)h;
			double x1=floor(x);
			double y1=floor(y);
			double x2=ceil(x);
			double y2=ceil(y);
			if(x1<0)x1=0;
			if(y1<0)y1=0;
			if(x1==x2){
				x2++;
			}
			if(y1==y2){
				y2++;
			}
			COLORREF px11=sdc->GetPixel(x1, y1);
			COLORREF px12=sdc->GetPixel(x1, y2);
			COLORREF px21=sdc->GetPixel(x2, y1);
			COLORREF px22=sdc->GetPixel(x2, y2);

			double r11=GetRValue(px11);
			double r12=GetRValue(px12);
			double r21=GetRValue(px21);
			double r22=GetRValue(px22);

			double r=(r11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(r21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(r12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(r22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);

			double g11=GetGValue(px11);
			double g12=GetGValue(px12);
			double g21=GetGValue(px21);
			double g22=GetGValue(px22);

			double g=(g11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(g21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(g12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(g22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);

			double b11=GetBValue(px11);
			double b12=GetBValue(px12);
			double b21=GetBValue(px21);
			double b22=GetBValue(px22);

			double b=(b11/((x2-x1)*(y2-y1)))*(x2-x)*(y2-y)+
					(b21/((x2-x1)*(y2-y1)))*(x-x1)*(y2-y)+
					(b12/((x2-x1)*(y2-y1)))*(x2-x)*(y-y1)+
					(b22/((x2-x1)*(y2-y1)))*(x-x1)*(y-y1);
			ddc->SetPixel(j, i, RGB(r,g,b));
		}
	}
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
