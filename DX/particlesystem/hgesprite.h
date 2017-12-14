/*
	注释时间:2014-4-25
	author: glp
	精灵类
*/
#ifndef HGESPRITE_H
#define HGESPRITE_H

#include "hgerect.h"
#include "../staff/image/RenderEImage.h"
#include <string>

float Texture_GetWidth(std::string imgsetName);
float Texture_GetHeight(std::string imgsetName);
/*
** HGE Sprite class
*/
class CGLSprite : public RenderEImage
{
public:
	CGLSprite(/*HTEXTURE tex*/float x, float y, int w, int h,Imageset* imgset);
	CGLSprite(const CGRect& rect,Imageset* imgset);
	CGLSprite(const CGLSprite &spr);
	virtual ~CGLSprite() { /*hge->Release();*/ }
	
	void		drawRender(float x, float y);
#ifdef SPECIAL_VERSION
	void		drawRenderEx(float x, float y, float rot,bool bFirstVB=true, float hscale=1.0f, float vscale=0.0f);
#else
	void		drawRenderEx(float x, float y,float z, float rot,float hscale=1.0f, float vscale=0.0f);
#endif
	void		drawRenderStretch(float x1, float y1, float x2, float y2);
	void		drawRender4V(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);

	void		SetTextureRect(float x, float y,float z, int w, int h, bool adjSize = true);
	void		SetColor(unsigned long col, int i=-1);//设置4个顶点的颜色，从0代表左上角，1代表右上角，2代表右下角，3代表左下角
	void		SetZ(float z, int i=-1);

	void							GetTextureRect(float *x, float *y, float *w, float *h) const { *x=m_rect.origin.x; *y=m_rect.origin.y; *w=(float)m_rect.size.width; *h=(float)m_rect.size.height; }
	unsigned long		GetColor(int i=0) const;
	float							GetZ(int i=0) const { return m_z[i]; }

	hgeRect*	GetBoundingBox(float x, float y, hgeRect *rect) const { rect->Set(x-m_fCenterX, y-m_fCenterY, x-m_fCenterX+m_rect.size.width, y-m_fCenterY+m_rect.size.height); return rect; }
	hgeRect*	GetBoundingBoxEx(float x, float y, float rot, float hscale, float vscale,  hgeRect *rect) const;

protected:
	CGLSprite();
	GColorRect	m_color_rect;
	float		m_z[4];
};


#endif
