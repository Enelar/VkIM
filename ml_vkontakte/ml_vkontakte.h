// ml_vkontakte.h : main header file for the ml_vkontakte DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cml_vkontakteApp
// See ml_vkontakte.cpp for the implementation of this class
//

class Cml_vkontakteApp : public CWinApp
{
public:
	Cml_vkontakteApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
