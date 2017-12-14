#include <stdafx.h>
#include "GifSprite.h"
#include "ImgsetMgr.h"
#include "../OpenGLWindow.h"

CGifSprite::CGifSprite(std::wstring name)
:m_pImgset(NULL)
,m_pImage(NULL)
,m_nCur(0)
,m_nDelay(0)
,m_nMs(0)
{
	
	assert(cls_gl != NULL);
	ImgsetMgr* pMgr = cls_gl->m_pImgsetMgr;
	m_pImgset = pMgr->getImageset(name);

	if (m_pImgset)
		assert(m_pImgset->m_type == Set_Gif);
}

CGifSprite::~CGifSprite(void)
{
	m_pImage = NULL;
	m_pImgset = NULL;
}

void CGifSprite::setGifName(std::wstring name)
{
	ImgsetMgr* pMgr = cls_gl->m_pImgsetMgr;
	m_pImgset = pMgr->getImageset(name);

	if (m_pImgset)
		assert(m_pImgset->m_type == Set_Gif);
}

void CGifSprite::begin()
{
	if(!m_pImgset)
		return ;
	m_nMs = 0;
	m_nCur = 0;
	m_pImage = m_pImgset->getImage(m_nCur);
	if(m_pImage)
		m_nDelay = m_pImage->getDelay();
}

void CGifSprite::update(unsigned long delta_ms)
{
	m_nMs += delta_ms;

	if(m_nMs >= m_nDelay)
	{
		m_pImage = m_pImgset->getImage(m_nCur);
		m_nMs -= m_nDelay;
		if(m_pImage)
			m_nDelay = m_pImage->getDelay();
	}
}

void CGifSprite::draw(const CGRect& rect)
{
	if(m_pImage)
		m_pImage->draw(rect);
}

void CGifSprite::draw(const GPoint& point)
{
	if(m_pImage)
	{
		m_pImage->setFlip(false,true);
		m_pImage->draw(point);
	}
}

void CGifSprite::end()
{
	m_pImage = NULL;
	m_nCur = 0;
	m_nMs = 0;
	m_nDelay = 0;
}

