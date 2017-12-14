#include "stdafx.h"
#include "SwfMgr.h"
#include "../file/FileUtil.h"

//#include "gameswf/gameswf_player.h"

//tu_string flash_vars;

CSwfMgr::CSwfMgr()
//:m_pPlayer(NULL)
{
// 	m_pPlayer = new gameswf::player();
// 	if(m_pPlayer)
// 	{
// 		m_pPlayer->set_force_realtime_framerate(false);
// 		m_pPlayer->set_flash_vars(flash_vars);
// 	}
// 
// 	char swfDirAbsolute[260] = {0};
// 	const char* swfDir = Ini_GetString(cls_gl->getszIniFile(),"DIR","SwfDir",".");
// 	sprintf(swfDirAbsolute,"%s%s",cls_gl->getAppDir(),swfDir);
// 
// 	CFileUtil::GetFileListFromDir(swfDirAbsolute,".swf",m_listFiles);
// 
// 	LISTSTRING::iterator it = m_listFiles.begin();
// 	for(;it!=m_listFiles.end();it++)
// 	{
// 		const std::string& file = *it;
// 		int posStart = file.rfind("/");
// 		int posEnd = file.rfind(".swf");
// 		std::string name = file.substr(posStart+1,posEnd);
// 		int id = atoi(name.c_str());
// 		addSwfToMap(id,file);
// 	}
	
}
CSwfMgr::~CSwfMgr()
{
// 	assert(m_mapSwf.size() == 0);
// 
// 	SAFE_DELETE(m_pPlayer);
}

bool CSwfMgr::initSwfRender()
{
// 	CSwf::InitSwfRender();
// 	return initAllSwf();
	//return initAllSwfTexture();
	return true;
}
void CSwfMgr::uninitSwfRender()
{
	//uninitAllSwfTexture();
// 	clearSwf();
// 	CSwf::UninitSwfRender();
}

//增加一个Swf文件，并预加载
bool CSwfMgr::addSwf(int id,const char* file)
{
// 	CSwf* pSwf = new CSwf();
// 	if(!pSwf)
// 		return false;
// 
// 	bool b = pSwf->loadFile(m_pPlayer,file);
// 	if(!b)
// 	{
// 		SAFE_DELETE(pSwf);
// 		return b;
// 	}
// 
// 	b = addSwfToMap(id,pSwf);
// 	if(!b)//如果已经存在对应ID
// 		SAFE_DELETE(pSwf);
// 	return b;
	return true;
}

//清空所有Swf
void CSwfMgr::clearSwf()
{
// 	MAPIDSWF::iterator it = m_mapSwf.begin();
// 	for(;it!=m_mapSwf.end();it++)
// 	{
// 		if(it->second)
// 			delete it->second;
// 	}
// 	m_mapSwf.clear();
}

//加载需要使用到的swf文件，读取配置目录
bool CSwfMgr::initAllSwf()
{
// 	MAPIDSTRING::iterator it = m_mapSwfFile.begin();
// 	for(;it!=m_mapSwfFile.end();it++)
// 	{
// 		if(!addSwf(it->first,it->second.c_str()))
// 		{
// 			WLOGH_ERRO <<"swf file load filed.";
// 			return false;
// 		}
// 	}
	return true;
}

bool CSwfMgr::initAllSwfTexture()
{
	bool ret = true;
// 	MAPIDSWF::iterator it = m_mapSwf.begin();
// 	for(;it!=m_mapSwf.end();it++)
// 	{
// 		if(it->second)
// 			ret = it->second->initTexture();
// 		if(!ret)
// 			break;
// 	}
	return ret;
}
void CSwfMgr::uninitAllSwfTexture()
{
// 	MAPIDSWF::iterator it = m_mapSwf.begin();
// 	for(;it!=m_mapSwf.end();it++)
// 	{
// 		if(it->second)
// 			it->second->uninitTexture();
// 	}
}
//获取指定ID的Swf
CSwf* CSwfMgr::getSwf(int id)
{
// 	if(id == 0)
// 		return NULL;
// 
// 	MAPIDSWF::iterator it = m_mapSwf.find(id);
// 	if(it != m_mapSwf.end())
// 		return it->second;
	return NULL;
}

std::string CSwfMgr::getSwfFile(int id)
{
// 	if(id == 0)
// 		return "";
// 
// 	MAPIDSTRING::iterator it = m_mapSwfFile.find(id);
// 	if(it != m_mapSwfFile.end())
// 		return it->second;
	return "";
}

bool CSwfMgr::addSwfToMap(int id,CSwf* pSwf)
{
// 	if(!pSwf || id==0)
// 		return false;
// 
// 	std::pair<MAPIDSWF::iterator,bool> re;
// 	re = m_mapSwf.insert(std::make_pair(id,pSwf));
// 
// 	return re.second;
	return false;
}

bool CSwfMgr::addSwfToMap(int id,const std::string file)
{
// 	if(file.empty() || id == 0)
// 		return false;
// 
// 	std::pair<MAPIDSTRING::iterator,bool> re;
// 	re = m_mapSwfFile.insert(std::make_pair(id,file));
// 
// 	return re.second;
	return false;
}
