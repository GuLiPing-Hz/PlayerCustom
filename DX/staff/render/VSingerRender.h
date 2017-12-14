/*
	注释时间:2014-4-25
	author: glp
	歌手信息渲染，主播信息渲染
*/
#pragma  once

#ifndef VSINGERRENDER__H__
#define VSINGERRENDER__H__
#include "IComRender.h"

class COpenGLWindow;
class CGLFont;
class ISceneObject;
class CVSingerRender: IComRender
{
public:
	CVSingerRender(bool& bInit,ISceneObject* pObj);
	virtual ~CVSingerRender();

	virtual void update(const ulong delta_ms);
	virtual void drawGraphBuffer();
	
	void				setFont(CGLFont* pFont);
private:
	int							m_nWidthFont;
	GPoint						m_gPointFont;
	CGLFont*				m_pFont;//main font
	GSize						m_videosize;

	bool							m_bHost;
	bool							m_bSing;
	RenderEImage*					m_ImgFontBg;
	RenderEImage*					m_ImgBorder;
	RenderEImage*					m_ImgVideoHost;
	RenderEImage*					m_ImgVideoSing;
	Imageset*				m_ImgsetFontbg;
public:
	CImgsetEx*				m_ImgsetHost;
	CImgsetEx*				m_ImgsetSing;
};

#endif//VSINGERRENDER__H__
