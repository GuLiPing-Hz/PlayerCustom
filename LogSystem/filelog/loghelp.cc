#include "loghelp.h"
#include "logwrapper.h"
#include <Windows.h>



const char* DescribeFile(const char* file) 
{
	if(!file)
		return NULL;

	const char* end1 = ::strrchr(file, '/');
	const char* end2 = ::strrchr(file, '\\');
	if (!end1 && !end2)
		return file;
	else
		return (end1 > end2) ? end1 + 1 : end2 + 1;
}

LogHelp::LogHelp(const char* file,int line,eLogLevel ell,bool is_unicode)
:is_unicode_(is_unicode)
{
	level  = ell;
	if(!is_unicode_)
	{
		switch(ell)
		{
		case LL_INFO:
			{
				slog_ << "[LL_INFO]:";
				break;
			}
		case LL_WARNING:
			{
				slog_ << "[LL_WARNING]:";
				break;
			}
		case LL_ERROR:
		default:
			{
				slog_ << "[LL_ERROR]:";
				break;
			}
		}
		SYSTEMTIME st;
		GetLocalTime(&st);
		static char timeBuf[260];
		sprintf_s(timeBuf,259,"[%04d-%02d-%02d,%02d:%02d:%02d.%03d]:",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		slog_ << timeBuf;
		slog_ << "(" << DescribeFile(file) << ":" << line <<":";
	}
	else
	{
		switch(ell)
		{
		case LL_INFO:
			{
				wslog_ << L"[LL_INFO]:";
				break;
			}
		case LL_WARNING:
			{
				wslog_ << L"[LL_WARN]:";
				break;
			}
		case LL_ERROR:
		default:
			{
				wslog_ << L"[LL_ERRO]:";
				break;
			}
		}
		SYSTEMTIME st;
		GetLocalTime(&st);
		static wchar_t timeBuf[260];
		swprintf(timeBuf,L"[%04d-%02d-%02d,%02d:%02d:%02d.%03d]:",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		wslog_ << timeBuf;

		const char* pFile = DescribeFile(file);
		wchar_t wFile[260] = {0};
		MultiByteToWideChar(CP_ACP,0,pFile,strlen(pFile),wFile,259);
		wslog_ << L"(" << wFile << L":" << line <<L":";
	}
	
}

LogHelp::~LogHelp()
{
#ifdef LOG_NOINFO
	if(level != LL_INFO)
#endif//LOG_NOINFO
	{
		if(is_unicode_)
		{
			std::wstring wslog = wslog_.str();
			static char static_log[1024] = {0};
			int n = WideCharToMultiByte(CP_UTF8,0,wslog.c_str(),wslog.length(),static_log,1020,NULL,NULL);

#ifdef WIN32
			static_log[n++] = '\r';
			static_log[n++] = '\n';
#else
			static_log[n++] = '\n';
#endif
			static_log[n] = 0;
			LogWrapper::writeLog(static_log);
		}
		else
		{
			std::string slog= slog_.str();
			LogWrapper::writeLog(slog.c_str());
		}
	}
}

void LogHelp::InitLogHelp(const char* logfile,bool utf8)
{
	LogWrapper::createLogWrapper(utf8,logfile);
}

std::ostream& LogHelp::stream()
{ 
	return slog_; 
}

std::wostream& LogHelp::wstream()
{
	return wslog_;
}

