/*
	注释时间:2014-4-25
	author: glp
	句以及相关内容渲染
*/
#ifndef SENTENCERENDER__H__
#define SENTENCERENDER__H__

#include "IComRender.h"

class CGLFont;
class CImgsetEx;
class COpenGLWindow;
//-----------------------------------------------------------------------------
class SentenceRender : IComRender
{
protected:
//设定粒子特效等级
	void setParticleEnable(GLSentenceLevel  nLevel){m_nParticleLevel=nLevel;}

public:
	SentenceRender(bool &bInit,ISceneObject* pObj,const eShowWave eType,bool bFirst,const GSize& winsize,const std::wstring cur_songName,const std::wstring cur_singerName
		,const std::wstring curSingerPart,const float staff_pos_height,const float staff_height,const int wavewnd_left_pixels,bool bktvMode=false);
	~SentenceRender();
public:
//设置谱线位置高度
	void setStaffPosHeight(const float fHeight,const eShowWave eType);
//设定是否显示当前句评分
	void setShowScore(bool b){m_bShowScore=b;}
	bool getShowScore()const {return m_bShowScore;}
//设置字库
	void setFont(CGLFont* pFont);
//设置星星轨迹的起始点
	void setStarStartPoint(const float x,const float y);
//设定句分数
	void showSentenceLevelScore(int nPitchScore,int nLyricScore);
//重置
	void resetSentenceRender();
//设定是否ktv模式
	void setMode(bool bktvMode){m_bktvMode=bktvMode;}
//设定句的数量
	void setSentenceNum(uint nSentence);
//获取星星值
	int getMyCombo(){return m_nMyCombo;}
//获得总分
	int getFinalScore(){return m_nScoreAveDest;}
//最终数据
	void getFinalData(int &nDestAveScore,int &nStarCombo,int &nSentence){nDestAveScore=m_nScoreAveDest;nStarCombo=m_nMyCombo;nSentence=m_nSentence;}
public:
	//更新
	virtual void update(const ulong delta_ms);
	//描绘所有的四边形
	virtual void drawGraphBuffer();
protected:
	void updateRingBG(ulong delta_tm);
	void updateMicScore(ulong delta_ms);
	void updateLevelScore(ulong delta_ms);
	//设定句评分等级
	void showSentenceLevel(GLSentenceLevel sentence_level);

	void moveCurve(float &star_start_x,float &star_start_y,/*float s_fix_x,float s_fix_y,*/ulong delta_ms);
//描绘星星值，以及星星图片
	void drawStarValue();
//描绘顶部的麦克风，以及当前的平均分值
	void _drawScore(float& pos_x,float& pos_y);
	void drawMicScore();

	void drawRingBG();

	void drawScoreLevel();//描绘当前句的句评分等级
private:
	float											m_star_start_x;
	float											m_star_start_y;
	float											m_parabola_a;
	float											m_parabola_b;
	float											m_excursion_x;
	float											m_excursion_y;
	float											m_per_ms_move_x;

	int											m_nPreScore;
	int											m_nScore;
	bool											m_b_start_score_tm;
	ulong										m_nscore_last_tm;
	ulong										m_nscore_per_last_tm;
	ulong										m_nroll_pernumber_tm;

	//NSMutableArray					*mNumberImages;
	bool											m_bShowScore;
	VECTORIMAGE						m_NumberImages;
	VECTORIMAGE						m_StarNumberImages;
	VECTORIMAGE						m_SentenceNumberImages;
	VECTORINT								m_veccurnumberBuf;

	float											m_fScorePercent;
	int											m_nScoreDigit;
	int											m_nScoreAveCur;
	int											m_nScoreAveNext;
	int											m_nScoreAveDest;//3位数，第三位为小数
	int											m_nScoreSum;
	uint											m_nSentence;
	VECTORINT								m_vectDestScore;
	VECTORINT								m_vectCurScore;

	bool											m_bNumberResize;
	VECTORINT								m_vecsrcnumberBuf;
	VECTORINT								m_vecdestnumberBuf;//评分值
	VECTORINT								m_vecstardestnumberBuf;//星星值
	RenderEImage *						m_ImgPoint;//小数点
	RenderEImage *						m_ImgPoint2;//第二个小数点
	//NSMutableArray                 *mGradeImages;
	VECTORIMAGE						m_GradeImages;
	//////////////////////////////////////////////////////////////////////////glp
	//VECTORIMAGE						m_KtvGradeImages;
	//歌词评分低于多少分，进行提醒
	bool												m_bLyricScoreNotify;
	RenderEImage*							m_ImgLyricScoreNotify;
	CAlpha										m_gAlphaLyricScoreNotify;

	Imageset * m_imgset;
	//评分相关图片
	RenderEImage * m_ImgMic;
	RenderEImage * m_ImgScoreBg;
	//评分特效图片
	bool			m_b_ring_florid;
	bool			m_b_level_ring;
	ulong		m_nlevel_last_tm;//持续的时间
	RenderEImage*	m_ImgRingBg;
	//星星图片
	RenderEImage*	m_ImgStar;
	RenderEImage*	m_ImgPlus;
	RenderEImage*	m_ImgFen;
	//歌曲信息
	eShowWave								m_eShowWaveType;
	bool												m_bFirst;

	CGLFont*									m_pFont;
	std::wstring									m_cur_songName;
	std::wstring									m_next_songName;
	std::wstring									m_cur_singerPart;
	bool												m_bSIMoveUp;
	bool												m_bSIMoveRight;
	bool												m_bSIKeep;
	bool												m_bSIDisappear;
	bool												m_bShowSongInfo;
	ulong											m_nlast_songinfo_ms;
	//wave 特效
	RenderEImage*	m_ImgWaveHLine;
	bool			m_bStarFlorid;
	bool			m_bStarFloridSec;
	bool			m_bStarFloridThr;
	bool			m_bStar_Big;
	bool			m_bStar_Little;
	ulong		m_nstar_loop_ms;
	ulong		m_nstar_last_tm;//持续的时间

	//////////////////////////////////////////////////////////////////////////
	GSize										m_windowsize;
///glp
	float											STAFF_POS_HEIGHT;//曲谱位置高度
	float											STAFF_HEIGHT;//曲谱的高度
	int											WAVEWND_LEFT_PIXELS;

	bool											m_bktvMode;
	GLSentenceLevel					m_nParticleLevel;
	int											m_nMyCombo;
	int											m_nPreCombo;

	CGLFlorid*								m_starFlorid;//星星图标
	CGLFlorid*								m_FloridGood;
	CGLFlorid*								m_FloridNice1;
	CGLFlorid*								m_FloridNice2;
	CGLFlorid*								m_FloridCool1;
	CGLFlorid*								m_FloridCool2;
	CGLFlorid*								m_FloridPerfect1;
	CGLFlorid*								m_FloridPerfect2;
	CGLFlorid*								m_FloridPerfect3;
	CGLFlorid*								m_FloridGodlike1;
	CGLFlorid*								m_FloridGodlike2;
	CGLFlorid*								m_FloridGodlike3;
	CGLFlorid*								m_FloridGodlike4;
	CGLFlorid*								m_FloridGodlike5;
	CGLFlorid*								m_FloridUnbelivable1;
	CGLFlorid*								m_FloridUnbelivable2;
	CGLFlorid*								m_FloridUnbelivable3;
	CGLFlorid*								m_FloridUnbelivable4;
	CGLFlorid*								m_FloridUnbelivable5;
	CGLFlorid*								m_FloridWaveRight;
	CGLFlorid*								m_FloridWaveLeft;
	CGLFlorid*								m_FloridStarMove;

	ulong										m_nlevel_icon_ms;
	bool											m_bsentence_level_start;
	ulong										m_nplus_number_ms;
	bool											m_bplus_number;
	bool											m_bflorid_start;
	bool											m_bLevel_F;
	bool											m_bLevel_S;
public:
	bool											m_bSingerBufferInit;
	bool											m_bSentenceRender_All_Init;
};



#endif//SENTENCERENDER__H__
