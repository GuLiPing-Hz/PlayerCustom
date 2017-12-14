#include "stdafx.h"
#include "MiniDump.h"
#include <cctype>
#include <algorithm>
#include <Dbghelp.h>
#include <string>

//-----------------------------------------------------------------------------
unsigned long CMiniDump::msId = 0;
bool CMiniDump::mbFulldump = false;
CMiniDump::IListener * CMiniDump::mpListener = NULL;
MINIDUMP_TYPE minidump_type_level_mini	= (MINIDUMP_TYPE)(MiniDumpNormal);
MINIDUMP_TYPE minidump_type_level_full	= (MINIDUMP_TYPE)(MiniDumpWithFullMemory);

unsigned char	CMiniDump::mOldCode[16] = { 0 };
void *			CMiniDump::mpFunSetUnhandledExceptionFilter = NULL;
int				CMiniDump::mSize = 0;

//-----------------------------------------------------------------------------
// This static method registers our own exception handler with Windows.
void CMiniDump::setup( IListener * listener )
{
	mpListener = listener;

	::SetUnhandledExceptionFilter( CMiniDump::_callbackException );

	_disableSetUnhandledExceptionFilter();
}

//-----------------------------------------------------------------------------
// This method is called by n_assert() and n_error() to write out a minidump file.
bool CMiniDump::writeMiniDump()
{
	return !! CMiniDump::_writeMiniDumpImpl( NULL );
}

void CMiniDump::setCreateFulldumpFile( bool fulldump )
{
	mbFulldump = fulldump;
}

//-----------------------------------------------------------------------------
void CMiniDump::enableSetUnhandledExceptionFilter()
{
	if ( NULL != mpFunSetUnhandledExceptionFilter )
	{
		DWORD dwOldFlag, dwTempFlag;
		::VirtualProtect( mpFunSetUnhandledExceptionFilter, mSize, PAGE_READWRITE, &dwOldFlag );
		::WriteProcessMemory( ::GetCurrentProcess(), mpFunSetUnhandledExceptionFilter, mOldCode, mSize, NULL );
		::VirtualProtect( mpFunSetUnhandledExceptionFilter, mSize, dwOldFlag, &dwTempFlag );
	}
}

//-----------------------------------------------------------------------------
// Exception handler function called back by Windows when something unexpected happens.
LONG WINAPI CMiniDump::_callbackException( EXCEPTION_POINTERS * exceptionInfo )
{
	CMiniDump::_writeMiniDumpImpl( exceptionInfo );

	return EXCEPTION_CONTINUE_SEARCH;
}

//-----------------------------------------------------------------------------
std::string CMiniDump::_buildMiniDumpFilename()
{
	char szPath[ MAX_PATH ];
	::GetModuleFileName( NULL, szPath, MAX_PATH );
	std::string file_name( szPath );
	transform(file_name.begin(), file_name.end(), file_name.begin(), tolower); 
	file_name = file_name.substr(0, file_name.rfind(".exe"));

	DWORD proc_id = ::GetCurrentProcessId();

	SYSTEMTIME t;
	::GetLocalTime( &t );
	char buf[1024] = {0};
	sprintf_s(buf, 1024, "_[%04d-%02d-%02d][%02d-%02d-%02d]%d_%d.dmp", t.wYear , t.wMonth , t.wDay , t.wHour , t.wMinute , t.wSecond , proc_id , msId);
	file_name += buf;

	msId ++;

	return file_name;
}

//-----------------------------------------------------------------------------
// Private method to write a mini-dump with extra exception info. This 
// method is either called from the public WriteMiniDump() method or
// from the ExceptionCallback() function.
BOOL CMiniDump::_writeMiniDumpImpl( EXCEPTION_POINTERS * exceptionInfo )
{//add by glp
	std::string szDumpFile = _buildMiniDumpFilename();
	char buf [260] = {0};
	sprintf_s(buf,259,"szDumpFile %s\n",szDumpFile.c_str());
	OutputDebugString(buf);
	HANDLE hFile_Mini = ::CreateFile( szDumpFile.c_str(),
		GENERIC_WRITE,            // dwDesiredAccess
		0/*FILE_SHARE_READ*/,          // dwShareMode
		NULL,                        // lpSecurityAttributes
		CREATE_ALWAYS,            // dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,    // dwFlagsAndAttributes
		NULL);                    // hTemplateFile
	if ( INVALID_HANDLE_VALUE == hFile_Mini )
	{
		sprintf_s(buf,259,"hFile_Mini is invalid %x\n",(DWORD_PTR)hFile_Mini);
		OutputDebugString(buf);
		return false;
	}
	
	HANDLE hFile_Full = NULL;
	if ( mbFulldump )
	{
		// add by Eric.Liu 2011.4.12
		// 生成完整的内存转储信息-暂行
		TCHAR szPath[ MAX_PATH ];
		::GetModuleFileName( NULL, szPath, MAX_PATH );
		std::string file_name( szPath );
		transform(file_name.begin(), file_name.end(), file_name.begin(), tolower); 
		file_name = file_name.substr(0, file_name.rfind(".exe"));

		hFile_Full = ::CreateFile( (file_name+"_fulldumpfile.dmp").c_str(),
			GENERIC_WRITE,            // dwDesiredAccess
			0/*FILE_SHARE_READ*/,          // dwShareMode
			NULL,                        // lpSecurityAttributes
			CREATE_ALWAYS,            // dwCreationDisposition,
			FILE_ATTRIBUTE_NORMAL,    // dwFlagsAndAttributes
			NULL);                    // hTemplateFile
		//if ( NULL == hFile_Full ) return false;
	}

	HANDLE hProc = ::GetCurrentProcess();
	DWORD procId = ::GetCurrentProcessId();
	BOOL res = FALSE;
	if ( exceptionInfo )
	{
		// extended exception info is available
		MINIDUMP_EXCEPTION_INFORMATION extInfo = { 0 };
		extInfo.ThreadId = ::GetCurrentThreadId();
		extInfo.ExceptionPointers = exceptionInfo;
		extInfo.ClientPointers = TRUE;
		res = ::MiniDumpWriteDump( hProc, procId, hFile_Mini, minidump_type_level_mini, &extInfo, NULL, NULL );
		char buf [260] = {0};
		sprintf_s(buf,259,"MiniDumpWriteDump write %s,hProc %x, procId %d\n",res?"OK":"Failed",(DWORD_PTR)hProc, procId);
		OutputDebugString(buf);
		if ( hFile_Full ) res = ::MiniDumpWriteDump( hProc, procId, hFile_Full, minidump_type_level_full, &extInfo, NULL, NULL );
	}
	else
	{
		// extended exception info is not available
		res = ::MiniDumpWriteDump( hProc, procId, hFile_Mini, minidump_type_level_mini, NULL, NULL, NULL );
		if ( hFile_Full != INVALID_HANDLE_VALUE ) res = ::MiniDumpWriteDump( hProc, procId, hFile_Full, minidump_type_level_full, NULL, NULL, NULL );
	}

	::CloseHandle( hFile_Mini );
	if ( hFile_Full != INVALID_HANDLE_VALUE && !hFile_Full ) ::CloseHandle( hFile_Full );

	if ( mpListener )
	{
		mpListener->onDump( szDumpFile );
	}

	return res;
}

//-----------------------------------------------------------------------------
void CMiniDump::_disableSetUnhandledExceptionFilter()
{
	mpFunSetUnhandledExceptionFilter = ( void * )::GetProcAddress( 
		::LoadLibrary( _T( "kernel32.dll" ) ), "SetUnhandledExceptionFilter" );

	if ( mpFunSetUnhandledExceptionFilter ) 
	{
		unsigned char code[ 16 ];
		mSize = 0;
		code[ mSize ++ ] = 0x33;
		code[ mSize ++ ] = 0xC0;
		code[ mSize ++ ] = 0xC2;
		code[ mSize ++ ] = 0x04;
		code[ mSize ++ ] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		::VirtualProtect( mpFunSetUnhandledExceptionFilter, mSize, PAGE_READWRITE, &dwOldFlag );
		//::ReadProcessMemory( ::GetCurrentProcess(), mpFunSetUnhandledExceptionFilter, mOldCode, mSize, NULL );
		::WriteProcessMemory( ::GetCurrentProcess(), mpFunSetUnhandledExceptionFilter, code, mSize, NULL );
		::VirtualProtect( mpFunSetUnhandledExceptionFilter, mSize, dwOldFlag, &dwTempFlag );
	}
}
