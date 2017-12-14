/*
	注释时间:2014-4-25
	author: glp
	图片精灵
*/
#pragma once

#include <string>
#include "../particlesystem/hgedef.h"
#include "../RenderConfig.h"
#include "../KKType.h"

class Imageset;

#ifndef SPECIAL_VERSION

class RenderEImage 
{
public:
	RenderEImage();
	RenderEImage(RenderEImage& img);
	RenderEImage(const CGRect& rect,Imageset* imgset);
	virtual ~RenderEImage();

	const int & getWidth() const;
	const int & getHeight() const;

	void				getHotSpot(float *x, float *y) const { *x=m_fCenterX; *y=m_fCenterY; }
	void				getFlip(bool *bX, bool *bY) const { *bX=m_bXFlip; *bY=m_bYFlip; }
	void				setHotSpot(float x, float y) { m_fCenterX=x; m_fCenterY=y; }
	void				setHotSpot(){m_fCenterX=m_rect.size.width/2.0f;m_fCenterY = m_rect.size.height/2.0f;}
	void				setBlendMode(int blend) { m_nBlend=blend; }
	int				getBlendMode() const { return m_nBlend; }
	void				setFlip(bool bX, bool bY, bool bHotSpot = false);
	void				setPoint(GPoint point);
	void				setRect(CGRect rect);
	void				setImageset(Imageset* imgset);
	void				setDelay(uint delay){m_nDelay = delay;};
	uint				getDelay(){return m_nDelay;}
	Imageset*	getImageset() {return m_pImgset;}
	//画图函数
	void draw(const CGRect& dest_rect, CGRect* pclip_rect=NULL,const GColorRect& color_rect=WHITE_RECT,bool bdest_pclip=true);
	void draw(const GPoint& dest_point, CGRect* pclip_rect=NULL,bool bdest_pclip=true,const GColorRect& color_rect=WHITE_RECT);//默认白色
	void draw(const GPoint& dest_point,const GSize& dest_size, CGRect* pclip_rect=NULL,bool bdest_pclip=true,const GColorRect& color_rect=WHITE_RECT);//默认白色
	void draw(const GPoint& v1,const GPoint& v2,const GPoint& v3,const GPoint& v4,const GColorRect& color_rect=WHITE_RECT);
	void draw(const CUSTOMVERTEX& v1,const CUSTOMVERTEX& v2,const CUSTOMVERTEX& v3,const CUSTOMVERTEX& v4);
	//这里的dest_point 是指旋转中心的坐标
	void drawPSEx(const GPoint& dest_point,float rot,const GColorRect& colour_rect=WHITE_RECT,float hscale=1.0f,float vscale=0.0f);
	void drawPSEx(const GPoint& hot_point,const GPoint& dest_point,float rot,float hscale, float vscale=0.0f,const GColorRect& colour_rect=WHITE_RECT);//默认白色
	//void draw(CGPoint& dest_point,CGRect* pclip_rect);
public:
	float				m_fCenterX, m_fCenterY;//旋转中心
	CGRect		m_rect;//纹理中的位置
	GPoint			m_offset;
	bool				m_bXFlip;//横向翻转
	bool				m_bYFlip;//纵向翻转
	bool				m_bHSFlip;//中心翻转
	int				m_nBlend;
	Imageset *  m_pImgset;
	uint				m_nDelay;//used for gif;
};
#else
#include <../Player/DX/staff/image/RenderEImage.h>
#endif//SPECIAL_VERSION

