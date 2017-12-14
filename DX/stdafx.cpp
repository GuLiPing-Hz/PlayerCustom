// stdafx.cpp : 只包括标准包含文件的源文件
// DX.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#ifdef _DEBUG
void DbgMsg( char* szMessage, ... )
{
	char szFullMessage[MAX_PATH];
	char szFormatMessage[MAX_PATH];

	// format message
	va_list ap;
	va_start(ap, szMessage);
	(void)StringCchVPrintfA( szFormatMessage, NUMELMS(szFormatMessage), szMessage, ap );
	va_end(ap);
	(void)StringCchCatA( szFormatMessage, NUMELMS(szFormatMessage), "\n" );
	(void)StringCchCopyA( szFullMessage, NUMELMS(szFullMessage), "~*~*~*~*~*~ " );
	(void)StringCchCatA( szFullMessage, NUMELMS(szFullMessage), szFormatMessage );
	OutputDebugStringA( szFullMessage );
}
#else
void DbgMsg( char* szMessage, ... ){;}
#endif

