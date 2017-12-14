/*
	注释时间:2014-4-25
	dump收集类
*/
#pragma once
//#include "Prerequisites.h"
#include <string>

#ifndef TS_FILE_CRASHREPORTER
#define TS_FILE_CRASHREPORTER "CrashReporter.exe"
#endif

#ifndef _M_IX86
#error "The following code only works for x86!"
#endif

//-----------------------------------------------------------------------------
class  CMiniDump
{
public:
	class IListener
	{
	public:
		virtual ~IListener() {}

		virtual void onDump( const std::string & dump_filename ) = 0;
	};

public:
	// setup the the Win32 exception callback hook
	static void setup( IListener * listener );

	// write a mini dump
	static bool writeMiniDump();

	// add by Eric.Liu 2011.4.12
	// create a fullmemory dump file
	static void setCreateFulldumpFile( bool fulldump=false );

	static void enableSetUnhandledExceptionFilter();

private:
	// internal mini-dump-writer method with extra exception info
	static BOOL _writeMiniDumpImpl( EXCEPTION_POINTERS * exceptionInfo );

	// build a filename for the dump file
	static std::string _buildMiniDumpFilename();

	// the actual exception handler function called back by Windows
	static LONG WINAPI _callbackException( EXCEPTION_POINTERS * exceptionInfo );

	static void _disableSetUnhandledExceptionFilter();

private:
	static unsigned long			msId;
	static IListener *					mpListener;
	static bool							mbFulldump;
	static unsigned char			mOldCode[16];
	static void *						mpFunSetUnhandledExceptionFilter;
	static int								mSize;
};

class CDumpListener : public CMiniDump::IListener
{
public:
	//使用exe 提交dump文件
	virtual void onDump( const std::string & dump_filename )
	{
		PROCESS_INFORMATION piProcInfoGPS;
		STARTUPINFO siStartupInfo;
		SECURITY_ATTRIBUTES saProcess, saThread;
		ZeroMemory( &siStartupInfo, sizeof(siStartupInfo) );
		siStartupInfo.cb = sizeof(siStartupInfo);
		saProcess.nLength = sizeof(saProcess);
		saProcess.lpSecurityDescriptor = NULL;
		saProcess.bInheritHandle = true;
		saThread.nLength = sizeof(saThread);
		saThread.lpSecurityDescriptor = NULL;
		saThread.bInheritHandle = true;

		std::string filename = TS_FILE_CRASHREPORTER;
		filename += " ";
		filename += dump_filename;

		::CreateProcessA( NULL, ( LPSTR )filename.c_str(),
			&saProcess, &saThread, FALSE,
			CREATE_DEFAULT_ERROR_MODE, NULL, NULL, 
			&siStartupInfo, &piProcInfoGPS );
	}
};


