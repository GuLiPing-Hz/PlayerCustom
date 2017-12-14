/*
	注释时间:2014-4-25
	author: glp
	提供基础方法
*/
#ifndef OPENGL__H__
#define  OPENGL__H__

#include "GLDef.h"
#include <WinDef.h>
#include <string>
#include <vector>
#include <loghelp.h>

GPoint CGPointMake(float x,float y,float z=GL3D_Z_BASE);

GSize CGSizeMake(int x,int y);

CGRect CGRectMake(float x,float y,float z,int w,int h);

CGRect CGRectMake(const GPoint& point,const GSize& size);

CGRect CGRectIntersection(const CGRect& a,const CGRect& b);//返回两Rect相交部分

//配置文件
void			Ini_SetInt(const char *szIniFile,const char *section, const char *name, int value);
int 			Ini_GetInt(const char *szIniFile,const char *section, const char *name, int def_val);
void			Ini_SetFloat(const char *szIniFile,const char *section, const char *name, float value);
float			Ini_GetFloat(const char *szIniFile,const char *section, const char *name, float def_val);
void			Ini_SetString(const char *szIniFile,const char *section, const char *name, const char *value);
char*		Ini_GetString(const char *szIniFile,const char *section, const char *name, const char *def_val);
wchar_t*	Ini_GetStringW(const char *szIniFile,const char *section, const char *name, const char *def_val);

void Str2Wstr(IN const std::string str,OUT std::wstring &wstr);
void Wstr2Str(IN const std::wstring wstr,OUT std::string &str);
void Unicode2Utf8(OUT char* utf8,IN OUT int& utf8_size,IN wchar_t* unicode);
void GetOnlyDifferentFont(IN OUT std::wstring &font_all);
bool CalculateDB(IN const std::vector<float> buffer,OUT double& db);

void initLogFile(const char* logFile);
void writeLog(const char *msg,...);
void writeLog(const wchar_t *msg);

#endif //OPENGL__H__
