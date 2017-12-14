/*
	注释时间:2014-4-25
	author: glp
	评分界面渲染
*/
#pragma  once
//分数结算画面，在一首歌唱完后显示
#include <vector>
#include <map>
#include <string>
#include <wchar.h>


#include "IComRender.h"

class CGLSoundMgr;
class COpenGLWindow;
interface IDirect3DDevice9;
enum LEVEL_SHOW
{
	LEVEL_SHOW_NONE=0,
	LEVEL_SHOW_ONE,
	LEVEL_SHOW_TWO,
	LEVEL_SHOW_THREE,
	LEVEL_SHOW_FOUR,
	LEVEL_SHOW_FIVE,
	LEVEL_SHOW_SIX,
	LEVEL_SHOW_SEVEN,
};


class CScoreFrame : IComRender
{
public:
	CScoreFrame(bool &bInit,ISceneObject* pObj,bool bFirst);
	virtual ~CScoreFrame();
public:
	void			updateRankingInfo(const PlayerScore* pRI);
	void			initScoreFrame(int nDestScore,int nDestStarNum,const std::wstring singerName,const std::wstring songName,eShowWave eType);
	bool			getScoreFrameAllInit()const {return m_bscorefram_allinit;}
	void			setFont(CGLFont* pFont){m_pFontRankinfo=pFont;}
	void			setDestScore(int nDestScore){m_nDestScore=nDestScore;}
public:
	virtual void update(const ulong delta_ms);
	//描绘多边形
	virtual void drawGraphBuffer();
protected:
	void updateLevel(ulong delta_ms);
	void updateSingInfo(ulong delta_ms);

	void animaNumber(float &pos_x,float &pos_y);
	void animaBiggingNumber(float x,float y,float size,GColor cur_colour);
	//画背景图片
	void drawBG();
	//画评分框架
	void drawFrame();
	//画歌手图片，以及横线，
	void drawSinger();
	//画歌曲名，歌手名，总分，连唱
	void drawSingleFont(CGLFont* pflFont,const std::wstring ch,GPoint& pt,CGRect* pclip_rect,const GColor colour=0xFFFFFFFF);
	//画麦克风，得分，以及星星图片，星星值
	void drawScoreCombo();
	//根据当前加的分数
	void drawLevel();
	//画排名信息
	void drawBaseInfo(const std::wstring baseStr,const std::wstring numberStr,float& height,uchar alpha);
	void drawExtInfo(const std::wstring baseStr,const std::wstring numberStr,float& height,uchar alpha,const std::wstring tailStr=L"演唱者");
	void drawRanking();
private:
	PlayerScore				m_structPs;
	RenderEImage*		m_ImgCongratulate;

	bool							m_bscorefram_allinit;
	bool							m_bend_scoreframe;

	std::wstring				m_wstrSongName;
	std::wstring				m_wstrSingerName;
	
	//声音特效
	CGLSound*				m_SoundStar;
	CGLSound*				m_SoundUpgrade;
	CGLSound*				m_SoundParticle;
	CGLSound*				m_SoundClapping1;
	CGLSound*				m_SoundClapping2;
	CGLSound*				m_SoundClapping3;
	CGLSound*				m_SoundBgm;
	//粒子特效
	CGLFlorid*				m_SingleFloridPlanet;
	CGLFlorid*				m_SingleFloridPlanet2;
	CGLFlorid*				m_FloridBigBang1;
	CGLFlorid*				m_FloridBigBang2;
	CGLFlorid*				m_FloridBigBang3;
	CGLFlorid*				m_FloridLeft1;
	CGLFlorid*				m_FloridLeft2;
	CGLFlorid*				m_FloridRight1;
	CGLFlorid*				m_FloridRight2;
	CGLFlorid*				m_FloridStar;
	CGLFlorid*				m_FloridPlanet;
	CGLFlorid*				m_FloridPlanet6;
	CGLFlorid*				m_FloridPlanet7;
	CGLFlorid*				m_FloridExplode;
	CGLFlorid*				m_FloridFlashScreen;
	bool							m_bstartStar;
	//黑幕
	RenderEImage*			m_ImgBlackCurtain;
	//评分相关图片
	RenderEImage*			m_ImgMic;
	RenderEImage*			m_ImgScoreBg;
	RenderEImage*			m_ImgTotalScore;
	RenderEImage*			m_ImgStarNum;
	RenderEImage*			m_ImgBackground;//背景图片
	ulong							m_bg_last_tm;
	RenderEImage*    		m_ImgVerticalLine;//分数框
	bool								m_blineanim;
	bool								m_btextanim;
	ulong							m_hline_last_tm;
	ulong							m_text_last_tm;
	RenderEImage*    		m_ImgHorizontalLine;//水平线
	RenderEImage*			m_ImgHorizontalLineShort;//更短的
	RenderEImage*			m_ImgHLine;
	RenderEImage*			m_ImgRing;
	//星星图片
	RenderEImage*			m_ImgStar;
	bool							m_bScoreBiggingF;
	bool							m_bScoreBiggingS;
	bool							m_bScoreBiggingT;
	//bool						m_bScoreDisplay;
	VECTORIMAGE		m_NumberImages;
	RenderEImage *		m_ImgPoint;//小数点
	ulong						m_nlast_ms;
	ulong						m_nscore_per_last_ms;
	ulong						m_nscore_pernumber_ms;
	ulong						m_nstar_per_last_ms;
	ulong						m_nstar_pernumber_ms;
	VECTORIMAGE		m_StarNumberImages;
	int							m_dest_allnumber_width;
	
	ulong						m_cur_levelmask_text_last_ms;
	RenderEImage*		m_ImgLevelTextMask;
	VECTORIMAGE		m_ImgLevelPicture;//等级图片
	//ulong					m_rock_last_tm;
	ulong						m_cur_level_last_ms;
	bool							m_cur_bchangelevelpic;
	bool							m_cur_bchangeleveltext;
	uint							m_pre_level;
	uint							m_cur_level;
	VECTORIMAGE		m_ImgLevelText;//等级文字
	RenderEImage*		m_ImgSinger;//歌手图片 que
	//Image*					m_ImgSingerBg;//歌手图片alpha通道

	bool							m_bSingerImgsetInitBuffer;
	bool							m_bPreBlackHole;
	bool							m_bBlackHole;

	float							m_fScorePercent;
	int							m_nScoreDigit;
	int							m_nDestScore;//3位数，第三位为小数
	int							m_nCurScore;
	int							m_nNextScore;
	VECTORINT				m_veccurnumberBuf;//当前的buf数字
	VECTORINT				m_vecdestnumberBuf;//评分值数字

	int							m_nStarDigit;
	int							m_nDestStarNum;
	int							m_nCurStarNum;
	int							m_nNextStarNum;
	VECTORINT				m_vecstarcurnumberBuf;
	VECTORINT				m_vecstardestnumberBuf;//星星值数字

	GSize						m_winsize;
	float							FRAME_THINGS_CENTER;
	float							SONGINFO_LEFT_X;
public:
	CGLFont*				m_pFontRankinfo;
	CGLFont*				m_pFontSingerName;
	CGLFont*				m_pFontSongName;
	Imageset*				m_imgset;
	Imageset*				m_imgset2;
	CImgsetEx*				m_imgsetEx;
	bool							m_bFirst;
	bool							m_bScoreFrame_New_Init;
};
