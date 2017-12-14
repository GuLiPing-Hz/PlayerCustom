#include "StdAfx.h"
#include "FloridMgr.h"
#include "../Opengl.h"
#include "../staff/image/ImgsetMgr.h"

CFloridMgr::CFloridMgr(void)
:m_bEnvInit(false)
{
	m_mapFlorid.clear();
}

CFloridMgr::~CFloridMgr(void)
{
	clearVect();
}

void CFloridMgr::clearVect()
{
	if (!m_mapFlorid.empty())
	{
		MAPSTRFLORID::iterator i;
		for (i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
		{
			SAFE_DELETE(i->second);
		}
	}
	m_mapFlorid.clear();
}

void CFloridMgr::setEnvArg(const std::string szIniFile,const wchar_t* appDir)
{
	m_szIniFile = szIniFile;
	m_appDir = appDir;

	m_bEnvInit = true;
}


CGLFlorid*	CFloridMgr::getFlorid(const std::string strFloridName)
{
	if (!m_mapFlorid.empty())
	{
		MAPSTRFLORID::iterator i;
		for(i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
		{
			if (strFloridName == i->first)
			{
				return (i->second);
			}
		}
	}
	return NULL;
}

bool CFloridMgr::addFlorid(const std::string strFloridName,CGLFlorid* florid)
{
	MAPSTRFLORID::iterator i = m_mapFlorid.find(strFloridName);
	if (i==m_mapFlorid.end())
	{
		m_mapFlorid.insert(std::pair<std::string,CGLFlorid*>(strFloridName,florid));
		return true;
	}
	return false;
}

bool CFloridMgr::addFlorid(const std::string strFloridName,int n,float z,const char* maskFile)
{
	if (!m_bEnvInit)
		return false;

	CGLFlorid*	pglFlorid = new CGLFlorid(m_szIniFile);
	if (!pglFlorid)
	{
		WLOGH_ERRO << L"glFlorid new error";
		return false;
	}
	pglFlorid->initPS(m_appDir.c_str(),n,z);
	if (maskFile)
	{
		if(!pglFlorid->setMaskPng(maskFile))
			goto failed;
	}
	if(!addFlorid(strFloridName,pglFlorid))
		goto failed;

	return true;

failed:
	WLOGH_ERRO << L"load psi file failed";
	SAFE_DELETE(pglFlorid);
	return false;
}


void CFloridMgr::subFlorid(const std::string strFloridName)
{
	if (!m_mapFlorid.empty())
	{
		MAPSTRFLORID::iterator i;
		for(i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
		{
			if (strFloridName == i->first)
			{
				SAFE_DELETE(i->second);
				m_mapFlorid.erase(i);
			}
		}
	}
}

void CFloridMgr::start(const std::string strFloridName,float fliveTime)
{
	if (!m_mapFlorid.empty())
	{
		MAPSTRFLORID::iterator i;
		for(i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
		{
			if (strFloridName == i->first)
			{
				(i->second)->startPS(fliveTime);
			}
		}
	}
}

void CFloridMgr::stopAll()
{
	if (!m_mapFlorid.empty())
	{
		MAPSTRFLORID::iterator i;
		for(i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
		{
			(i->second)->stopPS();
		}
	}
}

void CFloridMgr::startAll()
{
	if (!m_mapFlorid.empty())
	{
		MAPSTRFLORID::iterator i;
		for(i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
		{
			(i->second)->startPS();
		}
	}
}

void CFloridMgr::clearFlorid()
{
	clearVect();
}

void CFloridMgr::updateAll(const float delta_second)
{
	MAPSTRFLORID::iterator i;
	for(i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
	{
		if(i->second)
		{
			(i->second)->move(delta_second);
			(i->second)->updatePS(delta_second);
		}
	}
}

void CFloridMgr::renderAll()
{
	MAPSTRFLORID::iterator i;
	for(i=m_mapFlorid.begin();i!=m_mapFlorid.end();i++)
	{
		if(i->second && !i->second->m_bFromOther)//暂时替代
			(i->second)->renderPS();
	}
}

bool CFloridMgr::initAllFlorid()
{
	bool b = addFlorid("starflorid",1);//wave界面的，星星特效
	b &= addFlorid("sstarflorid",1);
	b &= addFlorid("cursor",2);//游标特效
	b &= addFlorid("cursor2",2);//游标2特效
	//first
	b &= addFlorid("scoreleftF",3);//评分界面的左边特效1
	b &= addFlorid("scoreleftS",4);//评分界面的左边特效2
	b &= addFlorid("scorerightF",5);//评分界面的右边特效1
	b &= addFlorid("scorerightS",6);//评分界面的右边特效2
	b &= addFlorid("star",7);//评分界面的星球图片的上的落星特效
	b &= addFlorid("starS",8);//评分界面的星星特效
	b &= addFlorid("planetcommon",9);//评分界面的星体图片特效
	b &= addFlorid("explode",10);//评分界面的爆炸效果
	b &= addFlorid("screenstar",11);//评分界面的繁星特效，暂时不用

	CGLFlorid* pFlorid = new CGLFlorid(m_szIniFile,true);
	Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
	if(!pFlorid)
		return false;
	pFlorid->setImageset(pImgset,"ParticleBG");
	pFlorid->initPS(m_appDir.c_str(),12,GL3D_Z_BASE);
	bool bTmp = addFlorid("finalplanet",pFlorid);
	if(!bTmp)
		delete pFlorid;

	pFlorid = new CGLFlorid(m_szIniFile,true);
	if(!pFlorid)
		return false;
	pFlorid->setImageset(pImgset,"ParticleBG");
	pFlorid->initPS(m_appDir.c_str(),12,GL3D_Z_BASE);
	bTmp = addFlorid("finalplanetS",pFlorid);
	if(!bTmp)
		delete pFlorid;

	//b &= addFlorid("finalplanet",12);//特殊特效
	pFlorid = new CGLFlorid(m_szIniFile,true);
	if(!pFlorid)
		return false;
	pFlorid->setImageset(pImgset,"ParticleBG2");
	pFlorid->initPS(m_appDir.c_str(),13,GL3D_Z_BASE);
	b &= addFlorid("finalplanet2",pFlorid);
	if(!bTmp)
		delete pFlorid;

	pFlorid = new CGLFlorid(m_szIniFile,true);
	if(!pFlorid)
		return false;
	pFlorid->setImageset(pImgset,"ParticleBG2");
	pFlorid->initPS(m_appDir.c_str(),13,GL3D_Z_BASE);
	b &= addFlorid("finalplanet2S",pFlorid);
	if(!bTmp)
		delete pFlorid;
	//b &= addFlorid("finalplanet2",13);//特殊特效2

	b &= addFlorid("bigbangF",14);//宇宙大爆炸3个特效
	b &= addFlorid("bigbangS",15);
	b &= addFlorid("bigbangT",16);
	b &= addFlorid("planet6",17);//星体六的特效
	b &= addFlorid("planet7",18);//星体七的特效
	//second
	b &= addFlorid("sscoreleftF",3);//评分界面的左边特效1
	b &= addFlorid("sscoreleftS",4);//评分界面的左边特效2
	b &= addFlorid("sscorerightF",5);//评分界面的右边特效1
	b &= addFlorid("sscorerightS",6);//评分界面的右边特效2
	b &= addFlorid("sstar",7);//评分界面的星球图片的上的落星特效
	b &= addFlorid("sstarS",8);//评分界面的星星特效
	b &= addFlorid("splanetcommon",9);//评分界面的星体图片特效
	b &= addFlorid("sexplode",10);//评分界面的爆炸效果
	b &= addFlorid("sscreenstar",11);//评分界面的繁星特效，暂时不用

	b &= addFlorid("sbigbangF",14);//宇宙大爆炸3个特效
	b &= addFlorid("sbigbangS",15);
	b &= addFlorid("sbigbangT",16);
	b &= addFlorid("splanet6",17);//星体六的特效
	b &= addFlorid("splanet7",18);//星体七的特效


	b &= addFlorid("good",19);//好的特效
	b &= addFlorid("sgood",19);//好2的特效
	b &= addFlorid("nice1",20);//酷的特效
	b &= addFlorid("nice2",21);//
	b &= addFlorid("snice1",20);//酷2的特效
	b &= addFlorid("snice2",21);//
	b &= addFlorid("cool1",22);//厉害的特效
	b &= addFlorid("cool2",23);//
	b &= addFlorid("scool1",22);//厉害2的特效
	b &= addFlorid("scool2",23);//
	b &= addFlorid("perfect1",24);//完美的特效
	b &= addFlorid("perfect2",25);//
	b &= addFlorid("perfect3",26);//
	b &= addFlorid("sperfect1",24);//完美2的特效
	b &= addFlorid("sperfect2",25);//
	b &= addFlorid("sperfect3",26);//
	b &= addFlorid("godlike1",27);//天籁之音的特效
	b &= addFlorid("godlike2",28);//
	b &= addFlorid("godlike3",29);//
	b &= addFlorid("godlike4",30);//
	b &= addFlorid("godlike5",31);//
	b &= addFlorid("sgodlike1",27);//天籁之音2的特效
	b &= addFlorid("sgodlike2",28);//
	b &= addFlorid("sgodlike3",29);//
	b &= addFlorid("sgodlike4",30);//
	b &= addFlorid("sgodlike5",31);//
	b &= addFlorid("unbelivable1",32);//K歌之王的特效
	b &= addFlorid("unbelivable2",33);//
	b &= addFlorid("unbelivable3",34);//
	b &= addFlorid("unbelivable4",35);//
	b &= addFlorid("unbelivable5",36);//
	b &= addFlorid("sunbelivable1",32);//K歌之王2的特效
	b &= addFlorid("sunbelivable2",33);//
	b &= addFlorid("sunbelivable3",34);//
	b &= addFlorid("sunbelivable4",35);//
	b &= addFlorid("sunbelivable5",36);//
	b &= addFlorid("waveflorid1",37);//中间效果
	b &= addFlorid("waveflorid2",38);//
	b &= addFlorid("swaveflorid1",37);//中间效果2
	b &= addFlorid("swaveflorid2",38);//

	b &= addFlorid("starmove",39);//星星移动特效
	b &= addFlorid("sstarmove",39);//星星移动特效2

	b &= addFlorid("specialstar",40,GL3D_Z_NOTIFY);//上榜通知特效
	
	b &= addFlorid("goldget1",41,GL3D_Z_NOTIFY);//金币获取特效
	b &= addFlorid("goldget2",42,GL3D_Z_NOTIFY);
	b &= addFlorid("goldget3",43,GL3D_Z_NOTIFY);
	b &= addFlorid("goldget4",44,GL3D_Z_NOTIFY);

	pFlorid = new CGLFlorid(m_szIniFile,true);
	if(!pFlorid)
		return false;
	pFlorid->setImageset(pImgset,"GoldBagParticle");
	pFlorid->initPS(m_appDir.c_str(),45,GL3D_Z_BASE);
	b &= addFlorid("goldbagbg",pFlorid);
	if(!bTmp)
		delete pFlorid;
	//////////////////////////////////////////////////////////////////////////
	return b;
}

