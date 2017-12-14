#include "stdafx.h"
#include "ObjectNotify.h"
#include "../render/OS_HeadMessage.h"
#include "../render/OS_NotifyMessage.h"

#ifndef SPECIAL_VERSION
#include "../swf/SwfMgr.h"
#endif


//歌手信息X坐标
#define  CONFIG_POSX_SINGERTEXT 1050
//歌曲信息
#define CONFIG_SONGINFO_X 100.0f
#define CONFIG_SONGINFO_Y 400.0f
//唱片背景宽度
#define CONFIG_SONGINFO_WIDTH  228
//歌曲信息的背景宽度,高度
#define CONFIG_SONGINFO2_WIDTH 200
#define CONFIG_SONGINFO2_HEIGHT 300

#define SONGINFO_START (0)
#define SONGINFO_TIME_MOVEUP (150)
#define SONGINFO_TIME_MOVERIGHT (500)
#define SONGINFO_TIME_KEEP (2000)
#define SONGINFO_TIME_DISAPPEAR (1000)
#define SONGINFO_TIME (SONGINFO_START+3650)

static unsigned int SPIC_MOVERIGHT = SONGINFO_START+SONGINFO_TIME_MOVEUP;
static unsigned int SINFO_MOVERIGHT = SONGINFO_START+SONGINFO_TIME_MOVEUP+SONGINFO_TIME_MOVERIGHT;
static unsigned int SINFO_DISAPPEAR = SONGINFO_START + SONGINFO_TIME_MOVEUP + SONGINFO_TIME_MOVERIGHT+SONGINFO_TIME_KEEP;

CNotifyObject::CNotifyObject(bool& bInit,std::wstring songName,std::wstring singName,std::wstring nextSongName
							 ,const StructSound& ss,bool bAccompany,bool bMute)
:ISceneObject(NOTIFYOBJ)
,headmessage_render(NULL)
,notifymessage_render(NULL)
,m_sCurSongName(songName)
,m_sCurSinger(singName)
,m_sNextSongName(nextSongName)
,m_nlast_songinfo_ms(0)
,m_ImgSongInfoBkg(NULL)
,m_ImgSongInfoBkg2(NULL)
,m_bSIMoveUp(true)
,m_bSIMoveRight(true)
,m_bSIKeep(true)
,m_bSIDisappear(true)
,m_ImgSingerPic(NULL)
,m_imgsetEx(NULL)
,m_bShowSongInfo(true)
,m_pFont(NULL)
,m_pSwf(NULL)
{
	assert(cls_gl != NULL);

	//是MV的时候才需要显示歌曲信息
	m_bShowSongInfo = cls_gl->m_gCurPlayInfo.m_isMV;

	bInit = true;
	bool tmp_bInit;
	headmessage_render = new COS_HeadMessage(tmp_bInit,this,songName,nextSongName);
	if (!headmessage_render || !tmp_bInit)
	{
		SceneMgr::s_error_msg = L"headmessage_render new failed";
		bInit = false ;
	}
	notifymessage_render = new COS_NotifyMessage(tmp_bInit,this,ss,bAccompany,bMute);
	if (!notifymessage_render || !tmp_bInit)
	{
		SceneMgr::s_error_msg = L"notifymessage_render new failed";
		bInit = false ;
	}

	Imageset* imgset = cls_gl->m_pImgsetMgr-> getImageset(L"renderer");
	assert(imgset != NULL);
	m_ImgSongInfoBkg = imgset->getImage(L"SongInfoBkg");
	m_ImgSongInfoBkg2 = imgset->getImage(L"SongInfoBkg2");
	if (!m_ImgSongInfoBkg || !m_ImgSongInfoBkg2)
	{
		SceneMgr::s_error_msg = L"songinfo background 1 or 2 image miss";
		bInit = false;
	}

	m_imgsetEx = (CImgsetEx*)(cls_gl->m_pImgsetMgr->getImageset(L"singerpic2"));
	if (m_imgsetEx)
	{
		m_ImgSingerPic = m_imgsetEx->getImage(L"singerpic2");
		if (!m_ImgSingerPic)
		{
			SceneMgr::s_error_msg = L"Singer image miss";
			bInit = false;
		}
	}

	setFont(cls_gl->m_pSceneMgr->m_pGLFont);
}

CNotifyObject::~CNotifyObject()
{
	SAFE_DELETE(headmessage_render);

	SAFE_DELETE(notifymessage_render);
}

void CNotifyObject::onSwfEnd()
{
	int nSwfId = 0;
	CAutoLock lock(&cls_gl->m_csSwfEffect);
	if(!cls_gl->m_listSwfEffect.empty())
	{
		nSwfId = cls_gl->m_listSwfEffect.front();
		cls_gl->m_listSwfEffect.pop_front();
	}

#ifndef SPECIAL_VERSION
	m_pSwf = cls_gl->m_pSwfMgr->getSwf(nSwfId);
	if(m_pSwf)
	{
		m_pSwf->setSwfListener(this);
		//重置开始播放
		m_pSwf->resumePlay();
	}
#endif
}

void CNotifyObject::drawSongInfo()
{
	if (m_nlast_songinfo_ms<SONGINFO_START)
		return;
	if (!m_pFont)
		return ;

	static float s_pos_x = CONFIG_SONGINFO_X;
	static float s_pos_y = cls_gl->m_winSize.height - CONFIG_SONGINFO_Y;
	static float s_pic_pos_x = s_pos_x +40;
	static float s_pic_pos_y = s_pos_y +5;
	static float right_pos_x = s_pos_x+m_ImgSongInfoBkg->getWidth()-35;
	static float right_pos_y = s_pos_y+m_ImgSongInfoBkg->getHeight()/2.0f-m_ImgSongInfoBkg2->getHeight()/2.0f-15;
	static int right_width = m_ImgSongInfoBkg2->getWidth();
	static int right_height = m_ImgSongInfoBkg2->getHeight();
	static float s_font1_y = right_pos_y + 12;
	static float s_font2_y = s_font1_y+54;

// #ifdef SPECIAL_VERSION
// 	if(m_imgsetEx)
// 		m_imgsetEx->lockBatchBuffer();
// #endif

	//信息向右移动
	if (m_bSIMoveUp)
	{
		float cur_shedule = (m_nlast_songinfo_ms-SONGINFO_START)*1.0f/SONGINFO_TIME_MOVEUP;
		//float move__pos_y = m_WindowSize.height-cur_shedule*CONFIG_SONGINFO_Y;
		float move_pos_x = cur_shedule*CONFIG_SONGINFO_X;

		m_ImgSongInfoBkg->draw(GPoint(move_pos_x,s_pos_y,GL3D_Z_NOTIFY));
		if (m_ImgSingerPic)
			m_ImgSingerPic->draw(GPoint(move_pos_x+40,s_pic_pos_y,GL3D_Z_NOTIFY));
	}
	else
	{
		//信息向右移动
		if (m_bSIMoveRight)
		{
			static CGRect s_clip_rect = CGRectMake(right_pos_x,right_pos_y,GL3D_Z_NOTIFY,right_width,right_height);
			static float s_pos_start_x = right_pos_x - right_width;
			float f_shedule = (m_nlast_songinfo_ms-SPIC_MOVERIGHT)*1.0f/SONGINFO_TIME_MOVERIGHT;
			float bkg_pos_x = s_pos_start_x + f_shedule*right_width;
			m_ImgSongInfoBkg2->draw(GPoint(bkg_pos_x,right_pos_y,GL3D_Z_NOTIFY),&s_clip_rect);

			m_ImgSongInfoBkg->draw(GPoint(s_pos_x,s_pos_y,GL3D_Z_NOTIFY));
			if (m_ImgSingerPic)
				m_ImgSingerPic->draw(GPoint(s_pic_pos_x,s_pic_pos_y,GL3D_Z_NOTIFY));
		}
		else
		{
			static GPoint dest_point1 = CGPointMake(s_pos_x,s_pos_y,GL3D_Z_NOTIFY);
			static GPoint dest_point2 = CGPointMake(right_pos_x,right_pos_y,GL3D_Z_NOTIFY);
			if (m_bSIKeep)
			{
				m_ImgSongInfoBkg->draw(dest_point1);
				if (m_ImgSingerPic)
					m_ImgSingerPic->draw(GPoint(s_pic_pos_x,s_pic_pos_y,GL3D_Z_NOTIFY));
				m_ImgSongInfoBkg2->draw(dest_point2);
				static float final_x = right_pos_x+30;
				static GPoint dest_point3 = CGPointMake(final_x,s_font1_y,GL3D_Z_SONGINFO);
				m_pFont->drawText(m_sCurSongName,dest_point3,NULL,WHITE_RECT,0.9f);
				static GPoint des_point4 = CGPointMake(final_x,s_font2_y,GL3D_Z_SONGINFO);
				m_pFont->drawText(m_sCurSinger,des_point4,NULL,WHITE_RECT,0.8f,0.7f);
			}
			else
			{
				//信息渐隐
				float f_shedule = (m_nlast_songinfo_ms-SINFO_DISAPPEAR)*1.0f/SONGINFO_TIME_DISAPPEAR;
				float cur_alpah = 1-f_shedule;
				if (cur_alpah<=0)
					cur_alpah = 0;
				GColor cur_color (1.0f,1.0f,1.0f,cur_alpah);
				m_ImgSongInfoBkg->draw(dest_point1,NULL,true,cur_color);
				if (m_ImgSingerPic)
				{
					m_ImgSingerPic->draw(GPoint(s_pic_pos_x,s_pic_pos_y,GL3D_Z_NOTIFY),NULL,true,cur_color);
				}
				m_ImgSongInfoBkg2->draw(dest_point2,NULL,true,cur_color);
				static float final_x = right_pos_x+30;
				static GPoint dest_point3 = CGPointMake(final_x,s_font1_y,GL3D_Z_SONGINFO);
				m_pFont->drawText(m_sCurSongName,dest_point3,NULL,cur_color,0.9f);
				static GPoint des_point4 = CGPointMake(final_x,s_font2_y,GL3D_Z_SONGINFO);
				m_pFont->drawText(m_sCurSinger,des_point4,NULL,cur_color,0.8f,0.7f);
			}
		}
	}

// #ifdef SPECIAL_VERSION
// 	if(m_imgsetEx)
// 		m_imgsetEx->unlockBatchBuffer();
// #endif
}
//唤起，初始化操作
bool CNotifyObject::awake(void* info)
{
	if (headmessage_render)
	{
		headmessage_render->updateRightStr(m_sNextSongName);
	}

	return ISceneObject::awake(info);
}
//设置当前时间值，
void CNotifyObject::setCurMs(unsigned int ms)
{

}
//更新操作场景对象
//@param ms:逝去的时间，单位（毫秒）
bool CNotifyObject::update(unsigned int ms)
{
	if(!ISceneObject::update(ms))
		return false;

	if(!cls_gl->m_pDrawListener)
		return false;
	if (cls_gl->m_bShowEndScore)
		return false;

#ifndef SPECIAL_VERSION	
// 	if(!m_pSwf)//如果没有正在展示的swf，则去instance那边获取看看
// 		onSwfEnd();
#endif//SPECIAL_VERSION

	if (notifymessage_render && cls_gl->m_gCurPlayInfo.m_isMV)
		notifymessage_render->update(ms);
	if (headmessage_render)
		headmessage_render->update(ms);

#ifndef SPECIAL_VERSION
// 	if(m_pSwf)
// 		m_pSwf->update(ms);
#endif//SPECIAL_VERSION

	if(m_bShowSongInfo)
	{
		if (m_nlast_songinfo_ms >= SONGINFO_TIME)
		{
			m_bShowSongInfo = false;
			return true;
		}
		m_nlast_songinfo_ms += ms;	

		//信息向右移动
		if (m_bSIMoveUp)
		{
			if (m_nlast_songinfo_ms >= SPIC_MOVERIGHT)
				m_bSIMoveUp = false;
		}
		else
		{
			//信息向右移动
			if (m_bSIMoveRight)
			{
				if (m_nlast_songinfo_ms >= SINFO_MOVERIGHT)
					m_bSIMoveRight = false;
			}
			else
			{
				if (m_bSIKeep)
				{
					if (m_nlast_songinfo_ms >= SINFO_DISAPPEAR)
						m_bSIKeep = false;
				}
			}
		}
	}

	return true;
}
//渲染对象
bool CNotifyObject::render()
{
	if(!ISceneObject::render())
		return false;

	if(!cls_gl->m_pDrawListener)
		return false;
	if (cls_gl->m_bShowEndScore)
		return false;

	//如果是MV播放，则需要给出MV相关提示，否则直接略过
	if (notifymessage_render && cls_gl->m_gCurPlayInfo.m_isMV)
		notifymessage_render->drawGraphBuffer();
	if (headmessage_render)
		headmessage_render->drawGraphBuffer();

#ifndef SPECIAL_VERSION
// 	if(m_pSwf)
// 		m_pSwf->render();
#endif//SPECIAL_VERSION
	
	if(m_bShowSongInfo && cls_gl->m_gCurPlayInfo.m_isMV)
		drawSongInfo();//描绘歌曲信息

	return true;
}
