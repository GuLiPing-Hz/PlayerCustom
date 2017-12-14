/*
	注释时间:2014-4-25
	author: glp
	CGifSprite 处理gif 精灵
*/
#pragma once

#include <string>
#include "../Opengl.h"

class Imageset;
class RenderEImage;
class COpenGLWindow;

#ifndef SPECIAL_VERSION

class CGifSprite
{
public:
	CGifSprite(std::wstring name);
	~CGifSprite(void);

	void setGifName(std::wstring name);

	void begin();
	void update(unsigned long delta_ms);
	void draw(const CGRect& rect);
	void draw(const GPoint& point);
	void end();
private:
	COpenGLWindow*	  cls_gl;
	Imageset*				  m_pImgset;
	RenderEImage*					  m_pImage;
	unsigned int 			  m_nCur;
	unsigned int			  m_nDelay;
	unsigned int			  m_nMs;
};
#else
#include <../Player/DX/staff/GifSprite.h>
#endif
