#include "stdafx.h"
#include "LyricRender.h"

// 相对于16号字体的缩放比例 20 * 1.5f = 30
#define  FONT_WIDTH_SCALE  1.0f		
// 字体高度（大概的）
#define  FONT_HEIGHT  460
// 第一句歌词左边缩进
#define LEFT_INDENT  130
// 第二句歌词右边缩进
#define RIGHT_INDENT  100

//-----------------------------------------------------------------------------
LyricRender::~LyricRender()
{
// 	if (m_pGLFont)
// 	{
// 		delete m_pGLFont;
// 		m_pGLFont = NULL;
// 	}
}

// 初始化
LyricRender::LyricRender(ISceneObject* pObj,eShowWave eType,bool bAS,bool bFirst,const float staff_pos_height,const float sing_start_b,const int max_lyric_width,bool bKtvMode)
:IComRender(pObj)
,SING_START_B(sing_start_b)
,MAX_LYRIC_WIDTH(max_lyric_width)
,mbKtvMode(bKtvMode)
,STAFF_POS_HEIGHT(staff_pos_height)
,m_bLyricRender_All_Init(false)
,m_bAS(bAS)
,m_bFirst(bFirst)
,m_eType(eType)
,m_z(0.0f)
{
	
    mbFirstSentence = true;
    mfMovieImagePos = 0.0f;
    mfTipImageTrans = 0.0f;
    mfTextPos = 0.0f;
    mfTextLittlePos = 0.0f;
    mfTextWidth = 0.0f;
    mfNextTextWidth = 0.0f;
    mfSongNameWidth = 0.0f;
    
	setMode(mbKtvMode);
// 	if ( mbKtvMode )
// 	{
// 		mfStartPos = 0.0f;
// 		mfNextStartPos = 0.0f;
// 	}
// 	else
// 	{
// 		mfStartPos = SING_START_B;//170.0f / 720.0f;
// 	}
    mbInvalidate = false;
    
    //m_pGLFont = new CGLFont(config_font_size);
	m_bLyricRender_All_Init = true;
}

void LyricRender::invalidate()
{
    mbInvalidate = true;
}

void LyricRender::clearCurLyric()
{
	m_VecLyric.clear();
}

//-----------------------------------------------------------------------------
// 设置曲谱模式是否为KTV模式(曲谱模式:KTV模式,游戏模式)
void LyricRender::setMode(bool ktv_mode)
{
	mbKtvMode = ktv_mode;
	if ( mbKtvMode )
	{
		mfStartPos = 0.0f;
		mfNextStartPos = 0.0f;
	}
	else
	{
		mfStartPos = SING_START_B;//170.0f / 720.0f;
	}
}

//-----------------------------------------------------------------------------
// 设置歌词框演唱起点
void LyricRender::setFirstLyricStartPos( float start_pos )
{
	mfStartPos = start_pos;
	mfNextStartPos = 0.0f;
	invalidate();
}

//-----------------------------------------------------------------------------
// 设置歌词框演唱起点
void LyricRender::setNextLyricStartPos(float start_pos)
{
	mfStartPos = 0.0f;
	mfNextStartPos = start_pos;
	invalidate();
}

//-----------------------------------------------------------------------------
/// 歌曲播放时间
void LyricRender::setSongTime(const std::string & curtime)
{
	mSongPlayedTime = curtime;
	invalidate();
}

//-----------------------------------------------------------------------------
// 歌曲名称
void LyricRender::setSongName(const std::string & curname)
{
/*
	//CEGUI::Font * songInfoFnt = &CEGUI::FontManager::getSingleton().get("simhei9"); 
	CEGUI::Font * songInfoFnt = &CEGUI::FontManager::getSingleton().get("simhei10"); 
	CEGUI::String & ch = Ci::CUtility::mbc2CeguiString( curname );
	mfSongNameWidth = songInfoFnt->getTextExtent( ch );

	mSongName = curname;
	invalidate();
*/
}

//-----------------------------------------------------------------------------
// 显示歌词(返回歌词长度)std::vector< _tGuiLyricInfo >
float LyricRender::setFirstLyric( VECTGUILYRICVECT & lyric_list, bool refresh/* = false*/ )
{

	mfTextWidth = 0;

	m_VecLyric = lyric_list;

	if ( refresh )
	{
		invalidate();
	}

	return mfTextWidth;
}

//-----------------------------------------------------------------------------
// 显示小歌词std::vector< Gui::_tGuiLyricInfo >
float LyricRender::showSecondLyric( LISTGUILYRICVECT & lyric_list, bool refresh/* = false*/  )
{
	mfNextTextWidth = 0.0f;
	
	GSize sz = cls_gl->m_winSize;
	std::wstring strTemp;
	float offset = 0.0f;
	LISTGUILYRICVECT::iterator i;
	for( i = lyric_list.begin(); i != lyric_list.end(); ++i )
	{
		_tGuiLyricInfo & lyric_info = *i;
		lyric_info.pos = offset;
		lyric_info.width = int(m_pGLFont->getTextWidth( lyric_info.lyric ) * FONT_WIDTH_SCALE);
		strTemp.append( lyric_info.lyric );
		offset += lyric_info.width + FONT24_SPACE;
	}

	float fRealTextWidth = m_pGLFont->getTextWidth(strTemp) * FONT_WIDTH_SCALE;		//20.0f / 16.0f
	mfNextTextWidth = fRealTextWidth + FONT24_SPACE * ( lyric_list.size() - 1 );
	mfTextLittlePos = sz.width - mfNextTextWidth - RIGHT_INDENT;

	//mVecLittleLyric = lyric_list;
	m_VecLittleLyric = lyric_list;

	if ( refresh )
	{
		invalidate();
	}

	return fRealTextWidth;
}

//-----------------------------------------------------------------------------
// 设置滑动图标位置
void LyricRender::setLyricMovePos(float position)
{
	mfMovieImagePos = position;
}

//-----------------------------------------------------------------------------
// 设置提示图标透明度
void LyricRender::setLyricTipImageTrans(float trans)
{
	mfTipImageTrans = trans;
}

//-----------------------------------------------------------------------------
// 设置当前句是否为第一句
void LyricRender::switchSentence(bool bFirstSentence)
{
	mbFirstSentence = bFirstSentence;
}

//-----------------------------------------------------------------------------
// 重置所有标志位
void LyricRender::reset()
{
	//mVecLyric.clear();
	m_VecLyric.clear();
	//mVecLittleLyric.clear();
	m_VecLittleLyric.clear();

	mbFirstSentence = true;
	mfMovieImagePos = 0.0f;
	mfTipImageTrans = 0.0f;

	mfTextPos = 0.0f;
	mfTextLittlePos = 0.0f;
	mfTextWidth = 0.0f;
	mfNextTextWidth = 0.0f;

	//GeometryBuffer & geo = getGeometryBuffer();
	//geo.reset();
	/*cls_gl->m_pImgsetMgr->getImageset( L"zfont" )*/
	m_pGLFont->getImgset()->clear();
}

//-----------------------------------------------------------------------------
void LyricRender::drawGraphBuffer()
{
	static GColorRect color_rect0( 0xFF65CCF4, 0xFF2366DF, 0xFF65CCF4, 0xFF2366DF );
	static GColorRect color_rect1( 0xFFDDDDDD, 0xFFDDDDDD, 0xFFDDDDDD, 0xFFDDDDDD );
    
    if ( !mbInvalidate )
    {
        return;
    }
    
	if ( mbKtvMode )
	{
		GSize & sz = cls_gl->m_winSize;
		if ( m_pGLFont )
		{
			GPoint pt;
			CGRect clip_rect_l;
			CGRect clip_rect_r;
			VECTGUILYRICVECT::iterator i;
			for( i = m_VecLyric.begin(); i != m_VecLyric.end(); ++i )
			{
				 _tGuiLyricInfo & li = *i;//const
				pt.x = li.pos; //+ mfTextPos;//( float )i.pos * sz.d_width;
				pt.y = li.y;
				pt.z = li.z;
				std::wstring & ch = li.lyric;
				int tmp_w = (int)(li.width*li.npass);
				clip_rect_l = CGRectMake(pt.x,pt.y,pt.z,tmp_w,li.height);
				clip_rect_r = CGRectMake(pt.x+tmp_w,pt.y,pt.z,li.width-tmp_w,li.height);
				// 101,204,244 下面是 35,102,223
				m_pGLFont->drawText( ch, pt, &clip_rect_l, color_rect0 );
				m_pGLFont->drawText( ch, pt, &clip_rect_r, color_rect1 );
			}
		}
	}
	else
	{
        if ( m_VecLyric.empty() )
        {
            return;
        }
        
        GSize sz = CGSizeMake( cls_gl->m_winSize.width, 40 );//getPixelSize();
		GPoint pt;
		if (m_bAS)
		{
			if (m_bFirst)
			{
				pt = CGPointMake( 0.0f,STAFF_POS_HEIGHT+150,m_z);
			}
			else
			{
				pt = CGPointMake(0.0f,STAFF_POS_HEIGHT-30-m_pGLFont->getConfigFontSize(),m_z);
			}
		}
		else
		{
			if (m_eType == SHOW_ONE)
			{
				pt = CGPointMake( 0.0f, STAFF_POS_HEIGHT+150,m_z);
			}
			else if (m_eType != SHOW_NONE)
			{
				static float fConst = STAFF_POS_HEIGHT+120+STAFF_POS_HEIGHT+SECONDWAVEHEIGHT;
				pt = CGPointMake( 0.0f, (fConst-m_pGLFont->getConfigFontSize()) /2.0f,m_z );
			}
		}
        CGRect rt = CGRectMake(pt.x, pt.y,m_z, sz.width, sz.height);

        CGRect clip_rect0 = rt;
        clip_rect0.size.width = (int)( clip_rect0.size.width * mfStartPos );
        CGRect clip_rect1 = rt;
        clip_rect1.size.width -= clip_rect0.size.width;
        clip_rect1.origin.x += clip_rect0.size.width;
        
		//std::vector< _tGuiLyricInfo >::const_iterator i;
		VECTGUILYRICVECT::iterator i;//const_
        for( i = m_VecLyric.begin(); i != m_VecLyric.end(); ++i )
        {
			_tGuiLyricInfo & li = *i;//const 
            if ( li.pos > -20 && li.pos < sz.width )
            {
				const std::wstring & ch = li.lyric;
                pt.x = li.pos;
// 				if (i == m_VecLyric.begin())
// 				{
// 					static float first_x = li.pos;
// 					float move_x = li.pos - first_x;
// 					std::string str_ch;
// 					Wstr2Str(ch,str_ch);
// 					char buf [260] = {0};
// 					sprintf_s(buf,259,"\n%s point.x %f",str_ch.c_str(),move_x);
// 					first_x += move_x;
// 					OutputDebugString(buf);
// 				}
                m_pGLFont->drawText( ch, pt, &clip_rect0, color_rect0);
                m_pGLFont->drawText( ch, pt, &clip_rect1, color_rect1);
            }
        }
    }

}