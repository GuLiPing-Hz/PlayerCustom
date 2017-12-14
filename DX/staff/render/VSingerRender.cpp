#include "stdafx.h"
#include "VSingerRender.h"

#define VIDEOBORDERY 50.0f
#define VIDEOXOFFSET 8.0f
#define VIDEOYOFFSET 7.0f

#define PERI_EMPTY_TIP "美女正在休息哦~"

CVSingerRender::CVSingerRender(bool& bInit,ISceneObject* pObj)
:IComRender(pObj)
,m_ImgBorder(NULL)
,m_ImgsetHost(NULL)
,m_ImgsetSing(NULL)
,m_bHost(false)
,m_bSing(false)
,m_pFont(NULL)
,m_ImgsetFontbg(NULL)
,m_nWidthFont(0)
{
	bInit = false;
	
	ASSERT(cls_gl != NULL);
	Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
	ASSERT(pImgset != NULL);
	m_ImgBorder = pImgset->getImage(L"VideoBorder");

	m_ImgsetFontbg = cls_gl->m_pImgsetMgr->getImageset(L"zzanima");
	ASSERT(m_ImgsetFontbg!=NULL);
	m_ImgFontBg = m_ImgsetFontbg->getImage(L"FontBg");

	m_ImgsetHost = (CImgsetEx*)cls_gl->m_pImgsetMgr->getImageset(L"vHost");
	m_ImgsetSing = (CImgsetEx*)cls_gl->m_pImgsetMgr->getImageset(L"vSing");

	m_ImgVideoHost = m_ImgsetHost->getImage(L"host");
	m_ImgVideoSing = m_ImgsetSing->getImage(L"sing");

	if (m_ImgsetSing && m_ImgsetHost && m_ImgFontBg && m_ImgBorder && m_ImgVideoHost && m_ImgVideoSing)
		bInit = true;
	else
		SceneMgr::s_error_msg = L"miss singer render image";
}

CVSingerRender::~CVSingerRender()
{
	
}

void	CVSingerRender::setFont(CGLFont* pFont)
{
	m_pFont = pFont;
}

void CVSingerRender::update(const ulong delta_ms)
{
	if(cls_gl->m_bShowEndScore)
		m_pOwner->end();
	if(cls_gl->m_gCurPlayInfo.m_isMV || cls_gl->m_gCurPlayInfo.m_bAdvertisement)//是MV或者是广告的时候，直接返回
		return ;
	cls_gl->updateVideo2Texture();//处理更新纹理，再描绘
}

void CVSingerRender::drawGraphBuffer()
{
	if(cls_gl->m_gCurPlayInfo.m_isMV || cls_gl->m_gCurPlayInfo.m_bAdvertisement)
		return ;

	//一般不会这种情况，也有可能主播脑残
	if (cls_gl->m_mapVideoControl[vtHost] == 2)//暂停排麦
	{
		static CGRect s_r_b = CGRectMake(0,0,GL3D_Z_XGVIDEO,960,720);
		m_ImgBorder->draw(s_r_b);

		//主持
		m_ImgVideoHost->setFlip(false,true);
		m_ImgVideoHost->draw(s_r_b);

//  		if(m_pFont)
// 		{
// 			static GSize fontbgsize(480,320);
// 			static GPoint point2(s_r_b.origin.x,s_r_b.origin.y+320-m_ImgFontBg->getHeight(),GL3D_Z_XGVIDEO);
// 			m_ImgFontBg->draw(point2,fontbgsize);
// 			int wfont = m_pFont->getTextWidth(cls_gl->m_mapVideoName[vtHost]);
// 			int fontx = m_ImgVideoHost->getWidth()-wfont;
// 			if(fontx < 0)
// 				fontx = 0;
// 			GPoint point3(point2.x+fontx/2,point2.y+6.0f,GL3D_Z_XGVIDEO);
// 			m_pFont->drawText(cls_gl->m_mapVideoName[vtHost],point3);
// 		}
	}
	else
	{
		if(cls_gl->m_mapVideoControl[vtHost])
		{
			static GSize s_size_peri(960,720);
			//主持
			static GPoint s_point_peri (0,0,GL3D_Z_XGVIDEO);
			m_ImgVideoHost->setFlip(false,true);
			m_ImgVideoHost->draw(s_point_peri,s_size_peri);
		}
		else
		{
			if(m_nWidthFont == 0)
			{
				m_nWidthFont = m_pFont->getTextWidth(PERI_EMPTY_TIP);
				m_gPointFont = GPoint((960-m_nWidthFont)/2.0f,360);
			}

			 
			if(m_pFont)
				m_pFont->drawText(PERI_EMPTY_TIP,m_gPointFont);
		}
		
		//演唱
		static CGRect s_rect_self= CGRectMake(960.0f,0.0f,GL3D_Z_XGVIDEO,320,240);
		
		if (cls_gl->m_mapVideoControl[vtSing])
		{
			//演唱
			m_ImgVideoSing->setFlip(true,true);
			m_ImgVideoSing->draw(s_rect_self);
		}
		else//如果没开启视频，则描绘默认图片
			m_ImgBorder->draw(s_rect_self);
	}
}
