/*
	注释时间:2014-4-25
	author: glp
	曲谱，pitch以及相关信息渲染
*/

#include <vector>

#include "../particlesystem/hgecolor.h"
#include "IComRender.h"

class COpenGLWindow;

class WaveRender : IComRender
{
public:
	// 构造函数
	WaveRender(bool &bInit,ISceneObject* pObj,bool bFirst,const float sing_start_b,const float staff_pos_height,const int max_lyric_width,const int staff_height,bool bKtvMode,int config_speed_particle);

	// 析构函数
	virtual ~WaveRender();

public:
	//设置谱线位置高度
	void setStaffPosHeight(const float fHeight);
	// 设置曲谱模式是否为KTV模式(曲谱模式:KTV模式,游戏模式)
	void setMode( bool ktv_mode );
	// 设置谱线
	void setLineGroup( const VECTGUIPITCHVECT & wave_list, bool refresh = false );//std::vector< _tGuiWaveInfo >

	// 设置游标
	void setRightPitch( const _tGuiCursorInfo & ci, float ktvErr, float gameErr );

	//设置当前句的粒子特效的位置
	void setCurrentXY( float x,float y,double db,bool bSing );
	//获取当前句的粒子特效的位置
	void getCurrentXY(float& x,float& y){x=m_cur_cursor_x;y=m_cur_cursor_y;}
	//设置当前句的起始游标位置
	void setCurSentenceStartPos(float x);

	// 设置单句移动速度
	void setMovePos( float position );

	// 清除游标
	void clearLyricCursor();

	// 设置句尾线
	void setSentenceLineList( const LISTSENTENCELINEVECT & sentenceline_list, bool refresh = false );
	//
	void resetWaveRender();

	// 开启/关闭谱线光环特效
	virtual void enableWaveEffect( bool enable = true );

	// 开启/关闭谱线星光特效
	//virtual void enableStarEffect( int pitch_index, bool enable = true );
	//获取曲谱高度
	int getWaveHeight() const {return m_waveHeight;}
	//清除当前唱对的描绘以及所有音符
	void clearCurRightPitchWave();
	//标志是否是切换了音符
	void setSwitchPitch(bool bSwitchPitch){m_nSwitchPitch=bSwitchPitch;}
	//设置当前pitch
	void setCurIPitch(int i){m_curIPitch = i;}
	//设置切换歌句标志
	void setSwitchSentence(bool bSwitchSentencd){m_bSwitchSentence = bSwitchSentencd;}
	//设置是否需要重置粒子系统的位置
	void setNeedSetPosition(bool bNeedSet){m_bNeedSetPosition=bNeedSet;}
	//画ktvframe
	void drawKtvFrame();
	//设置
	void setDrawKtvFrame(bool bDraw){m_bNeedDrawKtvFrame=bDraw;}
	//更新曲谱顶点的状态信息
	int    getFBBallSize() const;
	void pushFBBall();
	void popFBBall();
	bool	getCursorRunning();
	void setCursorStop();

	void setTimeLine(ulong nStartMS,ulong nEndMS);
	void setCurTimeLine(ulong nCurMS);
	void resetTimeLine(){m_nDuration=0;m_nCurMs=0;}
public:
	virtual void update(const ulong delta_ms);
	//描绘多边形
	virtual void drawGraphBuffer();
protected:
	void invalidate();
private:
	//绘制时间线
	void drawTimeLine();
	// 绘制句尾线
	void drawSentenceLine( const LISTSENTENCELINEVECT & sentenceline_list );//std::vector< int >

	// 绘制游戏模式光标
	void drawCursor();

	// 绘制谱线
	void drawPitch( VECTGUIPITCHVECT & waves);//std::vector< _tGuiWaveInfo > const

	// 计算游标绝对位置
	void _locateCursor();

	// 处理游标队列
	void _dealWithCursor( const _tGuiCursorInfo & ci );
	//解析游标队列
	void parseVecCursors();
	//画背景
	void drawBackground();
	//画线谱
	void drawFWaveLine();
private:
	bool											m_bFirst;

	typedef std::vector< _tGuiCursorInfo> VECTCURSORINFO;
	//控制时间线
	ulong										m_nCurMs;
public:
	float											m_fPerLength_Ms;
	ulong										m_nStartMs;
	ulong										m_nDuration;
	float											m_fAllLength;
private:
	int											m_nPreLength;
	int											m_nCurLength;

	float											mfStartPos;
	//LISTGUIPITCHVECT				mVecWave;// 谱线
	//_tGuiWaveInfo						mCurWave;
	VECTGUIPITCHVECT				m_VecWave;
	VECTCURSORINFO					mVecCursors; //游标数组
	VECTCURSORINFO					mVecCursorsToDraw;
	//std::vector< _tGuiLyricCursorInfo>				mVecCursorsPos;
	_tGuiCursorInfo						mCursorInfo;// 游标
	LISTSENTENCELINEVECT		mSentenceLineList;// 句尾线
	GPoint									mLyricCursorPos;
	GPoint									mLyricLastCursorPos;
	bool											m_bKtvMode;
	float											mfKtvErr;
	float											mfGameErr;
	float											m_start_cursor_x;

	float											m_cur_cursor_x;
	float											m_cur_cursor_y;
	float											m_nDrawCursorY;

	Imageset*								m_imgset;
	//背景头部
	RenderEImage *						m_head_img;
	//背景头部分隔
	RenderEImage *						m_headsep_img;
	// 标准的谱线
	RenderEImage *						m_ImgWaveFirstBall;
	RenderEImage *						m_ImgStandardWaveL;
	RenderEImage *						m_ImgStandardWaveM;
	RenderEImage *						m_ImgStandardWaveR;
	//控制呼吸灯效果
	LISTFBCONTROL						m_gWaveFirstBallControl;

	RenderEImage *						m_ImgWaveBg;
	//标准wave 的高度
	int											m_waveHeight;

	// 谱线光环
// 	Image *										mImageWaveLeft1;
// 	Image *										mImageWaveMiddle1;
// 	Image *										mImageWaveRight1;

	// 歌唱的靠谱的谱线
	RenderEImage *							m_ImgRightWaveL;
	RenderEImage *							m_ImgRightWaveM;
	RenderEImage *							m_ImgRightWaveR;

	// 歌唱的不靠谱的谱线
	RenderEImage *							m_ImgWrongWaveL;
	RenderEImage *							m_ImgWrongWaveM;
	RenderEImage *							m_ImgWrongWaveR;

	RenderEImage *		m_ImageSentenceLine;//句尾线
	int							m_ImageSentenceLineWidth;
	int							m_ImageSentenceLineHeight;

	RenderEImage * m_ImgCurLine;							// 竖线
	RenderEImage * m_ImgTopLine;
	//背景图片
	RenderEImage * m_ImgKtvFrameL;//ktv 框架
	RenderEImage * m_ImgKtvFrameM;
	RenderEImage * m_ImgKtvFrameR;
	RenderEImage * m_ImgKtvBkgL;//ktv 背景底图
	RenderEImage * m_ImgKtvBkgM;
	RenderEImage * m_ImgKtvBkgR;

	float			m_fChangePerMS;
	float			m_ndest_db_width;
	float			m_ndest_db_height;
	float			m_ncur_db_width;
	float			m_ncur_db_height;
// 	Image*	m_ImgDb;//唱的音量标识图片	

	RenderEImage*	m_ImgCommSingArrow;			
	RenderEImage*	m_ImgRightSingArrow;//唱准的图片 gamemode
	RenderEImage*	m_ImgCurrentArrow;
	
	ulong	  m_nsingr_last_ms;
	float		  m_cur_singr_alpha;
	//combo或时间线
	float				m_timeLine_x;
	float				m_timeLine_y;
	RenderEImage *		m_ImgComboLineL;//槽，起始点在左边框架的宽度-10像素，结束点在右边框架的宽度+10像素，+中间框架宽度
	RenderEImage *		m_ImgComboLineM;
	RenderEImage *		m_ImgComboLineR;
	RenderEImage *		m_ImgComboBall;//亮点,在槽的最右端
	bool				m_bControlBall;
	ulong			m_ntm_control_ball;

	bool				m_bNeedDrawKtvFrame;
	bool				mbEnableWaveEffect;		// 打开谱线光环效果
	bool				mbEnableMoveImage;		// 是否显示谱线上的移动光标
	bool				mbInvalidate;
	

	static const int mnStarImageCount = 13;		// 谱线中的星星特效
	RenderEImage * mStarImages[mnStarImageCount];

	float					SING_START_B;//歌词演唱框
	float					STAFF_POS_HEIGHT;//曲谱位置高度
	int					STAFF_HEIGHT;
	float					VERTICAL_LINE_HEIGHT;//竖线描绘的高度
	int					MAX_LYRIC_WIDTH;//最大歌词宽度

	hgeColorRGB	m_old_start_color;
	hgeColorRGB	m_old_end_color;
	float					m_old_start_size;
	float					m_old_end_size;
	float					m_old_min_speed;
	float					m_old_max_speed;
	CGLFlorid*		m_FloridCursor;//粒子游标

	bool					m_bSwitchSentence;
	bool					m_bSwitchPitch;//是否切换音符
	int					m_nSwitchPitch;//控制音符的切换
	int					m_curIPitch;
	bool					m_bFirstRightPitch;
	bool					m_bFirstPitchCanC;
	bool					m_bNeedMerge;
	bool					m_bNeedSetPosition;
	bool					m_b_cur_sing;
	int					CONFIG_SPEED_PARTICLE;

public:
	bool					m_bWaveRender_All_Init;
};

