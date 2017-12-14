#ifndef LOGWRAPPER__H__GLP
#define LOGWRAPPER__H__GLP
#include <string>
#include <file_wrapper.h>

class LogWrapper
{
public:
	static LogWrapper* createLogWrapper(bool utf8=true,const char* file=NULL);

	static void writeLog(const char* log);
	static void writeLog(const wchar_t* log);
private:
	LogWrapper();
	~LogWrapper();
private:
	webrtc::FileWrapper*			m_file;
	static char							s_fileName[260];
	static LogWrapper				s_instance;
	static bool							s_utf8;
};


#endif//LOGWRAPPER__H__GLP