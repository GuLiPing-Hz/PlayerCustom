/*
	注释时间:2014-4-25
	author: glp
	封装一个CFloridMgr类，用于管理CGLFlorid类。
	提供加载所有的粒子系统，
	停止所有，更新所有粒子系统等操作。
*/
#pragma once
#include "GLFlorid.h"
#include <map>
#include <string>
#include <string.h>

struct _tFastLessCompare
{
	bool operator() (const std::string& a, const std::string& b) const
	{
		const size_t la = a.length();
		const size_t lb = b.length();
		if (la == lb)
			return (memcmp(a.c_str(), b.c_str(), la) < 0);
		return (la < lb);
	}
};
typedef std::map<std::string,CGLFlorid*,_tFastLessCompare> MAPSTRFLORID;

class CFloridMgr
{
	void clearVect();
public:
	CFloridMgr(void);
	virtual ~CFloridMgr(void);

	CGLFlorid*	getFlorid(const std::string strFloridName);
	bool getEnvArgInit(){return m_bEnvInit;}
	//设置加载系统需要的相应信息
	void setEnvArg(const std::string szIniFile,const wchar_t* appDir);
	//添加一个已经创建的粒子系统，内存交由CFloridMgr管理。
	bool addFlorid(const std::string strFloridName,CGLFlorid* florid);
	//添加一个粒子系统根据提供的相应内容。
	bool addFlorid(const std::string strFloridName,int n,float z=GL3D_Z_PARTICLE,const char* maskFile=NULL);
	//去掉一个粒子系统，根据名字。
	void subFlorid(const std::string strFloridName);
	//清空所有粒子系统
	void clearFlorid();
	
	//启动指定名字的粒子系统，指定存活时间
	void start(const std::string strFloridName,const float fliveTime);
	//启动所有已加载的粒子系统
	void startAll();
	//停止所有
	void stopAll();
	//更新所有
	void updateAll(const float delta_second);//秒
	//渲染所有
	void renderAll();
	//初始化所有粒子系统
	bool initAllFlorid();
	
private:
	bool							m_bEnvInit;
	std::string				m_szIniFile;
	std::wstring				m_appDir;
protected:
	MAPSTRFLORID		m_mapFlorid;
};
