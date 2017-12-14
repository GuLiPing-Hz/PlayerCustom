
#include "stdafx.h"
#include "WaveRender.h"
#include <math.h>

#define SONGINFO_FONT_SIZE 20
#define NEXTSONG_Y (SONGINFO_FONT_SIZE+10)

#define TM_VOLUME_IMG_LOOP 3000
#define VOLUME_IMG_WIDTH 92

#define TM_WAVE_FIST_BALL_LOOP 1000
#define WAVE_EFFECT_WIDTH  1
#define  TIME_SING_RIGHT_MS 1500

hgeColorRGB RIGHT_START_COLOR(251,205,80,255);
hgeColorRGB RIGHT_END_COLOR(243,62,0,255);

const float FIRST_MIN_SPEED = -110;
const float FIRST_MAX_SPEED = -290;

const float SECOND_MIN_SPEED = -90;
const float SECOND_MAX_SPEED = -250;

const float THIRD_MIN_SPEED = -70;
const float THIRD_MAX_SPEED = -210;

const float FOURTH_MIN_SPEED = -50;
const float FOURTH_MAX_SPEED = -170;
//-----------------------------------------------------------------------------
// 构造函数
WaveRender::	WaveRender(bool &bInit,ISceneObject* pObj,bool bFirst,const float sing_start_b,const float staff_pos_height,const int max_lyric_width,const int staff_height,bool bKtvMode,int config_speed_particle)
:IComRender(pObj) 
, mfStartPos( 0.0f)
, mLyricCursorPos(0.0f,0.0f )/*149.8125f, 1.0f*/
, mLyricLastCursorPos( mLyricCursorPos )
, mCursorInfo( mfStartPos, -1.0f, false, RANK_GREEN )//_tGuiLyricCursorInfo::
, m_bKtvMode( bKtvMode )
, m_head_img(NULL)
, m_headsep_img(NULL)
, m_ImgStandardWaveL( NULL )
, m_ImgStandardWaveM( NULL )
, m_ImgStandardWaveR( NULL )
// , mImageWaveLeft1( NULL )
// , mImageWaveMiddle1( NULL )
// , mImageWaveRight1( NULL )
, m_ImgRightWaveL( NULL )
, m_ImgRightWaveM( NULL )
, m_ImgRightWaveR( NULL )
, m_ImgWrongWaveL( NULL )
, m_ImgWrongWaveM( NULL )
, m_ImgWrongWaveR( NULL )
, m_ImgCommSingArrow(NULL)
, m_ImgRightSingArrow(NULL)
, m_ImgCurrentArrow(NULL)
/*, mImageWave( NULL )*/
/*,m_ImgDb(NULL)*/
, mbEnableWaveEffect( false )
, mbEnableMoveImage( false )
, mbInvalidate( false )
,m_cur_cursor_x()
,m_cur_cursor_y()
,m_nDrawCursorY()
,m_start_cursor_x()
,SING_START_B(sing_start_b)
,STAFF_POS_HEIGHT(staff_pos_height)
,MAX_LYRIC_WIDTH(max_lyric_width)
/*,VERTICAL_LINE_HEIGHT(nVL_pos_height)*/
,STAFF_HEIGHT(staff_height)
,m_FloridCursor(NULL)
,m_nSwitchPitch(0)
,m_bSwitchPitch(true)
,m_curIPitch(0)
,m_bSwitchSentence(false)
,m_bFirstRightPitch(true)
// ,mCurWave(NULL)
,m_bFirstPitchCanC(false)
,m_bNeedMerge(false)
,m_bNeedSetPosition(true)
,m_imgset(NULL)
,m_bNeedDrawKtvFrame(false)
,m_b_cur_sing(true)
,CONFIG_SPEED_PARTICLE(config_speed_particle)
,m_bWaveRender_All_Init(false)
,m_nDuration(0)
,m_nCurMs(0)
,m_nPreLength(0)
,m_nCurLength(0)
,m_fPerLength_Ms()
,m_bControlBall(true)
,m_ntm_control_ball(0)
,m_nsingr_last_ms(0)
,m_cur_singr_alpha()
,m_bFirst(bFirst)
,m_ncur_db_width()
,m_ncur_db_height()
,m_ndest_db_width()
,m_ndest_db_height()
,m_fChangePerMS(VOLUME_IMG_WIDTH*1.0f/TM_VOLUME_IMG_LOOP)
{
	
	assert(cls_gl != NULL);

	bInit = m_bWaveRender_All_Init;
	m_imgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
	if (!m_imgset)
	{
		SceneMgr::s_error_msg = L"no renderer texture";
	}
	if (m_bFirst)
	{
		// 标准的谱线
		m_ImgWaveFirstBall		= m_imgset->getImage(L"SongLinePointA");
		m_ImgStandardWaveL	= m_imgset->getImage( L"SongLineAL" );
		m_ImgStandardWaveM	=  m_imgset->getImage( L"SongLineAM" );
		m_ImgStandardWaveR	=  m_imgset->getImage( L"SongLineAR" );
	}
	else
	{
		m_ImgWaveFirstBall		= m_imgset->getImage(L"SongLinePointB");
		m_ImgStandardWaveL	= m_imgset->getImage( L"SongLineBL" );
		m_ImgStandardWaveM	=  m_imgset->getImage( L"SongLineBM" );
		m_ImgStandardWaveR	=  m_imgset->getImage( L"SongLineBR" );
	}
	if (!m_ImgWaveFirstBall || !m_ImgStandardWaveL || !m_ImgStandardWaveM || !m_ImgStandardWaveR)
	{
		SceneMgr::s_error_msg = L"standard wave miss";
		return ;
	}
	m_waveHeight = m_ImgStandardWaveM->getHeight();

	m_ImgWaveBg = m_imgset->getImage(L"SongLineWaveLine");
	// 歌唱的靠谱的谱线
// 	m_ImgRightWaveL		=  m_imgset->getImage( L"SongLineAL" );
// 	m_ImgRightWaveM	=  m_imgset->getImage( L"SongLineAM" );
// 	m_ImgRightWaveR	=  m_imgset->getImage( L"SongLineAR" );

	// 句尾线
 	m_ImageSentenceLine			  = m_imgset->getImage( L"SongLineEndLine" );
 	m_ImageSentenceLineWidth  = m_ImageSentenceLine->getWidth();
 	m_ImageSentenceLineHeight = m_ImageSentenceLine->getHeight();
	if (!m_ImgWaveBg || !m_ImageSentenceLine || !m_ImageSentenceLineWidth || !m_ImageSentenceLineHeight)
	{
		SceneMgr::s_error_msg = L"wavebg sentenceline miss";
		return ;
	}
	// 谱线中的光标特效
	m_ImgTopLine = m_imgset->getImage(L"SongLineDivideTop");
	if (m_bFirst)
	{
		//演唱线
		m_ImgCurLine = m_imgset->getImage( L"SongLineDivideA" );
		//KTV框架
		m_ImgKtvFrameL = m_imgset->getImage(L"StarLineBgAL");
		m_ImgKtvFrameM = m_imgset->getImage(L"StarLineBgAM");
		m_ImgKtvFrameR = m_imgset->getImage(L"StarLineBgAR");

		m_ImgCommSingArrow = m_imgset->getImage(L"SongLineArrowA");
	}
	else
	{
		m_ImgCurLine = m_imgset->getImage( L"SongLineDivideB" );
		m_ImgKtvFrameL = m_imgset->getImage(L"StarLineBgBL");
		m_ImgKtvFrameM = m_imgset->getImage(L"StarLineBgBM");
		m_ImgKtvFrameR = m_imgset->getImage(L"StarLineBgBR");

		m_ImgCommSingArrow = m_imgset->getImage(L"SongLineArrowB");
	}
	if (!m_ImgTopLine || !m_ImgCurLine || !m_ImgKtvFrameL || !m_ImgKtvFrameM || !m_ImgKtvFrameR)
	{
		SceneMgr::s_error_msg = L"topline curline ktvframe miss";
		return ;
	}
	m_timeLine_x = (float)(m_ImgKtvFrameL->getWidth()-10);
	m_timeLine_y = float(STAFF_POS_HEIGHT - 13);/*-200-13*///中心点高度，与曲谱背景的差值

	m_fAllLength = cls_gl->m_winSize.width - m_timeLine_x*2;

	m_ImgKtvBkgL	=  m_imgset->getImage(L"SongLineBgL");
	m_ImgKtvBkgM	=  m_imgset->getImage(L"SongLineBgM");
	m_ImgKtvBkgR	= m_imgset->getImage(L"SongLineBgR");

	m_ImgRightSingArrow = m_imgset->getImage(L"SongLineArrowOK");
	if (!m_ImgKtvBkgL||!m_ImgKtvBkgM||!m_ImgKtvBkgR||!m_ImgRightSingArrow || !m_ImgCommSingArrow)
	{
		SceneMgr::s_error_msg = L"ktvbkg, right or common arrow miss";
		return ;
	}
	VERTICAL_LINE_HEIGHT = STAFF_POS_HEIGHT-(m_ImgCurLine->getHeight()-m_ImgKtvBkgM->getHeight())/2.0f-3;

	m_ImgComboBall		= m_imgset->getImage(L"StarLinePoint");
	m_ImgComboLineL	= m_imgset->getImage(L"StarLineL");
	m_ImgComboLineM	= m_imgset->getImage(L"StarLineM");
	m_ImgComboLineR	= m_imgset->getImage(L"StarLineR");
// 	m_ImgDb = m_imgset->getImage(L"SongLineVolume");
	if(!m_ImgComboBall||!m_ImgComboLineL||!m_ImgComboLineM||!m_ImgComboLineR/*||!m_ImgDb*/)
	{
		SceneMgr::s_error_msg = L"time point,time line miss";
		return ;
	}

	m_gWaveFirstBallControl.clear();

	//粒子游标
	CFloridMgr* floridMgr = cls_gl->m_pFloridMgr;
	if (!floridMgr)
	{
		SceneMgr::s_error_msg = L"floridMgr is null";
		return ;
	}
	if (m_bFirst)
		m_FloridCursor = floridMgr->getFlorid("cursor");
	else
		m_FloridCursor = floridMgr->getFlorid("cursor2");
	if (!m_FloridCursor)
	{
		SceneMgr::s_error_msg = L"cursor florid miss";
		return ;
	}
	m_old_start_size = m_FloridCursor->getParticleSystem()->m_info.fSizeStart;
	m_old_end_size = m_FloridCursor->getParticleSystem()->m_info.fSizeEnd;
	m_old_start_color = (m_FloridCursor->getParticleSystem()->m_info.colColorStart);
	m_old_end_color = (m_FloridCursor->getParticleSystem()->m_info.colColorEnd);
	m_old_min_speed = m_FloridCursor->getParticleSystem()->m_info.fSpeedMin;
	m_old_max_speed = m_FloridCursor->getParticleSystem()->m_info.fSpeedMax;

	m_bWaveRender_All_Init = true;
	bInit = m_bWaveRender_All_Init;
}

//-----------------------------------------------------------------------------
// 析构函数
WaveRender::~WaveRender()
{
	//SAFE_DELETE(m_pFont);
}
void WaveRender::invalidate()
{
    mbInvalidate = true;
}

void WaveRender::setStaffPosHeight(const float fHeight)
{
	STAFF_POS_HEIGHT = fHeight;	
	m_timeLine_y = STAFF_POS_HEIGHT - 13/*-200-13*/;//中心点高度，与曲谱背景的差值
	VERTICAL_LINE_HEIGHT = STAFF_POS_HEIGHT-(m_ImgCurLine->getHeight()-m_ImgKtvBkgM->getHeight())/2.0f-3;
}

//-----------------------------------------------------------------------------
// 设置曲谱模式是否为KTV模式(曲谱模式:KTV模式,游戏模式)
void WaveRender::setMode( bool ktv_mode )
{
	m_bKtvMode = ktv_mode;
}

//-----------------------------------------------------------------------------
// 设置谱线
void WaveRender::setLineGroup( const VECTGUIPITCHVECT & wave_list, bool refresh/* = false*/ )
{
	// 设置标准谱线
	m_VecWave = wave_list;
// 	if (m_bKtvMode)
// 	{
// 		if (!m_VecWave.empty())
// 		{
// 			mCurWave = m_VecWave[0];
// 		}
// 	}
	
	if ( refresh )
	{
		invalidate();
	}
}

//-----------------------------------------------------------------------------
void WaveRender::setCurrentXY( float x,float y,double db,bool bSing )
{
	m_cur_cursor_x = x;
	m_cur_cursor_y = y;
	m_nDrawCursorY = m_cur_cursor_y;
	m_b_cur_sing = bSing;
	if (!m_FloridCursor)
	{
		return ;
	}
	if (db > 80)
	{
		m_FloridCursor->setEmission(400);
		m_FloridCursor->setSpeed(FIRST_MIN_SPEED,FIRST_MAX_SPEED);
	}
	else if (db > 70)
	{
		m_FloridCursor->setEmission(300);
		m_FloridCursor->setSpeed(SECOND_MIN_SPEED,SECOND_MAX_SPEED);
	}
	else if (db > 60)
	{
		m_FloridCursor->setEmission(200);
		m_FloridCursor->setSpeed(THIRD_MIN_SPEED,THIRD_MAX_SPEED);
	}
	else if (db >50)
	{
		m_FloridCursor->setEmission(100);
		m_FloridCursor->setSpeed(FOURTH_MIN_SPEED,FOURTH_MAX_SPEED);
	}
	else
	{
		m_FloridCursor->setEmission(50);
		m_FloridCursor->setSpeed(m_old_min_speed,m_old_max_speed);
	}

	//m_ndest_db_height = m_ndest_db_width;
}

void WaveRender::setCurSentenceStartPos(float x)
{
	m_start_cursor_x = x;
}
//-----------------------------------------------------------------------------
// 设置唱准的音符
void WaveRender::setRightPitch( const _tGuiCursorInfo & ci, float ktvErr, float gameErr )
{
	mfKtvErr = ktvErr;
	mfGameErr = gameErr;
	if ( m_bKtvMode )
	{
		_dealWithCursor( ci );
	}
	else
	{
		static float h = ( float )m_waveHeight;			// Image高度
		mCursorInfo = ci;
		mCursorInfo.y = (ci.y / h * h);
	}
	invalidate();
}

//-----------------------------------------------------------------------------
// 设置单句移动
void WaveRender::setMovePos( float position )
{
	mfStartPos = position;
	invalidate();
}

void WaveRender::clearLyricCursor()
{
	mVecCursors.clear();
}

//-----------------------------------------------------------------------------
// 设置句尾线
void WaveRender::setSentenceLineList( const LISTSENTENCELINEVECT & sentenceline_list, bool refresh/* = false*/ )
{
	mSentenceLineList = sentenceline_list;
}

//-----------------------------------------------------------------------------
// 重置所有数据
void WaveRender::resetWaveRender()
{
	m_nCurMs = 0;
	m_nDuration = 0;
// 	m_fAllLength	= 0;
	m_nPreLength	= 0;
	m_nCurLength	= 0;
/*	m_fPerLength_Ms = 0;*/
	m_ntm_control_ball = 0;
	m_nsingr_last_ms = 0;
	m_cur_singr_alpha = 0;
	m_VecWave.clear();
	mVecCursors.clear();
	mfStartPos = 0.0f;
	mCursorInfo.x = mCursorInfo.y = 0;
	mCursorInfo.r = RANK_NONE;
	mSentenceLineList.clear();
	mLyricCursorPos.x = mLyricCursorPos.y = 0.0f;
	mLyricLastCursorPos.x = mLyricLastCursorPos.y = 0.0f;
    
	m_imgset->clear();
}

//-----------------------------------------------------------------------------
// 开启/关闭谱线光环特效
void WaveRender::enableWaveEffect( bool enable/* = true*/ )
{
	mbEnableWaveEffect = enable;
}

void WaveRender::clearCurRightPitchWave()
{
	mVecCursors.clear();
	m_VecWave.clear();
}
void WaveRender::update(const ulong delta_ms)
{
	LISTFBCONTROL::iterator it_list_ball = m_gWaveFirstBallControl.begin();
	for (;it_list_ball!=m_gWaveFirstBallControl.end();it_list_ball++)
	{
		if ((*it_list_ball).control)
		{
			(*it_list_ball).alpha += delta_ms*1.0f/TM_WAVE_FIST_BALL_LOOP;
			if ((*it_list_ball).alpha>=1 )
			{
				(*it_list_ball).alpha = 1;
				(*it_list_ball).control = false;
			}
		}
		else
		{
			(*it_list_ball).alpha -= delta_ms*1.0f/TM_WAVE_FIST_BALL_LOOP;
			if ((*it_list_ball).alpha <=0 )
			{
				(*it_list_ball).alpha = 0;
				(*it_list_ball).control = true;
			}
		}
	}


	static float h_2 = m_ImgRightSingArrow->getHeight()/2.0f;
	float tmp_cursor_x = m_cur_cursor_x-35;
	//粒子游标
	//if (m_b_cur_sing)
	{
		if (m_FloridCursor)
		{
			if (!m_FloridCursor->getRunning() && !cls_gl->m_bShowEndScore)
				m_FloridCursor->startPS();
// 			if(cls_gl->m_bShowEndScore)
// 				m_FloridCursor->stopPS();

			if (m_bKtvMode)
			{
				if (m_bNeedSetPosition)
				{
					m_FloridCursor->setPosition(tmp_cursor_x,m_cur_cursor_y);//-8 调整粒子特效x的位置
					m_bNeedSetPosition = false;
				}
			}
			else
				m_FloridCursor->moveLineToY(tmp_cursor_x,m_cur_cursor_y,delta_ms/1000.0f,CONFIG_SPEED_PARTICLE,false);
		}
		else
			WLOGH_ERRO << L"cursor florid is null";
	}

	if(!m_bFirst)
		return ;

	if (m_nPreLength >=18 )
	{
		float f_ball_x = m_timeLine_x+m_nPreLength-24;
		m_ntm_control_ball += delta_ms;

		if (m_ntm_control_ball>TM_WAVE_FIST_BALL_LOOP)
		{
			m_ntm_control_ball -= TM_WAVE_FIST_BALL_LOOP;
			m_bControlBall = !m_bControlBall;
		}
	}
}
//-----------------------------------------------------------------------------
void WaveRender::drawGraphBuffer()
{
    if ( !mbInvalidate )
    {
        return;
    }
	//if (m_bNeedDrawKtvFrame)//预留
	{
		//画底图,光标
		drawBackground();
	}
	//return ;
    //画五线谱
	drawFWaveLine();
	// 画谱线
	drawPitch(m_VecWave);

	if ( !m_bKtvMode )
	{
		// 画句尾线
		drawSentenceLine( mSentenceLineList );
	}

	//if (m_bNeedDrawKtvFrame)
	{
		//画框架，
		drawKtvFrame();
	}
	// 画,粒子
	drawCursor();

	if (m_bFirst)
	{
		//描绘时间线
		drawTimeLine();
	}
}

//绘制时间线
void WaveRender::drawTimeLine()
{
	static int tm_line_width = m_ImgComboLineL->getWidth();
	static int tm_line_2_width = tm_line_width*2;
	static int tm_line_width_2 = int(m_ImgComboLineL->getWidth()/2.0f);
	static int tm_line_height = m_ImgComboLineL->getHeight();
	static int tm_line_height_2 = int(tm_line_height/2.0f);
	float tmline_fix_y = m_timeLine_y-tm_line_height_2;
	if (m_nPreLength<tm_line_width_2*4)
	{
		return ;
	}

	static int point_height = m_ImgComboBall->getHeight();
	static int point_width_2 = int(m_ImgComboBall->getWidth()/2.0f);
	static int point_height_2 = int(point_height/2.0f);
	float poing_fix_y = m_timeLine_y-point_height_2;

	m_ImgComboLineL->draw(GPoint(m_timeLine_x,tmline_fix_y));

	float cur_right_x = m_timeLine_x+m_nPreLength-tm_line_width;
	m_ImgComboLineR->draw(GPoint(cur_right_x,tmline_fix_y));

	int cur_center_width = m_nPreLength-tm_line_2_width;
	if (cur_center_width)
	{
		static float tmline_center_startx = m_timeLine_x+tm_line_width;
		m_ImgComboLineM->draw(GPoint(tmline_center_startx,tmline_fix_y),GSize(cur_center_width,tm_line_height));
	}

	if (m_nPreLength >=18 )
	{
		float f_ball_x = m_timeLine_x+m_nPreLength-24;
		float f_shedule = m_ntm_control_ball*1.0f/TM_WAVE_FIST_BALL_LOOP;
		f_shedule = m_bControlBall?f_shedule:(1-f_shedule);
		f_shedule = f_shedule<0.0f?0.0f:f_shedule>1.0f?1.0f:f_shedule;
		GColor cur_color(1.0f,1.0f,1.0f,f_shedule);
		m_ImgComboBall->draw(GPoint(f_ball_x,poing_fix_y),NULL,true,cur_color);
	}
}

//-----------------------------------------------------------------------------
// 绘制句尾线
void WaveRender::drawSentenceLine( const LISTSENTENCELINEVECT & sentenceline_list )
{
	GSize & rt = cls_gl->m_winSize;
	float x = 0.0f;
	LISTSENTENCELINEVECT::const_iterator i;
	for( i = sentenceline_list.begin(); i != sentenceline_list.end(); ++i )
	{
		x = *i;
		if ( x >= 0 && x < rt.width )
		{
			m_ImageSentenceLine->draw( GPoint(x, STAFF_POS_HEIGHT ), GSize( m_ImageSentenceLineWidth, m_ImageSentenceLineHeight) );
		}
	}
}

//-----------------------------------------------------------------------------
// 绘制游戏模式光标
void WaveRender::drawCursor()
{
	// 移动光标的位置
	//CGPoint move_pos( m_cur_cursor_x, m_cur_cursor_y );//移动粒子特效

	if ( m_bKtvMode )
	{
		GSize & sz = cls_gl->m_winSize;

		if ( !m_ImgRightWaveM || !m_ImgRightWaveR || !m_ImgRightWaveL || 
			!m_ImgWrongWaveM || !m_ImgWrongWaveR || !m_ImgWrongWaveL ||
			!m_ImgCurLine )
		{// 没有相应的Image
			return;
		}
		//移动竖线，相当于光标

		// 靠谱的谱线图片参数
		static int h = m_ImgRightWaveM->getHeight();
		static float h_2 = h / 2.0f;
		static int lw = m_ImgRightWaveL->getWidth();
		static int rw = m_ImgRightWaveR->getWidth();

		// 不靠谱的谱线图片参数
// 		static float h_s = mImageWaveMiddle3->getHeight();
// 		static float hh_s = h_s / 2;
// 		static float lw_s = mImageWaveLeft3->getWidth();
// 		static float rw_s = mImageWaveRight3->getWidth();
		parseVecCursors();

		int width = 0;
		std::vector< _tGuiCursorInfo>::iterator it, it_next;
		for ( it = mVecCursorsToDraw.begin(); it != mVecCursorsToDraw.end(); it+=2)
		{
			mCursorInfo = (*it);
			GPoint pos, next, tmp;

			pos.x = mCursorInfo.x;
			pos.y = mCursorInfo.y;

			it_next = it + 1;
			if ( it_next != mVecCursorsToDraw.end() )
			{
				next.y = it_next->y;
				if ( (int)next.y == (int)pos.y )
				{
					static RenderEImage * _il = NULL;
					static RenderEImage * _im = NULL;
					static RenderEImage * _ir = NULL;

					static int _h = 0;
					static float _hh = 0.0f;
					static int _lw = 0;
					static int _rw = 0;

					static float _alpha = 1.0f;

					if ( mCursorInfo.b )
					{// 靠谱
						_il = m_ImgRightWaveL;
						_im = m_ImgRightWaveM;
						_ir = m_ImgRightWaveR;

						_h = h;
						_hh = h_2;
						_lw = lw;
						_rw = rw;

						_alpha = 1.0f;

						mbEnableMoveImage = true;
//////////////////////////////////////////////////////////////////////////glp
						float dest_width = (*it_next).x - (*it).x;
						pos.y -= _hh;
						GColorRect cr( GColor( 1.0f, 1.0f, 1.0f, _alpha ) );
						if ( dest_width > ( _lw + _rw ) )
						{
							width = _lw;
							_il->draw( pos, GSize( _lw, _h ), NULL,true, cr );

							width  = (int)((*it_next).x - (*it).x);
							width -= _rw;
							width -= _lw;
							pos.x += _lw;
							_im->draw( pos, GSize( width, _h ), NULL,true, cr );
							pos.x += width;
							_ir->draw( pos, GSize( rw, _h ), NULL,true, cr );
						}
						else //if ( dest_width >= 10.0f )
						{
							int draw_width = int(dest_width/2);
							CGRect clip_rect_left( pos, GSize(draw_width,_h));
							_il->draw( pos, GSize( _lw, _h ), &clip_rect_left,true, cr );
							
							float x2 = pos.x + draw_width;
							CGRect clip_rect_right(GPoint( x2, pos.y ), GSize(draw_width,_h));
							_ir->draw( GPoint( x2 - _rw, pos.y ),GSize( _rw, _h ), &clip_rect_right,true, cr );

							pos.x += draw_width;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	//粒子游标
	//if (m_b_cur_sing)
	{
		static float sf_arrow_height_2 = m_ImgCurrentArrow->getHeight()/2.0f;
		if (m_bKtvMode)
		{
			m_ImgCurrentArrow->draw(GPoint(m_cur_cursor_x-m_ImgCurrentArrow->getWidth()/2.0f,m_cur_cursor_y-sf_arrow_height_2));
		}
		else
		{
			static float sf_arrow_width_2 = 60.0f;
			m_ImgCurrentArrow->draw(GPoint(m_cur_cursor_x-sf_arrow_width_2,m_cur_cursor_y-sf_arrow_height_2));
		}
	}
}

int WaveRender::getFBBallSize() const 
{
	return (int)m_gWaveFirstBallControl.size();
}

void WaveRender::pushFBBall()
{
	FBControl tmp = {0.0f,true};
	m_gWaveFirstBallControl.push_back(tmp);
}

void WaveRender::popFBBall()
{
	if(!m_gWaveFirstBallControl.empty())
		m_gWaveFirstBallControl.pop_front();
}

bool	WaveRender::getCursorRunning()
{
	if (m_FloridCursor)
	{
		return m_FloridCursor->getRunning();
	}
	else
	{
		return false;
	}
}

void WaveRender::setCursorStop()
{
	if (m_FloridCursor)
	{
		m_FloridCursor->stopPS();
	}
}


void WaveRender::setTimeLine(ulong nStartMS,ulong nEndMS)
{
	m_nStartMs = nStartMS;
	m_nDuration=nEndMS-nStartMS;
	m_fPerLength_Ms = m_fAllLength/m_nDuration;
}

void WaveRender::setCurTimeLine(ulong nCurMS)
{
	m_nCurMs=nCurMS-m_nStartMs;
	m_nPreLength = m_nCurLength;
	m_nCurLength = ulong(m_nCurMs*m_fPerLength_Ms);
}
//-----------------------------------------------------------------------------
// 绘制谱线
void WaveRender::drawPitch( VECTGUIPITCHVECT & waves)//const 
{
	GPoint l( 0.0f, 0.0f );
	GPoint r( 0.0f, 0.0f );

	bool bSingRight = false;
	if ( m_bKtvMode )
	{
		static int h = m_waveHeight;				// Image的高度( 左中右 一样)
		static int h_2 = h / 2;													// Image一半的高度( 左中右 一样)
		static int lw = m_ImgStandardWaveL->getWidth();
		static int rw = m_ImgStandardWaveR->getWidth();
		// 		static int h1 = ( ( int )mImageWaveMiddle1->getHeight() );				// Image的高度( 左中右 一样)
		// 		static int hh1 = h1 / 2;												// Image一半的高度( 左中右 一样)
		// 		static int lw1 = ( int )mImageWaveLeft1->getWidth();
		// 		static int rw1 = ( int )mImageWaveRight1->getWidth();
		//////////////////////////////////////////////////////////////////////////
		VECTGUIPITCHVECT::iterator i;//const_

		for( i = waves.begin(); i != waves.end(); ++i )
		{
			_tGuiWaveInfo & wave = *i;//const
			l.x = wave.l;
			r.x = wave.r;

			l.y = r.y = wave.y - h_2;		// 减去mImageWaveMiddle0的一半高度 21 * 0.5

			l.x = ( l.x > 0 ) ? l.x : WAVE_EFFECT_WIDTH;
			r.x = ( r.x < cls_gl->m_winSize.width ) ? r.x : cls_gl->m_winSize.width - WAVE_EFFECT_WIDTH;

			float dest_width = r.x - l.x;
			static int edgeW = lw+rw;
			if ( dest_width > edgeW )
			{
				int width = int(dest_width - edgeW);
				m_ImgStandardWaveM->draw( GPoint( l.x + lw, l.y ), 
					GSize( (width> 0 ? width : 0 ), h ), NULL);
				m_ImgStandardWaveL->draw( GPoint(l.x, l.y ),
					GSize( lw, h ),NULL);
				m_ImgStandardWaveR->draw( GPoint(l.x + lw+width, l.y ),
					GSize( lw, h ),NULL);
			}
			else if ( dest_width >= 10.0f )//
			{
				int draw_width =  int(dest_width/2);
				CGRect clip_rect_left(GPoint(l.x, l.y ), GSize(draw_width,h));
				CGRect clip_rect_right(GPoint(l.x + draw_width, l.y ), GSize(draw_width,h));
				m_ImgStandardWaveL->draw( GPoint(l.x , l.y ),
					GSize( lw, h ), &clip_rect_left);
				m_ImgStandardWaveR->draw( GPoint(r.x - rw, l.y ),
					GSize( rw, h ), &clip_rect_right);
			}//小于10 不画
		}
	}
	else
	{
		static int h = m_waveHeight;			// 标准谱线高度
		static int h_2 = h / 2;							// 标准谱线一半的高度
		static int lw = m_ImgStandardWaveL->getWidth();
		static int rw = m_ImgStandardWaveR->getWidth();

		LISTFBCONTROL::iterator it_list_ball = m_gWaveFirstBallControl.begin();
		VECTGUIPITCHVECT::iterator i;//const_
		for( i = waves.begin(); i != waves.end(); ++i)
		{
			_tGuiWaveInfo & wave = *i;//const 
			float left = wave.l;
			float right = wave.r;
			float realY = wave.y;

			if (left <= m_cur_cursor_x &&m_cur_cursor_x <= right && m_b_cur_sing)
			{
				if (realY-mfGameErr < m_cur_cursor_y && m_cur_cursor_y < realY+mfGameErr )
				{
					bSingRight = true;
				}
			}
			//cur_point_ball_scale;
			if (it_list_ball == m_gWaveFirstBallControl.end())
				break ;
			float cur_point_ball_scale = (*it_list_ball).alpha;
			it_list_ball ++;

			if ( left >= 0 || right >= 0 )
			{
				GColor cur_pointball_colour = GColor(1.0f,1.0f,1.0f,cur_point_ball_scale);

				l.x = left;
				r.x = right;
				l.y = r.y = realY - h_2;					// 减去mImageWave的一半高度 10 * 0.5

				float dest_width = right-left;
				//////////////////////////////////////////////////////////////////////////
				//亮点
				static int wfb_w = m_ImgWaveFirstBall->getWidth();
				static int wfb_h = m_ImgWaveFirstBall->getHeight();
				static float wfb_w_2 = wfb_w/2.0f;
				static float wfb_h_2 = wfb_h/2.0f;
				static int sum_side_width = lw + rw;
				if ( dest_width > sum_side_width )
				{
					int width = int(dest_width - sum_side_width);
					//画ptich
					m_ImgStandardWaveM->draw( GPoint( l.x + lw, l.y ), GSize( (width> 0 ? width : 0 ), h ), NULL);
					m_ImgStandardWaveL->draw( GPoint(l.x, l.y ),GSize( lw, h ),NULL);
					m_ImgStandardWaveR->draw( GPoint(l.x + lw+width, l.y),GSize( lw, h ),NULL);
					//画亮点
					float x = l.x+7-wfb_w_2*cur_point_ball_scale;
					float y = l.y+7-wfb_h_2*cur_point_ball_scale;
					m_ImgWaveFirstBall->draw(GPoint(x,y),GSize(int(wfb_w*cur_point_ball_scale),int(wfb_h*cur_point_ball_scale)),NULL,true,cur_pointball_colour);
				}
				else if ( fabsf(dest_width-sum_side_width) < 0.001 )//
				{
					//画ptich
					int draw_width = int(dest_width/2);
					CGRect clip_rect_left(GPoint(l.x, l.y ), GSize(draw_width,h));
					CGRect clip_rect_right(GPoint(l.x + draw_width, l.y ), GSize(draw_width,h));
					m_ImgStandardWaveL->draw( GPoint(l.x , l.y ),GSize( lw, h ), &clip_rect_left);
					m_ImgStandardWaveR->draw( GPoint(r.x - rw, l.y ),GSize( rw, h ), &clip_rect_right);
					//画亮点
					float x = l.x+7-wfb_w_2*cur_point_ball_scale;
					float y = l.y+7-wfb_h_2*cur_point_ball_scale;
					m_ImgWaveFirstBall->draw(GPoint(x,y),GSize(int(wfb_w*cur_point_ball_scale),int(wfb_h*cur_point_ball_scale)),NULL,true,cur_pointball_colour);
				}
				//小于像素值，不画
			}
		}
	}

	if (m_b_cur_sing)
	{
		if (bSingRight)
		{
			m_FloridCursor->setSize(1.5f,0.1f);
			m_FloridCursor->setColor(RIGHT_START_COLOR,RIGHT_END_COLOR);
			m_ImgCurrentArrow = m_ImgRightSingArrow;
		}
		else
		{
			m_FloridCursor->setSize(m_old_start_size,m_old_end_size);
			m_FloridCursor->setColor(m_old_start_color,m_old_end_color);
			m_ImgCurrentArrow = m_ImgCommSingArrow;
		}
	}
	else
	{
		m_FloridCursor->setSize(m_old_start_size,m_old_end_size);
		m_FloridCursor->setColor(m_old_start_color,m_old_end_color);
		m_ImgCurrentArrow = m_ImgCommSingArrow;
	}
}
//--------------------------------------------------------------------------
// 计算游标绝对位置
void WaveRender::_locateCursor()
{
	static float h = ( float )m_waveHeight;			// Image高度
	GSize & sz = cls_gl->m_winSize;
	mLyricCursorPos.x = mCursorInfo.x * sz.width;
	mLyricCursorPos.y = ( sz.height - 4*h ) * mCursorInfo.y + 2*h;
}

void WaveRender::drawBackground()
{
	static int bg_side_width = m_ImgKtvBkgL->getWidth();
	static int bg_center_height = m_ImgKtvBkgM->getHeight();
	static int bg_center_dest_width = cls_gl->m_winSize.width - bg_side_width*2;
	//黑底
	float x = 0.0f;
	float y = STAFF_POS_HEIGHT;
	m_ImgKtvBkgL->draw(GPoint(x,y),NULL);//左黑底

	x = (float)(bg_side_width+bg_center_dest_width);
	m_ImgKtvBkgR->draw(GPoint(x,y),NULL);//右黑底

	x = (float)bg_side_width;
	m_ImgKtvBkgM->draw(GPoint(x,y),GSize(bg_center_dest_width,bg_center_height),NULL);//中黑底

	//画竖线
	static float mw = m_ImgCurLine->getWidth()-12.0f;//12像素
	//static float VERTICAL_LINE_HEIGHT = 
	//static float mh_2 = mMoveImage->getHeight() * 0.5;
	m_ImgCurLine->draw( GPoint( m_cur_cursor_x - mw, VERTICAL_LINE_HEIGHT), NULL );
}

void WaveRender::drawFWaveLine()
{
	//五线谱
	m_ImgWaveBg->draw(GPoint(0.0f,STAFF_POS_HEIGHT),GSize(cls_gl->m_winSize.width,STAFF_HEIGHT),NULL);
}

void WaveRender::drawKtvFrame()
{

	m_ImgTopLine->draw(GPoint(m_cur_cursor_x-2,VERTICAL_LINE_HEIGHT));

	static int wave_height = m_ImgKtvBkgM->getHeight();
	static int frame_side_height = m_ImgKtvFrameL->getHeight();
	static int frame_side_width = m_ImgKtvFrameL->getWidth();
	static int frame_center_height = m_ImgKtvFrameM->getHeight();
	static int frame_center_dest_width = cls_gl->m_winSize.width - frame_side_width*2;
	if (m_bFirst)
	{
		//框架
		float cur_wave_height = STAFF_POS_HEIGHT-frame_side_height;
		GPoint dest_point_left = CGPointMake(0.0f,cur_wave_height);
		m_ImgKtvFrameL->draw(dest_point_left);//左边框
		GPoint dest_point_right = CGPointMake(float(frame_side_width+frame_center_dest_width),cur_wave_height);
		m_ImgKtvFrameR->draw(dest_point_right);//右边框
		CGRect dest_rect = CGRectMake((float)frame_side_width,cur_wave_height,GL3D_Z_BASE,frame_center_dest_width,frame_center_height);
		m_ImgKtvFrameM->draw(dest_rect);//中间框

	}
	else
	{
		//框架
		static float cur_wave_height = STAFF_POS_HEIGHT+wave_height;
		static GPoint dest_point_left = CGPointMake(0.0f,cur_wave_height);
		m_ImgKtvFrameL->draw(dest_point_left);//左边框
		static GPoint dest_point_right = CGPointMake(float(frame_side_width+frame_center_dest_width),cur_wave_height);
		m_ImgKtvFrameR->draw(dest_point_right);//右边框
		static CGRect dest_rect = CGRectMake((float)frame_side_width,cur_wave_height,GL3D_Z_BASE,frame_center_dest_width,frame_center_height);
		m_ImgKtvFrameM->draw(dest_rect);//中间框

	}
	//连击图片待处理
	
}

void WaveRender::parseVecCursors()
{
	mVecCursorsToDraw.clear();

	VECTCURSORINFO::iterator i,i_next;
	i = mVecCursors.begin();
	if (i == mVecCursors.end())
	{
		return ;
	}
	if (! (*i).b)
	{
		return ;
	}
	
	while(i!=mVecCursors.end())
	{
		i_next = i+1;
		if (i_next == mVecCursors.end())
		{
			return ;
		}
		if (mVecCursorsToDraw.size()==0)
		{
			mVecCursorsToDraw.push_back(*i);
			mVecCursorsToDraw.push_back(*i_next);
			i+=2;
			continue;
		}
		else if (!((*i).b))
		{
			i = i_next;
			i_next = i+1;
			if (i_next == mVecCursors.end())
			{
				return ;
			}
			mVecCursorsToDraw.push_back(*i);
			mVecCursorsToDraw.push_back(*i_next);
			i+=2;
			continue;
		}
		
		if ((*i).b && (*i_next).b)
		{
			if ((*i).y == (*i_next).y)
			{
				int size = (int)mVecCursorsToDraw.size();
				mVecCursorsToDraw[size-1] = *i_next;
			}
			else
			{
				i = i_next;
				i_next = i+1;
				if (i_next == mVecCursors.end())
				{
					return ;
				}
				mVecCursorsToDraw.push_back(*i);
				mVecCursorsToDraw.push_back(*i_next);
			}
			i+=2;
		}
		else if ((*i).b && !(*i_next).b)
		{
			i = i_next;
		}
	}
	
}
//--------------------------------------------------------------------------
//处理唱准音符
void WaveRender::_dealWithCursor(  const _tGuiCursorInfo & cursor_info  )
{
	int size = (int)mVecCursors.size();

	static int h = m_ImgRightWaveM->getHeight();						// mImageWaveLeft3高度
	//static int hh = ( ( int )mImageWaveLeft2->getHeight() ) / 2;			// mImageWaveLeft2一半的高度
	
	_tGuiCursorInfo cur_cursor_info( cursor_info.x, cursor_info.y, cursor_info.b, cursor_info.r );
	
	bool bfind = false;
	bool b = false;													  // 是否落在标准谱线的某个区间内
	int nmodify_y = ( ( int )( cursor_info.y / h ) ) * h;//调整y坐标为h的整数倍
	static _tGuiWaveInfo  s_wave_info;

	//std::vector< _tGuiWaveInfo >::const_iterator i;
	VECTGUIPITCHVECT::iterator i;//const_

	for( i = m_VecWave.begin(); i != m_VecWave.end(); ++i )
	{//查看是否落在标准谱线的某个区间内，如果是，设置Y值为标准谱线的Y值。
		if (!m_bSwitchSentence)
		{
			if (cursor_info.x > m_VecWave[m_curIPitch].r)
			{
				m_curIPitch ++;
				m_bSwitchPitch = true;
				m_bFirstRightPitch = false;
				if (m_curIPitch >= (int)m_VecWave.size())
				{
					m_bSwitchSentence = true;
				}
			}
		}

		_tGuiWaveInfo & tmpwave_info = *i;//const
		float realY = tmpwave_info.y;
		if ( cursor_info.x >= tmpwave_info.l && cursor_info.x < tmpwave_info.r /*&& tmpwave_info->r - tmpwave_info->l >= 10*/ )
		{
			bfind = true;
			
			int temp = int(cursor_info.y - realY);
			if ( temp >= -mfKtvErr && temp <= mfKtvErr )//认为是唱准的,如果没唱准，则s_wave_info保存的是上次唱准的音符
			{
				s_wave_info = tmpwave_info;//////////////////////////////////////////////////////////////////////////
				nmodify_y = int(realY);
				b = true;
			}
		}

		if ( bfind )
		{
			break;
		}
	}

	cur_cursor_info.y = (float)nmodify_y;
	cur_cursor_info.b = b;

	if (size >= 2)
	{
		_tGuiCursorInfo &last_cursor_info = mVecCursors[size-1];
		if (!last_cursor_info.b && !cur_cursor_info.b)//两次都没唱准，替换掉
		{
			last_cursor_info = cur_cursor_info;
		}
		else if (last_cursor_info.b && !cur_cursor_info.b)//前一次唱准，这一次没唱准
		{
			if (cur_cursor_info.x >= s_wave_info.r)//已经超过上一个音符
			{
				_tGuiCursorInfo tmp_cursor_info = last_cursor_info;
				tmp_cursor_info.x = s_wave_info.r;
				tmp_cursor_info.b = true;
				mVecCursors.push_back(tmp_cursor_info);//唱的上个音符的结束
// 				tmp_cursor_info.b =false;
// 				mVecCursors.push_back(tmp_cursor_info);//加入切换
// 				tmp_cursor_info.b = true;
// 				mVecCursors.push_back(tmp_cursor_info);//唱的这个音符的开始
				mVecCursors.push_back(cur_cursor_info);
			}
			else//没有唱准
			{
				_tGuiCursorInfo tmp_cursor_info = last_cursor_info;
				tmp_cursor_info.x = last_cursor_info.x;
				tmp_cursor_info.b = true;
				mVecCursors.push_back(tmp_cursor_info);
				mVecCursors.push_back(cur_cursor_info);
			}
		}
		else//都加入
		{
			if (m_bSwitchPitch && cur_cursor_info.b && !m_bFirstRightPitch && last_cursor_info.b)//加入分隔符
			{
				_tGuiCursorInfo tmp_cursor_info = cur_cursor_info;
				tmp_cursor_info.b = false;
				mVecCursors.push_back(tmp_cursor_info);
				m_bSwitchPitch = false;
			}
			mVecCursors.push_back(cur_cursor_info);	
		}
	}
	else if (size == 1)
	{
		_tGuiCursorInfo &first_cursor_info = mVecCursors[0];
		if (first_cursor_info.x != cur_cursor_info.x)
		{
			if (first_cursor_info.b)
			{
				if (cur_cursor_info.b)
				{
					mVecCursors.push_back(cur_cursor_info);
					m_bFirstRightPitch = true;
				}
				else
				{
					if (cur_cursor_info.x > s_wave_info.r)
					{
						_tGuiCursorInfo tmp_cursor_info = first_cursor_info;
						tmp_cursor_info.x = s_wave_info.r;
						tmp_cursor_info.b = true;
						mVecCursors.push_back(tmp_cursor_info);//唱的上个音符的结束
// 						tmp_cursor_info.b = false;
// 						mVecCursors.push_back(tmp_cursor_info);//切换标记
// 						tmp_cursor_info.b = true;
// 						mVecCursors.push_back(tmp_cursor_info);//唱的这个音符的开始
						mVecCursors.push_back(cur_cursor_info);
					}
					else
					{
						_tGuiCursorInfo tmp_cursor_info = first_cursor_info;
						tmp_cursor_info.b = true;
						mVecCursors.push_back(tmp_cursor_info);
						mVecCursors.push_back(cur_cursor_info);
					}
				}
			}
			else
			{
				mVecCursors[0] = cur_cursor_info;
			}
		}
	}
	else
	{
		mVecCursors.push_back(cur_cursor_info);
	}

}
