#include "stdafx.h"
#include "ObjectSing.h"
#include "../render/LyricRender.h"
#include "../render/WaveRender.h"
#include "../render/SentenceRender.h"

extern CCritSec								g_csGrade;
extern LISTGLREALTIMEGRADE	g_listRGradeLeft;
extern LISTGLREALTIMEGRADE	g_listRGradeRight;
extern LISTGLSENTENCEGRADE	g_listSGradeLeft;
extern LISTGLSENTENCEGRADE	g_listSGradeRight;
extern HWND g_hWnd;

#define JUMP_BEGINEND_MS 3000

CSingObject::CSingObject(bool& bInit,SingInfo* pSingInfo,PlayInfo* pPlayInfo,bool bFirst)
:ISceneObject(bFirst?SINGOBJF:SINGOBJS)
,mnCallbackRealtimeGradeCount(0)
,mnCallbackSentenceGradeCount(0)
,miLastElapsedCount(0)										
,m_bIsCallback(false)
,mbChangeMode(false)		
,m_bShowEndScore(false)
,mCurTime(0)
,mfKtvErr(0.0f)//容错像素值
,mfGameErr(7.0f)//容错像素值
,m_iCurSentence(0)
,cursor_start_pos(0)
,cursor_start_pos_next(0)
,cursor_end_pos(0)
,cursor_end_pos_next(0)
,cur_sentence_start_tm(0)
,cur_sentence_end_tm(0)
,cur_sentence_ahead_tm(3000)
,m_pre_left_pitch_count(0)
,m_star_number(0)
,m_bAccompany(true)
,m_pIEval(NULL)
,m_bLoadFromDat(false)
,m_nsentence(0)
,mbInterpolation(true)
,wave_render(NULL)
,sentence_render(NULL)
,lyric_render(NULL)
,m_bFirst(bFirst)
,m_ave_dest_score(0)
{
	bInit = true;
	assert(cls_gl != NULL);

	if (pSingInfo)
		memcpy(&m_gSingInfo,pSingInfo,sizeof(SingInfo));
	else
		bInit = false;
	if(pPlayInfo)
		memcpy(&m_gPlayInfo,pPlayInfo,sizeof(PlayInfo));
	else
		bInit = false;

	m_wcur_song_name = pSingInfo->songName;
	m_wcur_singer_name = pSingInfo->singerName;
	m_next_song_name = pSingInfo->nextSongName;
	m_cur_singer_part = pSingInfo->singerSingle;
	m_bAS = cls_gl->m_gCurPlayInfo.m_bAntiphonalSinging;

	ZeroMemory(&m_CurGrade,sizeof(m_CurGrade));
	ZeroMemory(&m_songInfo,sizeof(m_songInfo));
	ZeroMemory(&mPreSentence_info,sizeof(mPreSentence_info));
	ZeroMemory(&mCurSentence_info,sizeof(mCurSentence_info));
	ZeroMemory(&mNextSentence_info,sizeof(mNextSentence_info));

	if (m_bFirst)
	{
		if (m_gPlayInfo.eType == SHOW_ONE)
			POS_WAVE_HEIGHT = m_gPlayInfo.POS_WAVE_HEIGHTSINGLE;
		else
			POS_WAVE_HEIGHT = m_gPlayInfo.POS_WAVE_HEIGHTDOUBLE;
	}
	else
		POS_WAVE_HEIGHT = m_gPlayInfo.POS_WAVE_HEIGHTDOUBLE;

	bool tmp_bInit =  false;
	if (m_bFirst || m_bAS)
	{
		lyric_render = new LyricRender(this,m_gPlayInfo.eType,m_bAS,m_bFirst,POS_WAVE_HEIGHT,cls_gl->m_pSceneMgr->m_gWaveConfig.SING_START_B
			,cls_gl->m_pSceneMgr->m_gWaveConfig.MAX_LYRIC_WIDTH,m_gPlayInfo.ktv_mode);
		if(!lyric_render)
		{
			SceneMgr::s_error_msg = L"lyric_render new failed";
			bInit = false;
		}
	}

	wave_render = new WaveRender(tmp_bInit,this,m_bFirst,cls_gl->m_pSceneMgr->m_gWaveConfig.SING_START_B
		,POS_WAVE_HEIGHT,cls_gl->m_pSceneMgr->m_gWaveConfig.MAX_LYRIC_WIDTH
		,cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV,m_gPlayInfo.ktv_mode,cls_gl->m_pSceneMgr->m_gWaveConfig.CONFIG_SPEED_PARTICLE);
	if (!wave_render)
	{
		SceneMgr::s_error_msg = L"wave_render new failed";
		bInit = false;
	}	

	if(!tmp_bInit)
		bInit = false;

	sentence_render = new SentenceRender(tmp_bInit,this,m_gPlayInfo.eType,m_bFirst,cls_gl->m_winSize,m_wcur_song_name
		,m_wcur_singer_name,m_cur_singer_part,(float)POS_WAVE_HEIGHT,(float)cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV
		,cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_LEFT_PIXELS,m_gPlayInfo.ktv_mode);
	if (!sentence_render)
	{
		SceneMgr::s_error_msg = L"sentence_render new failed";
		bInit = false ;
	}

	if(!tmp_bInit)
		bInit = false;

	m_GuiLyricVect.clear();
	m_GuiPitchVect.clear();
	m_GuiSentencelineVect.clear();
	m_ParagraphVect.clear();

	m_tmpgui_lyric_list.clear();
	m_tmpgui_pitch_list.clear();

	setFont(cls_gl->m_pSceneMgr->m_pGLFont);

	if(m_bFirst)
		setIEval(cls_gl->m_pSceneMgr->m_pIEval1);
	else
		setIEval(cls_gl->m_pSceneMgr->m_pIEval2);
}
CSingObject::~CSingObject()
{
	SceneMgr::freeSongInfo();
	if(!m_bLoadFromDat)
	{
		if (m_pIEval)
		{
			m_pIEval->unloadSongInfo(m_songInfo);
		}
	}

	m_GuiLyricVect.clear();
	m_GuiPitchVect.clear();
	m_GuiSentencelineVect.clear();
	m_ParagraphVect.clear();
	m_tmpgui_lyric_list.clear();
	m_tmpgui_pitch_list.clear();

	SAFE_DELETE(lyric_render);
	SAFE_DELETE(wave_render);
	SAFE_DELETE(sentence_render);
}

// 生成谱线和歌词
void CSingObject::createWaveLyric(const _tSongInfo& song_info)
{   
	m_GuiLyricVect.clear();
	m_GuiPitchVect.clear();
	m_GuiSentencelineVect.clear();
	m_ParagraphVect.clear();

	unsigned int sentence_count = song_info.sentence_size;
	_tSentenceInfo * sentences = song_info.sentence_info;

	// 生成歌词及位置信息（位置为的像素值）但是不包含数据库传递的高度值
	float pos = .0f;

	for ( unsigned int i = 0; i < sentence_count; ++i )
	{
		const _tSentenceInfo & sentence = sentences[ i ];

		for ( unsigned int j = 0; j < sentence.word_size; ++j )
		{
			const _tWordInfo & word_info = sentence.word_list[ j ];
			if ( word_info.lyric_size )
			{
				//_tGuiLyricInfo * gui_lyric_info = new _tGuiLyricInfo;
				_tGuiLyricInfo gui_lyric_info;
				wchar_t lyricBuf[50] = {0};
				static char szLyric[50] = { 0 };
				memcpy(szLyric,word_info.lyric,word_info.lyric_size);
				szLyric[word_info.lyric_size] = 0;

				MultiByteToWideChar(CP_ACP, 0, szLyric, -1, lyricBuf, 49);//一个汉字一个宽字节，英文不超过49
				gui_lyric_info.lyric	= lyricBuf;
				gui_lyric_info.pos	= 1.0f*word_info.begin_tm * cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS / cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_TIMEMS;
				// 如果通过实践计算出的字符位置与前一个字符重叠，校正
				if ( gui_lyric_info.pos < pos )
				{
					gui_lyric_info.pos = pos;
				}
				else
				{
					// 避免与前一个字符重叠
					pos = gui_lyric_info.pos+1.0f;
				}
				m_GuiLyricVect.push_back(gui_lyric_info);

				for ( unsigned int k = 0; k < word_info.lyric_size; ++ k )
				{
					pos += cls_gl->m_pSceneMgr->getFont()->getConfigFontSize();
				}
			}
		}
	}

	// 生成谱线句尾线及位置信息（位置为全局的像素值）但是不包含数据库传递的高度值
	for ( unsigned int i = 0; i < sentence_count; ++i )
	{
		_tGuiWaveInfo gui_pitch;
		const _tSentenceInfo & sentence = sentences[ i ];

		for ( unsigned int j = 0; j < sentence.pitchgroup_size; ++j )
		{
			const _tPitchGroup & pitch_group = sentence.pitchgroup_list[j];

			for ( unsigned int k = 0; k < pitch_group.pitch_size; ++k )
			{
				//gui_pitch = new _tGuiWaveInfo;//[[_tGuiWaveInfo alloc] init];
				const _tPitch & pitch_info = pitch_group.pitch_list[ k ];
				gui_pitch.l = 1.0f*pitch_info.begin_tm * cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS / cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_TIMEMS;
				gui_pitch.r = 1.0f*pitch_info.end_tm * cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS / cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_TIMEMS;
				//处理成相对位置*************************************glp
				gui_pitch.y = (1.0f*( cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV-16 ) * ( song_info.pitch_max - pitch_info.pitch ) / ( song_info.pitch_max - song_info.pitch_min ) );
				m_GuiPitchVect.push_back(gui_pitch);
			}
		}

		// 最后谱线的结尾应该是句结尾
		float _sentenceline_pos = 1.0f*sentence.endMsec * cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS / cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_TIMEMS;
		m_GuiSentencelineVect.push_back(_sentenceline_pos);
	}

	// 设置段落(需要提前显示歌词并提示)
	unsigned long end_msec = 0;			// 第一句，上句结束时间为0；第二句上句时间为第一句的结束时间
	for ( unsigned int i = 0; i < sentence_count; ++i )
	{
		const _tSentenceInfo & sentence = sentences[ i ];
		if ( sentence.beginMsec >= end_msec )
		{
			int duration = 0;		// 当前句离上一句结束的时间
			duration = sentence.beginMsec - end_msec;

			if ( duration >= cls_gl->m_pSceneMgr->m_gWaveConfig.NEED_SHOW_MOVE_IMAGE_MSEC )
			{
				duration = duration / 500 * 500;
				if ( duration > cls_gl->m_pSceneMgr->m_gWaveConfig.SHOW_TIP_IMAGE_MSEC )
				{
					duration = cls_gl->m_pSceneMgr->m_gWaveConfig.SHOW_TIP_IMAGE_MSEC;
				}
				_tGuiParagraphInfo paragraph_info;// = new _tGuiParagraphInfo;//[[_tGuiParagraphInfo alloc] init];
				paragraph_info.sentence_index = i;
				paragraph_info.duration = duration;
				m_ParagraphVect.push_back(paragraph_info);
			}
		}
		else
		{
			char logBuf[256] = {0};
			sprintf(logBuf,"sentence time stamp is error,sentence index is [%d].\n", ( int )i);
			OutputDebugStringA( logBuf );
		}
		end_msec = sentence.endMsec;
	}
}

void CSingObject::dealRealtimeGrade( const GLRealtimeGrade &  realtime_grade)
{
	if ( mnCallbackRealtimeGradeCount < 16 )
	{
		mCallbackRealtimeGrade[mnCallbackRealtimeGradeCount] = realtime_grade;
		mnCallbackRealtimeGradeCount++;
	}
	else
	{
		mnCallbackRealtimeGradeCount = 0;
		mCallbackRealtimeGrade[mnCallbackRealtimeGradeCount] = realtime_grade;
		mnCallbackRealtimeGradeCount++;
	}
}

void CSingObject::dealSentenceGrade(const GLSentenceGrade &  sentence_grade)
{
	if ( mnCallbackSentenceGradeCount < 8 )
	{
		mCallbackSentenceGrade[mnCallbackSentenceGradeCount] = sentence_grade;
		mnCallbackSentenceGradeCount++;
	}
	else
	{
		mnCallbackSentenceGradeCount = 0;
		mCallbackSentenceGrade[mnCallbackSentenceGradeCount] = sentence_grade;
		mnCallbackSentenceGradeCount++;
	}
}

void CSingObject::eventMainUpdate(unsigned int elapsed_ms)
{
	bool call_realtime_grade = false;
	// 谱线描绘是否插值
	if ( mbInterpolation )
	{
		if ( !m_bIsCallback )
		{
			call_realtime_grade = true;
		}
	}

	if ( call_realtime_grade )
	{
		eventAvRealtimeGrade(elapsed_ms , m_CurGrade , false);
	}

	if ( m_gPlayInfo.ktv_mode && ( mbInterpolation || m_bIsCallback ) )
	{
		// 2.着色歌词
		// 当前演唱语句的字数
		unsigned int nWordCount = mCurSentence_info.word_size;

		// 歌词Vect的总数
		unsigned int nGuiLyricCount = ( unsigned int )m_tmpgui_lyric_list.size();

		if ( m_iCurSentence >= 1 && nWordCount > 0 && nGuiLyricCount > 0 )
		{
			{
				// 定义每个字需要着色的百分比（单位：像素）
				for ( unsigned int i = 0; i < nWordCount && i < nGuiLyricCount; ++i )
				{
					_tWordInfo & word_info = mCurSentence_info.word_list[i];
					_tGuiLyricInfo  &lyric_info = m_tmpgui_lyric_list[i];
					if ( m_CurGrade.cur_tm > word_info.end_tm )
					{
						lyric_info.npass = 1.0f;
					}
					else
					{
						if ( m_CurGrade.cur_tm >= word_info.begin_tm)
						{
							float word_msec = (float)(word_info.end_tm-word_info.begin_tm);
							if ( word_msec < 1.0f )
								word_msec = 1.0f;

							long temp_msec = m_CurGrade.cur_tm - word_info.begin_tm;
							if ( temp_msec < 0 )
								temp_msec = 0;

							lyric_info.npass = temp_msec / word_msec;
						}
						break;
					}
				}
			}
		}
	}

	if ( m_bIsCallback )
	{
		m_bIsCallback = false;
	}
}

float CSingObject::checkSentenceCurTM()
{
	unsigned int sentence_duration = mCurSentence_info.endMsec - mCurSentence_info.beginMsec;
	unsigned int def_duration = 5000;
	while(sentence_duration > def_duration )
	{
		def_duration += 2000;
	}
	cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR = 1280.0f/def_duration;
	return sentence_duration*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR;
}

float CSingObject::checkSentenceNextTM()
{
	unsigned int sentence_duration = mNextSentence_info.endMsec - mNextSentence_info.beginMsec;
	unsigned int def_duration = 5000;
	while(sentence_duration > def_duration )
	{
		def_duration += 2000;
	}
	cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_NEXT = 1280.0f/def_duration;
	return sentence_duration*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_NEXT;
}

void CSingObject::eventAvRealtimeGrade(unsigned int elasped_ms/*毫秒*/ ,GLRealtimeGrade  grade ,bool is_callback/* = true*/)
{
	char bufLog[512] ={0};
	m_bIsCallback = is_callback;

	miLastElapsedCount += elasped_ms;

	m_CurGrade = grade;
	m_CurGrade.cur_tm = mCurTime;
	const _tSongInfo & song_info = m_songInfo;

	if ( m_CurGrade.cur_tm > cls_gl->m_gCurPlayInfo.m_notify_get_score_ms )
	{
		if(!cls_gl->m_gCurPlayInfo.m_bNotifyScore)
		{
			::PostMessage(g_hWnd,WM_PLAY_FINALSCORE,NULL,NULL);
			cls_gl->m_gCurPlayInfo.m_bNotifyScore = true;
		}
		
		if(m_CurGrade.cur_tm > (song_info.song_tm+5)*1000)//让曲谱走完
			return ;
	}

	unsigned int sentence_count = song_info.sentence_size;
	_tSentenceInfo * sentences = song_info.sentence_info;

	// 当前的时间，确定在分割线上，分割线的左右。分割线的左侧是已经唱过的歌词，分割线的右测是没有唱过的歌词
	// 如果改变方案，那么需要将时间改正，显示应该提前5秒显示
	//clearListVectT(m_tmpgui_lyric_list);
	m_tmpgui_lyric_list.clear();
	//clearListVectT(m_tmpgui_pitch_list);
	m_tmpgui_pitch_list.clear();

	if ( m_gPlayInfo.ktv_mode )
	{
		if (!wave_render)
		{
			return ;
		}
		static int WAVE_HEIGHT_CONST = wave_render->getWaveHeight();
		bool bSwitchSentence = false;
		bool bShowKtvFrame = false;
		if ( mbChangeMode )
		{
			bSwitchSentence = true;
		}

		unsigned int i = 0;
		// 显示歌词的当前时间
		unsigned long show_sentence_tm = m_CurGrade.cur_tm + cur_sentence_ahead_tm;//加上提前显示的时间
		unsigned long show_ktv_frame_tm = show_sentence_tm + cur_sentence_ahead_tm;//框架提前显示的时间
		for ( i = m_iCurSentence; i < sentence_count; ++i )
		{
			_tSentenceInfo & sentence_info = sentences[i];//SentenceInfo
			if (show_ktv_frame_tm >= sentence_info.beginMsec && show_ktv_frame_tm <= sentence_info.endMsec)
			{
				bShowKtvFrame = true;
			}
			if ( show_sentence_tm >= sentence_info.beginMsec && show_sentence_tm <= sentence_info.endMsec )
			{
				if ( mCurSentence_info.beginMsec != sentence_info.beginMsec )
				{
					mCurSentence_info = sentence_info;
					m_iCurSentence = i + 1;
					if (i>0)
					{
						mPreSentence_info = sentences[i-1];
					}
					if ( i + 1 < sentence_count )
					{
						mNextSentence_info = sentences[ i + 1 ];
					}
					else
					{
						memset( &mNextSentence_info, 0, sizeof(mNextSentence_info) );
					}
					bSwitchSentence = true;
				}
				break;
			}
		}

		if ( m_CurGrade.cur_tm > song_info.end_tm )
		{
			memset(&mPreSentence_info,0,sizeof(mPreSentence_info));
			memset( &mCurSentence_info, 0, sizeof(mCurSentence_info) );
			memset( &mNextSentence_info, 0, sizeof(mNextSentence_info) );
		}
		if (bShowKtvFrame)
		{
			wave_render->setDrawKtvFrame(true);
		}
		if ( bSwitchSentence )
		{
			wave_render->setNeedSetPosition(true);//设置重置粒子系统位置
			wave_render->setCurIPitch(0);//设置当前的音符
			wave_render->setSwitchSentence(false);//设置是否切换句子
			wave_render->clearCurRightPitchWave();//清除当前句的唱准音符和音符
			wave_render->setSwitchPitch(false);//设置切换音符为false

			if (lyric_render)
			{
				lyric_render->clearCurLyric();//清除当前句的歌词
			}

			int pitch_width = (int)checkSentenceCurTM();
			int pitch_width_next = (int)checkSentenceNextTM();
			int pitch_start_pos = (cls_gl->m_winSize.width-pitch_width)/2;
			int pitch_start_pos_next = (cls_gl->m_winSize.width-pitch_width_next)/2;

			if (m_iCurSentence == 1)
			{
				cur_sentence_start_tm = 0;//song_info.begin_tm;
				cur_sentence_end_tm = (mNextSentence_info.beginMsec+mCurSentence_info.endMsec)/2;
				cur_sentence_ahead_tm = (mNextSentence_info.beginMsec - mCurSentence_info.endMsec)/2;

				cursor_start_pos = (int)(pitch_start_pos - (mCurSentence_info.beginMsec-cur_sentence_start_tm)*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR);
				cursor_end_pos = (int)(pitch_start_pos+pitch_width+cur_sentence_ahead_tm*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR);

				cursor_start_pos_next = int(pitch_start_pos_next - cur_sentence_ahead_tm*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_NEXT); 
			}
			else if (m_iCurSentence == sentence_count)
			{
				cur_sentence_start_tm = (mPreSentence_info.endMsec+mCurSentence_info.beginMsec)/2;
				cur_sentence_end_tm = (song_info.end_tm + mCurSentence_info.endMsec)/2;
				cur_sentence_ahead_tm = 0;

				cursor_start_pos =int (pitch_start_pos - (mCurSentence_info.beginMsec - mPreSentence_info.endMsec)/2*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR);
				cursor_end_pos = int (pitch_start_pos+pitch_width+(song_info.end_tm - mCurSentence_info.endMsec)*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR);

				cursor_start_pos_next = cursor_end_pos;
			}
			else
			{
				cur_sentence_start_tm = (mPreSentence_info.endMsec+mCurSentence_info.beginMsec)/2;
				cur_sentence_end_tm = (mNextSentence_info.beginMsec + mCurSentence_info.endMsec)/2;
				cur_sentence_ahead_tm = (mNextSentence_info.beginMsec - mCurSentence_info.endMsec)/2;

				cursor_start_pos = int(pitch_start_pos - (mCurSentence_info.beginMsec - mPreSentence_info.endMsec)/2*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR);
				cursor_end_pos = int(pitch_start_pos+pitch_width+cur_sentence_ahead_tm*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR); 

				cursor_start_pos_next = int(pitch_start_pos_next - cur_sentence_ahead_tm*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_NEXT); 
			}

			// 清空歌词列表
			//			mNextGuiLyricVect.clear();

			// 			unsigned long pt_left	= mCurSentence.beginMsec;
			// 			unsigned long pt_right	= mCurSentence.endMsec - mCurSentence.beginMsec;

			//std::vector< _tGuiWaveInfo* > gui_pitch_list;
			unsigned int pitch_count = mCurSentence_info.pitchgroup_size;//音高组与词的个数相同
			for ( unsigned int i = 0; i < pitch_count; ++i )
			{
				const _tPitchGroup & pitch_group = mCurSentence_info.pitchgroup_list[i];
				//if ( pt_right > 0.0f )
				for(unsigned int j=0;j<pitch_group.pitch_size;j++)
				{
					// 显示谱线
					//_tGuiWaveInfo* gui_pitch_info = new _tGuiWaveInfo;
					_tGuiWaveInfo gui_pitch_info;
					//ZeroMemory(&gui_pitch_info,sizeof(_tGuiWaveInfo));

					const _tPitch & pitch_info = pitch_group.pitch_list[j];
					gui_pitch_info.l = pitch_start_pos+( pitch_info.begin_tm - mCurSentence_info.beginMsec ) * cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR;// /pt_right;
					gui_pitch_info.r = pitch_start_pos+( pitch_info.end_tm - mCurSentence_info.beginMsec ) * cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR;// / pt_right;
					gui_pitch_info.y = ( cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV-16 ) * ( song_info.pitch_max - pitch_info.pitch ) / ( song_info.pitch_max - song_info.pitch_min );
					gui_pitch_info.end_tm = pitch_info.end_tm;
					if (j == 0)//根据第一个音高处理字的位置
					{
						const _tWordInfo & word_info = mCurSentence_info.word_list[ i ];
						if ( word_info.lyric )
						{
							// 显示歌词
							//_tGuiLyricInfo* gui_lyric_info = new _tGuiLyricInfo;
							_tGuiLyricInfo gui_lyric_info;
							//ZeroMemory(&gui_lyric_info,sizeof(_tGuiLyricInfo));

							wchar_t lyricBuf[50] = {0};
							static char szLyric[50] = {0};
							memcpy(szLyric,word_info.lyric,word_info.lyric_size);
							szLyric[word_info.lyric_size] = 0;

							MultiByteToWideChar(CP_ACP, 0, szLyric, -1, lyricBuf, 49);//一个汉字一个宽字节，英文不超过49
							gui_lyric_info.lyric = lyricBuf;
							gui_lyric_info.y = gui_pitch_info.y+WAVE_HEIGHT_CONST;//加曲谱的image高度
							gui_lyric_info.z = GL3D_Z_LYRIC;
							gui_lyric_info.pos = gui_pitch_info.l;
							gui_lyric_info.width = m_pGLFont->getTextWidth(gui_lyric_info.lyric);
							gui_lyric_info.height = m_pGLFont->getTextHeight(gui_lyric_info.lyric);
							m_tmpgui_lyric_list.push_back( gui_lyric_info );
						}
					}
					m_tmpgui_pitch_list.push_back( gui_pitch_info );
				}

			}

			wave_render->setLineGroup(m_tmpgui_pitch_list,true);//设置当前句的曲谱

			if (lyric_render)
			{
				lyric_render->setFirstLyric(m_tmpgui_lyric_list,true);//设置当前句的歌词，每个字的宽度
			}

		}        
		// 显示游标+（粒子特效）

		if (m_iCurSentence >= 1 && m_iCurSentence<=(int)m_songInfo.sentence_size)
		{
			float cur_pitch = m_CurGrade.cur_pitch;

			//_tGuiLyricCursorInfo cursor;
			m_tmpcursor.r = RANK_YELLOW;//_tGuiLyricCursorInfo::

			float current_cursor_y;
			bool bSing = true;
			if (cur_pitch == 0)
			{
				cur_pitch = m_pre_cur_pitch;
				bSing = false;
			}
			else
			{
				m_pre_cur_pitch = cur_pitch;
			}
			if (cur_pitch<song_info.pitch_min)//如果唱了比最低音还低
			{
				current_cursor_y = (POS_WAVE_HEIGHT+cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV);//-8;
			}
			else if (cur_pitch>song_info.pitch_max)//如果唱了比最高音还高
			{
				current_cursor_y = POS_WAVE_HEIGHT;
			}
			else
			{
				current_cursor_y = (( cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV-16 ) * ( song_info.pitch_max - cur_pitch ) / ( song_info.pitch_max - song_info.pitch_min ) + POS_WAVE_HEIGHT+8);
			}
			// cursor.y = ( song_info.pitch_max - mCurGrade.cur_pitch ) / ( song_info.pitch_max - song_info.pitch_min );
			//w riteLog("song_info.pitch_min %f,cur_pitch %f,current_cursor_y %d",song_info.pitch_min,cur_pitch,current_cursor_y);
			float current_cursor_x = (cursor_start_pos+(m_CurGrade.cur_tm - cur_sentence_start_tm)*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR);//误差/*WAVEWND_WIDTH_PIXELS_KTVm_windowsize.width * mMoveRate;*/
			if (/*current_cursor_x >= cursor_end_pos*/m_CurGrade.cur_tm >= cur_sentence_end_tm)
			{
				cur_sentence_start_tm = cur_sentence_end_tm;
				cursor_start_pos = cursor_start_pos_next;
				cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR = cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_NEXT;
				//cursor_end_pos = cursor_end_pos_next; 
				current_cursor_x = (cursor_start_pos+(m_CurGrade.cur_tm - cur_sentence_start_tm)*cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS_KTV_TM_CUR);
				// 				wave_render->clearCurRightPitchWave();//清除当前句的唱准音符和音符
				// 				lyric_render->clearCurLyric();//清除当前句的歌词
			}
			//w riteLog("mCurGrade.cur_tm %d,pitch_start_pos %d,current_cursor_x %d",mCurGrade.cur_tm,cursor_start_pos,current_cursor_x);
			m_tmpcursor.x = current_cursor_x;
			m_tmpcursor.y = current_cursor_y;

			wave_render ->setCurrentXY(current_cursor_x,current_cursor_y,m_CurGrade.cur_db,bSing);//粒子系统的y坐标
			wave_render ->setRightPitch(m_tmpcursor , mfKtvErr , mfGameErr);//计算唱准的曲谱
		}
	}
	else
	{
		// 计算偏移
		float tmp_offset = 1.0f*m_CurGrade.cur_tm * cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS / cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_TIMEMS-cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_LEFT_PIXELS;

		// 显示歌词
		LISTGUILYRICVECT::iterator iL;
		for (iL=m_GuiLyricVect.begin();iL!=m_GuiLyricVect.end();++iL)
		{
			float pos = (*iL).pos - tmp_offset;
			if ( pos < -20.0f )
			{
				continue;
			}

			if ( pos > cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS )
			{
				break;
			}

			_tGuiLyricInfo temp;
			temp.lyric = (*iL).lyric;
			temp.width = (*iL).width;
			temp.pos = pos;
			temp.width = m_pGLFont->getTextWidth(temp.lyric);
			temp.height = m_pGLFont->getTextHeight(temp.lyric);
			m_tmpgui_lyric_list.push_back(temp);
		}

		if (lyric_render)
		{
			lyric_render->setFirstLyric(m_tmpgui_lyric_list , true);
		}

		// 显示谱线
		int all_wave_pitch_count=1;
		int left_wave_pitch_count=0;
		int j = 0;
		LISTGUIPITCHVECT::iterator iP;
		for (iP=m_GuiPitchVect.begin();iP!=m_GuiPitchVect.end();iP++,all_wave_pitch_count++)
		{
			float right = (*iP).r - tmp_offset;
			if (right<=0.01)
			{
				left_wave_pitch_count++;
				continue;
			}
			if (m_pre_left_pitch_count != left_wave_pitch_count)//出栈
			{
				wave_render->popFBBall();
				m_pre_left_pitch_count = left_wave_pitch_count;
			}
			float left = (*iP).l - tmp_offset;
			if ( left >= cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS )
			{
				break;
			}
			else if (left>0.01f)
			{
				if (wave_render->getFBBallSize() < (all_wave_pitch_count-left_wave_pitch_count))//入栈
				{
					wave_render->pushFBBall();
				}
			}

			//if ( left > 0 || right >0 )
			{
				_tGuiWaveInfo temp;
				temp.y = (*iP).y+POS_WAVE_HEIGHT+8;//加上曲谱位置高度以及位移
				temp.l = left;
				temp.r = right;

				m_tmpgui_pitch_list.push_back(temp);
			}
		}

		if ( !wave_render )
		{
			return ;
		}
		wave_render->setDrawKtvFrame(true);
		wave_render->setLineGroup(m_tmpgui_pitch_list , true);

		//显示句尾线
		//NSMutableArray * gui_sentenceline_list = NULL;
		LISTSENTENCELINEVECT gui_sentenceline_list;
		//for( NSNumber * i in mGuiSentencelineVect )
		LISTSENTENCELINEVECT::iterator iS;
		for (iS=m_GuiSentencelineVect.begin();iS!=m_GuiSentencelineVect.end();iS++)
		{
			float pos = *iS - tmp_offset;
			if ( pos > cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_WIDTH_PIXELS )
			{
				break;
			}

			if ( pos >= 0.01f )
			{
				gui_sentenceline_list.push_back(pos);
			}
		}

		wave_render->setSentenceLineList( gui_sentenceline_list , true);

		// 显示游标
		float cur_pitch = m_CurGrade.cur_pitch;

		m_tmpcursor.r = RANK_YELLOW;//(int)
		//32像素为预留像素，上下各16个像素
		float current_cursor_y;
		bool bSing = true;
		//w riteLog("%f",cur_pitch);
		if (cur_pitch <= 0.01)
		{
			cur_pitch = m_pre_cur_pitch;
			bSing = false;
		}
		else
		{
			m_pre_cur_pitch = cur_pitch;//保存之前的值
		}
		if (cur_pitch<song_info.pitch_min)//如果唱了比最低音还低
		{
			current_cursor_y = POS_WAVE_HEIGHT+cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV;
		}
		else if (cur_pitch>song_info.pitch_max)//如果唱了比最高音还高
		{
			current_cursor_y = POS_WAVE_HEIGHT;
		}
		else
		{
			current_cursor_y = (( cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_HEIGHT_PIXELS_KTV-16 ) * ( song_info.pitch_max - cur_pitch ) / ( song_info.pitch_max - song_info.pitch_min ) + POS_WAVE_HEIGHT+8);
		}
		m_tmpcursor.x = (float)cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_LEFT_PIXELS;
		m_tmpcursor.y = current_cursor_y;

		wave_render ->setCurrentXY((float)cls_gl->m_pSceneMgr->m_gWaveConfig.WAVEWND_LEFT_PIXELS,current_cursor_y,m_CurGrade.cur_db,bSing);
		wave_render ->setRightPitch(m_tmpcursor , mfKtvErr , mfGameErr);
	}

	// 复位标志位
	if ( mbChangeMode )
	{
		mbChangeMode = false;
	}
}

// av：音频单句评分
void CSingObject::eventAvSentenceGrade (GLSentenceGrade & grade)
{
	// 界面模块，句评分分数显示
	sentence_render ->showSentenceLevelScore( ( int )grade.sentence_pitch_score,(int)grade.sentence_lyric_score);

	float cur_cursor_x;
	float cur_cursor_y;
	wave_render->getCurrentXY(cur_cursor_x,cur_cursor_y);
	sentence_render->setStarStartPoint(cur_cursor_x,cur_cursor_y);
}
int CSingObject::calculateSentence(const uint start_ms,const uint end_ms)
{
	int nSentenceSize = 0;
	for (uint i=0;i<m_songInfo.sentence_size;i++)
	{
		_tSentenceInfo &tmp_sentence_info = m_songInfo.sentence_info[i];	
		if (start_ms < tmp_sentence_info.beginMsec && tmp_sentence_info.endMsec<end_ms)
		{
			nSentenceSize ++;
		}
	}
	return nSentenceSize;
}
// 获取歌曲中所有歌词(排除重复)
// 返回Unicode编码的不重复的歌词
std::wstring & CSingObject::getLyric(const _tSongInfo& song_info)
{
	static std::wstring strLyric;//
	strLyric.clear();

	MAPWCHARINT mapWord ;
	for (unsigned int i = 0; i < song_info.sentence_size; i++)
	{
		for (unsigned int j = 0; j < song_info.sentence_info[i].word_size; j++)
		{
			const _tWordInfo & word_info = song_info.sentence_info[i].word_list[j];
			char lyricBuf[256] = {0};
			if (word_info.lyric_size>255)
			{
				OutputDebugStringA("lyric_size error >255\n");
				memcpy(lyricBuf,word_info.lyric,255);
			}
			else
			{
				memcpy(lyricBuf,word_info.lyric,word_info.lyric_size);
			}
			wchar_t unicodeStr[256]= {0};
			MultiByteToWideChar(CP_ACP, 0, lyricBuf, -1, unicodeStr, 255);
			size_t len = wcslen(unicodeStr);
			for ( unsigned int k = 0; k < len;k++)
			{
				wchar_t word = unicodeStr[k];
				MAPWCHARINT::iterator w;
				w = mapWord.find(word);
				if ( w == mapWord.end() )
				{
					mapWord.insert(std::pair<wchar_t,int>(word,1));
					strLyric.push_back(word);
				}
				else
				{
					(w->second)++;
				}
			}
		}
	}
	return strLyric;
}

bool CSingObject::sampleAwake(uint start_ms,uint end_ms)
{
	m_gPlayInfo.start_ms = start_ms;
	m_gPlayInfo.end_ms = end_ms;

	int nSentenceSize = m_songInfo.sentence_size;
	if (m_gPlayInfo.start_ms || m_gPlayInfo.end_ms)
	{
		nSentenceSize = calculateSentence(m_gPlayInfo.start_ms,m_gPlayInfo.end_ms);
		if (!nSentenceSize)
			return false;
	}
	sentence_render->setSentenceNum(nSentenceSize);
	return true;
}

void CSingObject::realtime(ulong ns_cur_ms,eShowWave eType)
{
	if (m_bFirst)
	{
		wave_render->setCurTimeLine(ns_cur_ms);//更新时间轴
	}

	if (eType == SHOW_ONE)
	{
		if (m_gPlayInfo.eType != eType)
		{
			POS_WAVE_HEIGHT = m_gPlayInfo.POS_WAVE_HEIGHTSINGLE;
			wave_render->setStaffPosHeight(POS_WAVE_HEIGHT);
			sentence_render->setStaffPosHeight(POS_WAVE_HEIGHT,eType);
			lyric_render->setStaffPosHeight(POS_WAVE_HEIGHT,eType);
			m_gPlayInfo.eType = eType;
		}

		CAutoLock lock(&g_csGrade);
		if (g_listRGradeLeft.empty()  || (cls_gl->m_gCurPlayInfo.m_isMV&&g_listRGradeRight.empty()))
			return ;

		LISTGLREALTIMEGRADE::iterator iR1 = g_listRGradeLeft.begin();
		LISTGLREALTIMEGRADE::iterator iR2 = g_listRGradeRight.begin();
		LISTGLSENTENCEGRADE::iterator iS1 = g_listSGradeLeft.begin();
		LISTGLSENTENCEGRADE::iterator iS2 = g_listSGradeRight.begin();

		if(!cls_gl->m_gCurPlayInfo.m_isMV)//以前的逻辑，处理星光线上过来的评分
		{
			dealRealtimeGrade(*iR1);
			if(iS1 != g_listSGradeLeft.end())
			{
				dealSentenceGrade(*iS1);
			}
			g_listSGradeLeft.clear();
		}
		else if ((iR1->cur_pitch <= 0.01) && (iR2->cur_pitch>0))
		{
			dealRealtimeGrade(*iR2);
			if (iS2 != g_listSGradeRight.end() && iS1!=g_listSGradeLeft.end())
			{
				if (iS1->sentence_pitch_score > iS2->sentence_pitch_score)
				{
					dealSentenceGrade(*iS1);
				}
				else
				{
					dealSentenceGrade(*iS2);
				}
				g_listSGradeLeft.clear();
				g_listSGradeRight.clear();
			}
		}
		else
		{
			dealRealtimeGrade(*iR1);
			if (iS2 != g_listSGradeRight.end() && iS1!=g_listSGradeLeft.end())
			{
				if (iS1->sentence_pitch_score > iS2->sentence_pitch_score)
				{
					dealSentenceGrade(*iS1);
				}
				else
				{
					dealSentenceGrade(*iS2);
				}
				g_listSGradeLeft.clear();
				g_listSGradeRight.clear();
			}
		}

		g_listRGradeLeft.clear();
		g_listRGradeRight.clear();
	}
	else if (eType == SHOW_TWO || eType == SHOW_THREE)
	{
		if (m_bFirst)
		{
			if (m_gPlayInfo.eType != eType)
			{
				POS_WAVE_HEIGHT = m_gPlayInfo.POS_WAVE_HEIGHTDOUBLE;
				wave_render->setStaffPosHeight(POS_WAVE_HEIGHT);
				sentence_render->setStaffPosHeight(POS_WAVE_HEIGHT,eType);
				lyric_render->setStaffPosHeight(POS_WAVE_HEIGHT,eType);
				m_gPlayInfo.eType = eType;
			}
			CAutoLock lock(&g_csGrade);
			if (g_listRGradeLeft.empty())
				return;

			dealRealtimeGrade(*(g_listRGradeLeft.begin()));
			LISTGLSENTENCEGRADE::iterator iS1 = g_listSGradeLeft.begin();
			if (iS1 != g_listSGradeLeft.end())
			{
				dealSentenceGrade(*iS1);
				g_listSGradeLeft.clear();
			}
			g_listRGradeLeft.clear();
		}
		else
		{
			CAutoLock lock(&g_csGrade);
			if (g_listRGradeRight.empty())
				return;

			dealRealtimeGrade(*(g_listRGradeRight.begin()));
			LISTGLSENTENCEGRADE::iterator iS2 = g_listSGradeRight.begin();
			if (iS2 != g_listSGradeRight.end())
			{
				dealSentenceGrade(*iS2);
				g_listSGradeRight.clear();
			}
			g_listRGradeRight.clear();
		}
	}
}

int CSingObject::getFinalScoreStar(int &nFinalScore,int &nFinalStarNum)
{
	if(sentence_render)
	{
		int nSentence;
		m_bShowEndScore = true;
		sentence_render->getFinalData(nFinalScore,nFinalStarNum,nSentence);
		m_ave_dest_score = nFinalScore;
		m_star_number = nFinalStarNum;	
		m_nsentence = nSentence;
		return 0; 
	}
	return -1;
}

void CSingObject::clearGrade()
{
	mnCallbackRealtimeGradeCount=0;
	mnCallbackSentenceGradeCount=0;
}

//唤起，初始化操作
bool CSingObject::awake(void* info)
{
	if (!m_pIEval)
	{
		SceneMgr::s_error_msg = L"IEval is null";
		return false;
	}
	std::wstring strLyric;
	if (cls_gl->m_gCurPlayInfo.m_bGradeSong && cls_gl->m_gCurPlayInfo.m_isMV)
	{
		if(/*!SceneMgr::loadSongInfo(m_gSingInfo.mdmFile,m_songInfo)*/true)
		{
			if ( !m_pIEval->loadSongInfo( m_gSingInfo.mdmFile,m_songInfo))
			{
				if(SceneMgr::sNalgorithm == 0)
				{
					wchar_t tmpWchar[260] = {0};
					wmemcpy(tmpWchar,m_gSingInfo.mdmFile,wcslen(m_gSingInfo.mdmFile)-3);
					wstring tmpWstr = std::wstring(tmpWchar)+std::wstring(L"dat]");
					SceneMgr::s_error_msg = std::wstring(L"eval mdm load failed,[") + tmpWstr;
				}
				else
					SceneMgr::s_error_msg = std::wstring(L"eval mdm load failed,[") + std::wstring(m_gSingInfo.mdmFile) + std::wstring(L"]");
				return false;
			}
		}
		else
			m_bLoadFromDat = true;

		SceneMgr::parseTime(m_songInfo.begin_tm,m_songInfo.end_tm);

		int nSentenceSize = m_songInfo.sentence_size;
		if (m_gPlayInfo.start_ms || m_gPlayInfo.end_ms)
		{
			nSentenceSize = calculateSentence(m_gPlayInfo.start_ms,m_gPlayInfo.end_ms);
			if (!nSentenceSize)
				return false;
		}
		sentence_render->setSentenceNum(nSentenceSize);

		m_songInfo.song_tm = (unsigned int)(m_songInfo.end_tm / 1000.0f + 1.0f);
		unsigned int notify_score_ms = m_songInfo.end_tm+1000;
		if(notify_score_ms > cls_gl->m_gCurPlayInfo.m_notify_get_score_ms)
			cls_gl->m_gCurPlayInfo.m_notify_get_score_ms = notify_score_ms;
		strLyric = getLyric(m_songInfo);
		createWaveLyric(m_songInfo);
		if (!wave_render)//前面有判断
			return false;
	}

	//增加评分结果页面的字,歌手，歌名，总分，连唱
	std::wstring wStr = strLyric + L"正在演唱主歌曲星下一首:;当前播放"+m_wcur_song_name+m_next_song_name+m_wcur_singer_name
		+cls_gl->m_mapVideoName[vtHost]+cls_gl->m_mapVideoName[vtSing];
	m_pGLFont->addFont(wStr);//添加字符串,只管添加,里面会判断的

	if (lyric_render)
		lyric_render->setFont(m_pGLFont,GL3D_Z_LYRIC);

	if (m_bFirst)//放到Notify对象中
	{
		//////////////////////////////////////////////////////////////////////////
		if(!cls_gl->m_gCurPlayInfo.m_isMV)//不是MV
		{
			if(!cls_gl->m_gCurPlayInfo.m_bAdvertisement)//不是广告，有结束时间，如果是广告则会循环播放
			{
				ulong endMs = m_songInfo.song_tm*1000+35000;
				wave_render->setTimeLine(0,endMs);
				cls_gl->m_nDurationMs = endMs;
			}
		}
		else if (m_gPlayInfo.start_ms || m_gPlayInfo.end_ms)//是MV，并且有时间上的限制（简版歌曲）//deprecated
		{
			SAFEPOINTER_CALL(cls_gl->m_pGraph)setStartStopPosition(m_gPlayInfo.start_ms,m_gPlayInfo.end_ms);
			wave_render->setTimeLine(m_gPlayInfo.start_ms,m_gPlayInfo.end_ms);
			cls_gl->m_nDurationMs = m_gPlayInfo.end_ms-m_gPlayInfo.start_ms;
		}
		else//是MV，并且播放整首歌曲
		{
			ulong n_duration_ms = 0;
			SAFEPOINTER_CALL(cls_gl->m_pGraph)getDuration(n_duration_ms);
			wave_render->setTimeLine(0,n_duration_ms);
			cls_gl->m_nDurationMs = n_duration_ms;
		}
	}
	return ISceneObject::awake(info);
}
//设置当前时间值，
void CSingObject::setCurMs(unsigned int ms)
{
	mCurTime = ms;
}
//更新操作场景对象
//@param ms:逝去的时间，单位（毫秒）
bool CSingObject::update(unsigned int ms)
{
	if(!ISceneObject::update(ms))
		return false;

	//不是MV，不是评分歌曲
	if(!cls_gl->m_gCurPlayInfo.m_isMV || !cls_gl->m_gCurPlayInfo.m_bGradeSong)
		return true;

	if ( mnCallbackRealtimeGradeCount > 0 )
	{
		GLRealtimeGrade  realtime_grade = mCallbackRealtimeGrade[mnCallbackRealtimeGradeCount-1];
		eventAvRealtimeGrade(ms , realtime_grade);
		mnCallbackRealtimeGradeCount--;
	}

	eventMainUpdate( ms );

	if ( mnCallbackSentenceGradeCount > 0 )
	{
		GLSentenceGrade & sentence_grade = mCallbackSentenceGrade[mnCallbackSentenceGradeCount-1];
		eventAvSentenceGrade( sentence_grade);
		mnCallbackSentenceGradeCount--;
	}

	if (lyric_render)
		lyric_render-> update(ms);
	if (wave_render)
		wave_render-> update(ms);
	if (sentence_render)
		sentence_render-> update(ms);

	return true;
}
//渲染对象
bool CSingObject::render()
{
	if(!ISceneObject::render())
		return false;

	{
		if(!cls_gl->m_gCurPlayInfo.m_isMV || !cls_gl->m_gCurPlayInfo.m_bGradeSong)
			return true;

		if (lyric_render)
			lyric_render-> drawGraphBuffer();
		if (wave_render)
			wave_render-> drawGraphBuffer();
		if (sentence_render)
			sentence_render-> drawGraphBuffer();
	}

	return true;
}

//结束，资源等回收操作
void CSingObject::end()
{
	ISceneObject::end();

	eShowWave eType = cls_gl->m_eShowType;
	if(m_bFirst)
	{
		static AwakeStatistics asf = {0};
		asf.bFirst = m_bFirst;
		wcscpy(asf.m_wcur_singer_name,m_wcur_singer_name.c_str());
		wcscpy(asf.m_wcur_song_name,m_wcur_song_name.c_str());
		asf.m_ave_dest_score = m_ave_dest_score;
		asf.m_star_number = m_star_number;
		asf.eType = eType;

		informNext(&asf);
	}
	else
	{
		//如果是第二个Object，如果需要唤醒
		if (eType == SHOW_TWO || eType == SHOW_THREE)
		{
			static AwakeStatistics ass = {0};
			ass.bFirst = m_bFirst;
			wcscpy(ass.m_wcur_singer_name,m_wcur_singer_name.c_str());
			wcscpy(ass.m_wcur_song_name,m_wcur_song_name.c_str());
			ass.m_ave_dest_score = m_ave_dest_score;
			ass.m_star_number = m_star_number;
			ass.eType = eType;

			informNext(&ass);
		}
	}

}

