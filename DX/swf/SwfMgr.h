#ifndef SWFMGR__H__
#define SWFMGR__H__

#include "../GLListDef.h"
#include "Swf.h"

typedef std::map<int,CSwf*> MAPIDSWF;

class CSwfMgr
{
public:
	CSwfMgr();
	~CSwfMgr();

	bool initSwfRender();
	void uninitSwfRender();

	//增加一个Swf文件，并预加载
	bool addSwf(int id,const char* file);

	//清空所有Swf
	void clearSwf();
	
	//加载需要使用到的swf文件，读取配置目录
	bool initAllSwf();

	
	//获取指定id的Swf
	CSwf* getSwf(int id);
	std::string getSwfFile(int id);
private:
	bool initAllSwfTexture();
	void uninitAllSwfTexture();
	bool addSwfToMap(int id,CSwf* pSwf);
	bool addSwfToMap(int id,const std::string file);
private:
	//gameswf::player*	m_pPlayer;
	MAPIDSWF				m_mapSwf;
	MAPIDSTRING		m_mapSwfFile;
	LISTSTRING				m_listFiles;
};

#endif//SWFMGR__H__
