#include "stdafx.h"
#include "OS_NotifyMessage.h"


#define APPEAR_MS 500
#define DISAPPEAR_MS 1200
#define DISAPPEAR_TOTAL_MS 800
#define TOTAL_MS 2000
//调音背景图片位置
const float CONFIG_BKG_X =0;
const float CONFIG_BKG_Y =0;
//进度条配置位置
#define CONFIG_SHEDULE_X (CONFIG_BKG_X+300)
#define CONFIG_SHEDULE_Y (CONFIG_BKG_Y+100)
//具体值的位置配置
#define CONFIG_NUMBER_X (CONFIG_BKG_X+300)
#define CONFIG_NUMBER_Y (CONFIG_BKG_Y+100)
//中间png序列时长
#define TOTAL_PNGARRAY_MS 6000
#define AVERAGE_PNGARRAY_MS 100

#define GOLD_LEVEL1 950 
#define GOLD_LEVEL2 900
#define GOLD_LEVEL3 800
#define GOLD_LEVEL4 400

#define GOLD_NUMBER_MS 5000
#define GOLD_PER_MS 71
#define GOLD_BAG_APPEAR_MS 300
#define GLOD_BAG_NUMBER_READY 1000
#define GLOD_BAG_KEEP 1000
#define GOLD_BAG_DISAPPEAR_MS 300

#define GOLD_NUMBER_POS_Y 580

COS_NotifyMessage::COS_NotifyMessage(bool& bInit,ISceneObject* pObj,const StructSound& ss,bool bAccompany,bool bMute)
:IComRender(pObj)
,m_structSound(ss)
,m_ImgLeftCurrent1(NULL)
,m_ImgLeftCurrent2(NULL)
,m_ImgCenterCurrent(NULL)
,m_ImgCenterCurrent2(NULL)
,m_ImgCenterCurrentAnima(NULL)
,m_left_last_ms(TOTAL_MS)
,m_center_last_ms(TOTAL_MS)
,m_center_last_ms2(TOTAL_MS)
,m_center_anima_last_ms(TOTAL_PNGARRAY_MS)
,m_bneed_addms(true)
,m_bDrawLeft(true)
,m_bDrawCenter(true)
,m_bDrawCenter2(true)
,m_bneed_return(false)
,m_bneed_addms_center(true)
,m_nReturn(NOTIFY_NONE)
,m_nPreCurrent1(NOTIFY_NONE)
,m_nPreCurrent2(NOTIFY_NONE)
,m_fCurSheduleMic(0)
,m_fCurSheduleMusic(0)
,m_fCurSheduleTone(0)
,m_ImgMicVolumeBkg(NULL)
,m_ImgMusicVolumeBkg(NULL)
,m_ImgToneBkg(NULL)
,m_ImgGLSH(NULL)
,m_nSheduleValue(0)
,m_fCurShedule(0)
,m_ImgNagetive(NULL)
,m_bInitAccompany(bAccompany)
,m_nTop(-1)
,m_bLeftKeep(false)
,m_imgset(NULL)
,m_nAnimaI(0)
,m_center_anima_singlems(0)
,m_ImgGoldBag(NULL)
,m_gFloridGold1(NULL)
,m_gFloridGold2(NULL)
,m_gFloridGold3(NULL)
,m_gFloridGold4(NULL)
,m_gFloridLight(NULL)
,m_nNumberLastMs(0)
,m_nGoldStep(1)
,m_fFloridGoldMs(0.0f)
,m_nGoldEffectLastMs(0)
,m_nGoldCur(0)
,m_nGoldDest(0)
,m_nGoldNext(0)
,m_nDigit(1)
,m_bStartGoldGetting(false)
,m_bStartFlorid(false)
,m_fGoldPercent(0.0f)
,m_gSoundGold(NULL)
{
	bInit = true;
	
	assert(cls_gl != NULL);

	//是否需要重置为伴唱
	if(!cls_gl->m_isNeedResetAccompany)
		m_bInitAccompany = true;

	Imageset* imgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
	if (!imgset)
	{
		WLOGH_ERRO << L"renderer texture miss";
		bInit = false ;
	}
	for (int i=0;i<18;i++)//Notify xml 文件中
	{
		wchar_t buf[260] = {0};
		swprintf(buf,L"Notify%d",i);
		RenderEImage* img = imgset->getImage(buf);
		if(img)
			m_vectImgNotify.push_back(img);
		else
		{
			WLOGH_ERRO << L"notify img miss";
			bInit = false ;
		}
	}

	for (int i=0;i<10;i++)
	{
		wchar_t buf[260] = {0};
		swprintf(buf,L"NotifyNumber%d",i);
		RenderEImage* img = imgset->getImage(buf);
		if (img)
			m_vectImgNumber.push_back(img);
		else
		{
			WLOGH_ERRO << L"number img miss";
			bInit = false ;
		}
	} 

	m_ImgGLSH = imgset->getImage(L"Shedule");
	m_ImgMicVolumeBkg = imgset->getImage(L"MicVolumeBkg");
	m_ImgMusicVolumeBkg = imgset->getImage(L"MusicVolumeBkg");
	m_ImgToneBkg = imgset->getImage(L"ToneBkg");
	m_ImgNagetive = imgset->getImage(L"Nagetive");
	if (!m_ImgGLSH||!m_ImgMicVolumeBkg||!m_ImgMusicVolumeBkg||!m_ImgToneBkg||!m_ImgNagetive)
	{
		WLOGH_ERRO << L"shedule,mic volume bkg,music volume bkg,tone bkg,nagetive image miss";
		bInit = false ;
	}

	m_imgset = cls_gl->m_pImgsetMgr->getImageset(L"zzanima");
	if (!m_imgset)
	{
		WLOGH_ERRO << L"atmosphere animat image miss";
		bInit = false ;
	}
	//
	for(int i=0;i<6;i++)
	{
		wchar_t buf[260] = {0};
		swprintf(buf,L"Applaud%d",i);
		RenderEImage* img = m_imgset->getImage(buf);
		if (img)
		{
			m_vectImgAnimaApplaud.push_back(img);
		}
		else
		{
			WLOGH_ERRO << L"applaud image miss";
			bInit = false ;
		}
		swprintf(buf,L"Cheer%d",i);
		img = m_imgset->getImage(buf);
		if (img)
		{
			m_vectImgAnimaCheer.push_back(img);
		}
		else
		{
			WLOGH_ERRO << L"cheer image miss";
			bInit = false ;
		}
	}
	m_vectImgAnimaCom.clear();

	if (bMute)
	{
		m_nTop = 0;
		m_arraHistory[0] = NOTIFY_MUTE_OPEN;
	}
	else
		m_arraHistory[0] = NOTIFY_NONE;
	m_arraHistory[1] = NOTIFY_NONE;

	m_nMicRange = m_structSound.max_mic-m_structSound.min_mic;//m_nMicMax - m_nMicMin;
	m_nMusicRange = m_structSound.max_music-m_structSound.min_music;//m_nMusicMax - m_nMusicMin;
	m_nToneRange = m_structSound.max_tone-m_structSound.min_tone;//m_nToneMax - m_nToneMin;
	m_fCurSheduleMic = 1.0f*m_structSound.cur_mic/m_nMicRange;
	m_fPreSheduleMic = m_fCurSheduleMic;
	m_fCurSheduleMusic = 1.0f*m_structSound.cur_music/m_nMusicRange;
	m_fPreSheduleMusic = m_fCurSheduleMusic;
	m_fCurSheduleTone = 1.0f*m_structSound.cur_tone/m_nToneRange;
	m_fPreSheduleTone = m_fCurSheduleTone;

	imgset = cls_gl->m_pImgsetMgr->getImageset(L"zsbag");
	assert(imgset != NULL);
	m_ImgGoldBag = imgset->getImage(L"GoldBag");
	CFloridMgr* pFMgr = cls_gl->m_pFloridMgr;
	if(pFMgr && m_ImgGoldBag)
	{
		m_gFloridGold1 = pFMgr->getFlorid("goldget1");
		m_gFloridGold2 = pFMgr->getFlorid("goldget2");
		m_gFloridGold3 = pFMgr->getFlorid("goldget3");
		m_gFloridGold4 = pFMgr->getFlorid("goldget4");

		m_gFloridLight = pFMgr->getFlorid("goldbagbg");

		if(!m_gFloridGold1 || !m_gFloridGold2 || !m_gFloridGold3 || !m_gFloridGold4 || !m_gFloridLight)
		{
			WLOGH_ERRO << L"miss some gold florid";
			bInit = false;
		}
	}
	else
	{
		WLOGH_ERRO << L"miss gold bag image";
		bInit = false;
	}

	for (int i=0;i<10;i++)
	{
		wchar_t buf[260] = {0};
		swprintf(buf,L"GoldNum%d",i);
		RenderEImage* img = imgset->getImage(buf);
		if(img)
			m_vectImgGoldNumber.push_back(img);
		else
		{
			WLOGH_ERRO << L"miss gold number image";
			bInit = false;
		}
	}

	m_gSoundGold = cls_gl->m_pSoundMgr->getGLSound("goldfall");
	if(!m_gSoundGold)
		WLOGH_ERRO << L"miss gold fall sound";
}

COS_NotifyMessage::~COS_NotifyMessage()
{
	
}

//初始化金币掉落
void COS_NotifyMessage::initGettingGold(int highScore)
{
	//highScore = 500; //glp debug 查看金币掉落特效
	int gold = 0;
	if(highScore>GOLD_LEVEL1)
		gold = highScore*2;
	else if(highScore>GOLD_LEVEL2)
		gold = (int)(highScore*1.5);
	else if(highScore>GOLD_LEVEL3)
		gold = highScore*1;
	else if(highScore>GOLD_LEVEL4)
		gold = highScore*1;
	else
		gold = 0;

	int total_ms = gold * GOLD_PER_MS;
	int n = (total_ms%GOLD_NUMBER_MS)?1:0;
	m_nGoldStep = total_ms / GOLD_NUMBER_MS + n;//计算步长
	m_fFloridGoldMs = 1.0f*total_ms / m_nGoldStep;//计算粒子持续时间，可能需要稍微再减一点
	float floridLife = m_fFloridGoldMs/1000;
	m_gFloridGold1->setKeepTime(floridLife);
	m_gFloridGold2->setKeepTime(floridLife);
	m_gFloridGold3->setKeepTime(floridLife);
	m_gFloridGold4->setKeepTime(floridLife);

	m_gFloridLight->setKeepTime(floridLife);//考虑到粒子的时间
	//endSize 3->5
	m_fGoldBagFloridEndsize = m_gFloridLight->getParticleSystem()->m_info.fSizeEnd;
	m_fDeltaSize = (5.0f-m_fGoldBagFloridEndsize)/m_fFloridGoldMs;

	m_nGoldCur = 0;
	m_nGoldDest = gold;
	//由于时间好像不大够啊，，由于歌曲时间的限制，所以直接先保存一个最终的vector
	CAnimaNumber::DealWithNumber(m_nGoldDest,m_vectDest);
	m_nDigit = CAnimaNumber::CaculateDigit(m_nGoldDest);

	if((m_nGoldDest > 0) && cls_gl->m_pSceneMgr->m_bEffectGold)
		m_bStartGoldGetting = true;
}

void COS_NotifyMessage::setCenterNotifyAtomosphere(const int n)
{
	CAutoLock lock(&m_csCenter3);
	switch(n)
	{
	case NOTIFY_APPLAUD:
		{
			m_vectImgAnimaCom = m_vectImgAnimaApplaud;
			break;
		}
	case NOTIFY_CHEER:
		{
			m_vectImgAnimaCom = m_vectImgAnimaCheer;
			break;
		}
	default:
		{
			m_vectImgAnimaCom.clear();
			break;
		}
	}
	m_nAnimaI = 0;
	m_center_anima_singlems = 0;
	m_center_anima_last_ms = 0;
}

void COS_NotifyMessage::setCenterNotifyGrade(const int n)
{
	CAutoLock lock(&m_csCenter2);
	m_center_last_ms2 = 0;
	m_bDrawCenter2 = true;
	if (n>=NOTIFY_CLOSE_GRADE && n<=NOTIFY_THREE_GRADE)
	{
		m_ImgCenterCurrent2 = m_vectImgNotify[n];
	}
	else
	{
		m_bDrawCenter2 = false;
		m_ImgCenterCurrent = NULL;
	}
}

void COS_NotifyMessage::setCenterCurrentNotify(const int n,const int nValue)
{
	CAutoLock lock(&m_csCenter);
	m_center_last_ms = 0;
	m_bDrawCenter = true;
	switch(n)
	{
	case NOTIFY_MICVOLUME_SET:
		{
			m_ImgCenterCurrent = m_ImgMicVolumeBkg;
			m_fPreSheduleMic = m_fCurSheduleMic;
			m_fCurSheduleMic = 1.0f*(nValue-m_structSound.min_mic)/m_nMicRange;
			m_nSheduleValue = nValue;
			m_fCurShedule = m_fCurSheduleMic;
			break;
		}
	case NOTIFY_MUSICVOLUME_SET:
		{
			m_ImgCenterCurrent = m_ImgMusicVolumeBkg;
			m_fPreSheduleMusic = m_fCurSheduleMusic;
			m_fCurSheduleMusic = 1.0f*(nValue-m_structSound.min_music)/m_nMusicRange;
			m_nSheduleValue = nValue;
			m_fCurShedule = m_fCurSheduleMusic;
			break;
		}
	case NOTIFY_TONE_SET:
		{
			m_ImgCenterCurrent = m_ImgToneBkg;
			m_fPreSheduleTone = m_fCurSheduleTone;
			m_fCurSheduleTone = 1.0f*(nValue-m_structSound.min_tone)/m_nToneRange;
			m_nSheduleValue = nValue;
			m_fCurShedule = m_fCurSheduleTone;
			break;
		}
	default:
		{
			m_bDrawCenter = false;
			m_ImgCenterCurrent = NULL;
			break;
		}
	}
}

void COS_NotifyMessage::setLeftCurrentNotify(const int n)
{
	if ((n<0)||(n>=(int)m_vectImgNotify.size()))
	{
		return ;
	}
	CAutoLock lock(&m_csLeft);
	m_ImgLeftCurrent1 = m_vectImgNotify[n];
	//Image* ImgCurrent = m_vectImgNotify[n];
	m_left_last_ms = 0;

	if (m_nPreCurrent1 == NOTIFY_MUTE_OPEN)
	{//保存静音的状态
		m_nTop = 0;
		m_arraHistory[m_nTop] = m_nPreCurrent1;
	}
	else if (m_nPreCurrent1 == NOTIFY_PLAY_STOPPING)
	{//保存暂停的状态
		m_nTop = 1;
		m_arraHistory[m_nTop] = m_nPreCurrent1;
	}
	

	if (n == NOTIFY_PLAY_STOPPING || n == NOTIFY_MUTE_OPEN)
	{
		//一直显示，不需要定时
		m_bneed_addms = false;
		m_bLeftKeep = true;
		//m_bneed_return = true;
	}
	else
	{
		m_bneed_addms = true;
		m_bLeftKeep = false;
		//m_bneed_return = false;
		if (n == NOTIFY_MUTE_CLOSE)
		{//去掉保存静音的状态,并将置顶置为播放状态
			m_arraHistory[0] = NOTIFY_NONE;
			m_nTop = 1;
		}
		else if (n == NOTIFY_PLAY_RUNNING)
		{//去掉保存暂停的状态,并将置顶置为静音状态
			m_arraHistory[1] = NOTIFY_NONE;
			m_nTop = 0;
		}
	}
	//保存到上个状态中
	m_nPreCurrent1 = n;	
}

void COS_NotifyMessage::dealLeftNotifTimeOut()
{
	//if (m_bneed_return)
	{
		//查询置顶信息是否需要置顶显示
		int n = m_arraHistory[m_nTop];
		if (n == NOTIFY_NONE)
		{
			return ;
		}
		//m_nPreCurrent1 = m_nReturn;
		m_ImgLeftCurrent1 = m_vectImgNotify[n];
		m_bneed_addms = false;
		m_bLeftKeep = true;
		//m_bneed_return = false;
		m_left_last_ms = 0;
	}
}

void COS_NotifyMessage::drawGoldGetting()
{
/*
先出钱袋（从小到大，从下往上），就位后。

钱袋背后发光（粒子特效待定），金币开始掉落。。
钱袋上方数字走动。

掉落的过程中，钱袋背景光变大~ 

消失时，整体向下缩小消失
*/
	static int s_bag_width = m_ImgGoldBag->getWidth();
	static int s_bag_height = m_ImgGoldBag->getHeight();

	static int s_number_width = m_vectImgGoldNumber[0]->getWidth();
	static int s_number_height = m_vectImgGoldNumber[0]->getHeight();

	static float s_x = (cls_gl->m_winSize.width-s_bag_width)/2.0f;
	static float s_y = (float)cls_gl->m_winSize.height-s_bag_height;

	int total_number_width = m_nDigit*s_number_width;
	float number_pos_x = (cls_gl->m_winSize.width-total_number_width)/2.0f;

	//已经结束
	if(m_nGoldEffectLastMs >GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY+m_fFloridGoldMs+GLOD_BAG_KEEP+GOLD_BAG_DISAPPEAR_MS)
	{
		m_bStartGoldGetting = false;
		return ;
	}
	//bag消失
	else if(m_nGoldEffectLastMs >GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY+m_fFloridGoldMs+GLOD_BAG_KEEP )
	{
		float percent = 1.0f-1.0f*(m_nGoldEffectLastMs-GLOD_BAG_KEEP-GLOD_BAG_NUMBER_READY-GOLD_BAG_APPEAR_MS-m_fFloridGoldMs)/GOLD_BAG_DISAPPEAR_MS;
		
		int cur_number_width = (int)(s_number_width*percent);
		int cur_number_total_width = cur_number_width*m_nDigit;
		int cur_number_height = (int)(s_number_height*percent);
		int cur_bag_width = (int)(s_bag_width*percent);
		int cur_bag_height = (int)(s_bag_height*percent);

		float f_bag_pos_x = (cls_gl->m_winSize.width-cur_bag_width)/2.0f;
		float f_bag_pos_y = (float)cls_gl->m_winSize.height-cur_bag_height;

		float f_number_x = (cls_gl->m_winSize.width-cur_number_total_width)/2.0f;
		float f_number_y = f_bag_pos_y+cur_bag_height/2.0f-cur_number_height/2.0f;

		GColor cur_color(1.0f,1.0f,1.0f,percent);
		m_ImgGoldBag->draw(GPoint(f_bag_pos_x,f_bag_pos_y,GL3D_Z_NOTIFY),GSize(cur_bag_width,cur_bag_height)
			,NULL,true,cur_color);
		VECTORINT::const_iterator it = m_vectDest.begin();
		for (;it!=m_vectDest.end();it++)
		{
			const int& n = *it;
			m_vectImgGoldNumber[n]->draw(GPoint(f_number_x,f_number_y,GL3D_Z_NOTIFY),GSize(cur_number_width,cur_number_height)
				,NULL,true,cur_color);
			f_number_x += cur_number_width;
		}
	}
	//keep
	else if(m_nGoldEffectLastMs > GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY+m_fFloridGoldMs)
	{
		m_ImgGoldBag->draw(GPoint(s_x,s_y,GL3D_Z_NOTIFY));
		//描绘数字
		VECTORINT::const_iterator it = m_vectDest.begin();
		for(;it!=m_vectDest.end();it++)
		{
			int n = *it;
			m_vectImgGoldNumber[n]->draw(GPoint(number_pos_x,GOLD_NUMBER_POS_Y,GL3D_Z_NOTIFY));
			number_pos_x += s_number_width;
		}
	}
	//金币掉落特效
	else if(m_nGoldEffectLastMs > GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY)
	{
		m_ImgGoldBag->draw(GPoint(s_x,s_y,GL3D_Z_NOTIFY));

		if(m_nGoldNext == m_nGoldCur)//结束变化
		{
			//描绘数字
			VECTORINT::const_iterator it = m_vectGoldCur.begin();
			for(;it!=m_vectGoldCur.end();it++)
			{
				int n = *it;
				m_vectImgGoldNumber[n]->draw(GPoint(number_pos_x,GOLD_NUMBER_POS_Y,GL3D_Z_NOTIFY));
				number_pos_x += s_number_width;
			}
		}
		else//变化
		{
			VECTORINT vectNext;
			CAnimaNumber::DealWithNumber(m_nGoldNext,vectNext);
			CAnimaNumber::FillZeroNumber(vectNext,m_nDigit);

			assert(vectNext.size() == m_vectGoldCur.size());
			//描绘数字
			VECTORINT::const_iterator itCur = m_vectGoldCur.begin();
			VECTORINT::const_iterator itNext = vectNext.begin();
			for(;itCur!=m_vectGoldCur.end();itCur++,itNext++)
			{
				const int& n1 = *itCur;
				const int& n2 = *itNext;
				if(n1 == n2)
					m_vectImgGoldNumber[n1]->draw(GPoint(number_pos_x,GOLD_NUMBER_POS_Y,GL3D_Z_NOTIFY));
				else
				{
					int height2 = (int)(s_number_height*m_fGoldPercent);
					int height1 = s_number_height - height2;

					float pos_y = (float)GOLD_NUMBER_POS_Y+height1-s_number_height;
					if(height1)
					{
						CGRect clip_rect = CGRectMake(number_pos_x,GOLD_NUMBER_POS_Y,GL3D_Z_NOTIFY,s_number_width,height1);
						m_vectImgGoldNumber[n1]->draw(GPoint(number_pos_x,pos_y,GL3D_Z_NOTIFY),&clip_rect);
					}

					pos_y += s_number_height;
					
					if(height2)
					{
						CGRect clip_rect = CGRectMake(number_pos_x,pos_y,GL3D_Z_NOTIFY,s_number_width,height2);
						m_vectImgGoldNumber[n2]->draw(GPoint(number_pos_x,pos_y,GL3D_Z_NOTIFY),&clip_rect);
					}
				}

				number_pos_x += s_number_width;
			}
		}
	}
	//钱包,金币就位
	else if(m_nGoldEffectLastMs > GOLD_BAG_APPEAR_MS)
	{
		m_ImgGoldBag->draw(GPoint(s_x,s_y,GL3D_Z_NOTIFY));
		CAnimaNumber::FillZeroNumber(m_vectGoldCur,m_nDigit);
		for (int i=0;i<(int)m_vectGoldCur.size();i++)
		{
			m_vectImgGoldNumber[0]->draw(GPoint(number_pos_x,GOLD_NUMBER_POS_Y,GL3D_Z_NOTIFY));
			number_pos_x += s_number_width;
		}
	}
	//bag出现
	else
	{
		float percent = 1.0f*m_nGoldEffectLastMs/GOLD_BAG_APPEAR_MS;

		int cur_width = (int)(s_bag_width * percent);
		int cur_height = (int)(s_bag_height * percent*1.3f);
		float cur_x = (cls_gl->m_winSize.width-cur_width)/2.0f;
		float cur_y = (float)cls_gl->m_winSize.height-cur_height;
		m_ImgGoldBag->draw(GPoint(cur_x,cur_y,GL3D_Z_NOTIFY),GSize(cur_width,cur_height)
			,NULL,true,GColor(1.0f,1.0f,1.0f,percent));
	}
}

void COS_NotifyMessage::drawSlot(RenderEImage* img,const float fShedule,GPoint dest_point)
{
	if (m_ImgGLSH)
	{
		dest_point.x += 117;
		dest_point.y += 58.5;
		static int h = m_ImgGLSH->getHeight();
		int w = int(m_ImgGLSH->getWidth()*fShedule);
		m_ImgGLSH->draw(dest_point,GSize(w,h));
	}
}

void COS_NotifyMessage::drawValue(const int iValue,GPoint dest_point)
{
	if (m_vectImgNumber.empty())
	{
		return ;
	}
	
	dest_point.x += 480;
	dest_point.y += 48;
	static int s_w = m_vectImgNumber[0]->getWidth();
	static int s_h = m_vectImgNumber[0]->getHeight();
	dest_point.y -= s_h/2.0f;
	int tmp_value = iValue;
	tmp_value = tmp_value<0?-tmp_value:tmp_value;
	while(tmp_value)//反向描绘
	{
		int iImg = tmp_value%10;
		RenderEImage* img = m_vectImgNumber[iImg];
		if (img)
		{
			img->draw(dest_point);
		}

		dest_point.x -= s_w;
		tmp_value /= 10;
	}

	if (iValue == 0)
	{
		m_vectImgNumber[0]->draw(dest_point);
	}
	else if (iValue < 0)
	{
		if (m_ImgNagetive)
		{
			m_ImgNagetive->draw(dest_point);
		}
	}
}

void COS_NotifyMessage::updateNotifyTemplate(bool bNeedAddMS,ulong& nLastMS,const ulong delta_ms,const ulong total_ms,TIMEOUT_FUNC pFunc1)
{
	if (nLastMS >= total_ms)
	{
		if (pFunc1)
		{
			(this->*pFunc1)();
		}
		return ;
	}

	if (bNeedAddMS)
	{
		nLastMS += delta_ms;
	}
}

void COS_NotifyMessage::drawCenterNotify3()
{
	CAutoLock lock(&m_csCenter3);
	if (!m_vectImgAnimaCom.empty())
	{
		RenderEImage* img = m_vectImgAnimaCom[m_nAnimaI];
		static GPoint dest_point = CGPointMake(540,400,GL3D_Z_NOTIFY);
		if (img)
		{
// #ifdef SPECIAL_VERSION
// 			m_imgset->lockBatchBuffer();
// #endif
			img->draw(dest_point);
// #ifdef SPECIAL_VERSION
// 			m_imgset->unlockBatchBuffer();
// #endif
		}
	}
}

void COS_NotifyMessage::drawCenterNotify2()
{
	CAutoLock lock(&m_csCenter2);
	if (m_ImgCenterCurrent2)
	{
		static float pos_x = (cls_gl->m_winSize.width-m_ImgCenterCurrent2->getWidth())/2.0f;
		static float pos_y = 50;
		static GPoint dest_point = CGPointMake(pos_x,pos_y,GL3D_Z_NOTIFY);
		m_ImgCenterCurrent2->draw(dest_point);
	}
}

void COS_NotifyMessage::drawCenterNotify1()
{
	CAutoLock lock(&m_csCenter);
	if (m_ImgCenterCurrent)
	{
		//描绘底层图片
		static float pos_x = (cls_gl->m_winSize.width-m_ImgCenterCurrent->getWidth())/2.0f;
		static float pos_y = (cls_gl->m_winSize.height-m_ImgCenterCurrent->getHeight())/2.0f;
		static GPoint dest_point = CGPointMake(pos_x,pos_y,GL3D_Z_NOTIFY);
		m_ImgCenterCurrent->draw(dest_point);
		//描绘量度条
		drawSlot(m_ImgGLSH,m_fCurShedule,dest_point);
		//描绘具体值
		drawValue(m_nSheduleValue,dest_point);
	}
}

void	COS_NotifyMessage::drawLeftNotify()
{
	if (m_ImgLeftCurrent1)
	{
		//static float pos_x = (cls_gl->m_winSize.width-m_ImgLeftCurrent1->getWidth())/2.0f;
		static float pos_y = (cls_gl->m_winSize.height-m_ImgLeftCurrent1->getHeight())/2.0f;
		if (m_left_last_ms>=DISAPPEAR_MS)
		{
			GColor current_color(1.0f,1.0f,1.0f,1.0f-(m_left_last_ms-DISAPPEAR_MS)*1.0f/DISAPPEAR_TOTAL_MS);
			m_ImgLeftCurrent1->draw(GPoint(10,pos_y,GL3D_Z_NOTIFY),NULL,true,current_color);
		}
		else if (m_left_last_ms<=APPEAR_MS)
		{
			if (m_bLeftKeep)
			{
				m_ImgLeftCurrent1->draw(GPoint(10,pos_y,GL3D_Z_NOTIFY));
			}
			else
			{
				GColor current_color(1.0f,1.0f,1.0f,m_left_last_ms*1.0f/APPEAR_MS);
				m_ImgLeftCurrent1->draw(GPoint(10,pos_y,GL3D_Z_NOTIFY),NULL,true,current_color);
			}
		}
		else
		{
			m_ImgLeftCurrent1->draw(GPoint(10,pos_y,GL3D_Z_NOTIFY));
		}
	}
	if (m_ImgLeftCurrent2)
	{
		static float pos_x = (float)m_ImgLeftCurrent2->getWidth();
		static float pos_y = (cls_gl->m_winSize.height-m_ImgLeftCurrent2->getHeight())/2.0f;
		static GPoint dest_point = CGPointMake(pos_x,pos_y,GL3D_Z_NOTIFY);
		m_ImgLeftCurrent2->draw(dest_point);
	}
}

void COS_NotifyMessage::updateGoldGetting(ulong delta_ms)
{
	//已经结束
	if(m_nGoldEffectLastMs >GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY+m_fFloridGoldMs+GLOD_BAG_KEEP+GOLD_BAG_DISAPPEAR_MS)
	{
		m_bStartGoldGetting = false;
		return ;
	}
	//bag消失
	else if(m_nGoldEffectLastMs >GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY+m_fFloridGoldMs+GLOD_BAG_KEEP )
	{
	}
	//keep
	else if(m_nGoldEffectLastMs > GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY+m_fFloridGoldMs)
	{
		if(m_gSoundGold)
			m_gSoundGold->soundStop();
	}
	//金币掉落特效
	else if(m_nGoldEffectLastMs > GOLD_BAG_APPEAR_MS+GLOD_BAG_NUMBER_READY)
	{
		//背光粒子变大
		m_fGoldBagFloridEndsize += m_fDeltaSize*delta_ms;
		m_gFloridLight->getParticleSystem()->m_info.fSizeEnd = m_fGoldBagFloridEndsize;

		CAnimaNumber::SmoothNumber(m_nGoldDest,m_nGoldCur,m_nGoldNext,m_nNumberLastMs,GOLD_PER_MS
			,delta_ms,m_fGoldPercent,m_nGoldStep);

		CAnimaNumber::DealWithNumber(m_nGoldCur,m_vectGoldCur);
		CAnimaNumber::FillZeroNumber(m_vectGoldCur,m_nDigit);
	}
	//钱包,金币就位
	else if(m_nGoldEffectLastMs > GOLD_BAG_APPEAR_MS)
	{
	}
	//bag出现
	else
	{
		if(!m_bStartFlorid)
		{
			m_gFloridLight->startPS();
			m_gFloridGold1->startPS();
			m_gFloridGold2->startPS();
			m_gFloridGold3->startPS();
			m_gFloridGold4->startPS();
			if(m_gSoundGold)
				m_gSoundGold->soundPlay();
			m_bStartFlorid = true;
		}
	}

	m_nGoldEffectLastMs += delta_ms;
}

void COS_NotifyMessage::update(const ulong delta_ms)
{
	updateNotifyTemplate(m_bneed_addms,m_left_last_ms,delta_ms,TOTAL_MS,&COS_NotifyMessage::dealLeftNotifTimeOut);
	
	updateNotifyTemplate(true,m_center_last_ms,delta_ms,TOTAL_MS);
	updateNotifyTemplate(true,m_center_last_ms2,delta_ms,TOTAL_MS);

	updateNotifyTemplate(true,m_center_anima_last_ms,delta_ms,TOTAL_PNGARRAY_MS);
	if(!m_vectImgAnimaCom.empty())
	{
		m_center_anima_singlems += delta_ms;
		if (m_center_anima_singlems >= AVERAGE_PNGARRAY_MS)
		{
			m_nAnimaI ++;
			if (m_nAnimaI == 6)
			{
				m_nAnimaI = 0;
			}
			m_center_anima_singlems -= AVERAGE_PNGARRAY_MS;
		}
	}
	if(m_bStartGoldGetting)
		updateGoldGetting(delta_ms);
}
void COS_NotifyMessage::drawGraphBuffer()
{
	m_csLeft.Lock();
	if(m_left_last_ms< TOTAL_MS)
		drawLeftNotify();
	if (!m_bInitAccompany)
	{
		setLeftCurrentNotify(NOTIFY_ORIGIN_CLOSE);
		m_bInitAccompany = true;
	}
	m_csLeft.Unlock();
	
	if(m_center_last_ms<TOTAL_MS)
		drawCenterNotify1();
	if(m_center_last_ms2<TOTAL_MS)
		drawCenterNotify2();
	if(m_center_anima_last_ms<TOTAL_PNGARRAY_MS)
		drawCenterNotify3();
	
	//描绘金币获得信息
	if(m_bStartGoldGetting)
	{
		if (m_gFloridLight)
			m_gFloridLight->renderPS();
		drawGoldGetting();
	}
}

