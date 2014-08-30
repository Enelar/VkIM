// VkAPIRequest.cpp : implementation file
//

#include "stdafx.h"
#include "VkAPIRequest.h"
#include "Utils.h"
#include "CryptMD5.h"

extern char* sid;
extern unsigned int uid;
extern char* secret;
// CVkAPIRequest

CVkAPIRequest::CVkAPIRequest(wchar_t* method_name)
{
	param_names.push_back(L"method");
	param_values.push_back(method_name);
	param_names.push_back(L"v");
	param_values.push_back(L"3.0");
	param_names.push_back(L"api_id");
	param_values.push_back(L"1911346");
	param_names.push_back(L"format");
	param_values.push_back(L"json");
}

CVkAPIRequest::~CVkAPIRequest()
{
	param_names.clear();
	param_values.clear();
}

void CVkAPIRequest::AddParameter(wchar_t* name, wchar_t* value){
	for(int i=0;i<param_names.size();i++){
		if(wcscmp(param_names[i], name)==0){
			param_names.erase(param_names.begin()+i);
			param_values.erase(param_values.begin()+i);
			break;
		}
	}
	param_names.push_back(name);
	param_values.push_back(value);
}

char* CVkAPIRequest::GetRequestString(){
	for(int j=0;j<param_names.size();j++){
		for(int i=0;i<param_names.size()-1;i++){
			if(wcscmp(param_names[i], param_names[i+1])>0){
				wchar_t* tmp1=param_names[i];
				param_names[i]=param_names[i+1];
				param_names[i+1]=tmp1;
				wchar_t* tmp2=param_values[i];
				param_values[i]=param_values[i+1];
				param_values[i+1]=tmp2;
			}
		}
	}
	char* uidBuf=new char[15];
	memset(uidBuf, 0, 15);
	sprintf(uidBuf, "%i", uid);
	std::string str_for_sig(uidBuf);
	for(int i=0;i<param_names.size();i++){
		char* pn=CUtils::wtoc(param_names[i], wcslen(param_names[i])+1);
		str_for_sig+=pn;
		str_for_sig+='=';
		char* pv=CUtils::WcharToUtf8(param_values[i]);
		str_for_sig+=pv;
		delete pn;
		delete pv;
	}
	str_for_sig+=secret;
	MD5Context ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, (const unsigned char*)str_for_sig.c_str(), str_for_sig.length());
	unsigned char md[16];
	MD5Final(md, &ctx);
	char* sig=new char[33];
	memset(sig, 0, 33);
	for(int i=0;i<16;i++){
	sprintf(sig, "%s%02x", sig, md[i]);
	}
	std::string req="";
	for(int i=0;i<param_names.size();i++){
		char* pv=CUtils::wtoc(param_names[i], wcslen(param_names[i])+1);
		req+=pv;
		req+='=';
		char* ue=CUtils::urlEncode(param_values[i]);
		req+=ue;
		req+='&';
		delete [] ue;
		delete [] pv;
	}
	req+="sig=";
	req+=sig;
	req+="&sid=";
	req+=sid;
	char* r=new char[req.length()+1];
	memset(r, 0, req.length()+1);
	memcpy(r, req.c_str(), req.length());
	delete [] uidBuf;
	delete [] sig;

	return r;
}

// CVkAPIRequest member functions
