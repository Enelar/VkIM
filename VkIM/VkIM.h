// VkIM.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#ifdef _DEBUG
#include <vld.h>
#endif

// CVkIMApp:
// See VkIM.cpp for the implementation of this class
//

class CVkIMApp : public CWinApp
{
public:
	CVkIMApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	void CheckHostsFile();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CVkIMApp theApp;