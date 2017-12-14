#include "stdafx.h"
#include "../OpenGLWindow.h"
#include "hgesprite.h"
#include <math.h>

float Texture_GetWidth(std::string imgsetName)
{
	if (imgsetName.size())
	{
		Imageset *imgset = cls_gl->m_pImgsetMgr->getImageset(imgsetName);
		if (imgset)
		{
			return (float)imgset->getTextureSize().width;
		}
		WLOGH_ERRO << L"Texture_GetWidth getImgset failed";
	}
	WLOGH_ERRO << L"Texture_GetWidth imgsetName failed";
	return 0.0f;
}
float Texture_GetHeight(std::string imgsetName)
{
	if (imgsetName.size())
	{
		Imageset *imgset = cls_gl->m_pImgsetMgr->getImageset(imgsetName);
		if (imgset)
		{
			return (float)imgset->getTextureSize().height;
		}
		WLOGH_ERRO << L"Texture_GetHeight getImgset failed";
	}
	WLOGH_ERRO << L"Texture_GetHeight imgsetName failed";
	return 0.0f;
}


CGLSprite::CGLSprite(float texx, float texy, int w, int h,Imageset* imgset)
:RenderEImage(CGRect(GPoint(texx,texy),GSize(w,h)),imgset)
,m_color_rect(WHITE_RECT)
{
	m_fCenterX=0;
	m_fCenterY=0;
	m_z[0] = m_z[1] = m_z[2] = m_z[3] = 0.0f;
}

CGLSprite::CGLSprite(const CGRect& rect,Imageset* imgset)
:RenderEImage(rect,imgset)
,m_color_rect(WHITE_RECT)
{
	m_fCenterX=0;
	m_fCenterY=0;
	m_z[0] = m_z[1] = m_z[2] = m_z[3] = 0.0f;
}

CGLSprite::CGLSprite(const CGLSprite &spr)
:m_color_rect(WHITE_RECT)
{
	memcpy(this, &spr, sizeof(CGLSprite));
}

void CGLSprite::drawRender(float x, float y)
{
	float tempx1, tempy1;

	tempx1 = x-m_fCenterX;
	tempy1 = y-m_fCenterY;
	draw(GPoint(tempx1,tempy1),NULL,m_color_rect);
}


#ifdef SPECIAL_VERSION
void	CGLSprite::drawRenderEx(float x, float y, float rot,bool bFirstVB, float hscale, float vscale)
{
	drawPSEx(GPoint(m_fCenterX,m_fCenterY),GPoint(x,y),rot,hscale,vscale,m_color_rect,bFirstVB);
}
#else
void CGLSprite::drawRenderEx(float x, float y,float z, float rot, float hscale, float vscale)
{//////////////////////////////////////////////////////////////////////////
	drawPSEx(GPoint(m_fCenterX,m_fCenterY,z),GPoint(x,y,z),rot,hscale,vscale,m_color_rect);
}
#endif


void CGLSprite::drawRenderStretch(float x1, float y1, float x2, float y2)
{
	int w = (int)(x2-x1);
	int h = (int)(y2-y1);
	draw(CGRect(x1,y1,w,h),NULL,m_color_rect);
}


void CGLSprite::drawRender4V(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
	draw(GPoint(x0,y0,m_z[0]),GPoint(x1,y1,m_z[1]),GPoint(x2,y2,m_z[2]),GPoint(x3,y3,m_z[3]),m_color_rect);
}


hgeRect* CGLSprite::GetBoundingBoxEx(float x, float y, float rot, float hscale, float vscale, hgeRect *rect) const
{
	float tx1, ty1, tx2, ty2;
	float sint, cost;

	rect->Clear();
	
	tx1 = -m_fCenterX*hscale;
	ty1 = -m_fCenterY*vscale;
	tx2 = (m_rect.size.width-m_fCenterX)*hscale;
	ty2 = (m_rect.size.height-m_fCenterY)*vscale;

	if (rot != 0.0f)
	{
		cost = cosf(rot);
		sint = sinf(rot);
			
		rect->Encapsulate(tx1*cost - ty1*sint + x, tx1*sint + ty1*cost + y);	
		rect->Encapsulate(tx2*cost - ty1*sint + x, tx2*sint + ty1*cost + y);	
		rect->Encapsulate(tx2*cost - ty2*sint + x, tx2*sint + ty2*cost + y);	
		rect->Encapsulate(tx1*cost - ty2*sint + x, tx1*sint + ty2*cost + y);	
	}
	else
	{
		rect->Encapsulate(tx1 + x, ty1 + y);
		rect->Encapsulate(tx2 + x, ty1 + y);
		rect->Encapsulate(tx2 + x, ty2 + y);
		rect->Encapsulate(tx1 + x, ty2 + y);
	}

	return rect;
}

void CGLSprite::SetTextureRect(float x, float y,float z, int w, int h, bool adjSize)
{
	bool bX,bY,bHS;

	if (adjSize)
	{
		m_rect = CGRectMake(x,y,z,w,h);
	}

	bX=m_bXFlip; bY=m_bYFlip; bHS=m_bHSFlip;
	m_bXFlip=false; m_bYFlip=false;
	setFlip(bX,bY,bHS);
}

unsigned long CGLSprite::GetColor(int i) const
{
	if (i == 0)
	{
		return m_color_rect.left_top_color.c;
	}
	else if(i == 1)
	{
		return m_color_rect.right_top_color.c;
	}
	else if (i == 2)
	{
		return m_color_rect.right_bottom_color.c;
	}
	else if (i == 3)
	{
		return m_color_rect.left_bottom_color.c;
	}
	return 0xffffffff;
}

void CGLSprite::SetColor(unsigned long col, int i)
{
	if (i == 0)
	{
		m_color_rect.left_top_color.c = col;
	}
	else if(i == 1)
	{
		m_color_rect.right_top_color.c = col;
	}
	else if (i == 2)
	{
		m_color_rect.right_bottom_color.c = col;
	}
	else if (i == 3)
	{
		m_color_rect.left_bottom_color.c = col;
	}
	else
	{
		m_color_rect.left_top_color.c = col;
		m_color_rect.right_top_color.c = col;
		m_color_rect.right_bottom_color.c = col;
		m_color_rect.left_bottom_color.c = col;
	}
}

void CGLSprite::SetZ(float z, int i)
{
	if (i == -1)
	{
		m_z[0] = z;
		m_z[1] = z;
		m_z[2] = z;
		m_z[3] = z;
	}
	else
	{
		m_z[i] = z;
	}
}
