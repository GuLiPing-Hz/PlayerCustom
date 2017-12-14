#include "logwrapper.h"

LogWrapper LogWrapper::s_instance;
bool LogWrapper::s_utf8 = true;
char	LogWrapper::s_fileName[260]={0};

LogWrapper::LogWrapper()
{
	m_file = webrtc::FileWrapper::Create();
}

LogWrapper::~LogWrapper()
{
	if (m_file)
	{
		delete m_file;
	}
}

LogWrapper* LogWrapper::createLogWrapper(bool utf8,const char* file)
{
	if (file)
	{
		s_utf8 = utf8;
		strcpy_s(s_fileName,file);
		s_instance.m_file->OpenFile(file,s_utf8,false,false,true,true);
	}
	return &s_instance;
}

void LogWrapper::writeLog(const char* log)
{
	int ret = s_instance.m_file->WriteText(log);
	if( ret == -2)
	{
		s_instance.m_file->OpenFile(s_instance.s_fileName,s_utf8,false,false,true,true);
		s_instance.m_file->WriteText(log);
	}
}

void LogWrapper::writeLog(const wchar_t* log)
{
	int ret = s_instance.m_file->WriteText(log);
	if( ret == -2)
	{
		s_instance.m_file->OpenFile(s_instance.s_fileName,s_utf8,false,false,true,true);
		s_instance.m_file->WriteText(log);
	}
}


