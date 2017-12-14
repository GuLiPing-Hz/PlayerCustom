#include "stdafx.h"
#include "SceneMgr.h"
#include "../image/ImgsetMgr.h"
#include "../Timer.h"
#include "../fto/eval.h"
#include "../DX/Grapha.h"

#include "ObjectSing.h"
#include "ObjectFir.h"
#include "ObjectNotify.h"
#include "ObjectStatistics.h"
#include "ObjectVideo.h"

extern CWaveUnit*					g_WavRecorder;

_tSongInfo*		SceneMgr::s_si = NULL;

std::wstring SceneMgr::s_error_msg;

int SceneMgr::sNalgorithm;

unsigned long			SceneMgr::s_jump_begin_ms = 0;
unsigned long			SceneMgr::s_jump_end_ms = 0;

SceneMgr::SceneMgr(HWND hwnd,bool bLoop)
:curTimeInterval(0)
,m_bLoop(bLoop)
,m_durationMS(0)
,queue(NULL)
,Char(0)
,VKey(0)
,Zpos(0)
,Xpos(0.0f)
,Ypos(0.0f)
,bMouseOver(false)
,bCaptured(false)
,m_hwnd(hwnd)
,m_pIEval1(NULL)
,m_pIEval2(NULL)
,m_pGLFont(NULL)
,m_pFontRankinfo(NULL)
,m_bWaveConfig(false)
,CONFIG_PITCHSCOREPERCENT(1.0f)
,CONFIG_LYRICSCOREPERCENT(0.0f)
,m_bEffectGold(false)
,CONFIG_MATH_ADD(0)
{
	
	keyz[0] = 0;
	m_mapSceneObjs.clear();
	m_listObjsToAdd.clear();
	m_listObjsToRemove.clear();

	m_pIEval1 = createEval();
	m_pIEval2 = createEval();
	if (!m_pIEval1 || !m_pIEval2)
	{
		WLOGH_ERRO << L"IEval create error";
		return ;
	}
	sNalgorithm = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","ALGORITHM",1);
	CWave wave;
	wave.BuildFormat(1,44100,16);

	_tWaveFormat tmpWf = {0};
	memcpy(&tmpWf,&wave.GetFormat(),sizeof(_tWaveFormat));
	m_pIEval1->init((_eAlgorithmType)sNalgorithm,tmpWf,11025, 80, 50 );//init eval模块
	m_pIEval2->init((_eAlgorithmType)sNalgorithm,tmpWf,11025, 80, 50 );//init eval模块

	int bCombo = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","COMBO",0);
	m_pIEval1->setIfCombo(!!bCombo);
	m_pIEval2->setIfCombo(!!bCombo);

	int bSqrtX100 = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","SQRTX100",1);
	m_pIEval1->setSqrtX100(!!bSqrtX100);
	m_pIEval2->setSqrtX100(!!bSqrtX100);

	int bAutoAdjust = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","AUTOADJUST",0);
	m_pIEval1->setAutoAdjustTone(!!bAutoAdjust);
	m_pIEval2->setAutoAdjustTone(!!bAutoAdjust);

	int bDetectSilent = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","DETECSILENT",0);
	m_pIEval1->setDetectSilent(!!bDetectSilent);
	m_pIEval2->setDetectSilent(!!bDetectSilent);

	float fDetectSilent = Ini_GetFloat(cls_gl->getszIniFile(),"PARAM","DETECSILENTLIMIT",0.1f);
	m_pIEval1->SetParaCutNoise(fDetectSilent);
	m_pIEval2->SetParaCutNoise(fDetectSilent);

	int bEnableRecord = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","ENABLERECORD",0);
	m_pIEval1->enableRecord(!!bEnableRecord,!!bEnableRecord,!!bEnableRecord);
	m_pIEval2->enableRecord(!!bEnableRecord,!!bEnableRecord,!!bEnableRecord);

	int nAutoType = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","AUTOTYPE",0);
	float fAutoDiff = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","AUTODIFF",1.0f);
	float fAutoDuration = Ini_GetFloat(cls_gl->getszIniFile(),"PARAM","AUTODURATION",15000.0f);
	m_pIEval1->setAutoToneType((AUTOTONE_TYPE)nAutoType,fAutoDiff,fAutoDuration);
	m_pIEval2->setAutoToneType((AUTOTONE_TYPE)nAutoType,fAutoDiff,fAutoDuration);

	m_nLimitRecord = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","LIMITRECORDNUM",100);
	m_bEffectGold = !! Ini_GetInt(cls_gl->getszIniFile(),"PARAM","EFFECTGOLD",1);
	std::string	tmpStr = Ini_GetString(cls_gl->getszIniFile(),"DIR","RecordDir","");
	if (tmpStr == "")
	{
		WLOGH_ERRO << L"ini file read failed,RecordDir is NULL";
		return ;
	}
	m_record_dir = cls_gl->getAppDir() + tmpStr;

	CONFIG_FONT_SIZE = Ini_GetInt(cls_gl->getszIniFile(),"FONT","CONFIG_FONT_SIZE",0);
	if (CONFIG_FONT_SIZE == 0)
	{
		WLOGH_ERRO << L"ini file read failed,CONFIG_FONT_SIZE is 0";
		return ;
	}

	CONFIG_RECORD_SRCTYPE = Ini_GetInt(cls_gl->getszIniFile(),"MIXERSELECT","MIXERDEVUNIT",0);

	CONFIG_RECORD_DEVNAME = Ini_GetString(cls_gl->getszIniFile(),"MIXERSELECT","MIXERDEV","");

	m_bWaveConfig = loadWaveConfigure(cls_gl->getszIniFile());

	CONFIG_MONEYTOTWINKLE = Ini_GetInt(cls_gl->getszIniFile(),"SPECIALNOTIFY","MONEYTOTWINKLE",5000);

	CONFIG_LYRICSCORE_NOTIFY = Ini_GetInt(cls_gl->getszIniFile(),"SPECIALNOTIFY","LYRICSCORENOTIFY",500);

	CONFIG_PITCHSCOREPERCENT = Ini_GetFloat(cls_gl->getszIniFile(),"PARAM","PITCHSCOREPERCENT",0.6);
	CONFIG_LYRICSCOREPERCENT = 1.0f - CONFIG_PITCHSCOREPERCENT;
	if(CONFIG_LYRICSCOREPERCENT == 0.0f)
	{
		m_pIEval1->lyricEval(false);
		m_pIEval2->lyricEval(false);
	}

	CONFIG_MATH_ADD = Ini_GetInt(cls_gl->getszIniFile(),"PARAM","GRADEMATHADD",1);//GRADEMATHADD
}

SceneMgr::~SceneMgr()
{
	SAFE_DELETE(m_pFontRankinfo);
	SAFE_DELETE(m_pGLFont);
	removeAllObject();
	m_pIEval1->uninit();
	m_pIEval2->uninit();
	destroyEval(m_pIEval1);
	destroyEval(m_pIEval2);
}

bool SceneMgr::loadWaveConfigure(const char* iniFile)
{
	m_gWaveConfig.SING_START_B = Ini_GetFloat(iniFile,"GL", "SING_START_B",0.0f);
	if (m_gWaveConfig.SING_START_B == 0.0f)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	m_gWaveConfig.WAVEWND_WIDTH_TIMEMS=Ini_GetInt(iniFile,"GL","WAVEWND_WIDTH_TIMEMS",0);// = 10000;//2222;//5000;		// 一屏5000毫秒
	if (m_gWaveConfig.WAVEWND_WIDTH_TIMEMS == 0)
	{
		return false;
	}
	m_gWaveConfig.WAVEWND_WIDTH_PIXELS=Ini_GetInt(iniFile,"GL","WAVEWND_WIDTH_PIXELS",0);// 1280;//320;//720;		// 一屏宽度720像素
	if (m_gWaveConfig.WAVEWND_WIDTH_PIXELS == 0)
	{
		return false;
	}
	m_gWaveConfig.WAVEWND_HEIGHT_PIXELS=Ini_GetInt(iniFile,"GL","WAVEWND_HEIGHT_PIXELS",0);// 87;//79;		// 曲谱的高度40像素
	if (m_gWaveConfig.WAVEWND_HEIGHT_PIXELS == 0)
	{
		return false;
	}
	m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV=Ini_GetInt(iniFile,"GL","WAVEWND_HEIGHT_PIXELS_KTV",0);// 87;//105;	// 曲谱的高度40像素
	if (m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV == 0)
	{
		return false;
	}
	m_gWaveConfig.WAVEWND_LEFT_PIXELS=Ini_GetInt(iniFile,"GL","WAVEWND_LEFT_PIXELS",0);// 200;//170;		// 左边的像素
	if (m_gWaveConfig.WAVEWND_LEFT_PIXELS == 0)
	{
		return false;
	}
	// 	CURSOR_WIDTH_PIXELS=Ini_GetInt(m_szIniFile,"GL","CURSOR_WIDTH_PIXELS",0);// 36;			// 光标宽度
	// 	if (CURSOR_WIDTH_PIXELS == 0)
	// 	{
	// 		return false;
	// 	}
	m_gWaveConfig.WAVEWND_LEFT_TIMEMS=Ini_GetInt(iniFile,"GL","WAVEWND_LEFT_TIMEMS",0);// 1210;		// WAVEWND_WIDTH_TIMEMS * WAVEWND_LEFT_PIXELS / WAVEWND_WIDTH_PIXELS + 30;
	if (m_gWaveConfig.WAVEWND_LEFT_TIMEMS == 0)
	{
		return false;
	}
	// 	WAVEWND_WIDTH_PIXELS_KTV_TM=Ini_GetFloat("GL","WAVEWND_WIDTH_PIXELS_KTV_TM",0);// //指定谱线 像素/时间(毫秒) ktv 模式
	// 	if (WAVEWND_WIDTH_PIXELS_KTV_TM == 0)
	// 	{
	// 		return false;
	// 	}
	m_gWaveConfig.SHOW_TIP_IMAGE_MSEC=Ini_GetInt(iniFile,"GL","SHOW_TIP_IMAGE_MSEC",0);// 4000;		// 4000毫秒~2000毫秒	显示提示图标（图标闪烁）
	if (m_gWaveConfig.SHOW_TIP_IMAGE_MSEC == 0)
	{
		return false;
	}
	m_gWaveConfig.SHOW_MOVE_IMAGE_MSEC=Ini_GetInt(iniFile,"GL","SHOW_MOVE_IMAGE_MSEC",0);// 2000;		// 2000毫秒~0毫秒		显示光标滑动动画
	if (m_gWaveConfig.SHOW_MOVE_IMAGE_MSEC == 0)
	{
		return false;
	}
	m_gWaveConfig.NEED_SHOW_MOVE_IMAGE_MSEC=Ini_GetInt(iniFile,"GL","NEED_SHOW_MOVE_IMAGE_MSEC",0);// 500;	// >= 1000毫秒			需要显示光标滑动动画
	if (m_gWaveConfig.NEED_SHOW_MOVE_IMAGE_MSEC == 0)
	{
		return false;
	}
	// 估算16号字体的中文英文字符宽度
	m_gWaveConfig.CHAR_WIDTH_CHS=Ini_GetInt(iniFile,"GL","CHAR_WIDTH_CHS",0);// = 21;				// 中文字体宽度高度
	if (m_gWaveConfig.CHAR_WIDTH_CHS == 0)
	{
		return false;
	}
	m_gWaveConfig.CHAR_WIDTH_EN=Ini_GetInt(iniFile,"GL","CHAR_WIDTH_EN",0);// 12;						// 英文字体宽度高度
	if (m_gWaveConfig.CHAR_WIDTH_EN == 0)
	{
		return false;
	}
	m_gWaveConfig.MAX_LYRIC_WIDTH=Ini_GetInt(iniFile,"GL","MAX_LYRIC_WIDTH",0);
	if (m_gWaveConfig.MAX_LYRIC_WIDTH == 0)
	{
		return false;
	}
	m_gWaveConfig.VERTICAL_LINE_HEIGHT=Ini_GetFloat(iniFile,"GL","VERTICAL_LINE_HEIGHT",0);
	if (m_gWaveConfig.VERTICAL_LINE_HEIGHT == 0)
	{
		return false;
	}
	m_gWaveConfig.CONFIG_SPEED_PARTICLE = Ini_GetInt(iniFile,"PS","CONFIG_SPEED_PARTICLE",0);
	if (m_gWaveConfig.CONFIG_SPEED_PARTICLE == 0)
	{
		return false;
	}

	m_gWaveConfig.CONFIG_BARRAGEF_SIZE = Ini_GetInt(iniFile,"BARRAGE","CONFIG_FONT_SIZE",30);
	return true;
}

bool SceneMgr::initFont()
{
	if(m_pGLFont || m_pFontRankinfo)
	{
		assert(false);
		return false;
	}

	m_pGLFont = new CGLFont(CONFIG_FONT_SIZE);
	if (!m_pGLFont)
	{
		WLOGH_ERRO << L"m_pGLFont new failed";
		return false;
	}
	m_pGLFont->setBufferSize(600);
	m_pFontRankinfo = new CGLFont(20,L"zrank");
	m_pFontRankinfo->setBufferSize(800);
	if(!m_pFontRankinfo)
	{
		WLOGH_ERRO << L"Rank Info Font new efailed";
		return false;
	}
	std::wstring tmp_str = L"0123456789%的演唱在同歌曲中排名今日本店全国击败了当前包厢金币美女正在休息哦~";//+PERI_EMPTY_TIP
	tmp_str += WSTR_MONEYEXCHANGE_NOTIFY;
	m_pGLFont->addFont(tmp_str);
	if(m_pFontRankinfo->addFont(tmp_str))
	{
		bool b = m_pFontRankinfo->loadFont(cls_gl->getWinSize(),true,FONT_BANDING,SONGNAME_BANDCOLOR,TEXT_COLOR,2);
		if(!b)
		{
			WLOGH_ERRO << L"rank info font load error";
			return false;
		}
	}

	return true;
}

CGLFont* SceneMgr::getFont()
{
	ASSERT( m_pGLFont != NULL);
	return m_pGLFont;
}

bool SceneMgr::loadSongInfoFromDat( const std::string & file_name,int iDatOneLyricLength, _tDatOneLyric * fDatOneLyric
			 , int iDatSongPitchLength, _tDatPitchGroup * fDatSongPitch, int iAvSongSentenceLineLength
			 , unsigned long * plAvSongSentenceLine,unsigned int song_tm,_tSongInfo& song_info)
{
	float pitch_max = 0.0f;
	float pitch_min = 2147483647.0f;

	song_info.sentence_size = iAvSongSentenceLineLength;
	song_info.sentence_info = new _tSentenceInfo[song_info.sentence_size];
	ZeroMemory(song_info.sentence_info,sizeof(_tSentenceInfo)*iAvSongSentenceLineLength);

	if ( iDatSongPitchLength != iDatOneLyricLength )
		WLOGH_WARN << L"ERROR: iDatSongPitchLength != iDatOneLyricLength";

	unsigned int i = 0, m = 0;
	int j = 0, n = 0;

	for ( i = 0; i < song_info.sentence_size; ++i )
	{
		m = 0;

		_tSentenceInfo & sentence_info = song_info.sentence_info[i];
		sentence_info.endMsec = plAvSongSentenceLine[i];

		sentence_info.word_size = 0;
		sentence_info.word_list = 0;
		sentence_info.pitchgroup_size = 0;
		sentence_info.pitchgroup_list = 0;

		bool first_pitch = true;
		while ( j < iDatSongPitchLength )
		{
			_tDatOneLyric & l0 = fDatOneLyric[ j ];
			_tDatPitchGroup & pg0 = fDatSongPitch[ j ];

			++m;
			++j;
			if ( first_pitch )
			{
				sentence_info.beginMsec = pg0.pitch[ 0 ].tm;
				first_pitch = false;
			}
			if ( sentence_info.endMsec == pg0.pitch[ pg0.pitch_size - 1 ].tm )
			{
				sentence_info.word_size = m;
				sentence_info.word_list = new _tWordInfo[ m ];
				ZeroMemory(sentence_info.word_list,sizeof(_tWordInfo)*m);
				sentence_info.pitchgroup_size = m;
				sentence_info.pitchgroup_list = new _tPitchGroup[ m ];
				ZeroMemory(sentence_info.pitchgroup_list,sizeof(_tPitchGroup)*m);

				break;
			}
		}
	}
	song_info.begin_tm = song_info.sentence_info[0].beginMsec;
	song_info.end_tm = song_info.sentence_info[ song_info.sentence_size - 1 ].endMsec;

	j = 0;

	for ( i = 0; i < song_info.sentence_size; ++i )
	{
		_tSentenceInfo & sentence_info = song_info.sentence_info[i];

		for ( m = 0; m < sentence_info.pitchgroup_size; ++m )
		{
			_tWordInfo & l1 = sentence_info.word_list[ m ];
			_tPitchGroup & pg1  = sentence_info.pitchgroup_list[ m ];

			_tDatOneLyric & l0 = fDatOneLyric[ j ];
			_tDatPitchGroup & pg0 = fDatSongPitch[ j ];

			l1.begin_tm = pg0.pitch[0].tm;
			l1.end_tm = pg0.pitch[ pg0.pitch_size - 1 ].tm;

			l1.lyric_size = l0.lyric_size;
			l1.lyric = new char[l0.lyric_size];
			ZeroMemory(l1.lyric,l1.lyric_size);
			memcpy( l1.lyric, l0.lyric, l0.lyric_size );

			if ( ( pg0.pitch_size % 2 ) != 0 )
			{
				WLOGH_ERRO << L"pitch group size invalid!!!" ;
				return false;
			}

			int pitch_size = pg0.pitch_size / 2;
			pg1.pitch_size = pitch_size;
			pg1.pitch_list = new _tPitch[ pitch_size ];
			ZeroMemory(pg1.pitch_list,sizeof(_tPitch)*pitch_size);

			for ( n = 0; n < pitch_size; ++n )
			{
				pg1.pitch_list[ n ].begin_tm = pg0.pitch[ n * 2 ].tm;
				pg1.pitch_list[ n ].end_tm = pg0.pitch[ n * 2 + 1 ].tm;

				float val = 20*log10(pg0.pitch[ n * 2 ].pitch);

				if ( val > pitch_max )
				{
					pitch_max = val;
				}
				if ( val < pitch_min )
				{
					pitch_min = val;
				}

				if ( pg0.pitch[ n * 2 ].pitch != pg0.pitch[ n * 2 + 1 ].pitch )
				{
					WLOGH_ERRO << L"pitch value not invalid!!!" ;
					return false;
				}
				pg1.pitch_list[ n ].pitch = val;
			}

			++j;
		}
	}
	song_info.pitch_max = pitch_max;
	song_info.pitch_min = pitch_min;
	song_info.song_tm = song_tm;

	return true;
}

void SceneMgr::freeSongInfo()
{
	if (s_si)
	{
		for (unsigned int i = 0;i<s_si->sentence_size;i++)
		{
			_tSentenceInfo *sentenceInfo = s_si->sentence_info+i;
			if (sentenceInfo)
			{
				for (unsigned int j=0;j<sentenceInfo->word_size;j++)
				{
					_tWordInfo *wordInfo = sentenceInfo->word_list+j;
					if (wordInfo)
					{
						if (wordInfo->lyric)
						{
							delete [] wordInfo->lyric;
							wordInfo->lyric = NULL;
						}
					}
				}

				for (unsigned int j=0;j<sentenceInfo->pitchgroup_size;j++)
				{
					_tPitchGroup *pitchGroup = sentenceInfo->pitchgroup_list+j;

					if (pitchGroup)
					{
						if (pitchGroup->pitch_list)
						{
							delete [] pitchGroup->pitch_list;
							pitchGroup->pitch_list = NULL;
						}
					}

				}
				delete sentenceInfo->word_list;
				sentenceInfo->word_list = NULL;

				delete [] sentenceInfo->pitchgroup_list;
				sentenceInfo->pitchgroup_list = NULL;	
			}	

		}

		delete [] s_si->sentence_info;
		s_si->sentence_info = NULL;
	}
	s_si = NULL;
}

#define JUMP_BEGINEND_MS 3000

bool SceneMgr::loadSongInfo(const char* file,_tSongInfo& song_info)
{
	if(!file || file[0] == 0)
		return false;

	bool ret = false;
	if(strstr(file,".dat"))
	{
		int iRes = InitDATInfo( file,true );
		if (iRes == 0 )
		{ 
			_tDatOneLyric *fDatOneLyric = NULL;
			int iDatOneLyricLength;
			bool b = GetSongLyric(fDatOneLyric,iDatOneLyricLength);//读取歌曲的所有歌词信息
			if(!b)
			{
				return false;
			}

			_tDatPitchGroup*    fDatSongPitch;
			int iDatSongPitchLength;
			b = GetSongPitch(fDatSongPitch,iDatSongPitchLength);//读取歌曲的所有音符信息
			if(!b)
			{
				return false;
			}

			unsigned long * plAvSongSentenceLine = NULL;
			int iAvSongSentenceLineLength;
			b =GetSongSentenceLine(plAvSongSentenceLine,iAvSongSentenceLineLength);//读取歌曲的所有句信息
			if(!b)
			{
				return false;
			}

			unsigned long ulSongDuration = GetSongDuration();//读取歌曲的总时间，单位毫秒
			static int numberFinished = 0;
			numberFinished++;

			memset(&song_info,0,sizeof(song_info));
			s_si = &song_info;

			if(loadSongInfoFromDat(file,iDatOneLyricLength, fDatOneLyric, iDatSongPitchLength
				, fDatSongPitch, iAvSongSentenceLineLength, plAvSongSentenceLine,ulSongDuration,song_info))
			{
				ret =  true;
			}
		}
		else
			ret = false;
		ClearDatInfo();
	}

	return ret;
}

void SceneMgr::parseTime(unsigned long start,unsigned long end)
{
	if(cls_gl->m_gCurPlayInfo.m_bJump)
	{
		if(s_jump_begin_ms == 0)
		{
			if(start - JUMP_BEGINEND_MS> 0)
				s_jump_begin_ms = start-JUMP_BEGINEND_MS;
		}
		else
		{
			if(start-JUMP_BEGINEND_MS < s_jump_begin_ms)
				s_jump_begin_ms = start-JUMP_BEGINEND_MS;
		}

		if(s_jump_end_ms == 0)
		{
			unsigned long duration_ms;
			cls_gl->m_pGraph->getDuration(duration_ms);
			if(end+JUMP_BEGINEND_MS < duration_ms)
				s_jump_end_ms = end+JUMP_BEGINEND_MS;
		}
		else
		{
			if(end+JUMP_BEGINEND_MS > s_jump_end_ms)
				s_jump_end_ms = end+JUMP_BEGINEND_MS;
		}
	}
}

bool SceneMgr::loadSongInfo(const wchar_t* file,_tSongInfo& song_info)
{
	char gbkFile[260] = {0};//目前就当评分文件里面没有韩日文歌曲。
	::WideCharToMultiByte(CP_ACP,0,file,wcslen(file),gbkFile,259,NULL,NULL);
	return loadSongInfo(gbkFile,song_info);
}
bool SceneMgr::addObjectToAdd(ISceneObject* obj,bool bAwake)
{
	if(!obj)
		return false;

	if(bAwake)
	{
		if(!obj->awake(NULL))
			return false;
	}

	CAutoLock lock(&m_csObjectAdd);
	m_listObjsToAdd.push_back(obj);
	return true;
}
bool SceneMgr::addObjectToRemove(const char* name)
{
	if(!name)
		return false;

	CAutoLock lock(&m_csObjectRemove);
	m_listObjsToRemove.push_back(std::string(name));
	return true;
}

void SceneMgr::sleepObj(const char* name)
{
	if(!name)
		return ;
	CAutoLock lock(&m_csObjectSleep);
	m_listObjsToSleep.push_back(std::string(name));
}
void SceneMgr::resumeObj(const char* name)
{
	if(!name)
		return ;
	CAutoLock lock(&m_csObjectSleep);
	m_listObjsToResume.push_back(std::string(name));
}

void SceneMgr::removeCurObject()
{
	MAPSCENEOBJECT::iterator it = m_mapSceneObjs.begin();
	for (;it!=m_mapSceneObjs.end();it++)
	{
		if(it->second)
			delete(it->second);
	}
	m_mapSceneObjs.clear();
}

void SceneMgr::removeAllObject()
{
	addObjectToScene();
	removeObjectFromScene();
	removeCurObject();
}

void SceneMgr::addObjectToScene()
{
	CAutoLock lock(&m_csObjectAdd);
	 LISTSCENEOBJ::iterator it;
	 for (it=m_listObjsToAdd.begin();it!=m_listObjsToAdd.end();it++)
	 {
		 std::string name = (*it)->m_sName;
		 std::pair<MAPSCENEOBJECT::iterator,bool> pair_ret;
		 pair_ret = m_mapSceneObjs.insert(std::make_pair(name,*it));
		 if (!pair_ret.second)//没添加成功
			 SAFE_DELETE(*it);
	 }
	 m_listObjsToAdd.clear();
}

void SceneMgr::removeObjectFromScene()
{
	CAutoLock lock(&m_csObjectRemove);
	LISTSCENEOBJNAME::iterator it;
	for (it=m_listObjsToRemove.begin();it!=m_listObjsToRemove.end();it++)
	{
		MAPSCENEOBJECT::iterator map_it = m_mapSceneObjs.find(*it);
		if(map_it!=m_mapSceneObjs.end())
		{
			if(map_it->second)
			{
				//调用结束并释放
				map_it->second->end();
				delete (map_it->second);
			}
			//从map中删除
			m_mapSceneObjs.erase(*it);
		}
	}
	m_listObjsToRemove.clear();
}

CScoreFrame* SceneMgr::getScoreRender(bool first)
{
	MAPSCENEOBJECT::iterator it = m_mapSceneObjs.find(first?STATISTICSOBJF:STATISTICSOBJS);
	if(it != m_mapSceneObjs.end())
	{
		CStatisticsObject* pObj = (CStatisticsObject*)it->second;
		if(pObj)
			return pObj->getScoreFrameRender();
	}
		
	return NULL;
}

CTopNotifyRender* SceneMgr::getTopNotify()
{
	MAPSCENEOBJECT::iterator it = m_mapSceneObjs.find(FIROBJ);
	if(it != m_mapSceneObjs.end())
	{
		CFirObject* pObj = (CFirObject*)it->second;
		if(pObj)
			return pObj->getTopNotify();
	}
	return NULL;
}

COS_NotifyMessage* SceneMgr::getNotifyMsg()
{
	MAPSCENEOBJECT::iterator it = m_mapSceneObjs.find(NOTIFYOBJ);
	if(it != m_mapSceneObjs.end())
	{
		CNotifyObject* pObj = (CNotifyObject*)it->second;
		if(pObj)
			return pObj->getNotifyMessageRender();
	}

	return NULL;
}

void SceneMgr::resetDurationMS()
{
	m_durationMS = 0;
	SAFE_DELETE(m_pFontRankinfo);
	SAFE_DELETE(m_pGLFont);
	//m_nrealtime_ms = 0;
}

void	SceneMgr::realRender()
{
	MAPSCENEOBJECT::iterator it = m_mapSceneObjs.begin();
	for (;it!=m_mapSceneObjs.end();it++)
	{
		if (it->second)
			it->second->render();
	}
}

void SceneMgr::parseSongInfo(SingInfo& si1,SingInfo& si2,PlayInfo& pi1,PlayInfo& pi2)
{
	wchar_t mdmFile[260] = {0};
	wchar_t mdmFile2[260] = {0};
	const wchar_t* mdmName = cls_gl->m_gCurPlayInfo.m_isMV?NULL:cls_gl->m_gCurPlayInfo.m_strMdmName;
	if(mdmName)
	{
		wcscpy(mdmFile,mdmName);
		wcscpy(mdmFile2,mdmName);
	}
	else
	{
		wchar_t shortName[260] = {0};
		wmemcpy(shortName,cls_gl->m_gCurPlayInfo.m_cur_songFile,wcslen(cls_gl->m_gCurPlayInfo.m_cur_songFile)-4);
		swprintf(mdmFile,L"%s%s.mdm",shortName,cls_gl->m_gCurPlayInfo.m_bAntiphonalSinging?L"(1)":L"");
		swprintf(mdmFile2,L"%s%s.mdm",shortName,cls_gl->m_gCurPlayInfo.m_bAntiphonalSinging?L"(2)":L"");
	}

	wcscpy(si1.mdmFile,mdmFile);
	wcscpy(si1.nextSongName,cls_gl->m_next_songName.c_str());
	wcscpy(si1.singerName,cls_gl->m_gCurPlayInfo.m_cur_singerName);
	//wcscpy(si1.singerSingle,tmpwSinger1.c_str());
	wcscpy(si1.songName,cls_gl->m_gCurPlayInfo.m_cur_songName);

	pi1.start_ms = cls_gl->m_gCurPlayInfo.m_song_start_ms;
	pi1.end_ms = cls_gl->m_gCurPlayInfo.m_song_end_ms;
	pi1.eType = cls_gl->m_eShowType;
	pi1.POS_WAVE_HEIGHTSINGLE = cls_gl->m_gCurPlayInfo.m_nStaffHeight1;
	pi1.POS_WAVE_HEIGHTDOUBLE = cls_gl->m_gCurPlayInfo.m_nStaffHeight2;
	pi1.ktv_mode = false;

	memcpy(&si2,&si1,sizeof(SingInfo));
	memcpy(&pi2,&pi1,sizeof(PlayInfo));

	wcscpy(si2.mdmFile,mdmFile2);
	//wcscpy(si2.singerSingle,tmpwSinger2.c_str());
	pi2.POS_WAVE_HEIGHTDOUBLE = cls_gl->m_gCurPlayInfo.m_nStaffHeight2+SECONDWAVEHEIGHT;
}

bool SceneMgr::initSceneObj() throw(CPlayerException)
{
	bool b = true;
	//每次切换歌曲都要调成伴唱，如果是静音状态需要显示提示
	//提醒对象
	CNotifyObject* pNotify = new CNotifyObject(b,cls_gl->m_gCurPlayInfo.m_cur_songName
		,cls_gl->m_gCurPlayInfo.m_cur_singerName,cls_gl->m_next_songName
		,cls_gl->m_cSound,cls_gl->m_bAccompany,cls_gl->m_bMute);
	if(!pNotify)
	{
		s_error_msg = L"notify object new failed";
		goto failed;
	}
	if(!b || !addObjectToAdd(pNotify))
	{
		SAFE_DELETE(pNotify);
		goto failed;
	}

	//火警对象
	CFirObject* pFir = new CFirObject(b);
	if(!pFir)
	{
		s_error_msg = L"fire object new failed";
		goto failed;
	}
	if(!b || !addObjectToAdd(pFir))
	{
		SAFE_DELETE(pFir);
		goto failed;
	}

	CStatisticsObject* pSta1 = NULL;
	CStatisticsObject* pSta2 = NULL;
	if(cls_gl->m_gCurPlayInfo.m_isMV)//是MV的时候，添加结算对象
	{
		//结算对象1
		pSta1 = new CStatisticsObject(b,true);
		if(!pSta1)
		{
			s_error_msg = L"statistics object first new failed";
			goto failed;
		}
		if(!b || !addObjectToAdd(pSta1,false))
		{
			SAFE_DELETE(pSta1);
			goto failed;
		}
		//结算对象2
		pSta2 = new CStatisticsObject(b,false);
		if(!pSta2)
		{
			s_error_msg = L"statistics object second new failed";
			goto failed;
		}
		if(!b || !addObjectToAdd(pSta2,false))
		{
			SAFE_DELETE(pSta2);
			goto failed;
		}
	}
	
	//视频对象
	CVideoObject* pVideo = new CVideoObject(b);
	if(!pVideo)
	{
		s_error_msg = L"video object new failed";
		goto failed;
	}
	if(!b || !addObjectToAdd(pVideo))
	{
		SAFE_DELETE(pVideo);
		goto failed;
	}

	if(cls_gl->m_gCurPlayInfo.m_isMV)//是MV的时候添加唱歌对象
	{
		SingInfo si1 = {0};
		SingInfo si2 = {0};
		PlayInfo pi1 = {0};
		PlayInfo pi2 = {0};
		parseSongInfo(si1,si2,pi1,pi2);
		//唱歌对象1
		CSingObject* pSing1 = new CSingObject(b,&si1,&pi1);
		if(!pSing1)
		{
			s_error_msg = L"sing object first new failed";
			goto failed;
		}
		if(!b || !addObjectToAdd(pSing1))
		{
			SAFE_DELETE(pSing1);
			goto failed;
		}
		pSing1->setNextObj(pSta1);
		//唱歌对象2
		CSingObject* pSing2 = new CSingObject(b,&si2,&pi2,false);
		if(!pSing2)
		{
			s_error_msg = L"sing object second new failed";
			goto failed;
		}
		if(!b || !addObjectToAdd(pSing2))
		{
			SAFE_DELETE(pSing2);
			goto failed;
		}
		if(!cls_gl->m_gCurPlayInfo.m_bAntiphonalSinging)
			pSing2->sleep();
		pSing2->setNextObj(pSta2);
	}

	return true;

failed:
	throw CPlayerException(s_error_msg.c_str());
	return false;
}

//只取前两个object的分数,其实一共就两个
int SceneMgr::getFinalScoreStar(int &nFinalScoreF,int &nFinalScoreS)
{
	int nFinalScore1 = 0;
	int nFinalScore2 = 0;
	int nStar1 = 0;
	int nStar2 = 0;
	
	MAPSCENEOBJECT::iterator it = m_mapSceneObjs.find(std::string(SINGOBJF));
	if (it != m_mapSceneObjs.end())
	{
		int n = -1;
		if(it->second)
			n = ((CSingObject*)it->second)->getFinalScoreStar(nFinalScore1,nStar1);
		if (n == -1)
		{
			return n;
		}
	}

	it = m_mapSceneObjs.find(std::string(SINGOBJS));
	if (it != m_mapSceneObjs.end())
	{
		int n = -1;
		if(it->second)
			n = ((CSingObject*)it->second)->getFinalScoreStar(nFinalScore2,nStar2);
		if (n == -1)
		{
			return n;
		}
	}

	nFinalScoreF = nFinalScore1;
	nFinalScoreS = nFinalScore2;
	return 0;
}

void SceneMgr::gameLoopUpdate(ulong durationMS,unsigned int lastEslapedTime)
{
	if (m_bLoop)
	{
		//添加任何排队的场景对象
		addObjectToScene();

		{
			CAutoLock lock(&m_csObjectSleep);
			LISTSCENEOBJNAME::iterator it = m_listObjsToSleep.begin();
			for (;it!=m_listObjsToSleep.end();it++)
			{
				MAPSCENEOBJECT::iterator map_it = m_mapSceneObjs.find(*it);
				if(map_it!=m_mapSceneObjs.end())
				{
					if(map_it->second)
						map_it->second->sleep();
				}
			}
			m_listObjsToSleep.clear();
		}
// #ifdef SPECIAL_VERSION
// 		cls_gl->m_pImgsetMgr->lockAll();
// 		Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
// // 		Imageset* pGolBag = cls_gl->m_pImgsetMgr->getImageset(L"zsbag");
// // 		pImgset->lockBatchBuffer();
// // 		pGolBag->lockBatchBuffer();
// // 		m_pGLFont->lockBuffer();
// 		pImgset->lockBatchBufferSec();
// #endif//SPECIAL_VERSION

		m_durationMS = durationMS;

		realUpdate(lastEslapedTime);//ns_elapsed_ms
		if (cls_gl->m_pFloridMgr)
		{
			float second_elapsed = lastEslapedTime/1000.0f;//秒
			cls_gl->m_pFloridMgr->updateAll(second_elapsed);
		}

// #ifdef SPECIAL_VERSION
// 		cls_gl->m_pImgsetMgr->unlockAll();
// 		pImgset->unlockBatchBufferSec();
// // 		m_pGLFont->unlockBuffer();
// // 		pGolBag->unlockBatchBuffer();
// // 		pImgset->unlockBatchBuffer();
// #endif//SPECIAL_VERSION

		{
			CAutoLock lock(&m_csObjectResume);
			LISTSCENEOBJNAME::iterator it = m_listObjsToResume.begin();
			for (;it!=m_listObjsToResume.end();it++)
			{
				MAPSCENEOBJECT::iterator map_it = m_mapSceneObjs.find(*it);
				if(map_it!=m_mapSceneObjs.end())
				{
					if(map_it->second)
						map_it->second->resume();
				}
			}
			m_listObjsToResume.clear();
		}

		//删除任何需要删除的对象
	   removeObjectFromScene();
	}
}

void SceneMgr::renderScene()
{
#ifdef SPECIAL_VERSION
	cls_gl->m_pImgsetMgr-> renderAll();
#endif//SPECIAL_VERSION
}

bool SceneMgr::preparePlay(HWND hWnd,const wchar_t* mpgFile,GSize windowSize,CRenderEngine* pallocator)
{
	s_jump_begin_ms = 0;
	s_jump_end_ms = 0;

	std::wstring error_msg;
	//播放与录音
	CGraph* pCGraph = cls_gl->m_pGraph;
	if (!pCGraph)
	{
		error_msg = L"CGraph::getGraphInstance is null";
		goto failed;
	}

	try
	{
		if (cls_gl->m_gCurPlayInfo.m_isMV || cls_gl->m_gCurPlayInfo.m_bAdvertisement)
			pCGraph->gStartGraph(hWnd,mpgFile,windowSize,pallocator);
	}
	catch (CPlayerException& ex)
	{
		throw ex;
		return false;
	}

	//如果是评分歌曲，并且最后需要显示评分结果则打开录音,
	if (g_WavRecorder&&cls_gl->m_gCurPlayInfo.m_isMV&&cls_gl->m_gCurPlayInfo.m_bGradeSong&&cls_gl->m_gCurPlayInfo.m_bGrade)
	{
		if (CONFIG_RECORD_DEVNAME == "" || CONFIG_RECORD_SRCTYPE == 0)
		{
			if(!g_WavRecorder->RecordOpen())
			{
				error_msg = L"record open error";
				goto failed;
			}
		}
		else
		{
			const MAPSTRDEVID map_devid = cls_gl->m_mapmx_str_id;
			MAPSTRDEVID::const_iterator i = map_devid.find(CONFIG_RECORD_DEVNAME);
			if (i!=map_devid.end())
			{
				if (!g_WavRecorder->RecordOpen(hWnd,CONFIG_RECORD_SRCTYPE,i->second,CONFIG_RECORD_DEVNAME.c_str()))
				{
					error_msg = L"record open error";
					goto failed;
				}
			}
			else
			{
				if(!g_WavRecorder->RecordOpen())
				{
					error_msg = L"record open error";
					goto failed;
				}
			}
		}
		g_WavRecorder->setRecordFlag(true);
	}

	return true;

failed:
	throw CPlayerException(error_msg.c_str());
	return false;
}

bool SceneMgr::readyPlay(bool bAccompany,bool bMute)
{
	if(!cls_gl->m_gCurPlayInfo.m_isMV&&!cls_gl->m_gCurPlayInfo.m_bAdvertisement)
		return true;

	if (g_WavRecorder&&cls_gl->m_gCurPlayInfo.m_isMV&&cls_gl->m_gCurPlayInfo.m_bGradeSong&&cls_gl->m_gCurPlayInfo.m_bGrade)
	{//是MV才需要录音
		//开始录音
		g_WavRecorder->RecordStart();
	}
	
	if(cls_gl->m_gCurPlayInfo.m_bJump)
	{
		if(s_jump_end_ms == 0)
			cls_gl->m_pGraph->getDuration(s_jump_end_ms);

		int ret = cls_gl->m_pGraph->setStartStopPosition(s_jump_begin_ms,s_jump_end_ms);
	}
	//开始播放,
	SAFEPOINTER_CALL(cls_gl->m_pGraph)ktvStartPlayer(!bAccompany);
	if (bMute)
		SAFEPOINTER_CALL(cls_gl->m_pGraph)setVolume(-10000);
	else
	{
		if(cls_gl->m_cSound.cur_music != cls_gl->m_cSound.max_music)
		{
			float f = 1.0f*(cls_gl->m_cSound.cur_music-cls_gl->m_cSound.min_music)
				/(cls_gl->m_cSound.max_music-cls_gl->m_cSound.min_music);
			SAFEPOINTER_CALL(cls_gl->m_pGraph)setVolume((long)(f*10000-10000));
		}
	}
	return true;
}

void SceneMgr::realtimeEval(eShowWave eType,unsigned int nDurationMs,bool bSecond)
{
	CAutoLock lock(&cls_gl->m_endScoreLock);
	if (!cls_gl->m_bShowEndScore)
	{
		MAPSCENEOBJECT::iterator it = m_mapSceneObjs.find(SINGOBJF);
		if(it != m_mapSceneObjs.end())
		{
			if(it->second)
				((CSingObject*)it->second)->realtime(m_durationMS,eType);
		}
		
		if (bSecond)
		{
			MAPSCENEOBJECT::iterator it = m_mapSceneObjs.find(SINGOBJS);
			if(it != m_mapSceneObjs.end())
			{
				if(it->second)
					((CSingObject*)it->second)->realtime(m_durationMS,eType);
			}
		}
	}
}

void SceneMgr::realUpdate(unsigned int ms)
{
	MAPSCENEOBJECT::iterator it = m_mapSceneObjs.begin();
	for (;it!=m_mapSceneObjs.end();it++)
	{
		if (it->second)
		{
			it->second->setCurMs(m_durationMS);
			it->second->update(ms);
		}
	}
}

void SceneMgr::getFileListFromDir(std::string dir, std::string pattern, std::list<std::string> &files)
{
	WIN32_FIND_DATAA FindFileData;

	HANDLE hFind = FindFirstFileA((dir+"\\*.*").c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	do {
		if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			//printf("+[%s]",find.cFileName);
			if(FindFileData.cFileName[0] != '.')
			{
				char pszTemp[MAX_PATH]={0};
				sprintf(pszTemp,"%s\\%s",dir.c_str(),FindFileData.cFileName);
				std::string tmpDir = pszTemp;
				//printf("%s\n",pszTemp);
				getFileListFromDir(tmpDir,pattern,files);
			}

		}
		else
		{
			if (strstr(FindFileData.cFileName,pattern.c_str()))
			{
				std::string str_file = dir + "\\" + FindFileData.cFileName;
				files.push_back(str_file);
			}
		}

	}
	while (FindNextFileA(hFind, &FindFileData) != 0);
	FindClose(hFind);
}

void SceneMgr::endModel()
{
	SAFEPOINTER_CALL(cls_gl->m_pGraph)stopPlayer();
	if (g_WavRecorder && g_WavRecorder->IsRecording())
	{
		//把多的录音文件删除
		std::list<std::string> files;
		getFileListFromDir(m_record_dir,".wav",files);
		while(files.size() > m_nLimitRecord)
		{
			DeleteFile(files.front().c_str());
			files.pop_front();
		}

		g_WavRecorder->RecordStop();
		char recordFile[260] ={0};
		SYSTEMTIME st;
		GetLocalTime(&st);
		bool bNeedSave = false;
		if (!m_mapSceneObjs.empty())
		{
			MAPSCENEOBJECT::iterator i=m_mapSceneObjs.find(std::string(SINGOBJF));
			sprintf_s(recordFile,"%s\\%04d-%02d-%02d_%02d-%02d",m_record_dir.c_str()
				,st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
			if(i!=m_mapSceneObjs.end())
			{
				//歌曲信息，以及第一个分数
				if(i->second)
				{
					int destScore = ((CSingObject*)i->second)->m_ave_dest_score;
					sprintf_s(recordFile,"%s_%s-%s_%0.1f",recordFile,cls_gl->m_gCurPlayInfo.m_cur_songName
					,cls_gl->m_gCurPlayInfo.m_cur_singerName,destScore/10.0f);
					if(destScore > 500)
						bNeedSave = true;
				}
			}

			i=m_mapSceneObjs.find(std::string(SINGOBJS));
			if(i!=m_mapSceneObjs.end())
			{
				//第二个分数
				if(i->second)
				{
					int destScore = ((CSingObject*)i->second)->m_ave_dest_score;
					sprintf_s(recordFile,"%s_%0.1f",recordFile,destScore/10.0f);
					if(destScore > 500)
						bNeedSave = true;
				}
			}
		}
		else
			strcpy_s(recordFile,10,"record");

		if(bNeedSave)
		{
			sprintf_s(recordFile,"%s.wav",recordFile);
			g_WavRecorder->SaveWavFileFromWaveIn(recordFile);
		}
		g_WavRecorder->RecordClose();
	}
}


// these methods are copied over from the EAGLView template

void SceneMgr::startAnimation()
{
	m_bLoop = true;
}

void SceneMgr::stopAnimation()
{
	m_bLoop = false;
	//self.animationTimer = NULL;
}
//////////////////////////////////////////////////////////////////////////
static char *KeyNames[] =
{
	"?",
	"Left Mouse Button", "Right Mouse Button", "?", "Middle Mouse Button",
	"?", "?", "?", "Backspace", "Tab", "?", "?", "?", "Enter", "?", "?",
	"Shift", "Ctrl", "Alt", "Pause", "Caps Lock", "?", "?", "?", "?", "?", "?",
	"Escape", "?", "?", "?", "?",
	"Space", "Page Up", "Page Down", "End", "Home",
	"Left Arrow", "Up Arrow", "Right Arrow", "Down Arrow",
	"?", "?", "?", "?", "Insert", "Delete", "?",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"?", "?", "?", "?", "?", "?", "?",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"Left Win", "Right Win", "Application", "?", "?",
	"NumPad 0", "NumPad 1", "NumPad 2", "NumPad 3", "NumPad 4",
	"NumPad 5", "NumPad 6", "NumPad 7", "NumPad 8", "NumPad 9",
	"Multiply", "Add", "?", "Subtract", "Decimal", "Divide",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"Num Lock", "Scroll Lock",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"Semicolon", "Equals", "Comma", "Minus", "Period", "Slash", "Grave",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?",
	"Left bracket", "Backslash", "Right bracket", "Apostrophe",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
	"?", "?", "?"
};
bool  SceneMgr::Input_GetEvent(hgeInputEvent *event)
{
	CInputEventList *eptr;

	if(queue)
	{
		eptr=queue;
		memcpy(event, &eptr->event, sizeof(hgeInputEvent));
		queue=eptr->next;
		delete eptr;
		return true;
	}

	return false;
}

void  SceneMgr::Input_GetMousePos(float *x, float *y)
{
	*x=Xpos; *y=Ypos;
}


void  SceneMgr::Input_SetMousePos(float x, float y)
{
	POINT pt;
	pt.x=(long)x; pt.y=(long)y;
	ClientToScreen(m_hwnd, &pt);
	SetCursorPos(pt.x,pt.y);
}

int  SceneMgr::Input_GetMouseWheel()
{
	return Zpos;
}

bool  SceneMgr::Input_IsMouseOver()
{
	return bMouseOver;
}

bool  SceneMgr::Input_GetKeyState(int key)
{
	return ((GetKeyState(key) & 0x8000) != 0);
}

bool  SceneMgr::Input_KeyDown(int key)
{
	return (keyz[key] & 1) != 0;
}

bool  SceneMgr::Input_KeyUp(int key)
{
	return (keyz[key] & 2) != 0;
}

char*  SceneMgr::Input_GetKeyName(int key)
{
	return KeyNames[key];
}

int  SceneMgr::Input_GetKey()
{
	return VKey;
}

int  SceneMgr::Input_GetChar()
{
	return Char;
}


//////// Implementation ////////
void SceneMgr::_InputInit()
{
	POINT	pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hwnd, &pt);
	Xpos = (float)pt.x;
	Ypos = (float)pt.y;

	memset(&keyz, 0, sizeof(keyz));
}

void SceneMgr::_UpdateMouse()
{
	POINT	pt;
	RECT	rc;

	GetCursorPos(&pt);
	GetClientRect(m_hwnd, &rc);
	MapWindowPoints(m_hwnd, NULL, (LPPOINT)&rc, 2);//2代表，一个rect包含2个点

	if(bCaptured || (PtInRect(&rc, pt) && WindowFromPoint(pt)==m_hwnd))
		bMouseOver=true;
	else
		bMouseOver=false;
}

void SceneMgr::_BuildEvent(int type, int key, int scan, int flags, int x, int y)
{
	CInputEventList *last, *eptr=new CInputEventList;
	unsigned char kbstate[256];
	POINT pt;

	eptr->event.type=type;
	eptr->event.chr=0;
	pt.x=x; pt.y=y;

	GetKeyboardState(kbstate);
	if(type==INPUT_KEYDOWN)
	{
		if((flags & HGEINP_REPEAT) == 0) keyz[key] |= 1;
		ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
	}
	if(type==INPUT_KEYUP)
	{
		keyz[key] |= 2;
		ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
	}
	if(type==INPUT_MOUSEWHEEL)
	{
		eptr->event.key=0; eptr->event.wheel=key;
		ScreenToClient(m_hwnd,&pt);
	}
	else { eptr->event.key=key; eptr->event.wheel=0; }

	if(type==INPUT_MBUTTONDOWN)
	{
		keyz[key] |= 1;
		SetCapture(m_hwnd);
		bCaptured=true;
	}
	if(type==INPUT_MBUTTONUP)
	{
		keyz[key] |= 2;
		ReleaseCapture();
		Input_SetMousePos(Xpos, Ypos);
		pt.x=(int)Xpos; pt.y=(int)Ypos;
		bCaptured=false;
	}

	if(kbstate[VK_SHIFT] & 0x80) flags|=HGEINP_SHIFT;
	if(kbstate[VK_CONTROL] & 0x80) flags|=HGEINP_CTRL;
	if(kbstate[VK_MENU] & 0x80) flags|=HGEINP_ALT;
	if(kbstate[VK_CAPITAL] & 0x1) flags|=HGEINP_CAPSLOCK;
	if(kbstate[VK_SCROLL] & 0x1) flags|=HGEINP_SCROLLLOCK;
	if(kbstate[VK_NUMLOCK] & 0x1) flags|=HGEINP_NUMLOCK;
	eptr->event.flags=flags;

	if(pt.x==-1) { eptr->event.x=Xpos;eptr->event.y=Ypos; }
	else
	{
		if(pt.x<0) pt.x=0;
		if(pt.y<0) pt.y=0;
		if(pt.x>=cls_gl->getWinSize().width) pt.x=cls_gl->getWinSize().width-1;
		if(pt.y>=cls_gl->getWinSize().height) pt.y=cls_gl->getWinSize().height-1;

		eptr->event.x=(float)pt.x;
		eptr->event.y=(float)pt.y;
	}

	eptr->next=0; 

	if(!queue) queue=eptr;
	else
	{
		last=queue;
		while(last->next) last=last->next;
		last->next=eptr;
	}

	if(eptr->event.type==INPUT_KEYDOWN || eptr->event.type==INPUT_MBUTTONDOWN)
	{
		VKey=eptr->event.key;Char=eptr->event.chr;
	}
	else if(eptr->event.type==INPUT_MOUSEMOVE)
	{
		Xpos=eptr->event.x;Ypos=eptr->event.y;
	}
	else if(eptr->event.type==INPUT_MOUSEWHEEL)
	{
		Zpos+=eptr->event.wheel;
	}
}

void SceneMgr::_ClearQueue()
{
	CInputEventList *nexteptr, *eptr=queue;

	memset(&keyz, 0, sizeof(keyz));

	while(eptr)
	{
		nexteptr=eptr->next;
		delete eptr;
		eptr=nexteptr;
	}

	queue=0; VKey=0; Char=0; Zpos=0;
}




