// FixHosts.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FixHosts.h"
#include "FixHostsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFixHostsApp

BEGIN_MESSAGE_MAP(CFixHostsApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFixHostsApp construction

CFixHostsApp::CFixHostsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CFixHostsApp object

CFixHostsApp theApp;


// CFixHostsApp initialization

BOOL CFixHostsApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CAppDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	int nArgs;
	wchar_t** cmdline=CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if(nArgs>1){
		if(cmdline[1][0]==L'v'){
			ShellExecute(0, L"open", L"VkIM.exe", NULL, NULL, SW_SHOWNORMAL);
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
