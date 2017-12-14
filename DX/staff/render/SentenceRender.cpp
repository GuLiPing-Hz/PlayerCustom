#include "stdafx.h"
#include "SentenceRender.h"

#define SCORE_LEVEL1 900
#define SCORE_LEVEL2 850
#define SCORE_LEVEL3 800
#define SCORE_LEVEL4 700

#undef GRADE_NEWLEVEL
#define GRADE_NEWLEVEL(score,level) \
	if ( score > SCORE_LEVEL1 ) \
	level = GLSENTENCEGRADELEVEL_PERFECT; \
else if( score > SCORE_LEVEL2 ) \
	level = GLSENTENCEGRADELEVEL_GREAT; \
else if ( score > SCORE_LEVEL3 ) \
	level = GLSENTENCEGRADELEVEL_COOL; \
else if(score >= SCORE_LEVEL4) \
	level = GLSENTENCEGRADELEVEL_GOOD; \
else \
	level = GLSENTENCEGRADELEVEL_NONE;

//评分效果滞留时间(单位:毫秒)
#define  TIME_TO_LIVE_COMMON  1000
#define  TIME_TO_STAR_BIG 30
#define  TIME_TO_STAR_MOVE 500
#define  TIME_TO_STARSMALLING_START (TIME_TO_STAR_BIG+TIME_TO_STAR_MOVE)
#define  TIME_TO_LIVE_STAR 500
#define  TIME_TO_END_SMALLINGSTAR (TIME_TO_STARSMALLING_START+TIME_TO_LIVE_STAR)
#define  TIME_TO_STAR_LOOP_TM 300
#define  TIME_TO_LIVE_LEVEL_RING 300

#define  TIME_TO_LIVE_PLUSNUMBER1 200
#define  TIME_TO_LIVE_PLUSNUMBER2 800
#define  TIME_TO_LIVE_PLUSNUMBER_END (TIME_TO_LIVE_PLUSNUMBER1+TIME_TO_LIVE_PLUSNUMBER2)

#define  TIME_TO_LEVEL_SMALLING 400
#define  TIME_TO_LEVEL_SEC 100
#define  TIME_TO_LEVEL_PARTICLE 500
#define  TIME_TO_LEVEL_BALL 50
#define  TIME_TO_LEVEL_KEEPSTART 600
#define  TIME_TO_LEVEL_GAMMA 1600
#define  TIME_TO_LEVEL_KEEP 1000
#define  TIME_TO_LEVEL_DISAPPEAR 1000
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

#define LYRICSCORE_NOTIFY_MS 3000
#define LYRICSCORE_NOTIFY_CYCLEMS 1500
//-----------------------------------------------------------------------------

SentenceRender::SentenceRender(bool &bInit,ISceneObject* pObj,const eShowWave eType,bool bFirst,const GSize& winsize,const std::wstring cur_songName
							   ,const std::wstring cur_singerName,const std::wstring curSingerPart,const float staff_pos_height,const float staff_height,const int wavewnd_left_pixels,bool bktvMode)
:IComRender(pObj)
,m_gAlphaLyricScoreNotify(LYRICSCORE_NOTIFY_CYCLEMS,LYRICSCORE_NOTIFY_MS)
,m_windowsize(winsize)
,m_nScore(0)
,m_nPreScore(0)
,STAFF_POS_HEIGHT(staff_pos_height)
,STAFF_HEIGHT(staff_height)
,WAVEWND_LEFT_PIXELS(wavewnd_left_pixels)
,m_star_start_x((float)wavewnd_left_pixels)
,m_star_start_y(staff_pos_height)
,m_bktvMode(bktvMode)
,m_nScoreSum(0)
,m_nSentence(0)
,m_nMyCombo(0)
,m_nPreCombo(0)
,m_nParticleLevel(GLSENTENCEGRADELEVEL_NONE)
,m_nstar_last_tm(0)
,m_bStarFlorid(false)
,m_bStarFloridSec(false)
,m_bStarFloridThr(false)
,m_parabola_a(0)
,m_parabola_b(0)
,m_nscore_last_tm(0)
,m_nroll_pernumber_tm(0)
,m_nscore_per_last_tm(0)
,m_nScoreAveCur(0)
,m_nScoreAveDest(0)
,m_bNumberResize(false)
,m_b_start_score_tm(false)
,m_bShowScore(true)
,m_bSentenceRender_All_Init(true)
,m_bsentence_level_start(false)
,m_bplus_number(false)
,m_bflorid_start(false)
,m_bLevel_F(false)
,m_bLevel_S(false)
//,m_bLevel_T(false)
,m_nlevel_icon_ms(0)
,m_nplus_number_ms(0)
,m_bStar_Big(true)
,m_bStar_Little(false)
,m_nstar_loop_ms(0)
,m_b_ring_florid(false)
,m_excursion_x(0)
,m_excursion_y(0)
,m_bFirst(bFirst)
,m_pFont(NULL)
,m_nlast_songinfo_ms(0)
,m_bSIMoveUp(true)
,m_bSIMoveRight(true)
,m_bSIKeep(true)
,m_bSIDisappear(true)
,m_bSingerBufferInit(false)
,m_bShowSongInfo(true)
,m_eShowWaveType(eType)
,m_cur_singerPart(curSingerPart)
,m_nlevel_last_tm(0)
,m_ImgLyricScoreNotify(NULL)
,m_bLyricScoreNotify(false)
,m_nScoreDigit(2)//小数点左右各一个0，所以是2两位
,m_nScoreAveNext(0)
,m_fScorePercent(0.0f)
{ 
	assert(cls_gl != NULL);

	bInit = m_bSentenceRender_All_Init;
	RenderEImage * img = NULL;
	m_imgset = cls_gl->m_pImgsetMgr-> getImageset(L"renderer");
	if (!m_imgset)
	{
		SceneMgr::s_error_msg = L"no renderer texture";
		m_bSentenceRender_All_Init = false;
	}

	m_ImgLyricScoreNotify = m_imgset->getImage(L"LyricScoreNotify");
	if(!m_ImgLyricScoreNotify)
	{
		SceneMgr::s_error_msg = L"miss LyricScoreNotify image";
		m_bSentenceRender_All_Init = false;
	}

	for ( int i = 0; i < 10; ++i )
	{
		wchar_t bufNumber[256] = {0};
		swprintf(bufNumber,255,L"ScoreNum%d",i);
		img = m_imgset-> getImage(bufNumber);
		m_NumberImages.push_back(img);

		swprintf(bufNumber,255,L"StarNum%d",i);
		img = m_imgset->getImage(bufNumber);
		m_StarNumberImages.push_back(img);

		swprintf(bufNumber,255,L"SentenceNum%d",i);
		img = m_imgset->getImage(bufNumber);
		m_SentenceNumberImages.push_back(img);
	}
	m_ImgPoint = m_imgset->getImage(L"ScoreNumDot");
	m_ImgPoint2      = m_imgset->getImage(L"SentenceNumDot");

	if ( NULL != (img = m_imgset-> getImage(L"Good") )) 
		m_GradeImages.push_back(img);
	if ( NULL != (img = m_imgset-> getImage(L"Cool") )) 
		m_GradeImages.push_back(img);
	if ( NULL != (img = m_imgset-> getImage(L"Great") )) 
		m_GradeImages.push_back(img);
	if ( NULL != (img = m_imgset-> getImage(L"Perfect") )) 
		m_GradeImages.push_back(img);

	if (m_GradeImages.size()<4 || !m_ImgPoint||!m_ImgPoint2)
	{
		SceneMgr::s_error_msg = L"grade point image miss";
		m_bSentenceRender_All_Init = false;
	}

	if (m_bFirst)
	{
		m_ImgMic		= m_imgset->getImage(L"ScoreIconA");
		m_ImgRingBg = m_imgset->getImage(L"SongEffect1A");
		m_ImgWaveHLine = m_imgset->getImage(L"SongEffect2A");
	}
	else
	{
		m_ImgMic		= m_imgset->getImage(L"ScoreIconB");
		m_ImgRingBg = m_imgset->getImage(L"SongEffect1B");
		m_ImgWaveHLine = m_imgset->getImage(L"SongEffect2B");
	}
	m_ImgScoreBg	= m_imgset->getImage(L"ScoreBG");
	m_ImgStar			= m_imgset->getImage(L"StarIcon");
	m_ImgFen			= m_imgset->getImage(L"SentenceNumFen");
	m_ImgPlus			= m_imgset->getImage(L"StarNumPlus");
	
	if(!m_ImgMic||!m_ImgScoreBg||!m_ImgStar||!m_ImgRingBg||!m_ImgFen||!m_ImgWaveHLine||!m_ImgPlus)
	{
		SceneMgr::s_error_msg = L"mic scorebg ringbg star wavehline plus miss";
		m_bSentenceRender_All_Init = false;
	}
	m_veccurnumberBuf.clear();
	m_vecdestnumberBuf.clear();
	m_vecstardestnumberBuf.clear();

	CFloridMgr* floridMgr = cls_gl->m_pFloridMgr;
	if (!floridMgr)
	{
		SceneMgr::s_error_msg = L"floridMgr is null";
		m_bSentenceRender_All_Init = false;
	}

	if (m_bFirst)
	{
		m_starFlorid = floridMgr->getFlorid("starflorid");
		m_FloridGood = floridMgr->getFlorid("good");
		m_FloridNice1 = floridMgr->getFlorid("nice1");
		m_FloridNice2 = floridMgr->getFlorid("nice2");
		m_FloridCool1 = floridMgr->getFlorid("cool1");
		m_FloridCool2 = floridMgr->getFlorid("cool2");
		m_FloridPerfect1 = floridMgr->getFlorid("perfect1");
		m_FloridPerfect2 = floridMgr->getFlorid("perfect2");
		m_FloridPerfect3 = floridMgr->getFlorid("perfect3");
		m_FloridGodlike1 = floridMgr->getFlorid("godlike1");
		m_FloridGodlike2 = floridMgr->getFlorid("godlike2");
		m_FloridGodlike3 = floridMgr->getFlorid("godlike3");
		m_FloridGodlike4 = floridMgr->getFlorid("godlike4");
		m_FloridGodlike5 = floridMgr->getFlorid("godlike5");
		m_FloridUnbelivable1 = floridMgr->getFlorid("unbelivable1");
		m_FloridUnbelivable2 = floridMgr->getFlorid("unbelivable2");
		m_FloridUnbelivable3 = floridMgr->getFlorid("unbelivable3");
		m_FloridUnbelivable4 = floridMgr->getFlorid("unbelivable4");
		m_FloridUnbelivable5 = floridMgr->getFlorid("unbelivable5");
		m_FloridWaveRight = floridMgr->getFlorid("waveflorid1");
		m_FloridWaveLeft = floridMgr->getFlorid("waveflorid2");
		m_FloridStarMove = floridMgr->getFlorid("starmove");
	}
	else
	{
		m_starFlorid = floridMgr->getFlorid("sstarflorid");
		m_FloridGood = floridMgr->getFlorid("sgood");
		m_FloridNice1 = floridMgr->getFlorid("snice1");
		m_FloridNice2 = floridMgr->getFlorid("snice2");
		m_FloridCool1 = floridMgr->getFlorid("scool1");
		m_FloridCool2 = floridMgr->getFlorid("scool2");
		m_FloridPerfect1 = floridMgr->getFlorid("sperfect1");
		m_FloridPerfect2 = floridMgr->getFlorid("sperfect2");
		m_FloridPerfect3 = floridMgr->getFlorid("sperfect3");
		m_FloridGodlike1 = floridMgr->getFlorid("sgodlike1");
		m_FloridGodlike2 = floridMgr->getFlorid("sgodlike2");
		m_FloridGodlike3 = floridMgr->getFlorid("sgodlike3");
		m_FloridGodlike4 = floridMgr->getFlorid("sgodlike4");
		m_FloridGodlike5 = floridMgr->getFlorid("sgodlike5");
		m_FloridUnbelivable1 = floridMgr->getFlorid("sunbelivable1");
		m_FloridUnbelivable2 = floridMgr->getFlorid("sunbelivable2");
		m_FloridUnbelivable3 = floridMgr->getFlorid("sunbelivable3");
		m_FloridUnbelivable4 = floridMgr->getFlorid("sunbelivable4");
		m_FloridUnbelivable5 = floridMgr->getFlorid("sunbelivable5");
		m_FloridWaveRight = floridMgr->getFlorid("swaveflorid1");
		m_FloridWaveLeft = floridMgr->getFlorid("swaveflorid2");
		m_FloridStarMove = floridMgr->getFlorid("sstarmove");
	}

	if (!m_starFlorid||!m_FloridGood||!m_FloridNice1||!m_FloridNice2||!m_FloridCool1||!m_FloridCool2||!m_FloridPerfect1||
		!m_FloridPerfect2||!m_FloridPerfect3||!m_FloridGodlike1||!m_FloridGodlike2||!m_FloridGodlike3||!m_FloridGodlike4||!m_FloridGodlike5||
		!m_FloridUnbelivable1||!m_FloridUnbelivable2||!m_FloridUnbelivable3||!m_FloridUnbelivable4||!m_FloridUnbelivable5||
		!m_FloridWaveRight||!m_FloridWaveLeft||!m_FloridStarMove)
	{
		SceneMgr::s_error_msg = L"grade florid miss";
		m_bSentenceRender_All_Init = false;
	}

	float frame_right_width_2 = m_imgset->getImage(L"StarLineBgAR")->getWidth()/2.0f;
	float frame_right_height_2 = m_imgset->getImage(L"StarLineBgAR")->getHeight()/2.0f;
	float florid_pos_x = 0.0f;
	float florid_pos_y = 0.0f;
	if (m_bFirst)
	{
		florid_pos_x = m_windowsize.width - frame_right_width_2;
		florid_pos_y = STAFF_POS_HEIGHT - frame_right_height_2;
	}
	else
	{
		florid_pos_x = m_windowsize.width - frame_right_width_2;
		florid_pos_y = STAFF_POS_HEIGHT + frame_right_height_2+120;
	}

	m_FloridGood->setPosition(florid_pos_x,florid_pos_y);
	m_FloridNice1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridNice2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridCool1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridCool2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridPerfect1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridPerfect2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridPerfect3->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike3->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike4->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike5->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable3->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable4->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable5->setPosition(florid_pos_x,florid_pos_y);
	m_FloridStarMove->stopPS();

	float wave_florid_pos_y = STAFF_POS_HEIGHT+65;
	m_FloridWaveLeft->setRectXY(WAVEWND_LEFT_PIXELS-10.0f,STAFF_POS_HEIGHT);
	m_FloridWaveLeft->setPosition((float)m_windowsize.width,wave_florid_pos_y);
	m_FloridWaveRight->setRectXY(WAVEWND_LEFT_PIXELS+10.0f,STAFF_POS_HEIGHT);
	m_FloridWaveRight->setPosition(0,wave_florid_pos_y);

	m_cur_songName = L"歌曲:"+cur_songName;
	m_next_songName = L"歌星:"+cur_singerName;

	CAnimaNumber::FillZeroNumber(m_vectCurScore,m_nScoreDigit);

	bInit = m_bSentenceRender_All_Init;
}

SentenceRender::~SentenceRender()
{
	m_NumberImages.clear();
	m_GradeImages.clear();
}

void SentenceRender::setStaffPosHeight(const float fHeight,const eShowWave eType)
{
	m_eShowWaveType = eType;
	STAFF_POS_HEIGHT = fHeight;

	float florid_pos_x = m_windowsize.width - 143.0f;
	float florid_pos_y = STAFF_POS_HEIGHT - 20.0f;

	m_FloridGood->setPosition(florid_pos_x,florid_pos_y);
	m_FloridNice1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridNice2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridCool1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridCool2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridPerfect1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridPerfect2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridPerfect3->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike3->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike4->setPosition(florid_pos_x,florid_pos_y);
	m_FloridGodlike5->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable1->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable2->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable3->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable4->setPosition(florid_pos_x,florid_pos_y);
	m_FloridUnbelivable5->setPosition(florid_pos_x,florid_pos_y);
	m_FloridStarMove->stopPS();

	float wave_florid_pos_y = STAFF_POS_HEIGHT+65;
	m_FloridWaveLeft->setRectXY(WAVEWND_LEFT_PIXELS-10.0f,STAFF_POS_HEIGHT);
	m_FloridWaveLeft->setPosition((float)m_windowsize.width,wave_florid_pos_y);
	m_FloridWaveRight->setRectXY(WAVEWND_LEFT_PIXELS+10.0f,STAFF_POS_HEIGHT);
	m_FloridWaveRight->setPosition(0.0f,wave_florid_pos_y);
}

void SentenceRender::setFont(CGLFont* pFont)
{
	m_pFont = pFont;
}

void SentenceRender::setStarStartPoint(const float x,const float y)
{
	static float star_width_2 = m_ImgStar->getWidth()/2.0f;
	static float star_height_2 = m_ImgStar->getHeight()/2.0f;
	static float s_fix_x = 170;
	float s_fix_y = 0;
	if (m_bFirst)
	{
		s_fix_y = 	STAFF_POS_HEIGHT-20;
	}
	else
	{
		s_fix_y = STAFF_POS_HEIGHT+150/*+130+20*/;
	}

	m_star_start_x=x;
	m_star_start_y=y;
	
	float x_x = m_star_start_x-s_fix_x;
	//float y2 = s_fix_y-m_star_start_y;
	m_parabola_b = m_star_start_y - s_fix_y;
	//static float x1 = (m_star_start_x-50-s_fix_x)/2.0f;
	static float x1 = s_fix_x - m_star_start_x;
	m_excursion_x = m_star_start_x;
	m_excursion_y = s_fix_y;
	m_parabola_a = -m_parabola_b/x1/x1;
	m_per_ms_move_x = x_x*1.0f/TIME_TO_STAR_MOVE;
	if (m_nMyCombo != m_nPreCombo/*m_nScore != m_nPreScore*/)
	{
		m_FloridStarMove->setPosition(m_star_start_x,m_star_start_y);
		m_FloridStarMove->startPS();
	}
}
//-----------------------------------------------------------------------------
void SentenceRender::showSentenceLevel(GLSentenceLevel sentence_level)
{
	setParticleEnable(sentence_level);//华丽的粒子特效
	m_bsentence_level_start = true;
	m_bflorid_start = false;
	if (m_nParticleLevel >= 5)
	{
		m_nMyCombo += 1;
		m_b_level_ring = true;
	}
	else if (m_nParticleLevel >= 3)
	{
		m_nMyCombo += 1;
		m_b_level_ring = true;
	}
	else if(m_nParticleLevel >= 1)
	{
		m_nMyCombo ++;
	}

	if (m_nParticleLevel>GLSENTENCEGRADELEVEL_NONE)
	{
		m_bStarFlorid = true;
		m_bStarFloridSec = true;
		m_bStarFloridThr = true;
	}
}

//-----------------------------------------------------------------------------
void SentenceRender::showSentenceLevelScore(int nPitchScore,int nLyricScore)
{
	//nPitchScore =nLyricScore = 990;//glp debug 查看句评分等级特效
	//计算音高得分和歌词得分的综合
	m_nScore = (int)(nPitchScore*cls_gl->m_pSceneMgr->CONFIG_PITCHSCOREPERCENT
				+nLyricScore*cls_gl->m_pSceneMgr->CONFIG_LYRICSCOREPERCENT);
    m_nScore = m_nScore < 0 ? 0 : ( m_nScore > 1000 ? 1000 : m_nScore );

	GLSentenceLevel level;
	GRADE_NEWLEVEL(m_nScore,level);
	showSentenceLevel(level);

	if (m_nScore>10)
		m_bplus_number = true;
	m_nScoreAveCur = (int)(((float)m_nScoreSum)/m_nSentence);
	m_nScoreSum += m_nScore;//
	m_nScoreAveDest = (int)(((float)m_nScoreSum)/m_nSentence);

	//初始化准备工作
	int nDigit;
	nDigit = CAnimaNumber::CaculateDigit(m_nScoreAveDest);
	m_nScoreDigit = nDigit>m_nScoreDigit?nDigit:m_nScoreDigit;
	CAnimaNumber::DealWithNumber(m_nScoreAveCur,m_vectCurScore);
	CAnimaNumber::FillZeroNumber(m_vectCurScore,m_nScoreDigit);
	CAnimaNumber::DealWithNumber(m_nScoreAveDest,m_vectDestScore);

	m_nroll_pernumber_tm = (int)(TIME_TO_LIVE_COMMON*1.0f/(m_nScoreAveDest-m_nScoreAveCur));

	m_vecsrcnumberBuf = m_vecdestnumberBuf;
	if (m_vecsrcnumberBuf.empty())
		m_vecsrcnumberBuf.push_back(0); 

	m_nscore_last_tm = 0;

	if(m_nScoreAveCur != m_nScoreAveDest)
	{
		m_b_start_score_tm = true;
		m_bNumberResize = true;
	}

	m_bLyricScoreNotify = cls_gl->m_pSceneMgr->CONFIG_LYRICSCOREPERCENT>0.0f?
		(nLyricScore < cls_gl->m_pSceneMgr->CONFIG_LYRICSCORE_NOTIFY) : false;
	if(m_bLyricScoreNotify)
		m_gAlphaLyricScoreNotify.reset();
}

//-----------------------------------------------------------------------------
void SentenceRender::resetSentenceRender()
{
	m_nlast_songinfo_ms=0;
	m_nPreScore = 0;
	m_nScore = 0;
	m_nScoreAveDest = 0;
	m_nScoreSum = 0;
	m_nMyCombo = 0;
	m_nPreCombo = 0;
	m_nParticleLevel = GLSENTENCEGRADELEVEL_NONE;
}

void SentenceRender::setSentenceNum(uint nSentence)
{
	if(nSentence)
	{
		m_nSentence=nSentence;
	}
	else
	{
		m_nSentence = 1;
	}
}

void SentenceRender::_drawScore(float& pos_x,float& pos_y)
{
	static int star_width = m_ImgMic->getWidth();
	static int number_width = m_NumberImages[0]->getWidth();
	static int number_height = m_NumberImages[0]->getHeight();

	if(m_nScoreAveCur != m_nScoreAveNext)
	{//变化中
		VECTORINT vectNext;
		CAnimaNumber::DealWithNumber(m_nScoreAveNext,vectNext);
		CAnimaNumber::FillZeroNumber(vectNext,m_nScoreDigit);
		assert(m_vectCurScore.size() == vectNext.size());

		int nInteger = (int)m_vectCurScore.size() - 1;
		for(int i=0;i<nInteger;i++)
		{
			const int& nCur = m_vectCurScore[i];
			const int& nNext = vectNext[i];

			if(nCur != nNext)
			{
				m_NumberImages[nCur]->draw(GPoint(pos_x,pos_y));

				GColor cur_color = GColor(1.0f,1.0f,1.0f,m_fScorePercent);
				float size_num = (1-m_fScorePercent)*2+1;
				int cur_width = int(number_width*size_num);
				int cur_height = int(number_height*size_num);
				float overlying_x = pos_x+number_width/2.0f-cur_width/2.0f;
				float overlyring_y = pos_y+number_height/2.0f-cur_height/2.0f;
				m_NumberImages[nNext]->draw(GPoint(overlying_x,overlyring_y),GSize(cur_width,cur_height),NULL,true,cur_color);
			}
			else
			{
				m_NumberImages[nCur]->draw(GPoint(pos_x,pos_y));
			}

			pos_x += number_width;//每描述一位，x坐标向右移动	
		}

		m_ImgPoint->draw(GPoint(pos_x,pos_y));//描绘小数点
		pos_x += m_ImgPoint->m_rect.size.width;

		{
			const int& nCur = m_vectCurScore.back();
			const int& nNext = vectNext.back();
			if(nCur != nNext)
			{
				m_NumberImages[nCur]->draw(GPoint(pos_x,pos_y));

				GColor cur_color = GColor(1.0f,1.0f,1.0f,m_fScorePercent);
				float size_num = (1-m_fScorePercent)*2+1;
				int cur_width = int(number_width*size_num);
				int cur_height = int(number_height*size_num);
				float overlying_x = pos_x+number_width/2.0f-cur_width/2.0f;
				float overlyring_y = pos_y+number_height/2.0f-cur_height/2.0f;
				m_NumberImages[nNext]->draw(GPoint(overlying_x,overlyring_y),GSize(cur_width,cur_height),NULL,true,cur_color);
			}
			else
			{
				m_NumberImages[nCur]->draw(GPoint(pos_x,pos_y) , NULL);
			}
			pos_x += number_width;//每描述一位，x坐标向右移动
		}
	}
	else
	{
		int nInteger = (int)m_vectCurScore.size() - 1;
		for(int i=0;i<nInteger;i++)
		{
			const int& nCur = m_vectCurScore[i];
			m_NumberImages[nCur]->draw(GPoint(pos_x,pos_y) , NULL);
			pos_x += number_width;//每描述一位，x坐标向右移动	
		}
		m_ImgPoint->draw(GPoint(pos_x,pos_y),NULL);//描绘小数点
		pos_x += m_ImgPoint->m_rect.size.width;

		m_NumberImages[m_vectCurScore.back()]->draw(GPoint(pos_x,pos_y) , NULL);
		pos_x += number_width;//每描述一位，x坐标向右移动	
	}
}

void SentenceRender::drawMicScore()
{
	//话筒
	float pos_x = 0;//1280.0f;//坐标有待考量。320.0f

	if (m_bFirst)
	{
		if (m_eShowWaveType == SHOW_ONE)
			pos_x = m_windowsize.width/2.0f-90;//1280.0f;//坐标有待考量。320.0f
		else
			pos_x = m_windowsize.width/2.0f-180;
	}
	else
		pos_x = m_windowsize.width/2.0f+10;
	
	float pos_y = 0;//顶格
	m_ImgMic->draw(GPoint(pos_x,pos_y),NULL);//画麦克风
	
	if (!m_bShowScore)
		return ;

	pos_x += m_ImgMic->getWidth()-10;
	pos_y = 12;//调整坐标，让数字保持与麦克风高度相同
	float start_pos_x = pos_x;

	_drawScore(pos_x,pos_y);
	//////////////////////////////////////////////////////////////////////////
	pos_x = (start_pos_x + pos_x - m_ImgScoreBg->getWidth()+6)/2.0f;//+6让底图往右边移动一点
	pos_y = m_NumberImages[0]->getHeight()+5.0f;
	m_ImgScoreBg->draw(GPoint(pos_x,pos_y),NULL);//分数下面的图片


	if(!m_bLyricScoreNotify || !m_ImgLyricScoreNotify)
		return ;
	static int s_sn_w = m_ImgLyricScoreNotify->getWidth()*2;
	static int s_sn_h = m_ImgLyricScoreNotify->getHeight()*2;
	static float s_lsn_pos_x = (cls_gl->m_winSize.width - s_sn_w)/2.0f;
	static float s_lsn_pos_y = 50.0f;
	float fAlpha = m_gAlphaLyricScoreNotify.getAlpha();//
	if(fAlpha == 0.0f)
		return ;
	m_ImgLyricScoreNotify->draw(GPoint(s_lsn_pos_x,s_lsn_pos_y),GSize(s_sn_w,s_sn_h),NULL,true,GColor(1.0f,1.0f,1.0f,fAlpha));
}

void SentenceRender::drawRingBG()
{
	if (m_b_level_ring)
	{
		static CGRect s_dest_rect = CGRectMake(0.0f,STAFF_POS_HEIGHT,GL3D_Z_BASE,m_windowsize.width,(int)STAFF_HEIGHT);
		static int s_w = m_ImgRingBg->getWidth();
		static int s_h = m_ImgRingBg->getHeight();

		float x_scale = m_nlevel_last_tm*1.0f/TIME_TO_LIVE_LEVEL_RING;
		float y_scale = m_nlevel_last_tm*2.0f/TIME_TO_LIVE_LEVEL_RING;
		int tmp_w = (int)(s_w*(1-x_scale)+20);
		int tmp_h = (int)(s_h*(1-y_scale));
		CGRect tmp_src_pclip;
		tmp_src_pclip.origin = CGPointMake((s_w-tmp_w)/2.0f,(s_h-tmp_h)/2.0f,0.0f);
		tmp_src_pclip.size = CGSizeMake(tmp_w,tmp_h);

		m_ImgRingBg->draw(s_dest_rect,&tmp_src_pclip,WHITE_RECT,false);
	}
}

void SentenceRender::drawScoreLevel()
{
	static int frame_right_width = m_imgset->getImage(L"StarLineBgAR")->getWidth();
	static int frame_right_height = m_imgset->getImage(L"StarLineBgAR")->getHeight();

	if (m_bsentence_level_start)
	{
		int grade_level = (int)m_nParticleLevel;
		if ( grade_level >= GLSENTENCEGRADELEVEL_GOOD && grade_level <= GLSENTENCEGRADELEVEL_PERFECT )
		{
			RenderEImage * img = m_GradeImages[grade_level-1];
			if ( img )
			{
				static float s_middle_x = m_windowsize.width - frame_right_width/2.0f;
				float s_middle_y = 0;
				if (m_bFirst)
					s_middle_y = STAFF_POS_HEIGHT- frame_right_height/2.0f+10;
				else
					s_middle_y = STAFF_POS_HEIGHT+130+ frame_right_height/2.0f;

				int level_img_width = img->getWidth();
				int level_img_height = img->getHeight();

				if (m_nlevel_icon_ms>TIME_TO_LEVEL_GAMMA)
				{
					//渐隐
					float cur_shedule = (1.0f-(m_nlevel_icon_ms-TIME_TO_LEVEL_GAMMA)*1.0f/TIME_TO_LEVEL_DISAPPEAR);
					GColor cur_color(1.0f,1.0f,1.0f,cur_shedule);
					float pos_x = s_middle_x-level_img_width/2.0f;
					float pos_y = s_middle_y-level_img_height/2.0f;
					img->draw(GPoint(pos_x,pos_y),NULL,true,cur_color);
				}
				else if (m_nlevel_icon_ms>TIME_TO_LEVEL_KEEPSTART)
				{
					//keep
					float pos_x = s_middle_x-level_img_width/2.0f;
					float pos_y = s_middle_y-level_img_height/2.0f;
					img->draw(GPoint(pos_x,pos_y));
				}
				else if (m_nlevel_icon_ms>TIME_TO_LEVEL_PARTICLE+TIME_TO_LEVEL_BALL)
				{
					//变小
					float cur_shedule = (1-(m_nlevel_icon_ms-TIME_TO_LEVEL_BALL-TIME_TO_LEVEL_PARTICLE)/TIME_TO_LEVEL_BALL)*0.02f+1;
					int cur_width = (int)(level_img_width*cur_shedule);
					int cur_height = (int)(level_img_height*cur_shedule);
					img->draw(CGRect(s_middle_x-cur_width/2.0f,s_middle_y-cur_height/2.0f,cur_width,cur_height),NULL,WHITE_RECT);
				}
				else if (m_nlevel_icon_ms>TIME_TO_LEVEL_PARTICLE)
				{
					//变大
					float cur_shedule = (m_nlevel_icon_ms-TIME_TO_LEVEL_PARTICLE)*0.02f/TIME_TO_LEVEL_BALL+1;
					int cur_width = (int)(level_img_width*cur_shedule);
					int cur_height = (int)(level_img_height*cur_shedule);
					img->draw(CGRect(s_middle_x-cur_width/2.0f,s_middle_y-cur_height/2.0f,cur_width,cur_height),NULL,WHITE_RECT);
				}
				else 
				{
					//变小
					float cur_shedule_pre1 = m_nlevel_icon_ms*1.0f/TIME_TO_LEVEL_SMALLING;
					if (cur_shedule_pre1 > 1)//如果已经减小到最小
						cur_shedule_pre1 = 1;
					float cur_shedule1 = (1-cur_shedule_pre1)*0.5f+1;
					int cur_width = (int) (level_img_width*cur_shedule1);
					int cur_height = (int) (level_img_height*cur_shedule1);
					GColor cur_color1 (1.0f,1.0f,1.0f,cur_shedule_pre1);
					img->draw(CGRect(s_middle_x-cur_width/2.0f,s_middle_y-cur_height/2.0f,cur_width,cur_height),NULL,cur_color1);
					if (m_bLevel_S)
					{
						float cur_shedule_pre2 = (m_nlevel_icon_ms-TIME_TO_LEVEL_SEC)*1.0f/TIME_TO_LEVEL_SMALLING;
						float cur_shedule2 = (1-cur_shedule_pre2)*0.5f+1;
						cur_width = (int)(level_img_width*cur_shedule2);
						cur_height = (int)(level_img_height*cur_shedule2);
						GColor cur_color2 (1.0f,1.0f,1.0f,cur_shedule_pre2);
						img->draw(CGRect(s_middle_x-cur_width/2.0f,s_middle_y-cur_height/2.0f,cur_width,cur_height),NULL,cur_color2);
					}
				}
				//圆环
				switch (m_nParticleLevel)
				{
				case GLSENTENCEGRADELEVEL_GOOD:
				case GLSENTENCEGRADELEVEL_COOL:
					{
						break;
					}
				case GLSENTENCEGRADELEVEL_GREAT:
				case GLSENTENCEGRADELEVEL_PERFECT:
					{
						drawRingBG();
						break;
					}
				}
			}
		}
	}
}

void SentenceRender::moveCurve(float &star_start_x,float &star_start_y,/*float s_fix_x,float s_fix_y,*/ulong delta_ms)
{
}

void SentenceRender::drawStarValue()
{
	//画星星图片
	static float star_width = (float)m_ImgStar->getWidth();
	static float star_height = (float)m_ImgStar->getHeight();
	static float star_width_2 = m_ImgStar->getWidth()/2.0f;
	static float star_height_2 = m_ImgStar->getHeight()/2.0f;
	static float s_fix_x = 80.0f;//框架上圆环的中心点
	/*static float s_fix_y = STAFF_POS_HEIGHT-20;*/
	float s_fix_y = 0.0f;
	if (m_bFirst)
		s_fix_y = STAFF_POS_HEIGHT-20;
	else
		s_fix_y = STAFF_POS_HEIGHT+150/*+130+20*/;

	static float s_x = s_fix_x-star_width_2;//星星的图片起始位置
	float s_y = s_fix_y-star_height_2;//20是框架图片的高度值


	if (m_bStarFlorid)//有特效
	{
		if (m_bStarFloridSec)
		{
			if (m_bStarFloridThr)
			{
				float percent = m_nstar_last_tm*1.0f/TIME_TO_STAR_BIG;
				int cur_width = (int)(star_width*percent);
				int cur_height = (int)(star_height*percent);
				m_ImgStar->draw(GPoint(m_star_start_x-cur_width/2.0f,m_star_start_y-cur_height/2.0f),GSize(cur_width,cur_height));
			}
			else
				m_ImgStar->draw(GPoint(m_star_start_x-star_width_2,m_star_start_y-star_height_2));
		}
		else
		{
			float fscale = (TIME_TO_END_SMALLINGSTAR-m_nstar_last_tm)*1.0f/TIME_TO_LIVE_STAR*10;//如果唱准，则放大10倍星星图片
			float x = s_x-star_width_2*fscale;
			float y = s_y-star_height_2*fscale;
			int width = (int)(m_ImgStar->getWidth()*(1+fscale));
			int height = (int)(m_ImgStar->getHeight()*(1+fscale));
			m_ImgStar->draw(GPoint(x,y),GSize(width,height),NULL);
		}
	}
	else
	{
		float f_shedule_star = 0.5f*m_nstar_loop_ms/TIME_TO_STAR_LOOP_TM;
		f_shedule_star = m_bStar_Big?f_shedule_star:(0.5f-f_shedule_star);
		f_shedule_star = f_shedule_star<0.0f?0.0f:f_shedule_star>1.0f?1.0f:f_shedule_star;
		float x = s_x-star_width_2*f_shedule_star;
		float y = s_y-star_height_2*f_shedule_star;
		int width = (int)(m_ImgStar->getWidth()*(1+f_shedule_star));
		int height = (int)(m_ImgStar->getHeight()*(1+f_shedule_star));
		m_ImgStar->draw(GPoint(x,y),GSize(width,height));//放大的星星
		m_ImgStar->draw(GPoint(s_x,s_y));//普通星星
	}
	
	float pos_x = s_x + m_ImgStar->getWidth();
	float pos_y = 0;
	if (m_bFirst)
		pos_y = STAFF_POS_HEIGHT-20-m_SentenceNumberImages[0]->getHeight()/2.0f;
	else
		pos_y = STAFF_POS_HEIGHT+150-m_SentenceNumberImages[0]->getHeight()/2.0f;

	RenderEImage* img = NULL;
	if (m_vecstardestnumberBuf.empty())
	{
		return ;
	}
	int sum = (int)m_vecstardestnumberBuf.size()-1;
	if (sum)
	{
		for(int i=0;i<sum;i++)
		{
			//描绘在最左边，从高位开始描绘，
			img = /*m_StarNumberImages*/m_SentenceNumberImages[m_vecstardestnumberBuf[i]];
			img-> draw(GPoint(pos_x,pos_y) , NULL);
			pos_x += img->m_rect.size.width;//每描述一位，x坐标向右移动
		}
	}
	else
	{
		img = m_SentenceNumberImages[0];
		img-> draw(GPoint(pos_x,pos_y) , NULL);
		pos_x += img->m_rect.size.width;//每描述一位，x坐标向右移动
	}
	//dai
	m_ImgPoint2->draw(GPoint(pos_x,pos_y),NULL);//小数点
	pos_x += m_ImgPoint2->m_rect.size.width;

	img = m_SentenceNumberImages[m_vecstardestnumberBuf[sum]];//描绘0
	img->draw(GPoint(pos_x,pos_y),NULL);
	pos_x += img->m_rect.size.width;

	m_ImgFen->draw(GPoint(pos_x,pos_y));//分
	//如果是合唱歌曲，标明当前的歌曲的原演唱者
// 	if(m_pFont)
// 		m_pFont->drawText(m_cur_singerPart,GPoint(CONFIG_POSX_SINGERTEXT,pos_y-8.0f,GL3D_Z_NOTIFY),NULL,WHITE_RECT,0.7f);
}
extern COS_Timer g_Timer;//测试cpu占用时间
//-----------------------------------------------------------------------------
void SentenceRender::updateRingBG(ulong delta_tm)
{
	if (m_b_level_ring)
	{
		if (!m_b_ring_florid)
		{
			m_FloridWaveRight->startPS();
			m_FloridWaveLeft->startPS();
			m_b_ring_florid = true;
		}

		m_nlevel_last_tm += delta_tm;
		if (m_nlevel_last_tm>TIME_TO_LIVE_LEVEL_RING)
		{
			m_b_level_ring = false;
			m_b_ring_florid = false;
			m_nlevel_last_tm = 0;//可能要改变地方，以控制其他的特效时间
		}
	}
}
void SentenceRender::updateMicScore(ulong delta_ms)
{
	if (m_b_start_score_tm)
	{
		if (m_nscore_last_tm <= TIME_TO_LIVE_COMMON)
		{
			m_nscore_last_tm += delta_ms;

			CAnimaNumber::SmoothNumber(m_nScoreAveDest,m_nScoreAveCur,m_nScoreAveNext,m_nscore_per_last_tm
				,m_nroll_pernumber_tm,delta_ms,m_fScorePercent);

			CAnimaNumber::DealWithNumber(m_nScoreAveCur,m_vectCurScore);
			CAnimaNumber::FillZeroNumber(m_vectCurScore,m_nScoreDigit);
		}
		else
		{
			m_nScoreAveCur = m_nScoreAveDest;
			m_nScoreAveNext = m_nScoreAveDest;
			m_b_start_score_tm = false;
		}
	}

	m_gAlphaLyricScoreNotify.update(delta_ms);
}


void SentenceRender::updateLevelScore(ulong delta_ms)
{
	if (m_bsentence_level_start)
	{
		int grade_level = ((int)m_nParticleLevel);
		if ( grade_level >= GLSENTENCEGRADELEVEL_GOOD && grade_level <= GLSENTENCEGRADELEVEL_PERFECT )
		{
			{
				if (m_nlevel_icon_ms>TIME_TO_LEVEL_PARTICLE && m_nlevel_icon_ms<=TIME_TO_LEVEL_PARTICLE+TIME_TO_LEVEL_BALL)
				{
					if (!m_bflorid_start)
					{
						switch (m_nParticleLevel)//2个粒子系统预留,华丽的特效
						{
						case GLSENTENCEGRADELEVEL_GOOD:
							{
								m_FloridGood->startPS();
								break;
							}
						case GLSENTENCEGRADELEVEL_COOL:
							{
								m_FloridCool1->startPS();
								m_FloridCool2->startPS();
								//m_FloridPerfect1->startPS();
								//m_FloridPerfect2->startPS();
								//m_FloridPerfect3->startPS();
								break;
							}
						case GLSENTENCEGRADELEVEL_GREAT:
							{
								m_FloridGodlike1->startPS();
								m_FloridGodlike2->startPS();
								m_FloridGodlike3->startPS();
								m_FloridGodlike4->startPS();
								m_FloridGodlike5->startPS();
								break;
							}
						case GLSENTENCEGRADELEVEL_PERFECT:
							{
								m_FloridUnbelivable1->startPS();
								m_FloridUnbelivable2->startPS();
								m_FloridUnbelivable3->startPS();
								m_FloridUnbelivable4->startPS();
								m_FloridUnbelivable5->startPS();
								break;
							}
						}
						m_bflorid_start = true;
					}
				}
				else 
				{
					if (m_nlevel_icon_ms>TIME_TO_LEVEL_SEC)
					{
						m_bLevel_S = true;
					}
				}
				//圆环
				switch (m_nParticleLevel)
				{
				case GLSENTENCEGRADELEVEL_GOOD:
				case GLSENTENCEGRADELEVEL_COOL:
					{
						break;
					}
				case GLSENTENCEGRADELEVEL_GREAT:
				case GLSENTENCEGRADELEVEL_PERFECT:
					{
						updateRingBG(delta_ms);
						break;
					}
				}
			}
		}

		m_nlevel_icon_ms += delta_ms;
		if (m_nlevel_icon_ms > TIME_TO_LEVEL_GAMMA+TIME_TO_LEVEL_DISAPPEAR)
		{
			m_bsentence_level_start = false;
			m_bLevel_S = false;
			m_nlevel_icon_ms = 0;
		}
	}
}

void SentenceRender::update(const ulong delta_ms)
{
	static float star_width = (float)m_ImgStar->getWidth();
	static float star_height = (float)m_ImgStar->getHeight();
	static float star_width_2 = m_ImgStar->getWidth()/2.0f;
	static float star_height_2 = m_ImgStar->getHeight()/2.0f;
	static float s_fix_x = 80.0f;//框架上圆环的中心点
	float s_fix_y = 0.0f;
	if (m_bFirst)
	{
		s_fix_y = STAFF_POS_HEIGHT-20;
	}
	else
	{
		s_fix_y = STAFF_POS_HEIGHT+150;
	}

	static float s_x = s_fix_x-star_width_2;//星星的图片起始位置
	float s_y = s_fix_y-star_height_2;//20是框架图片的高度值


	if (m_bStarFlorid)//有特效
	{
		if (m_nstar_last_tm>=TIME_TO_END_SMALLINGSTAR)
		{
			m_bStarFlorid = false;
			m_nstar_last_tm = 0;
			m_starFlorid->setPosition(s_fix_x,s_fix_y);
			m_starFlorid->startPS();
		}
		else if (m_nstar_last_tm>=TIME_TO_STARSMALLING_START)
		{
			m_FloridStarMove->stopPS();
			m_bStarFloridSec = false;
			m_nPreCombo = m_nMyCombo;
			m_nPreScore = m_nScore;
		}
		else if(m_nstar_last_tm>TIME_TO_STAR_BIG)
		{
			m_bStarFloridThr = false;
		}

		if (m_bStarFloridSec)
		{
			if (!m_bStarFloridThr)
			{
				static float star_width_2 = m_ImgStar->getWidth()/2.0f;
				static float star_height_2 = m_ImgStar->getHeight()/2.0f;

				m_FloridStarMove->moveTo(m_star_start_x,m_star_start_y,false);
				m_star_start_x -= delta_ms*m_per_ms_move_x;
				m_star_start_y = (m_parabola_a*(m_star_start_x-m_excursion_x)*(m_star_start_x-m_excursion_x)+m_parabola_b+m_excursion_y);
			}
		}

		m_nstar_last_tm += delta_ms;
	}
	else
	{
		m_nPreScore = m_nScore;
		m_nstar_loop_ms += delta_ms;
		if (m_nstar_loop_ms>=TIME_TO_STAR_LOOP_TM)
		{
			m_nstar_loop_ms = 0;
			m_bStar_Big = !m_bStar_Big;
		}
	}

	int score = m_nPreScore;
	CAnimaNumber::DealWithNumber(score,m_vecstardestnumberBuf);

	updateMicScore(delta_ms);
	updateLevelScore(delta_ms);
}
void SentenceRender::drawGraphBuffer()
{
	//渲染星星值
	drawStarValue();

	// 渲染话筒分数
	drawMicScore();

    // 渲染句评分等级 
	drawScoreLevel();
}

