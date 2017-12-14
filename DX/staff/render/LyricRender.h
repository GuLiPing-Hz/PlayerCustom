/*
	注释时间:2014-4-25
	author: glp
	歌词渲染
*/
#pragma  once

#include "IComRender.h"

#define FONT24_SPACE 2

//-----------------------------------------------------------------------------
class LyricRender : IComRender
{
public:
	// 构造函数
	LyricRender( ISceneObject* pObj,eShowWave eType,bool bAS,bool bFirst,const float staff_pos_height,const float sing_start_b,const int max_lyric_width,bool bKtvMode);

	// 析构函数
	virtual ~LyricRender();
	//设置曲谱高度
	void setStaffPosHeight(const float staff_pos_height,eShowWave eType){STAFF_POS_HEIGHT=staff_pos_height;m_eType=eType;}
	//设置字体
	void setFont(CGLFont* pFont,float z){m_pGLFont=pFont;m_z=z;}

	// 设置曲谱模式是否为KTV模式(曲谱模式:KTV模式,游戏模式)
	void setMode( bool ktv_mode );

	// 设置歌词框演唱起点
	void setFirstLyricStartPos( float start_pos );

	// 设置歌词框演唱起点
	void setNextLyricStartPos( float start_pos );

	// 歌曲播放时间
	void setSongTime( const std::string& curtime );

	// 歌曲名称
	void setSongName( const std::string& curname );

	// 显示歌词std::vector< _tGuiLyricInfo >
	float setFirstLyric( VECTGUILYRICVECT & lyric_list, bool refresh = false );

	// 显示小歌词std::vector< _tGuiLyricInfo >
	float showSecondLyric( LISTGUILYRICVECT & lyric_list, bool refresh = false  );

	// 设置滑动图标位置
	virtual void setLyricMovePos( float position );

	// 设置提示图标透明度
	virtual void setLyricTipImageTrans( float trans );

	// 设置当前句是否为第一句
	virtual void switchSentence( bool bFirstSentence );

	// 重置所有标志位
	void reset();

	virtual void update(const ulong delta_ms){}

	virtual void drawGraphBuffer();

	void clearCurLyric();
protected:
	void invalidate();
private:
	float								m_z;
	bool								m_bAS;
	bool								m_bFirst;
	//std::vector< _tGuiLyricInfo >	mVecLyric;
	VECTGUILYRICVECT	m_VecLyric;
	//std::vector< _tGuiLyricInfo >	mVecLittleLyric;
	LISTGUILYRICVECT		m_VecLittleLyric;

	std::string					mSongName;
	std::string					mSongPlayedTime;

	bool								mbFirstSentence;

	float								mfMovieImagePos;
	float								mfTipImageTrans;
	float								mfStartPos;
	float								mfNextStartPos;
	float								mfTextPos;
	float								mfTextLittlePos;
	float								mfTextWidth;
	float								mfNextTextWidth;
	float								mfSongNameWidth;
	bool								mbKtvMode;

	CGLFont *					m_pGLFont;//不需要释放，保存一份
	bool								mbInvalidate;
	RenderEImage *			mMoveImage;

	float								SING_START_B;//歌词演唱框
	float								STAFF_POS_HEIGHT;//曲谱高度
	int								MAX_LYRIC_WIDTH;//歌词最大宽度 
	eShowWave				m_eType;
public:
	bool								m_bLyricRender_All_Init;
};
