#include "Lang.h"
#include "stdafx.h"

JSONNode* CLang::lngData;

CLang::CLang(void)
{
}


CLang::~CLang(void)
{
}


wchar_t* CLang::Get(wchar_t* key){
	if(!lngData) return key;
	if(lngData->GetNode(key)){
		return (wchar_t*)lngData->GetNode(key)->NodeAsString().c_str();
	}
	return key;
}

void CLang::Load(wchar_t* file){
	FILE* f=_wfopen(file, L"r");
	if(!f){
		std::wstring errmsg=L"Ошибка при открытии языкового файла \"";
		errmsg+=file;
		errmsg+=L"\"!!!";
		MessageBox(NULL, errmsg.c_str(), NULL, MB_ICONEXCLAMATION);
	}
	fseek(f,0,SEEK_END);
	int size = ftell(f);
	rewind(f);
	char* fdata=new char[size+1];
	memset(fdata, 0, size+1);
	char* rr=fdata;
	while(!feof(f)){
		fread(rr, 1, 1, f);
		rr++;
	}
	fclose(f);
	wchar_t* d=CUtils::Utf8ToWchar(fdata);
	delete fdata;
	lngData=libJSON::Parse(d);
	delete d;
	if(!lngData){
		std::wstring errmsg=L"Ошибка при загрузке языкового файла \"";
		errmsg+=file;
		errmsg+=L"\"!!!";
		MessageBox(NULL, errmsg.c_str(), NULL, MB_ICONEXCLAMATION);
	}
}

wchar_t* CLang::GetNum(wchar_t* key, int num)
{
	if(!lngData) return key;
		std::wstring _key=key;
		if(num%10==1 && (num%100>19 || num%100<11)){
			_key+=L"_1";
		}
		else if((num%10==2||num%10==3||num%10==4) && (num%100>19 || num%100<11)){
			_key+=L"_2";
		}
		else{
			_key+=L"_3";
		}
		if(lngData->GetNode(_key)){
		std::wstring ls=lngData->GetNode(_key)->NodeAsString();
		wchar_t* s=new wchar_t[10];
		_itow(num, s, 10);
		int nPos;
		while((nPos=ls.find(L"{N}"))!=-1){
			ls.replace(nPos, 3, s);
		}
		delete s;
		wchar_t* rs=new wchar_t[ls.length()+1];
		wcscpy(rs, ls.c_str());
		return rs;
		}
	return key;
}
