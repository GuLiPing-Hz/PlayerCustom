// DX.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#ifdef _DEBUG
//#include <vld.h> //检测内存泄露
#endif//好像有内存泄露误报
#include "DX.h"
#include "resource.h"
#include "OpenGLWindow.h"
#include "DX/util.h"
#include "DX/RenderEngine.h"
#include "DX/vmrutil.h"
#include "DX/Grapha.h"
#include "staff/object/SceneMgr.h"
#include <TlHelp32.h>
#include <algorithm>
#include "Dialog/ListDlg.h"
#include "MiniDump.h"
#include "progress/Wmcopydata.h"


#ifndef MAKERECT
#define MAKERECT(rect,l,t,r,b) \
	do \
	{\
		rect.left = l;\
		rect.top = t;\
		rect.right = r;\
		rect.bottom = b;\
	} while (false)
	
#endif//MAKERECT
HWND							g_hWnd=NULL;		// Holds Our Window Handle
extern  LRESULT	CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

static HANDLE g_hMutex;

CFactoryTemplate g_Templates[] = { 
	{ L"My ActiveX" 
	, &CLSID_SystemClock 
	, CSystemClock::CreateInstance 
	} 
}; 
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

CListDialog* g_listDlg= NULL;

static char WINDOW_CLASS_NAME[] = "GL_DX";
// CDXApp

BOOL CALLBACK GLMonitorEnumProc( HMONITOR hMonitor,HDC hdcMonitor, LPRECT lprcMonitor,LPARAM dwData )
{
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	BOOL b = GetMonitorInfo(hMonitor,&mi);
	if (b)
	{
		MonitorStruct tmp = {hMonitor,mi};
		cls_gl->m_vectmonitor.push_back(tmp);
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDXApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// Exception handler function called back by Windows when something unexpected happens.
LONG WINAPI callbackException( EXCEPTION_POINTERS * exceptionInfo )
{
	OutputDebugStringA( "exception.\r\n" );
	return EXCEPTION_EXECUTE_HANDLER;
}
// CDXApp 构造

CDXApp::CDXApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

CDXApp::~CDXApp()
{
}

// 唯一的一个 CDXApp 对象

CDXApp theApp;


// CDXApp 初始化
BOOL CDXApp::ExitInstance()
{
	COpenGLWindow::releaseWindowSingleton();
	UnregisterClass( WINDOW_CLASS_NAME, m_hInstance );
	CoUninitialize();
	WLOGH_INFO << L"main leave";
	TRACE(_T("以上的内存泄露信息均为错误信息，ExitInstance() for regular DLL: TESTDLL\n"));

	SAFE_CLOSEHANDLE(g_hMutex);
	//参考网站：http://support.microsoft.com/kb/167929/zh-cn
	return CWinApp::ExitInstance();
}

BOOL CDXApp::IsWindows7OrHigh()
{
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;    // Win 95 needs this

	if (dwPlatformId >= 2)
	{
		if ((dwMajorVersion >= 6) && (dwMinorVersion >= 1))
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CDXApp::prepareWindow(COpenGLWindow* popenGLWindow,const GSize& win_size)
{
	RECT wndRect;
	GetWindowRect(g_hWnd,&wndRect);
	int width = wndRect.right - wndRect.left;
	int height = wndRect.bottom - wndRect.top;
	int nHY_Width = 0;
	int nHY_Heigth = 0;
	DISPLAY_DEVICE dd_info;
	ZeroMemory(&dd_info,sizeof(dd_info));
	dd_info.cb = sizeof(dd_info);
	int i=0;
	while(EnumDisplayDevices(NULL,i,&dd_info,0))//例举显示器的信息
	{/*DISPLAY_DEVICE_ATTACHED_TO_DESKTOP*/

		i++;

		if (!(dd_info.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))//副显示器
		{
			DEVMODE devmode_get;
			int nSize = sizeof(devmode_get);
			ZeroMemory(&devmode_get,nSize);
			devmode_get.dmSize = nSize;
			if (0 == EnumDisplaySettingsEx(dd_info.DeviceName,ENUM_CURRENT_SETTINGS,&devmode_get,0))
				continue;

			if((devmode_get.dmPelsWidth == win_size.width) && (devmode_get.dmPelsHeight==win_size.height))
				continue;

			DEVMODE devmode_set;
			nSize = sizeof(devmode_set);
			ZeroMemory(&devmode_set,nSize);
			devmode_set.dmSize = nSize;
			devmode_set.dmFields = DM_PELSWIDTH|DM_PELSHEIGHT;//指定需要修改的值
			devmode_set.dmPelsWidth = win_size.width;
			devmode_set.dmPelsHeight = win_size.height;

			if(DISP_CHANGE_FAILED == ChangeDisplaySettingsEx(dd_info.DeviceName,&devmode_set
				,NULL,CDS_GLOBAL|CDS_UPDATEREGISTRY,NULL))
				WLOGH_ERRO << "ChangeDisplaySettingsEx failed";
		}
		else//主显示器
		{
			DEVMODE devmode_get;
			int nSize = sizeof(devmode_get);
			ZeroMemory(&devmode_get,nSize);
			devmode_get.dmSize = nSize;
			if (0 == EnumDisplaySettingsEx(dd_info.DeviceName,ENUM_CURRENT_SETTINGS,&devmode_get,0))
				WLOGH_ERRO << L"EnumDisplaySettingsEx failed";
			else
			{
				nHY_Width = devmode_get.dmPelsWidth;
				nHY_Heigth = devmode_get.dmPelsHeight;
			}
		}
	}

	if(i < 2)
	{
		::MoveWindow(g_hWnd,0,0,width,height,TRUE);//左上角
		goto End;
	}

	popenGLWindow->m_vectmonitor.clear();
	EnumDisplayMonitors(NULL, NULL, GLMonitorEnumProc, 0);
	if (popenGLWindow->m_vectmonitor.size() > 1)
	{
		std::vector<MonitorStruct>::iterator i;
		i = popenGLWindow->m_vectmonitor.begin();
		for (;i!=popenGLWindow->m_vectmonitor.end();i++)
		{
			if ((*i).mi.dwFlags != MONITORINFOF_PRIMARY)//不是主显示器
			{
				RECT tmp_rect;
				tmp_rect.left = (*i).mi.rcMonitor.left;
				tmp_rect.right = tmp_rect.left + win_size.width;
				tmp_rect.top = (*i).mi.rcMonitor.top;
				tmp_rect.bottom = tmp_rect.top + win_size.height;
				popenGLWindow->setMonitor((*i).h);
				popenGLWindow->setWinRect(tmp_rect);
				popenGLWindow->adjustWindow(g_hWnd,false);//模拟全屏
				break;
			}
		}
	}
	else
		popenGLWindow->adjustWindow(g_hWnd,false,false);

End:
	::ShowWindow(g_hWnd,SW_SHOW);
	::UpdateWindow(g_hWnd);
// 	::ShowWindow(hHY,SW_SHOW);
// 	::UpdateWindow(hHY);
}

BOOL CDXApp::InitInstance()
{
// 	如果一个运行在 Windows XP 上的应用程序清单指定要
// 		// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
// 		//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
// 		INITCOMMONCONTROLSEX InitCtrls;
// 		InitCtrls.dwSize = sizeof(InitCtrls);
// 		// 将它设置为包括所有要在应用程序中使用的
// 		// 公共控件类。
// 		InitCtrls.dwICC = ICC_WIN95_CLASSES;
// 		InitCommonControlsEx(&InitCtrls);
// 	
	//_CrtSetBreakAlloc(154);
	CWinApp::InitInstance();
	//openFlashWnd();
// 	
// 		AfxEnableControlContainer();
// 	
// 		// 标准初始化
// 		// 如果未使用这些功能并希望减小
// 		// 最终可执行文件的大小，则应移除下列
// 		// 不需要的特定初始化例程
// 		// 更改用于存储设置的注册表项
// 		// TODO: 应适当修改该字符串，
// 		// 例如修改为公司或组织名
 	SetRegistryKey(_T("天格科技（杭州）有限公司"));
//////////////////////////////////////////////////////////////////////////
	// hook Crash Dump
	CDumpListener dump_listener;
	CMiniDump::setup( &dump_listener );

	g_hMutex = ::CreateMutexA(NULL,TRUE,"89C28480-6297-4be8-9D64-2A0B040E5ECE");
	if(::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		SAFE_CLOSEHANDLE(g_hMutex);
		OutputDebugStringA("the player is running\n");
		return FALSE;
	}

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED/*COINIT_APARTMENTTHREADED*/);//初始化
	if(FAILED(hr))
	{
		 hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		 if(FAILED(hr))
		 {
			WLOGH_ERRO << L"CoInitializeEx failed";
			return FALSE;
		}
	}
	COpenGLWindow*				popenGLWindow = cls_gl;
	WLOGH_INFO << L"main enter";
// 	if (!IsWindows7OrHigh())
// 	{
// 		LOG_ERN0("operating system must be windows 7 or higher");
// 		return FALSE;
// 	}
 	// Verify that the VMR9 is present on this system
 	if(!VerifyVMR9())
 	{
 		return FALSE;
 	}
 	//////////////////////////////////////////////////////////////////////////
	GSize win_size = popenGLWindow->getWinSize();

	//popenGLWindow->CreateEx(0,NULL,"Video",popenGLWindow->m_styleFS | WS_CHILD,popenGLWindow->m_rectFS,NULL,0,NULL);

	// Register window class
	WNDCLASS wndclass;
	wndclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WindowProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= m_hInstance;
	wndclass.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName	= NULL; 
	wndclass.lpszClassName	= WINDOW_CLASS_NAME;
	//if(szIcon) wndclass.hIcon = LoadIcon(hInstance, szIcon);
	//else 
	wndclass.hIcon = ::LoadIcon(NULL,MAKEINTRESOURCE(IDR_MAINICON));

	if (!::RegisterClass(&wndclass)) {
		//w riteLog("[DXLOG CDXApp::InitInstance] : Can't register window class");
		WLOGH_ERRO << L"Can't register window class";
		return FALSE;
	}

	//g_hWnd = popenGLWindow->GetSafeHwnd();
	g_hWnd = ::CreateWindow(WINDOW_CLASS_NAME,"Video Open",popenGLWindow->m_styleFS,0,0,win_size.width,win_size.height,NULL,NULL,m_hInstance,NULL);
	//if (!popenGLWindow->CreateEx(0,WINDOW_CLASS_NAME,"Video Open",popenGLWindow->m_styleFS,popenGLWindow->m_rectFS,NULL,4703))//标题，宽高，颜色表示位,true
 	if (!g_hWnd)
	{//WS_MAXIMIZE|WS_POPUPWINDOW
		WLOGH_ERRO << L"main window create error";
 		return FALSE;								// Quit If Window Was Not Created
 	}
	
	if(!popenGLWindow->initMgr(g_hWnd))
	{
		WLOGH_ERRO << L"Init Mgr Failed";
		return FALSE;
	}

// 	nHY_Width = 400;
// 	nHY_Heigth = 300;
// 	HWND hHY = ::CreateWindow(WINDOW_CLASS_NAME,"幻影",popenGLWindow->m_styleFS,0,0,nHY_Width,nHY_Heigth,g_hWnd,NULL,m_hInstance,NULL);
// 	RECT rectHY;
// 	rectHY.left = 0;
// 	rectHY.top = 0;
// 	rectHY.right = nHY_Width;
// 	rectHY.bottom = nHY_Heigth;
// 	popenGLWindow->setHYHwndRect(hHY,rectHY);

	prepareWindow(popenGLWindow,win_size);

	popenGLWindow->CreateThread();

	MSG msg = {0};
	bool bLoop = true;
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	popenGLWindow->ReleaseThread();
	return FALSE;
}

bool KillProcessFromName(const char* pProcessName)  
{  
	bool re = false;
	std::string strProcessName = pProcessName;
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)  
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  

	//PROCESSENTRY32进程快照的结构体  
	PROCESSENTRY32 pe;  

	//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作  
	pe.dwSize = sizeof(PROCESSENTRY32);  


	//下面的IF效果同:  
	if((hSnapShot == INVALID_HANDLE_VALUE) || (!Process32First(hSnapShot,&pe)))
	{  
		SAFE_CLOSEFILEHANDLE(hSnapShot);
		goto final;
	}  

	//将字符串转换为小写  
	for_each(strProcessName.begin(),strProcessName.end(),tolower);
	//如果句柄有效  则一直获取下一个句柄循环下去  
	while (Process32Next(hSnapShot,&pe))
	{
		//pe.szExeFile获取当前进程的可执行文件名称  
		std::string scTmp = pe.szExeFile;  

		//将可执行文件名称所有英文字母修改为小写  
		for_each(scTmp.begin(),scTmp.end(),tolower);

		//比较当前进程的可执行文件名称和传递进来的文件名称是否相同  考虑到64位机器 (xxx.exe *32)
		if( 0 == scTmp.find(strProcessName))
		{  
			//从快照进程中获取该进程的PID(即任务管理器中的PID)  
			DWORD dwProcessID = pe.th32ProcessID;  
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);  
			::TerminateProcess(hProcess,0);  
			CloseHandle(hProcess);  
			goto final;
		}  
	}  

final:
	SAFE_CLOSEFILEHANDLE(hSnapShot);
	return re;
}  


