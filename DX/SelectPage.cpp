#include "StdAfx.h"
#include "OpenGLWindow.h"
#include "SelectPage.h"

CSelectPage::CSelectPage(const GSize& winsize)
:IComRender(NULL)
,m_pImgset(NULL)
,m_pImgBg(NULL)
,m_pImgBox(NULL)
,m_pImgSelect(NULL)
,m_fRate(10.0f)
,m_winSize(winsize)
,m_bStop(false)
{
	if(cls_gl->m_strCurCategory != "")
	{
		m_mapCurList = cls_gl->m_mapCategory[cls_gl->m_strCurCategory];
		assert (m_mapCurList.size() >= 5);
		m_itCur = m_mapCurList.begin();
	}

	m_structVD[0].y = 150.0f;
	m_structVD[0].z = 3.0f;
	m_structVD[0].alpha = 255;
	if(nextSong(m_structVD[0].sli))
		Str2Wstr(m_structVD[0].sli.songname,m_structVD[0].name);

	m_structVD[1].y = 240.0f;
	m_structVD[1].z = 3.0f;
	m_structVD[1].alpha = 255;
	if(nextSong(m_structVD[1].sli))
		Str2Wstr(m_structVD[1].sli.songname,m_structVD[1].name);

	m_structVD[2].y = 330.0f;
	m_structVD[2].z = 3.0f;
	m_structVD[2].alpha = 255;
	if(nextSong(m_structVD[2].sli))
		Str2Wstr(m_structVD[2].sli.songname,m_structVD[2].name);

	m_structVD[3].y = 420.0f;
	m_structVD[3].z = 3.0f;
	m_structVD[3].alpha = 255;
	if(nextSong(m_structVD[3].sli))
		Str2Wstr(m_structVD[3].sli.songname,m_structVD[3].name);

	m_structVD[4].y = 510.0f;
	m_structVD[4].z = 3.0f;
	m_structVD[4].alpha = 255;
	if(nextSong(m_structVD[4].sli))
		Str2Wstr(m_structVD[4].sli.songname,m_structVD[4].name);
}

CSelectPage::~CSelectPage(void)
{
}

bool CSelectPage::nextSong(SongListInfo& sli)
{
	if (m_mapCurList.empty())
	{
		return false;
	}

	if(m_itCur!=m_mapCurList.end())
	{
		sli.filename = m_itCur->second.filename;
		sli.singername = m_itCur->second.singername;
		sli.songname = m_itCur->second.songname;
	}
	else
	{
		m_itCur = m_mapCurList.begin();
		sli.filename = m_itCur->second.filename;
		sli.singername = m_itCur->second.singername;
		sli.songname = m_itCur->second.songname;
	}
	m_itCur++;
	return true;
}

inline void CSelectPage::algorithm(StructVertexDraw& svd,float change)
{
	svd.y+=change;
	svd.z=1.0f+fabsf(svd.y-330.0f)/90.0f;
	svd.alpha = uchar(255*(1.0f- fabsf(svd.y-330.0f)/230.0f));
	if(svd.y>560.0f)
	{
		//换歌
		nextSong(svd.sli);
		Str2Wstr(svd.sli.songname,svd.name);
		svd.alpha = 0;
		float littleChange=svd.y-560.0f;
		svd.y = 150.0f+littleChange;
	}
}

void CSelectPage::setStop(bool bstop)
{
	m_bStop=bstop;
	if(!m_bStop)
		m_fRate = 10.0f;
}

void	CSelectPage::init()
{
	m_pImgset = cls_gl->m_pImgsetMgr->getImageset(L"!renderer2");
	if(m_pImgset)
	{
		m_pImgBg = m_pImgset->getImage(L"bg");
		m_pImgBox = m_pImgset->getImage(L"box");
		m_pImgSelect = m_pImgset->getImage(L"select");
	}
}

void CSelectPage::update(const ulong delta_ms)
{
	StructVertexDraw& vd1 = m_structVD[0];
	StructVertexDraw& vd2 = m_structVD[1];
	StructVertexDraw& vd3 = m_structVD[2];
	StructVertexDraw& vd4 = m_structVD[3];
	StructVertexDraw& vd5 = m_structVD[4];

	algorithm(vd1,m_fRate*delta_ms/10.0f);
	algorithm(vd2,m_fRate*delta_ms/10.0f);
	algorithm(vd3,m_fRate*delta_ms/10.0f);
	algorithm(vd4,m_fRate*delta_ms/10.0f);
	algorithm(vd5,m_fRate*delta_ms/10.0f);

	if(m_bStop)
		m_fRate -= 0.01f;
	if(m_fRate <= 0.0f)
	{
		m_fRate = 0.0f;
		float minydistance;
		float ydistance1 = fabsf(vd1.y-330.0f);
		minydistance = ydistance1;
		float ydistance2 = fabsf(vd2.y-330.0f);
		minydistance = min(minydistance,ydistance2);
		float ydistance3 = fabsf(vd3.y-330.0f);
		minydistance = min(minydistance,ydistance3);
		float ydistance4 = fabsf(vd4.y-330.0f);
		minydistance = min(minydistance,ydistance4);
		float ydistance5 = fabsf(vd5.y-330.0f);
		minydistance = min(minydistance,ydistance5);
		if(minydistance > 10.0f)
		{
			m_fRate = 1.0f;
		}
		if(minydistance <= 10.0f)
		{
			//歌曲选中状态
		}
	}
}

void CSelectPage::drawGraphBuffer()
{
	if(!cls_gl->m_pSceneMgr->getFont())
		return ;

	StructVertexDraw& vd1 = m_structVD[0];
	StructVertexDraw& vd2 = m_structVD[1];
	StructVertexDraw& vd3 = m_structVD[2];
	StructVertexDraw& vd4 = m_structVD[3];
	StructVertexDraw& vd5 = m_structVD[4];

	if(m_pImgBg)
		m_pImgBg->draw(GPoint(-640.0f,-360.0f,4.0f),GSize(2560,1440));
	if(m_pImgBox)
	{
		static float s_x = m_winSize.width/2-m_pImgBox->getWidth()/2-100.0f;
		static float s_y = 50.0f;
		m_pImgBox->draw(GPoint(s_x,s_y,4.0f),GSize(1140,615));
	}

	int width = cls_gl->m_pSceneMgr->getFont()->getTextWidth(vd1.name);
	cls_gl->m_pSceneMgr->getFont()->drawText(vd1.name,GPoint(640-width/2.0f,vd1.y,vd1.z),NULL,GColor((vd1.alpha<<24) | 0x76bcbe));
	width = cls_gl->m_pSceneMgr->getFont()->getTextWidth(vd2.name);
	cls_gl->m_pSceneMgr->getFont()->drawText(vd2.name,GPoint(640-width/2.0f,vd2.y,vd2.z),NULL,GColor((vd2.alpha<<24) | 0x76bcbe));
	width = cls_gl->m_pSceneMgr->getFont()->getTextWidth(vd3.name);
	cls_gl->m_pSceneMgr->getFont()->drawText(vd3.name,GPoint(640-width/2.0f,vd3.y,vd3.z),NULL,GColor((vd3.alpha<<24) | 0x76bcbe));
	width = cls_gl->m_pSceneMgr->getFont()->getTextWidth(vd4.name);
	cls_gl->m_pSceneMgr->getFont()->drawText(vd4.name,GPoint(640-width/2.0f,vd4.y,vd4.z),NULL,GColor((vd4.alpha<<24) | 0x76bcbe));
	width = cls_gl->m_pSceneMgr->getFont()->getTextWidth(vd5.name);
	cls_gl->m_pSceneMgr->getFont()->drawText(vd5.name,GPoint(640-width/2.0f,vd5.y,vd5.z),NULL,GColor((vd5.alpha<<24) | 0x76bcbe));
}
