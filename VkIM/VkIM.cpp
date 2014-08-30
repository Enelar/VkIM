// VkIM.cpp : Defines the class behaviors for the application.
//

#include "targetver.h"
#include "stdafx.h"
#include "VkIM.h"
#include "VkIMDlg.h"
#include "BrowserDlg.h"
#include <stdio.h>
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "comsupp.lib")
#include "Connector.h"
#include "TalkDlg.h"
#include "Skin.h"
#include "Lang.h"
#include <GdiPlus.h>
#include <Shlwapi.h>
#include "Settings.h"
#include "Utils.h"
#include <atlbase.h>

FILE* logFile=NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char* sid;
unsigned int uid;
char* secret;
CConnector con;
CTalkDlg* talks;
CSkin skin;

// CVkIMApp

BEGIN_MESSAGE_MAP(CVkIMApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVkIMApp construction

CVkIMApp::CVkIMApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}



void strlower(char* str) {
   for (char *p = str; *p; p++) *p = tolower(*p);
}

void CVkIMApp::CheckHostsFile()
{
	CRegKey key;
	LONG status = key.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters", KEY_READ);
	if(status!=ERROR_SUCCESS)return;
	wchar_t buf[1024];
	unsigned long len=1023;
	key.QueryStringValue(L"DataBasePath", buf, &len);
	key.Close();
	std::wstring fn=buf;
	fn+=L"\\hosts";
	ExpandEnvironmentStrings(fn.c_str(), buf, 1023);
	FILE* f=_wfopen(buf, L"r");
	if(!f)return;
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
	std::vector<char*> lines;
	char* part = strtok (fdata,"\r\n");
	if(strlen(part)>0)lines.push_back(part);
	while (part != NULL)
	{
		part = strtok (NULL, "\r\n");
		if(part && strlen(part)>0)lines.push_back(part);
	}
	bool found=false;
	for(int i=0;i<lines.size();i++){
		if(lines[i][0]=='#')continue;
		char* ip = strtok (lines[i]," \t#");
		char* host = strtok (NULL, " \t#");
		if(ip && host){
			strlower(host);
			std::string h=host;
			if(h.find("vkontakte.ru")!=-1 || h.find("vk.com")!=-1){
				found=true;
			}
		}
	}
	delete fdata;
	if(found){
		INT_PTR result=MessageBox(0, L"В системном файле обнаружена запись, перенаправляющая запросы, предназначенные для vkontakte.ru, на посторонний сервер. Такие записи часто используются злоумышленниками для взлома страниц.\n\nХотите ли Вы чтобы программа Vk.IM удалила эту запись?",
			L"Vk.IM - быстрые сообщения ВКонтакте", MB_ICONQUESTION|MB_YESNO);
		if(result==IDYES){
			ShellExecute(0, L"open", L"FixHosts.exe", L"v", NULL, SW_SHOWNORMAL);
			ExitProcess(0);
		}
	}
}



// The one and only CVkIMApp object

CVkIMApp theApp;

// CVkIMApp initialization

BOOL CVkIMApp::InitInstance()
{

	bool logout=false;

	__try{
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

	AfxEnableControlContainer();

	HWND prevInst=FindWindow(NULL, L"VK_IM MAIN WINDOW");
	if(prevInst){
		//ShowWindow(prevInst, SW_SHOW);
		//SetForegroundWindow(prevInst);
		SendMessage(prevInst, WM_TRAY_ICON, 0, WM_LBUTTONDBLCLK);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	char* cdb=new char[1024];
	GetCurrentDirectoryA(1024, cdb);
	TRACE("Current dir = '%s'\n", cdb);
	delete cdb;

	int nArgs;
	wchar_t** cmdline=CommandLineToArgvW(GetCommandLineW(), &nArgs);
	std::wstring p=cmdline[0];
	p=p.substr(0, p.find_last_of(L"\\"));
	if(p[1]==L':'){
	TRACE("Setting current dir '%ws'\n", p.c_str());
	SetCurrentDirectory(p.c_str());
	}

	int driveType=GetDriveType(NULL);
	// Смотрим, запущена ли прога со съемного носителя
	// Если да, то сохраняем данные в папке из которой запустили, иначе - в Application Data
	wchar_t buf[512];
	if(driveType!=DRIVE_REMOVABLE){
		SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, buf);
		wsprintf(buf, L"%s\\VkIM", buf);
	}else{
		wsprintf(buf, L"%s\\Profile", p.c_str());
	}
	TRACE("Data path = '%ws'\n", buf);
	if(!PathIsDirectory(buf)){
		TRACE("Created dir '%ws'\n", buf);
	CreateDirectory(buf, NULL);
	}

	// Грузим настройки
	std::wstring set_file_name=buf;
	set_file_name+=L"\\settings.json";
	TRACE("Loading settings from '%ws'\n", set_file_name.c_str());
	CSettings::Load((wchar_t*)set_file_name.c_str());

	// Скин
	std::wstring skinpath=L"Skins\\";
	skinpath+=CSettings::skinFolder;
	skinpath+=L"\\";
	TRACE("Loading skin from '%ws'\n", skinpath.c_str());
	skin.Load((wchar_t*)skinpath.c_str());
	// И лэнгпак
	std::wstring langpath=L"Lang\\";
	langpath+=CSettings::langFile;
	TRACE("Loading lang from '%ws'\n", langpath.c_str());
	CLang::Load((wchar_t*)langpath.c_str());

	CheckHostsFile();

	con.needExit=false;

	// Проверяем, есть ли сессия. Если есть - используем, если нету - получаем
	FILE* sd;
	wchar_t sd_file_name[512];
	wsprintf(sd_file_name, L"%s\\session.dat", buf);
	TRACE("Loading session from '%ws'\n", sd_file_name);
	if(sd=_wfopen(sd_file_name, L"r")){
		TRACE("File exists\n");
		sid=new char[256];
		secret=new char[128];
		fscanf(sd, "%s\r\n%s\r\n%i", sid, secret, &uid);
		fclose(sd);
	}
	else{
		TRACE("File not exist\n");
		CBrowserDlg bdlg;
		INT_PTR result=bdlg.DoModal();
		if(result==IDOK){
		sd=_wfopen(sd_file_name, L"w");
		if(!sd){
			std::wstring e=L"Невозможно открыть файл для записи:\n\n";
			e+=sd_file_name;
			MessageBox(NULL, e.c_str(), NULL, MB_ICONEXCLAMATION|MB_OK);
		}
		fprintf(sd, "%s\r\n%s\r\n%i", sid, secret, uid);
		fclose(sd);
		}
		else{
		return false;
		}
	}

	talks=new CTalkDlg;
	
	// Инициализируем GDI+
	ULONG_PTR token;
	Gdiplus::GdiplusStartupInput gpi;
	gpi.GdiplusVersion=1;
	gpi.SuppressBackgroundThread=false;
	gpi.DebugEventCallback=NULL;
	Gdiplus::GdiplusStartup(&token, &gpi, NULL);

	// Это надо, чтобы у главного окна не было кнопки в панели задач.
	HWND pwnd=::CreateWindowEx(NULL,L"static", L"VK_IM MAIN WINDOW",
                                   WS_POPUP,
                                   0,0,0,0,NULL,(HMENU)NULL,
                                   NULL, NULL);
	// Создаем и показываем главное окно (список контактов)
	CVkIMDlg dlg(CWnd::FromHandle(pwnd));
	for(int i=1;i<nArgs;i++){
	if(wcscmp(cmdline[i], L"/autostart")==0){ // При автозагрузке оно изначально скрыто
		dlg.startHidden=true;
	}
	if(wcscmp(cmdline[i], L"/log")==0 && !logFile){ // logging
		std::wstring logfname=buf;
		logfname+=L"\\debug.log";
		std::wstring msg=L"Будет создан лог:\n\n"+logfname+wstring(L"\n\nЕсли файл существует, он будет перезаписан.");
		MessageBox(NULL, msg.c_str(), L"Vk.IM", MB_ICONINFORMATION);
		logFile=_wfopen(logfname.c_str(), L"w");
	}
	}

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
		logout=dlg.logout;
	}

	// Освобождаем ресурсы и завершаем программу
	
	delete [] sid;
	delete [] secret;
	delete talks;

	if(logout){
		ShellExecute(0, L"open", L"VkIM.exe", L"v", NULL, SW_SHOWNORMAL);
	}

	con.needExit=true;
	TRACE("Exiting [%i]\n", con.httpSck);
	//shutdown(con.httpSck, 2);
	//closesocket(con.httpSck);
	if(con.httpSck)InternetCloseHandle(con.httpSck);
	EnterCriticalSection(&con.lpCs);
	LeaveCriticalSection(&con.lpCs);
	EnterCriticalSection(&con.cs);
	LeaveCriticalSection(&con.cs);

	delete CLang::lngData;


	
	skin.btnDown.bmp.DeleteObject();
	skin.btnOver.bmp.DeleteObject();
	skin.btnUp.bmp.DeleteObject();
	skin.groupsBG.bmp.DeleteObject();
	skin.listHl.bmp.DeleteObject();
	skin.notifierBG.DeleteObject();
	skin.tabActive.bmp.DeleteObject();
	skin.tabbarBG.bmp.DeleteObject();
	skin.tabCloseAct.DeleteObject();
	skin.tabCloseInact.DeleteObject();
	skin.tabInactive.bmp.DeleteObject();
	skin.tabOver.bmp.DeleteObject();
	skin.talkBottomBar.bmp.DeleteObject();
	skin.talkDivider.bmp.DeleteObject();
	skin.userInfoBG.bmp.DeleteObject();
	skin.wbuttons.DeleteObject();

	}
	__except(CUtils::HandleException(GetExceptionCode(), GetExceptionInformation(), L"Main")){}

	/*TRACE("=====================================\n");
#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
	TRACE("=====================================\n");*/

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	
	return FALSE;
}
