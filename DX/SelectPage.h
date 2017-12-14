/*
	注释时间:2014-4-25
	author: glp
	随机选歌
*/
#pragma once

#include "staff/render/IComRender.h"

typedef struct _StructVertexDraw{
	float y;
	float z;
	SongListInfo sli;
	std::wstring name;
	uchar alpha;
}StructVertexDraw;

class RenderEImage;
class Imageset;
class CGLFont;
class COpenGLWindow;
class CSelectPage : public IComRender
{
public:
	CSelectPage(const GSize& winsize);
	~CSelectPage(void);

	void setStop(bool bstop);
	virtual void	init();
	void restartAnima(){m_fRate = 1.0f;}

	virtual void update(const ulong delta_ms);
	//描绘多边形
	virtual void drawGraphBuffer();
private:
	inline void algorithm(StructVertexDraw& svd,float change);
	bool nextSong(SongListInfo& sli);
private:
	Imageset*						m_pImgset;
	RenderEImage*							m_pImgBg;
	RenderEImage*							m_pImgBox;
	RenderEImage*							m_pImgSelect;
	MAPSONGLIST				m_mapCurList;
	MAPSONGLIST::iterator m_itCur;

	float									m_fRate;
	StructVertexDraw			m_structVD[5];
	GSize								m_winSize;

	bool									m_bStop;
};
