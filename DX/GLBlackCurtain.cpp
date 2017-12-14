#include "stdafx.h"
#include "DX/util.h"
#include "GLBlackCurtain.h"
#include "OpenGLWindow.h"

#define TIME_TO_END_BLACK 2000
CGLBlackCurtain::CGLBlackCurtain()
:CGLVideo()
,m_nlast_ms(0)
,m_ImgBlackCurtain(NULL)
,m_imgset1(NULL)
,m_imgset2(NULL)
,m_imgset3(NULL)
,m_ImgFontBkg(NULL)
,CONFIG_FONT_SIZE(30)
,m_pFont(NULL)
{

}

CGLBlackCurtain::~CGLBlackCurtain()
{
	//SAFE_DELETE(m_pFont);
}

void CGLBlackCurtain::setFont(CGLFont* pFont,const int font_size)
{
	m_pFont = pFont;
	CONFIG_FONT_SIZE = font_size;
}
void CGLBlackCurtain::update(ulong delta_ms)
{
	m_nlast_ms += delta_ms;
}
void CGLBlackCurtain::updateSongName(const std::wstring& next_song)
{
	if (m_pFont)
	{
		m_cs.Lock();
		m_next_song = L"切换到下一首："+next_song;
		if (m_pFont->addFont(m_next_song))
			m_pFont->loadFont(cls_gl->m_winSize,true,FONT_BANDING);
		m_cs.Unlock();
	}
}

bool CGLBlackCurtain::init()
{
	ImgsetMgr* pImgsetMgr = cls_gl->m_pImgsetMgr;
	m_imgset2 = pImgsetMgr->getImageset(L"zzmask");
	if (!m_imgset2)
	{
		SceneMgr::s_error_msg = L"blackcustain texture miss";
		return false;
	}
	m_ImgBlackCurtain = m_imgset2->getImage(L"BlackCurtain");
	m_imgset3 = pImgsetMgr->getImageset(L"zzzhuojing");
	if (!m_imgset3)
	{
		SceneMgr::s_error_msg = L"fire warning picture miss";
		return false;
	}
	m_ImgFireWarning = m_imgset3->getImage(L"zzzhuojing");

	return true;
}

void CGLBlackCurtain::drawFireWarning()
{
	static CGRect dest_rect = CGRectMake(0.0f,0.0f,GL3D_Z_FIRALARM
		,cls_gl->m_winSize.width,cls_gl->m_winSize.height);
	if(m_ImgFireWarning)
	{
// #ifdef SPECIAL_VERSION
// 		m_imgset3->lockBatchBuffer();
// #endif
		m_ImgFireWarning->draw(dest_rect,NULL,WHITE_RECT);
// #ifdef SPECIAL_VERSION
// 		m_imgset3->unlockBatchBuffer();
// #endif
	}
}

void CGLBlackCurtain::drawBlackCurtain()
{
	if (m_ImgBlackCurtain)
	{
		float f_color_schedule = m_nlast_ms*1.0f/TIME_TO_END_BLACK;
		if (f_color_schedule >= 1)
			f_color_schedule = 1.0f;

		GColor tmp_color(1.0f,1.0f,1.0f,f_color_schedule);
		static CGRect dest_rect = CGRectMake(0.0f,0.0f,GL3D_Z_FIRALARM
			,cls_gl->m_winSize.width,cls_gl->m_winSize.height);
		m_ImgBlackCurtain->draw(dest_rect,NULL,tmp_color);
	}
}
