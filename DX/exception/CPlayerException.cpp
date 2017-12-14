#include "CPlayerException.h"
#include <Windows.h>

CPlayerException::CPlayerException(const wchar_t* wmsg)
:std::runtime_error("")
{
	wcscpy(this->wmsg,wmsg);
}

CPlayerException::CPlayerException(const CPlayerException& ex)
:std::runtime_error("")
{
	wcscpy(this->wmsg,ex.getWWhat());
}

const char * CPlayerException::what() const
{
	static char msg[600] = {0};
	WideCharToMultiByte(CP_ACP,0,wmsg,wcslen(wmsg),msg,sizeof(msg)-1,NULL,NULL);
	return msg;
}

const wchar_t* CPlayerException::getWWhat() const
{
	return wmsg;
}

