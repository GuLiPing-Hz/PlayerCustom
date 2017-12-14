/*
	注释时间:2014-4-25
	author: glp
	黑幕
*/
#pragma once

#include "GLVideo.h"
#include "staff/image/ImgsetMgr.h"
#include "font/font.h"

class CGLBlackCurtain : public CGLVideo
{
	typedef struct _BLACKCURTAIN
	{
		GPoint point;
		D3DCOLOR colour;
	}BLACKCURTAIN;
public:
	CGLBlackCurtain();
	~CGLBlackCurtain();

	void update(ulong delta_ms);

	void updateSongName(const std::wstring& next_song);
	void drawBlackCurtain();
	void drawFireWarning();
	bool init();
	void setFont(CGLFont* pFont,const int font_size);
protected:

public:
	Imageset*		m_imgset1;
	Imageset*		m_imgset2;
	Imageset*		m_imgset3;
protected:
	CCritSec			m_cs;
	//KKColor	m_color;
	CGLFont*		m_pFont;
	ulong				m_nlast_ms;
	std::wstring		m_next_song;

	RenderEImage*			m_ImgFontBkg;
	RenderEImage*			m_ImgBlackCurtain;
	RenderEImage*			m_ImgFireWarning;

	int					CONFIG_FONT_SIZE;
};