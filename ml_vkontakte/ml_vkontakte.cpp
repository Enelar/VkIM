// ml_vkontakte.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ml_vkontakte.h"

#include "../gen_ml/ml.h"
#include "../Winamp/wa_ipc.h"
#include <api/service/api_service.h>

#include "PluginViewDlg.h"
#include "BrowserDlg.h"
#include <string>

extern api_service *serviceManager;
#define WASABI_API_SVC serviceManager

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char* sid;
unsigned int uid;
char* secret;

CPluginViewDlg* view;

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// Cml_vkontakteApp

BEGIN_MESSAGE_MAP(Cml_vkontakteApp, CWinApp)
END_MESSAGE_MAP()


// Cml_vkontakteApp construction

Cml_vkontakteApp::Cml_vkontakteApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only Cml_vkontakteApp object

Cml_vkontakteApp theApp;


// Cml_vkontakteApp initialization

BOOL Cml_vkontakteApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



int Init();
void Quit();
UINT_PTR xmlex_treeItem = 0;
api_service *serviceManager = 0;

INT_PTR PluginMessageProc(int message_type, INT_PTR param1, INT_PTR param2, INT_PTR param3);

winampMediaLibraryPlugin plugin =
{
	MLHDR_VER,
		"VKontakte Music",
		Init,
		Quit,
		PluginMessageProc,
		0,
		0,
		0,
};

extern "C" __declspec(dllexport) winampMediaLibraryPlugin *winampGetMediaLibraryPlugin()
{
	return &plugin;
}


INT_PTR PluginMessageProc(int message_type, INT_PTR param1, INT_PTR param2, INT_PTR param3)
{
	if (message_type == ML_MSG_TREE_ONCREATEVIEW && param1 == xmlex_treeItem)
	{
		wchar_t* buf=new wchar_t[512];
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, buf);
		wsprintf(buf, L"%s\\VkIM", buf);
		FILE* sd;
	std::wstring sd_file_name=buf+std::wstring(L"\\session.dat");
	if(sd=_wfopen(sd_file_name.c_str(), L"r")){
		sid=new char[256];
		secret=new char[128];
		fscanf(sd, "%s\r\n%s\r\n%i", sid, secret, &uid);
		fclose(sd);
	}
	else{
		CBrowserDlg bdlg;
		INT_PTR result=bdlg.DoModal();
		if(result==IDOK){
		sd=_wfopen(sd_file_name.c_str(), L"w");
		fprintf(sd, "%s\r\n%s\r\n%i", sid, secret, uid);
		fclose(sd);
		}
		else{
		return false;
		}
	}
	delete buf;

		view=new CPluginViewDlg(CWnd::FromHandle((HWND)(LONG_PTR)param2));
		view->Create(CPluginViewDlg::IDD, CWnd::FromHandle((HWND)(LONG_PTR)param2));
		return (INT_PTR)view->m_hWnd;
			//CreateDialog(plugin.hDllInstance, MAKEINTRESOURCE(IDD_VIEW_XMLEX), (HWND)(LONG_PTR)param2, (DLGPROC)view_xmlexDialogProc);
	}

	return 0;
}



int Init() 
{
	//starting point for wasabi, where services are shared
	WASABI_API_SVC = (api_service *)SendMessage(plugin.hwndWinampParent, WM_WA_IPC, 0, IPC_GET_API_SERVICE);
	
	//set up tree item, gen_ml will call xmlex_pluginMessageProc if/when the treeview item gets selected
	MLTREEITEMW newTree;
	newTree.size = sizeof(MLTREEITEMW);
	newTree.parentId = 0;
	newTree.title = L"ВКонтакте"; 
	newTree.hasChildren = 0;
	newTree.id = 0;
	MLTREEIMAGE img = {plugin.hDllInstance, IDB_ML_ICON, -1, (BMPFILTERPROC)FILTER_DEFAULT1, 0, 0};
	newTree.imageIndex = (int)(INT_PTR)SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, (WPARAM) &img, ML_IPC_TREEIMAGE_ADD);
	SendMessage(plugin.hwndLibraryParent, WM_ML_IPC, (WPARAM) &newTree, ML_IPC_TREEITEM_ADDW);
	xmlex_treeItem = newTree.id;	
	return 0; // 0 for success.  returning non-zero will cause gen_ml to abort loading your plugin
}

void Quit() 
{
}