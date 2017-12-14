/*
	注释时间:2014-4-25
	author: glp
	应用程序入口
*/
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "Opengl.h"
class COpenGLWindow;

BOOL CALLBACK GLMonitorEnumProc( HMONITOR hMonitor,HDC hdcMonitor, LPRECT lprcMonitor,LPARAM dwData );

// CDXApp:
// 有关此类的实现，请参阅 DX.cpp
//
class CDXApp : public CWinApp
{
public:
	CDXApp();
	~CDXApp();
	
// 重写
	public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	
	static void prepareWindow(COpenGLWindow* popenGLWindow,const GSize& win_size);

	BOOL IsWindows7OrHigh();
// 实现
	DECLARE_MESSAGE_MAP()
};

extern CDXApp theApp;