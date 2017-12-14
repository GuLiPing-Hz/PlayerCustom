//
//  Image.h
//
//  Created by 天格 on 12-8-21.
//
//
#include <stdafx.h>

#include "RenderEImage.h"
#include "Imageset.h"

RenderEImage::RenderEImage()
:m_pImgset(NULL)
,m_nBlend(BLEND_DEFAULT)
,m_bXFlip(false)
,m_bYFlip(false)
,m_bHSFlip(false)
,m_nDelay(0)
{
}

RenderEImage::RenderEImage(RenderEImage& img)
{
	if (&img != this)
	{
		memcpy(this,&img,sizeof(img));
	}
}

RenderEImage::RenderEImage(const CGRect& rect,Imageset* imgset)
:m_nBlend(BLEND_DEFAULT)
,m_bXFlip(false)
,m_bYFlip(false)
,m_bHSFlip(false)
,m_fCenterX(rect.size.width/2.0f)
,m_fCenterY(rect.size.height/2.0f)
,m_nDelay(0)
{
	m_rect = rect;
	m_pImgset = imgset;
}

RenderEImage::~RenderEImage()
{
    //SAFE_DELETE(m_imgset);
	m_pImgset = NULL;
}

const int & RenderEImage::getWidth() const
{
	return m_rect.size.width;
}

const int & RenderEImage::getHeight() const
{
	return m_rect.size.height;
}

void RenderEImage::setFlip(bool bX, bool bY, bool bHotSpot)
{
	m_bHSFlip = bHotSpot;

	if(bX != m_bXFlip)//glp
	{
		if (m_bHSFlip)
		{
			m_fCenterX = m_rect.size.width - m_fCenterX;
		}
		m_bXFlip=!m_bXFlip;
	}

	if(bY != m_bYFlip)
	{
		if (m_bHSFlip)
		{
			m_fCenterY = m_rect.size.height - m_fCenterY;
		}
		m_bYFlip=!m_bYFlip;
	}
}


void RenderEImage::setPoint(GPoint point)
{
	m_offset = point;
}

void RenderEImage::setRect(CGRect rect)
{
	m_rect = rect;
}
void RenderEImage::setImageset(Imageset* imgset)
{
	m_pImgset = imgset;
}

void RenderEImage::draw(const CGRect& dest_rect,CGRect* pclip_rect,const GColorRect& color_rect,bool bdest_pclip)
{
	if (m_pImgset)
		m_pImgset->render(m_rect , dest_rect ,bdest_pclip, pclip_rect , color_rect ,m_bXFlip,m_bYFlip,m_nBlend);
}

void RenderEImage::draw(const GPoint& dest_point,const GSize& dest_size,CGRect* pclip_rect,bool bdest_pclip,const GColorRect& color_rect)
{
	CGRect rect(dest_point,dest_size);
	draw(rect,pclip_rect,color_rect,bdest_pclip);
}

void RenderEImage::draw(const GPoint& dest_point,CGRect* pclip_rect,bool bdest_pclip,const GColorRect& color_rect)
{
	if (m_pImgset)
	{
		CGRect dest_rect = CGRectMake(dest_point,m_rect.size);
		m_pImgset->render(m_rect , dest_rect ,bdest_pclip, pclip_rect , color_rect ,m_bXFlip,m_bYFlip,m_nBlend);
	}   
}

void RenderEImage::draw(const GPoint& v1,const GPoint& v2,const GPoint& v3,const GPoint& v4,const GColorRect& color_rect)
{
	if (m_pImgset)
		m_pImgset->render(v1,v2,v3,v4,m_rect,color_rect,m_bXFlip,m_bYFlip,m_nBlend);
}

void RenderEImage::draw(const CUSTOMVERTEX& v1,const CUSTOMVERTEX& v2,const CUSTOMVERTEX& v3,const CUSTOMVERTEX& v4)
{
	if (m_pImgset)
		m_pImgset->render(v1,v2,v3,v4,m_nBlend);
}

void RenderEImage::drawPSEx(const GPoint& dest_point,float rot,const GColorRect& colour_rect,float hscale,float vscale)
{
	drawPSEx(GPoint(m_fCenterX,m_fCenterY),dest_point,rot,hscale,vscale,colour_rect);
}


void RenderEImage::drawPSEx(const GPoint& hot_point,const GPoint& dest_point,float rot,float hscale, float vscale,const GColorRect& colour_rect)
{
	if (m_pImgset)
		m_pImgset->render(hot_point,dest_point,m_rect,rot,hscale,vscale,colour_rect,m_nBlend);
}



