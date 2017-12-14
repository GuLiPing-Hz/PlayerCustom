#include "stdafx.h"
#include "ScoreFrame.h"

extern volatile bool	g_bGLThread;
//评分框的中间x坐标
#define CFRAME_THINGS_CENTER 896.0f
#define CFRAME_THINGS_CENTERF 660.0f
#define CFRAME_THINGS_CENTERS 1056.0f
#define RANKINFO_OFFSET1 105.0f
#define RANKINFO_OFFSET2 112.0f

#define LEFT_SONGINFO_XF 150
#define LEFT_SONGINFO_XS 100
#define LEFT_SONGINFO_Y 400

#define TIME_TO_START_BLACK 11000
//一般的持续时间
#define TIME_TO_LIVE_COMMON 8000
//粒子轨迹的持续时间
#define TIME_TO_LEVE_PARTICLE 10000
//评分结算画面的持续时间
#define TIME_TO_END 13000
//光环由小变大的时间
#define TIME_TO_LOOP 1000
//等级星体图片由小变大的时间
#define TIME_TO_LEVEL_PIC 200
//等级字体的遮罩出现时间
#define TIME_TO_LEVEL_TEXT_APP 200
//等级字体的消失时间
#define TIME_TO_LEVEL_TEXT_DIS  50
//水平线的滑动时间
#define TIME_TO_HLINE 400
//歌曲信息的动画时间
#define TIME_TO_SONGINFO 300
//分数的由小变大的时间
#define TIME_TO_SCORE_BIG 1000
#define TIME_TO_SCORE_BIG_S 500
#define TIME_TO_SCORE_BIG_T 1000

CScoreFrame::CScoreFrame(bool &bInit,ISceneObject* pObj,bool bFirst)
:IComRender(pObj)
,m_nDestScore(0)
,m_nCurScore(0)
,m_nDestStarNum(0)
,m_nCurStarNum(0)
,m_pFontSingerName(NULL)
,m_pFontSongName(NULL)
,m_bstartStar(false)
,m_bBlackHole(false)
,m_bPreBlackHole(false)
,m_cur_level_last_ms(0)
,m_cur_bchangelevelpic(false)
,m_cur_bchangeleveltext(false)
,m_pre_level(0)
,m_cur_level(0)
,m_cur_levelmask_text_last_ms(0)
,m_bg_last_tm(0)
,m_imgsetEx(NULL)
,m_ImgSinger(NULL)
,m_bSingerImgsetInitBuffer(false)
,m_hline_last_tm(0)
,m_blineanim(true)
,m_btextanim(true)
,m_text_last_tm(0)
,m_bscorefram_allinit(false)
,m_SingleFloridPlanet(NULL)
,m_SingleFloridPlanet2(NULL)
,m_bScoreBiggingF(false)
,m_bScoreBiggingS(false)
,m_bScoreBiggingT(false)
//,m_rock_last_tm(0)
,m_bScoreFrame_New_Init(true)
,m_bend_scoreframe(false)
,m_bFirst(bFirst)
,m_ImgHorizontalLineShort(NULL)
,m_ImgHorizontalLine(NULL)
,m_ImgHLine(NULL)
,m_pFontRankinfo(NULL)
,m_ImgCongratulate(NULL)
,m_fScorePercent(0.0f)
,m_nNextScore(0)
,m_nScoreDigit(2)
,m_nStarDigit(1)
{
	bInit = m_bScoreFrame_New_Init;

	memset(&m_structPs,0,sizeof(m_structPs));

	assert(cls_gl != NULL);
	m_imgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
	if (!m_imgset)
	{
		SceneMgr::s_error_msg = L"no main texture";
		m_bScoreFrame_New_Init = false;
	}

	if (m_bFirst)
		m_ImgMic = m_imgset->getImage(L"ScoreIconA");
	else
		m_ImgMic = m_imgset->getImage(L"ScoreIconB");

	m_ImgScoreBg	= m_imgset->getImage(L"ScoreBG");
	m_ImgStar			= m_imgset->getImage(L"StarIcon");
	m_ImgPoint		= m_imgset->getImage(L"ScoreNumDot");

	m_ImgTotalScore		= m_imgset->getImage(L"TextScore");
	m_ImgStarNum			= m_imgset->getImage(L"TextCombo");
	m_ImgRing					= m_imgset->getImage(L"ComboRing");	  
	m_ImgBackground		= m_imgset->getImage(L"ScoreframeBG");
	m_ImgVerticalLine		= m_imgset->getImage(L"Scoreframe");
	m_ImgHorizontalLine	= m_imgset->getImage(L"HLine");
	m_ImgHorizontalLineShort = m_imgset->getImage(L"HLineS");
	m_ImgCongratulate = m_imgset->getImage(L"TextCongratulations");
	//m_ImgSingerBg			= m_imgset->getImage(L"FacePicMask");
	if (!m_ImgTotalScore||!m_ImgStarNum||!m_ImgRing||!m_ImgBackground||!m_ImgVerticalLine||!m_ImgHorizontalLine||!m_ImgHorizontalLineShort)
	{
		SceneMgr::s_error_msg = L"totalscore starnum ring background line horizontalline miss";
		m_bScoreFrame_New_Init = false;
	}

	RenderEImage* img = NULL;
	for ( int i = 0; i < 10; ++i )
	{
		wchar_t bufNumber[256] = {0};
		swprintf(bufNumber,255,L"ScoreNum%d",i);
		img = m_imgset-> getImage(bufNumber);
		if ( img )
		{
			m_NumberImages.push_back(img);
		}
		swprintf(bufNumber,255,L"StarNum%d",i);
		img = m_imgset->getImage(bufNumber);
		m_StarNumberImages.push_back(img);
	}
	for(int i=0;i<7;i++)
	{
		wchar_t bufNumber[256] = {0};
		swprintf(bufNumber,255,L"StarName%d",i);
		img = m_imgset-> getImage(bufNumber);
		m_ImgLevelText.push_back(img);

		swprintf(bufNumber,255,L"StarPic%d",i);
		img = m_imgset->getImage(bufNumber);
		m_ImgLevelPicture.push_back(img);
	}
	img = m_imgset->getImage(L"StarName7");
	m_ImgLevelText.push_back(img);

	m_ImgLevelTextMask = m_imgset->getImage(L"FlashMask");

	CFloridMgr* floridMgr = cls_gl->m_pFloridMgr;

	if (m_bFirst)
	{
		m_FloridLeft1 = floridMgr->getFlorid("scoreleftF");
		m_FloridLeft2 = floridMgr->getFlorid("scoreleftS");
		m_FloridRight1 = floridMgr->getFlorid("scorerightF");
		m_FloridRight2 = floridMgr->getFlorid("scorerightS");
		m_FloridStar = floridMgr->getFlorid("star");
		m_FloridPlanet = floridMgr->getFlorid("planetcommon");
		m_FloridPlanet6 = floridMgr->getFlorid("planet6");
		m_FloridPlanet7 = floridMgr->getFlorid("planet7");
		m_FloridExplode = floridMgr->getFlorid("explode");
		m_FloridFlashScreen = floridMgr->getFlorid("screenstar");
		m_FloridBigBang1 = floridMgr->getFlorid("bigbangF");
		m_FloridBigBang2 = floridMgr->getFlorid("bigbangS");
		m_FloridBigBang3 = floridMgr->getFlorid("bigbangT");

		m_SingleFloridPlanet = floridMgr->getFlorid("finalplanet");
		m_SingleFloridPlanet2 = floridMgr->getFlorid("finalplanet2");
	}
	else
	{
		m_FloridLeft1 = floridMgr->getFlorid("sscoreleftF");
		m_FloridLeft2 = floridMgr->getFlorid("sscoreleftS");
		m_FloridRight1 = floridMgr->getFlorid("sscorerightF");
		m_FloridRight2 = floridMgr->getFlorid("sscorerightS");
		m_FloridStar = floridMgr->getFlorid("sstar");
		m_FloridPlanet = floridMgr->getFlorid("splanetcommon");
		m_FloridPlanet6 = floridMgr->getFlorid("splanet6");
		m_FloridPlanet7 = floridMgr->getFlorid("splanet7");
		m_FloridExplode = floridMgr->getFlorid("sexplode");
		m_FloridFlashScreen = floridMgr->getFlorid("sscreenstar");
		m_FloridBigBang1 = floridMgr->getFlorid("sbigbangF");
		m_FloridBigBang2 = floridMgr->getFlorid("sbigbangS");
		m_FloridBigBang3 = floridMgr->getFlorid("sbigbangT");

		m_SingleFloridPlanet = floridMgr->getFlorid("finalplanetS");
		m_SingleFloridPlanet2 = floridMgr->getFlorid("finalplanet2S");
	}
	if (!m_FloridLeft1||!m_FloridLeft2||!m_FloridRight1||!m_FloridRight2||!m_FloridStar||!m_FloridPlanet||
		!m_FloridPlanet6||!m_FloridPlanet7||!m_FloridExplode||!m_FloridFlashScreen||!m_FloridBigBang1||
		!m_FloridBigBang2||!m_FloridBigBang3 || !m_SingleFloridPlanet || !m_SingleFloridPlanet2)
	{
		SceneMgr::s_error_msg = L"miss florid file";
		m_bScoreFrame_New_Init = false;
	}

	CGLSoundMgr* soundMgr = cls_gl->m_pSoundMgr;
	m_SoundStar =soundMgr->getGLSound("star");
	m_SoundUpgrade =soundMgr->getGLSound("upgrade");
	m_SoundParticle =soundMgr->getGLSound("particle");
	m_SoundClapping1 =soundMgr->getGLSound("clapping1");
	m_SoundClapping2 =soundMgr->getGLSound("clapping2");
	m_SoundClapping3 =soundMgr->getGLSound("clapping3");
	m_SoundBgm =soundMgr->getGLSound("bgm");
	if (!m_SoundStar||!m_SoundUpgrade||!m_SoundParticle||!m_SoundClapping1||!m_SoundClapping2||!m_SoundClapping3||!m_SoundBgm)
	{
		SceneMgr::s_error_msg = L"miss some sound file";
		m_bScoreFrame_New_Init = false;
	}
	bInit = m_bScoreFrame_New_Init;
}

CScoreFrame::~CScoreFrame()
{
	SAFE_DELETE(m_pFontSingerName);
	SAFE_DELETE(m_pFontSongName);
}

void CScoreFrame::initScoreFrame(int nDestScore,int nDestStarNum,const std::wstring singerName,const std::wstring songName,eShowWave eType)
{
	if (m_bFirst)
	{
		m_imgsetEx = (CImgsetEx*)cls_gl->m_pImgsetMgr->getImageset(L"singerpic");
		if (m_imgsetEx)
			m_ImgSinger = m_imgsetEx->getImage(L"Singer");
		else
			WLOGH_ERRO << L"singer picture cant't find in the map";

		m_wstrSingerName = singerName;
		m_wstrSongName = songName;

		std::wstring tmp_str = m_wstrSingerName;
		GetOnlyDifferentFont(tmp_str);
		m_pFontSingerName = new CGLFont(20,L"singername");
		if(!m_pFontSingerName)
			return ;
		if (m_pFontSingerName->addFont(tmp_str))
		{
			bool b = m_pFontSingerName->loadFont(cls_gl->m_winSize,true,FONT_BANDING/*FONT_SHADE*/,SINGERNAME_BANDCOLOR,SINGERNAME_TEXTCOLOR,2);
			if (!b)
			{
				WLOGH_ERRO << L"singername font load error";
				return ;
			}
		}
		tmp_str = m_wstrSongName;
		GetOnlyDifferentFont(tmp_str);
		m_pFontSongName = new CGLFont(40,L"songname");
		if(!m_pFontSongName)
			return ;
		if (m_pFontSongName->addFont(tmp_str))
		{
			bool b = m_pFontSongName->loadFont(cls_gl->m_winSize,true,FONT_BANDING/*FONT_SHADE*/,SONGNAME_BANDCOLOR,SONGNAME_TEXTCOLOR,3);
			if (!b)
			{
				WLOGH_ERRO << L"songname font load error";
				return ;
			}
		}

		//声效
		m_SoundParticle ->soundPlay();
		m_SoundBgm->soundPlay();
	}

	if (eType == SHOW_ONE)
	{
		FRAME_THINGS_CENTER = CFRAME_THINGS_CENTER;
		SONGINFO_LEFT_X = LEFT_SONGINFO_XF;
		m_ImgHLine = m_ImgHorizontalLine;
	}
	else if(eType == SHOW_TWO || eType == SHOW_THREE)
	{
		if (m_bFirst)
		{
			FRAME_THINGS_CENTER = CFRAME_THINGS_CENTERF;
		}
		else
		{
			FRAME_THINGS_CENTER = CFRAME_THINGS_CENTERS;
		}
		m_ImgHLine = m_ImgHorizontalLineShort;
		SONGINFO_LEFT_X = (float)LEFT_SONGINFO_XS;
	}

	if (m_SingleFloridPlanet)
		m_SingleFloridPlanet->setPosition(FRAME_THINGS_CENTER,250.0f);
	if (m_SingleFloridPlanet2)
		m_SingleFloridPlanet2->setPosition(FRAME_THINGS_CENTER,250.0f);

	m_FloridLeft1->setPosition(FRAME_THINGS_CENTER+95,-10.0f);
	m_FloridLeft2->setPosition(FRAME_THINGS_CENTER+95,-10.0f);
	m_FloridRight1->setPosition(FRAME_THINGS_CENTER-95,-10.0f);
	m_FloridRight2->setPosition(FRAME_THINGS_CENTER-95,-10.0f);
	m_FloridPlanet->setPosition(FRAME_THINGS_CENTER,250.0f);
	m_FloridExplode->setPosition(FRAME_THINGS_CENTER,250.0f);
	m_FloridBigBang1->setPosition(FRAME_THINGS_CENTER,250.0f);
	m_FloridBigBang2->setPosition(FRAME_THINGS_CENTER,250.0f);
	m_FloridBigBang3->setPosition(FRAME_THINGS_CENTER,250.0f);
	m_FloridPlanet6->setPosition(FRAME_THINGS_CENTER,250.0f);
	m_FloridPlanet7->setPosition(FRAME_THINGS_CENTER,250.0f);
	m_FloridStar->setRectXY(FRAME_THINGS_CENTER-160.0f,77.0f);

	const float nParticleLift = (float)TIME_TO_LEVE_PARTICLE/1000;
	if (m_bFirst)
	{
		m_FloridLeft1->startPS(nParticleLift);
		m_FloridLeft2->startPS(nParticleLift);
		m_FloridRight1->startPS(nParticleLift);
		m_FloridRight2->startPS(nParticleLift);
		if (m_SingleFloridPlanet&&m_SingleFloridPlanet2)
		{
			m_SingleFloridPlanet->startPS();
			m_SingleFloridPlanet2->startPS();
		}
	}
	else
	{
		if (eType == SHOW_TWO || eType == SHOW_THREE)
		{
			m_FloridLeft1->startPS(nParticleLift);
			m_FloridLeft2->startPS(nParticleLift);
			m_FloridRight1->startPS(nParticleLift);
			m_FloridRight2->startPS(nParticleLift);
			if (m_SingleFloridPlanet&&m_SingleFloridPlanet2)
			{
				m_SingleFloridPlanet->startPS();
				m_SingleFloridPlanet2->startPS();
			}
		}
	}

	m_veccurnumberBuf.clear();
	m_vecdestnumberBuf.clear();
	m_vecstarcurnumberBuf.clear();
	m_vecstardestnumberBuf.clear();

	m_pre_level = LEVEL_SHOW_NONE;
	m_cur_level = LEVEL_SHOW_NONE;
	m_nCurScore = 0;
	m_nCurStarNum = 0;
	m_nDestScore = nDestScore;
	if (m_nDestScore<0)
		m_nDestScore = 0;
	m_nDestStarNum = nDestStarNum;
	m_nlast_ms = 0;
	m_nscore_per_last_ms = 0;
	m_nscore_pernumber_ms = m_nDestScore ? (int)(TIME_TO_LIVE_COMMON*1.0f/m_nDestScore*3.0f) : 0;

	m_nstar_per_last_ms = 0;
	m_nstar_pernumber_ms = (int)(TIME_TO_LIVE_COMMON*1.0f/m_nDestStarNum*3.0f);

	int nDigit = CAnimaNumber::CaculateDigit(m_nDestScore);
	m_nScoreDigit = nDigit>m_nScoreDigit?nDigit:m_nScoreDigit;

	CAnimaNumber::FillZeroNumber(m_veccurnumberBuf,m_nScoreDigit);
	CAnimaNumber::DealWithNumber(m_nDestScore,m_vecdestnumberBuf);
	m_dest_allnumber_width = m_NumberImages[0]->getWidth()*(int)m_vecdestnumberBuf.size()+m_ImgPoint->getWidth();

	CAnimaNumber::FillZeroNumber(m_vecstarcurnumberBuf,m_nStarDigit);
	CAnimaNumber::DealWithNumber(m_nDestStarNum,m_vecstardestnumberBuf);

	m_bscorefram_allinit = true;
}

//画背景图片，向右移动
void CScoreFrame::drawBG()
{
	static float delta_x = (1500-1280)*1.0f/TIME_TO_END;
	static CGRect clip_rect = CGRectMake(0.0f,0.0f,GL3D_Z_BASE,1280,720);
	float cur_x = -220+delta_x*m_bg_last_tm;
	m_ImgBackground->draw(GPoint(cur_x,0),&clip_rect);
}
//画评分框架
void CScoreFrame::drawFrame()
{
	static int s_frame_width_2 = (int)(m_ImgVerticalLine->getWidth()/2.0f);
	float s_frame_x=FRAME_THINGS_CENTER - s_frame_width_2;
	m_ImgVerticalLine->draw(GPoint(s_frame_x,0.0f));
}
//画歌手图片，以及横线，歌曲信息图片
void CScoreFrame::drawSinger()
{
	if (m_blineanim)
	{
		//画横线向左移动
		float f_hline_schedule = 1-m_hline_last_tm*1.0f/TIME_TO_HLINE;
		ASSERT(m_ImgHLine != NULL);
		m_ImgHLine->draw(GPoint(f_hline_schedule*1280,627.0f));
	}
	else
	{
		m_ImgHLine->draw(GPoint(0.0f,627.0f));

		//画歌曲名，歌手名
		if (m_btextanim)
		{
			//歌曲信息上下移动，alpha渐变
			float f_text_scale = m_text_last_tm*1.0f/TIME_TO_SONGINFO;

			GColor cur_colour(1.0f,1.0f,1.0f,f_text_scale);

			int lenSongName = (int)m_wstrSongName.size();

			wchar_t ch = m_wstrSongName[0];
			std::wstring str_ch;//(&ch);
			str_ch.append(1,ch);

			static int song_text_height = m_pFontSongName->getTextHeight(str_ch);
			static int song_text_width = m_pFontSongName->getTextWidth(str_ch);
			int cur_text_height = int(f_text_scale*song_text_height);
			GPoint dest_point(SONGINFO_LEFT_X,566.0f+song_text_height-cur_text_height,GL3D_Z_BASE);
			
			CGRect clip_rect = CGRectMake(dest_point,GSize(song_text_width*lenSongName,cur_text_height));

			for(int i=0;i<lenSongName;i++)
			{
				wchar_t ch = m_wstrSongName[i];
				std::wstring tmp_str_ch;//(&ch);
				tmp_str_ch.append(1,ch);

				drawSingleFont(m_pFontSongName,tmp_str_ch,dest_point,&clip_rect,cur_colour);
			}

			ch = m_wstrSingerName[0];
			str_ch.clear();//(&ch);
			str_ch.append(1,ch);

			static int singer_text_height = m_pFontSingerName->getTextHeight(str_ch);
			static int singer_text_width = m_pFontSingerName->getTextWidth(str_ch);
			cur_text_height = int(f_text_scale*singer_text_height);
			dest_point = CGPointMake(SONGINFO_LEFT_X+10.0f,640.0f+cur_text_height-singer_text_height,GL3D_Z_BASE);

			int lenSingerName = (int)m_wstrSingerName.size();
			clip_rect = CGRectMake(dest_point,GSize(singer_text_width*lenSingerName,cur_text_height));
			for(int i=0;i<lenSingerName;i++)
			{
				wchar_t ch = m_wstrSingerName[i];
				std::wstring tmp_str_ch;//(&ch);
				tmp_str_ch.append(1,ch);
				drawSingleFont(m_pFontSingerName,tmp_str_ch,dest_point,&clip_rect,cur_colour);
			}
			//歌手图片渐现效果
			if (m_ImgSinger)
			{
// #ifdef SPECIAL_VERSION
// 				m_imgsetEx->lockBatchBuffer();
// #endif
				m_ImgSinger->draw(GPoint(SONGINFO_LEFT_X,LEFT_SONGINFO_Y),NULL,true,cur_colour);
// #ifdef SPECIAL_VERSION
// 				m_imgsetEx->unlockBatchBuffer();
// #endif
			}
		}
		else
		{
			GPoint songname_pt = CGPointMake(SONGINFO_LEFT_X,566.0f);
			int lenSongName = (int)m_wstrSongName.size();
			for(int i=0;i<lenSongName;i++)
			{
				wchar_t ch = m_wstrSongName[i];
				std::wstring str_ch;//(&ch);
				str_ch.append(1,ch);
				drawSingleFont(m_pFontSongName,str_ch,songname_pt,NULL);
			}

			GPoint singername_pt = CGPointMake(SONGINFO_LEFT_X+10.0f,640.0f);
			int lenSingerName = (int)m_wstrSingerName.size();
			for(int i=0;i<lenSingerName;i++)
			{
				wchar_t ch = m_wstrSingerName[i];
				std::wstring str_ch;//(&ch);
				str_ch.append(1,ch);
				drawSingleFont(m_pFontSingerName,str_ch,singername_pt,NULL);
			}

			if (m_ImgSinger)
			{
// #ifdef SPECIAL_VERSION
// 				m_imgsetEx->lockBatchBuffer();
// #endif
				m_ImgSinger->draw(GPoint(SONGINFO_LEFT_X,LEFT_SONGINFO_Y));
// #ifdef SPECIAL_VERSION
// 				m_imgsetEx->unlockBatchBuffer();
// #endif
			}
		}
	}
}
void CScoreFrame::drawSingleFont(CGLFont* pflFont,const std::wstring ch,GPoint& pt,CGRect* pclip_rect,const GColor colour)
{
	if (!pflFont || !pflFont->getImgset())
	{
		return ;
	}
	int width = pflFont->drawText(ch,pt,pclip_rect,colour);
	pt.x += (ch[0] > 0x00FF)?width*0.8f:width*0.7f;
	// pt.x+= ( ch[0] > 0x00FF ) ? 22 : 20;
}

void CScoreFrame::updateLevel(ulong delta_ms)
{
	if (m_bBlackHole)
	{//黑洞粒子系统启用
		if (!m_bPreBlackHole)
		{
			if (m_FloridPlanet7->getRunning())
				m_FloridPlanet7->stopPS();
			m_SoundClapping3->soundPlay();
			m_FloridExplode->startPS();
			m_FloridBigBang1->startPS();
			m_FloridBigBang2->startPS();
			m_FloridBigBang3->startPS();
			m_SoundUpgrade->soundPlay();
			m_cur_bchangeleveltext = true;
			m_bPreBlackHole = true;
		}
		//黑洞粒子特效
	}
	else
	{//等级图片，以及星体显示动画
		if (m_pre_level != m_cur_level)
		{//如果升级
			m_cur_bchangelevelpic = true;
			m_cur_bchangeleveltext = true;
			m_SoundUpgrade->soundPlay();
			m_FloridExplode->stopPS();
			m_FloridExplode->startPS();
			m_pre_level = m_cur_level;
			switch(m_cur_level)//定义掌声播放规则
			{
			case LEVEL_SHOW_NONE:
			case LEVEL_SHOW_ONE:
				{
					break;
				}
			case LEVEL_SHOW_TWO:
			case LEVEL_SHOW_THREE:
				{
					m_SoundClapping1->soundPlay();
					break;
				}
			case LEVEL_SHOW_FOUR:
			case LEVEL_SHOW_FIVE:
				{
					m_SoundClapping2->soundPlay();
					break;
				}
			case LEVEL_SHOW_SIX:
				{
					if (m_FloridPlanet->getRunning())
						m_FloridPlanet->stopPS();
					m_FloridPlanet6->startPS();
					m_SoundClapping2->soundPlay();
					break;
				}
			case LEVEL_SHOW_SEVEN:
				{
					if (m_FloridPlanet6->getRunning())
						m_FloridPlanet6->stopPS();
					m_FloridPlanet7->startPS();
					m_SoundClapping3->soundPlay();
					break;
				}
			}
		}

		if (m_cur_bchangelevelpic)
		{//升级了
			if (m_cur_level==1)
			{//陨石，特殊处理，从右上角向左下移动
				m_FloridPlanet->setPosition(FRAME_THINGS_CENTER-50.0f,300.0f);//FRAME_THINGS_CENTER,550
				m_FloridPlanet->startPS();
			}
			else
			{//其他图片，从中间放大
				if (m_cur_level == 2)
				{				
					m_FloridPlanet->stopPS();
					m_FloridPlanet->setPosition(FRAME_THINGS_CENTER,250.0f);//
					m_FloridPlanet->startPS();
				}
			}
			if (m_cur_level_last_ms>TIME_TO_LEVEL_PIC)
			{
				m_cur_level_last_ms = 0;
				m_cur_bchangelevelpic = false;
			}
			m_cur_level_last_ms+= delta_ms;
		}
	}

	if (m_cur_bchangeleveltext)
	{//升级了
		if (m_cur_levelmask_text_last_ms>=TIME_TO_LEVEL_TEXT_APP)
		{
			m_cur_bchangeleveltext = false;
			m_cur_levelmask_text_last_ms = 0;
		}
		m_cur_levelmask_text_last_ms += delta_ms;
	}
}

void CScoreFrame::updateSingInfo(ulong delta_ms)
{
	if (m_blineanim)
	{
		//画横线向左移动
		m_hline_last_tm+=delta_ms;
		if (m_hline_last_tm>=TIME_TO_HLINE)
		{
			m_hline_last_tm =0;
			m_blineanim = false;
		}
	}
	else
	{
		if (m_btextanim)
		{
			m_text_last_tm += delta_ms;
			if (m_text_last_tm>=TIME_TO_SONGINFO)
			{
				m_btextanim = false;
			}
		}
	}
}
void CScoreFrame::animaNumber(float &pos_x,float &pos_y)
{
	static int number_width = m_NumberImages[0]->getWidth();	
	int nInteger = (int)m_veccurnumberBuf.size() - 1;
	for(int i=0;i<nInteger;i++)
	{
		const int& nCur = m_veccurnumberBuf[i];
		m_NumberImages[nCur]->draw(GPoint(pos_x,pos_y) , NULL);
		pos_x += number_width;//每描述一位，x坐标向右移动	
	}
	m_ImgPoint->draw(GPoint(pos_x,pos_y),NULL);//描绘小数点
	pos_x += m_ImgPoint->m_rect.size.width;

	m_NumberImages[m_veccurnumberBuf.back()]->draw(GPoint(pos_x,pos_y) , NULL);
	pos_x += number_width;//每描述一位，x坐标向右移动	
}

void CScoreFrame::animaBiggingNumber(float x,float y,float size,GColor cur_colour)
{
	RenderEImage* img = NULL;
	static int w = m_NumberImages[0]->getWidth();
	static int h = m_NumberImages[0]->getHeight();
	static int w_point = m_ImgPoint->getWidth();
	int h_size = int(h*size);
	GSize cur_number_size(int(w*size),h_size);
	GSize cur_point_size(int(w_point*size),h_size);
	if (m_veccurnumberBuf.empty())
	{
		return ;
	}
	int sum = (int)m_vecdestnumberBuf.size()-1;
	if (sum)
	{
		for(int i=0;i<sum;i++)
		{
			//描绘在最左边，从高位开始描绘，
			img = m_NumberImages[m_vecdestnumberBuf[i]];
			img-> draw(GPoint(x,y),cur_number_size,NULL,true,cur_colour);
			x += img->m_rect.size.width*size;//每描述一位，x坐标向右移动	
		}
	}
	else
	{
		img = m_NumberImages[0];
		img->draw(GPoint(x,y),cur_number_size,NULL,true,cur_colour);
		x += img->m_rect.size.width*size;//每描述一位，x坐标向右移动
	}

	//////////////////////////////////////////////////////////////////////////
	m_ImgPoint->draw(GPoint(x,y),cur_point_size,NULL,true,cur_colour);//描绘小数点
	x += m_ImgPoint->m_rect.size.width*size;

	img = m_NumberImages[m_vecdestnumberBuf[sum]];//描绘小数点后一位,或者0
	img->draw(GPoint(x,y),cur_number_size,NULL,true,cur_colour);
}

//画麦克风，得分，以及星星图片，星星值
void CScoreFrame::drawScoreCombo()
{
	static int w_2 = m_ImgTotalScore->getWidth()/2;
	static int h_2 = m_ImgStarNum->getHeight()/2;
	float x = FRAME_THINGS_CENTER - (float)w_2;
	m_ImgTotalScore->draw(GPoint(x,80.0f));

	//话筒
	static int star_width = m_ImgMic->getWidth();
	static int number_width = m_NumberImages[0]->getWidth();
	float pos_x = FRAME_THINGS_CENTER-90.0f;
	float pos_y = 100;//
	m_ImgMic->draw(GPoint(pos_x,pos_y),NULL);//画麦克风
	pos_x += m_ImgMic->getWidth()-10;
	pos_y = 112;//调整坐标，让数字保持与麦克风高度相同
	float start_pos_x = pos_x;

	static int number_height = m_NumberImages[0]->getHeight();
	float pos_big_x = pos_x;
	//总分的特效，变化数字
	animaNumber(pos_x,pos_y);
	//////////////////////////////////////////////////////////////////////////
	if (m_bScoreBiggingF)
	{
		/*static*/ float center_big_score = pos_big_x+50.0f;
		float f_bigging_schedule = (m_nlast_ms-TIME_TO_LIVE_COMMON)*1.0f/TIME_TO_SCORE_BIG;
		float f_size_schedule = 1+f_bigging_schedule*2;
		float f_alpha_schedule = (1-f_bigging_schedule)*0.6f;
		f_alpha_schedule = f_alpha_schedule<0.0f?0.0f:(f_alpha_schedule>1.0f?1.0f:f_alpha_schedule);
		GColor cur_bigscore_colour = GColor(1.0f,1.0f,1.0f,f_alpha_schedule);
		
		float big_pos_x = center_big_score - m_dest_allnumber_width*f_size_schedule/2.0f;
		float big_pos_y = 125.5f - 13.5f*f_size_schedule;
		animaBiggingNumber(center_big_score-50*f_size_schedule,big_pos_y,f_size_schedule,cur_bigscore_colour);
	}
	if (m_bScoreBiggingS)
	{
		/*static*/ float center_big_score = pos_big_x+50;
		float f_bigging_schedule = (m_nlast_ms-TIME_TO_LIVE_COMMON-TIME_TO_SCORE_BIG_S)*1.0f/TIME_TO_SCORE_BIG;
		float f_size_schedule = 1+f_bigging_schedule*2;
		float f_alpha_schedule = (1-f_bigging_schedule)*0.6f;
		f_alpha_schedule = f_alpha_schedule<0.0f?0.0f:(f_alpha_schedule>1.0f?1.0f:f_alpha_schedule);
		GColor cur_bigscore_colour = GColor(1.0f,1.0f,1.0f,f_alpha_schedule);

		float big_pos_x = center_big_score - m_dest_allnumber_width*f_size_schedule/2.0f;
		float big_pos_y = 125.5f - 13.5f*f_size_schedule;
		animaBiggingNumber(center_big_score-50*f_size_schedule,big_pos_y,f_size_schedule,cur_bigscore_colour);
	}
	if (m_bScoreBiggingT)
	{
		/*static*/ float center_big_score = pos_big_x+50.0f;
		float f_bigging_schedule = (m_nlast_ms-TIME_TO_LIVE_COMMON-TIME_TO_SCORE_BIG_T)*1.0f/TIME_TO_SCORE_BIG;
		float f_size_schedule = 1+f_bigging_schedule*2;
		float f_alpha_schedule = (1-f_bigging_schedule)*0.6f;
		f_alpha_schedule = f_alpha_schedule<0.0f?0.0f:(f_alpha_schedule>1.0f?1.0f:f_alpha_schedule);
		GColor cur_bigscore_colour = GColor(1.0f,1.0f,1.0f,f_alpha_schedule);

		float big_pos_x = center_big_score - m_dest_allnumber_width*f_size_schedule/2.0f;
		float big_pos_y = 125.5f - 13.5f*f_size_schedule;
		animaBiggingNumber(center_big_score-50*f_size_schedule,big_pos_y,f_size_schedule,cur_bigscore_colour);
	}

	pos_x = (start_pos_x + pos_x - m_ImgScoreBg->getWidth()+6)/2.0f;//+6让底图往右边移动一点
	pos_y = number_height+105.0f;
	m_ImgScoreBg->draw(GPoint(pos_x,pos_y),NULL);//分数下面的图


	//画光环，光环循环从小变大
	if(m_nlast_ms > TIME_TO_LIVE_COMMON)
		return ;
	m_ImgStarNum->draw(GPoint(x,600.0f));

	static int s_ring_width = m_ImgRing->getWidth();
	static int s_ring_height = m_ImgRing->getHeight();
	int ndelta_ms = m_nlast_ms%TIME_TO_LOOP;
	float f_cur_scale = ndelta_ms*1.0f/TIME_TO_LOOP;
	int cur_ring_width_2 = (int)(f_cur_scale*s_ring_width/2.0f);
	int cur_ring_height_2 = (int)(f_cur_scale*s_ring_height/2.0f);
	m_ImgRing->draw(GPoint(FRAME_THINGS_CENTER-cur_ring_width_2,580.0f-cur_ring_width_2),GSize(cur_ring_width_2*2,cur_ring_height_2*2));

	static int s_star_width = m_ImgStar->getWidth();
	float s_star_x = FRAME_THINGS_CENTER - s_star_width/2.0f;
	m_ImgStar->draw(GPoint(s_star_x,524));

	static int star_number_width = m_ImgStarNum[0].getWidth();
	static int s_star_number_widht_2 = (int)(m_StarNumberImages[0]->getWidth()/2.0f);
	//描绘星星值，变化星星值
	pos_x = FRAME_THINGS_CENTER - s_star_number_widht_2*m_vecstarcurnumberBuf.size();
	pos_y = 570.0f;

	for(int i=0;i<(int)m_vecstarcurnumberBuf.size();i++)
	{
		const int& nCur = m_vecstarcurnumberBuf[i];
		m_StarNumberImages[nCur]->draw(GPoint(pos_x,pos_y) , NULL);
		pos_x += star_number_width;//每描述一位，x坐标向右移动	
	}
}

void	CScoreFrame::updateRankingInfo(const PlayerScore* pRI)
{
	if(!pRI)
	{
		return ;
	}
	memcpy(&m_structPs,pRI,sizeof(PlayerScore));
}

//根据当前加的分数
void CScoreFrame::drawLevel()
{
 	if (m_SingleFloridPlanet)
 		m_SingleFloridPlanet->renderPS();
 	if (m_SingleFloridPlanet2)
 		m_SingleFloridPlanet2->renderPS();

	//等级图片
	if (!m_bBlackHole)
	{//等级图片，以及星体显示动画
		if (m_cur_bchangelevelpic)
		{//升级了
			float f_cur_scale = m_cur_level_last_ms*1.0f/TIME_TO_LEVEL_PIC;
			RenderEImage* img = m_ImgLevelPicture[m_cur_level];
			int n_levelpic_width = int(img->getWidth()*f_cur_scale);
			int n_levelpic_height = int(img->getHeight()*f_cur_scale);
			if (m_cur_level==1)
			{//陨石，特殊处理，从右上角向左下移动
				m_FloridPlanet->stopPS();
				m_FloridPlanet->setPosition(FRAME_THINGS_CENTER-50.0f,300.0f);//FRAME_THINGS_CENTER,550
				m_FloridPlanet->startPS();
				img->draw(GPoint(FRAME_THINGS_CENTER+234-n_levelpic_width,119,GL3D_Z_BASE)
					,GSize(n_levelpic_width,n_levelpic_height));
			}
			else
			{//其他图片，从中间放大
				int n_levelpic_width_2 = int(n_levelpic_width/2.0f);
				int n_levelpic_height_2 = int(n_levelpic_height/2.0f);
				img->draw(GPoint(float(FRAME_THINGS_CENTER-n_levelpic_width_2),float(250-n_levelpic_height_2),GL3D_Z_BASE)
					,GSize(n_levelpic_width,n_levelpic_height));
			}
		}
		//keep 
		else
		{
			RenderEImage* img = m_ImgLevelPicture[m_cur_level];
			if (m_cur_level == 0)//岩石旋转
			{
				static int n_rock_width_2 = (int)(img->getWidth()/2.f);
				static int n_rock_height_2 =(int)(img->getWidth()/2.0f);
				float rock_pos_x = FRAME_THINGS_CENTER;// - n_rock_width_2;
				static float rock_pos_y = 250;//-n_rock_height_2;
				img->drawPSEx(GPoint(rock_pos_x,rock_pos_y,GL3D_Z_BASE),m_nlast_ms/6000.0f);
			}
			else if (m_cur_level == 1)
				img->draw(GPoint(FRAME_THINGS_CENTER-131.0f,119.0f,GL3D_Z_BASE));
			else
			{
				int n_levelpic_width_2 = int(img->getWidth()/2.0f);
				int n_levelpic_height_2 = int(img->getHeight()/2.0f);
				float x = FRAME_THINGS_CENTER-n_levelpic_width_2;
				float y = 250.0f-n_levelpic_height_2;
				img->draw(GPoint(x,y,GL3D_Z_BASE));
			}	
		}
	}

	static int mask_width = m_ImgLevelTextMask->getWidth();
	static int mask_height = m_ImgLevelTextMask->getHeight();
	if (m_cur_bchangeleveltext)
	{//升级了
		//设置覆盖在等级文字上的特效
		float y = 389 - mask_height/2.0f;
		float f_scale_text = m_cur_levelmask_text_last_ms*1.0f/TIME_TO_LEVEL_TEXT_APP*2;
		int fcur_level_text_width = (int)(mask_width*f_scale_text);
		m_ImgLevelTextMask->draw(GPoint(FRAME_THINGS_CENTER-fcur_level_text_width/2.0f,y
			,GL3D_Z_BASE),GSize(fcur_level_text_width,mask_height));
		return ;
	}
	//等级文字的显示
	RenderEImage* img = m_ImgLevelText[m_cur_level];
	int nlevel_text_width = img->getWidth();
	int nlevel_text_height = img->getHeight();
	float x = FRAME_THINGS_CENTER-nlevel_text_width/2.0f;
	float y = 389-nlevel_text_height/2.0f;
	img->draw(GPoint(x,y,GL3D_Z_BASE));
}

void CScoreFrame::drawBaseInfo(const std::wstring baseStr,const std::wstring numberStr,float& height,uchar alpha)
{
	std::wstring allStr = baseStr + numberStr;
	float baseStrX = FRAME_THINGS_CENTER-RANKINFO_OFFSET1;
	int width = m_pFontRankinfo->drawText(baseStr,GPoint(baseStrX,height,GL3D_Z_BEFOREPARTICLE),NULL,GColor((alpha<<24)|0xa5a5a5),0.8f,0.8f);
	float rankX = baseStrX + width+20;
	m_pFontRankinfo->drawText(numberStr,GPoint(rankX,height,GL3D_Z_BEFOREPARTICLE),NULL,GColor((alpha<<24)|0xfcd303),0.8f,0.8f);//金黄色
	height += m_pFontRankinfo->getTextHeight(allStr);
}

void CScoreFrame::drawExtInfo(const std::wstring baseStr,const std::wstring numberStr,float& height,uchar alpha,const std::wstring tailStr)
{
	std::wstring allStr = baseStr + numberStr+tailStr;
	float baseStrX = FRAME_THINGS_CENTER-RANKINFO_OFFSET2;
	int width = m_pFontRankinfo->drawText(baseStr,GPoint(baseStrX,height,GL3D_Z_BEFOREPARTICLE),NULL,GColor((alpha<<24)|0xffffff),0.8f);
	float rankX = baseStrX + width+10;
	width = m_pFontRankinfo->drawText(numberStr,GPoint(rankX,height,GL3D_Z_BEFOREPARTICLE),NULL,GColor((alpha<<24)|0xfcd303),0.9f);//金黄色
	rankX = rankX + width+5;
	m_pFontRankinfo->drawText(tailStr,GPoint(rankX,height,GL3D_Z_BEFOREPARTICLE),NULL,GColor((alpha<<24)|0xffffff),0.8f);
	height += m_pFontRankinfo->getTextHeight(allStr);
}

void CScoreFrame::drawRanking()
{
	if(!m_structPs.valid || !m_pFontRankinfo)
	{
		return ;
	}

	uchar cur_alpha = (m_nlast_ms-TIME_TO_LIVE_COMMON<1000)?uchar((m_nlast_ms-TIME_TO_LIVE_COMMON)/1000.0f*255):255;

	bool bExtInfo = m_structPs.percentHistory >= 10;
	float height = bExtInfo?450.0f:550.0f;
	wchar_t wbuffer[260] = {0};
	swprintf_s(wbuffer,L"%d",m_structPs.noInSong);
	std::wstring numberStr = wbuffer;
	drawBaseInfo(L"您的演唱在同歌曲中排名",numberStr,height,cur_alpha);
	swprintf_s(wbuffer,L"%d",m_structPs.noInHistory);
	numberStr = wbuffer;
	drawBaseInfo(L"在今日的本店演唱中排名",numberStr,height,cur_alpha);
	swprintf_s(wbuffer,L"%d",m_structPs.noInCountry);
	numberStr = wbuffer;
	drawBaseInfo(L"在今日·的全国演唱中排名",numberStr,height,cur_alpha);

	if(!bExtInfo)
		return ;
	if(m_ImgCongratulate)
	{
		float tmpX = FRAME_THINGS_CENTER-m_ImgCongratulate->getWidth()/2.0f;
		static float s_Y = 552.0f;
		m_ImgCongratulate->draw(GPoint(tmpX,s_Y,GL3D_Z_BEFOREPARTICLE),NULL,true,GColor((cur_alpha<<24)|0xffffff));
	}

	float height2 = 602.0f;
	swprintf_s(wbuffer,L"%d%%",m_structPs.percentHistory);
	numberStr = wbuffer;
	drawExtInfo(L"击败了本店",numberStr,height2,cur_alpha);
	swprintf_s(wbuffer,L"%d%%",m_structPs.percentCountry);
	numberStr = wbuffer;
	drawExtInfo(L"击败了全国",numberStr,height2,cur_alpha);
}

void CScoreFrame::update(const ulong delta_ms)
{
	if (m_nlast_ms > TIME_TO_END)
	{
		if (m_bFirst)
		{
			if (!m_bend_scoreframe)
			{
				g_bGLThread = false;
				cls_gl->postRealEndSongMessage();
				m_SoundStar->soundStop();
				m_SoundParticle->soundStop();
				m_SoundBgm->soundStop();
				m_bend_scoreframe = true;
			}
		}
		return ;
	}

	m_bg_last_tm+= delta_ms;

	if (m_nCurScore>=880)
	{
		if (!m_bstartStar)
		{
			m_FloridStar->startPS();
			m_bstartStar = true;
		}
	}


	if (m_nlast_ms >= TIME_TO_LIVE_COMMON)
	{
		m_nCurScore = m_nDestScore;
		m_nNextScore = m_nDestScore;

		m_veccurnumberBuf = m_vecdestnumberBuf; 
		//如果总分是个位数，需要补一个0
		CAnimaNumber::FillZeroNumber(m_veccurnumberBuf,m_nScoreDigit);

		m_nCurStarNum = m_nDestStarNum;
		m_nNextStarNum = m_nDestStarNum;

		m_bScoreBiggingF = true;
		if (m_nlast_ms>=TIME_TO_LIVE_COMMON+TIME_TO_SCORE_BIG_S)
			m_bScoreBiggingS = true;
		if (m_nlast_ms>=TIME_TO_LIVE_COMMON+TIME_TO_SCORE_BIG_T)
			m_bScoreBiggingT = true;
		if (m_nlast_ms >= TIME_TO_LIVE_COMMON+TIME_TO_SCORE_BIG)
			m_bScoreBiggingF = false;
		if (m_nlast_ms >= TIME_TO_LIVE_COMMON+TIME_TO_SCORE_BIG+TIME_TO_SCORE_BIG_S)
			m_bScoreBiggingS = false;
		if (m_nlast_ms >= TIME_TO_LIVE_COMMON+TIME_TO_SCORE_BIG+TIME_TO_SCORE_BIG_T)
			m_bScoreBiggingT = false;
	}
	else
	{
		CAnimaNumber::SmoothNumber(m_nDestScore,m_nCurScore,m_nNextScore,m_nscore_per_last_ms
			,m_nscore_pernumber_ms,delta_ms,m_fScorePercent,3);
		CAnimaNumber::DealWithNumber(m_nCurScore,m_veccurnumberBuf);
		CAnimaNumber::FillZeroNumber(m_veccurnumberBuf,m_nScoreDigit);//至少2位数字

		float fPercent;
		CAnimaNumber::SmoothNumber(m_nDestStarNum,m_nCurStarNum,m_nNextStarNum
			,m_nstar_per_last_ms,m_nstar_pernumber_ms,delta_ms,fPercent,1);
		CAnimaNumber::DealWithNumber(m_nCurStarNum,m_vecstarcurnumberBuf);
		CAnimaNumber::FillZeroNumber(m_vecstarcurnumberBuf,m_nStarDigit);//至少1位数字
	}

	if (m_nCurScore>=950)//定义等级规则，当前分数和星星值的运算
	{
		m_bBlackHole = true;
		m_cur_level = LEVEL_SHOW_SEVEN;
	}
	else if (m_nCurScore>=910)
		m_cur_level = LEVEL_SHOW_SIX;
	else if (m_nCurScore>=880)
		m_cur_level = LEVEL_SHOW_FIVE;
	else if (m_nCurScore>=840)
		m_cur_level = LEVEL_SHOW_FOUR;
	else if(m_nCurScore>=810)
		m_cur_level = LEVEL_SHOW_THREE;
	else if (m_nCurScore>=710)
		m_cur_level = LEVEL_SHOW_TWO;
	else if (m_nCurScore>=600)
		m_cur_level = LEVEL_SHOW_ONE;
	else
		m_cur_level = LEVEL_SHOW_NONE;

	updateLevel(delta_ms);
	updateSingInfo(delta_ms);

	if(m_bFirst && m_nlast_ms>TIME_TO_START_BLACK)
		if(cls_gl->m_pDrawListener)
			cls_gl->m_pDrawListener->setBlackCurtain(true);

	m_nlast_ms += delta_ms;
}

void CScoreFrame::drawGraphBuffer()
{
	if (m_bFirst)
	{
		//画背景图片
		drawBG();
	}
	//画评分框架
	drawFrame();
	//画麦克风，得分，以及星星图片，星星值
	drawScoreCombo();
	//根据当前加的分数
	drawLevel();
	//画排名信息
	if(m_nlast_ms > TIME_TO_LIVE_COMMON)
	{
		drawRanking();
	}
	if (m_bFirst)
	{
		//画歌手图片，以及横线，
		drawSinger();
	}
}



