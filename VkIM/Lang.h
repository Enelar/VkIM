#pragma once

#include "jsonmain.h"
#include "JSONNode.h"
#include "Utils.h"

class CLang
{
public:
	CLang(void);
	~CLang(void);
	static JSONNode* lngData;
	static wchar_t* Get(wchar_t* key);
	static void Load(wchar_t* file);
	static wchar_t* GetNum(wchar_t* key, int num);
};

