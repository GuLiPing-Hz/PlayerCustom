// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ��������Ϊ�ʺ� Windows Me ����߰汾����Ӧֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS
#define STRSAFE_NO_DEPRECATE

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��

#include <objbase.h>
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <atlbase.h>
#include <streams.h>
#define __IDxtCompositor_INTERFACE_DEFINED__  
#define __IDxtAlphaSetter_INTERFACE_DEFINED__  
#define __IDxtJpeg_INTERFACE_DEFINED__  
#define __IDxtKey_INTERFACE_DEFINED__  
#include <qedit.h>
#include "wmsdk.h"

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <comdef.h>
#include <atlbase.h>
#include <commdlg.h>
#include <tchar.h>

// DirectShow Header Files
#include <dshow.h>
#include <streams.h>
#include <vmr9.h>
#include <assert.h>
//source filter E436EBB5-524F-11CE-9F530020AF0BA770
// static const GUID CLSID_FILESourceAsync =			{0xe436ebb5,0x524f,0x11ce, {0x9f,0x53,0x00,0x20,0xaf,0x0b,0xa7,0x70} };
//��Ƶ��Ƶ����transform filter "136DCBF5-3874-4B70-AE3E15997D6334F7"
// static const GUID CLSID_MPEG_Demultiplexer = {0x136dcbf5,0x3874,0x4b70,{0xae,0x3e,0x15,0x99,0x7d,0x63,0x34,0xf7} };
//avi ��Ƶ��Ƶ���� "1B544C20-FD0B-11CE-8C6300AA0044B51E"
// static const GUID CLSID_AVI_Demultiplexer = {0x1b544c20,0xfd0b,0x11ce,{0x8c,0x63,0x00,0xaa,0x00,0x44,0xb5,0x1e} };
//audio decoder filter "0F40E1E5-4F79-4988-B1A9CC98794E6B55"
// static const GUID CLSID_FFDShowAudioDecoder = {0x0f40e1e5,0x4f79,0x4988,{0xb1,0xa9,0xcc,0x98,0x79,0x4e,0x6b,0x55} };
//video decoder filter "04FE9017-F873-410E-871EAB91661A4EF7"
// static const GUID CLSID_FFDshowVideoDecoder = {0x04fe9017,0xf873,0x410e,{0x87, 0x1e, 0xab, 0x91, 0x66, 0x1a, 0x4e, 0xf7} }; 
//CLSID_LegacyAmFilterCategory ����ѡ��filter "18C16B08-6497-420E-AD1422D21C2CEAB7"
// static const GUID CLSID_GL_AudioSwitcher = {0x18C16B08,0x6497,0x420e,{0xAD,0x14,0x22,0xD2,0x1C,0x2C,0xEA,0xB7}};

#include "OpenGLWindow.h"
#include <afxdisp.h>

#define STATUS_OPENING						1
#define STATUS_OPENED							2
#define STATUS_OPEN_FAILED					3
#define STATUS_PLAYING							4
#define STATUS_STOPED							5
#define STATUS_PAUSED							6
#define STATUS_CLOSED							7
#define STATUS_BUFFERINGDATA			8

#ifndef CHECK_HR
#define CHECK_HR(hr, f) \
	if( FAILED(hr) ){ f; throw hr; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)  do{if((x)) \
	(x)->Release(); (x)=NULL; }while(0)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) \
	if ((x) != NULL)      \
{                   \
	delete (x);        \
	(x) = NULL;        \
}
#endif

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(x) \
	if ((x) != NULL)            \
{                         \
	delete[] (x);            \
	(x) = NULL;              \
}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x) \
	if ((x) != NULL)            \
{                         \
	free((x));            \
	(x) = NULL;              \
}
#endif

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(h)         \
	if ((h) != NULL)                  \
{                               \
	CloseHandle((h));             \
	(h) = NULL;                   \
}
#endif

#ifndef SAFE_CLOSEFILEHANDLE
#define SAFE_CLOSEFILEHANDLE(h)     \
	if ((h) != INVALID_HANDLE_VALUE)  \
{                               \
	CloseHandle((h));             \
	(h) = INVALID_HANDLE_VALUE;   \
}
#endif 

#ifndef MAKETEXTURESIZE
#define MAKETEXTURESIZE(w,h) \
	do \
{\
	int t_w=32;\
	int t_h=32;\
	while(t_w<(int)w)\
{\
	t_w <<= 1;\
}\
	while(t_h<(int)h)\
{\
	t_h <<= 1;\
}\
	w=t_w;\
	h=t_h;\
} while (false)
#endif 

#ifndef MAKESAMETEXTURESIZE
#define MAKESAMETEXTURESIZE(w,h) \
	do \
{\
	MAKETEXTURESIZE(w,h)\
	int t_x = max(w,h);\
	w=t_x;\
	h=t_x;\
} while (false)
#endif

void DbgMsg( char* szMessage, ... );

#endif // _AFX_NO_AFXCMN_SUPPORT



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


