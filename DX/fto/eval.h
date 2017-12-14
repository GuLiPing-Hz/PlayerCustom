#ifndef __EVAL_H__
#define __EVAL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <MMSystem.h>
#include <wchar.h>
#include "ftotype.h"
#include "EvalDefine.h"

#ifdef EVAL_EXPORTS
#define EVAL_DLLAPI extern "C" __declspec(dllexport)
#else
#define EVAL_DLLAPI extern "C" __declspec(dllimport)
#endif

class IEval
{
public:
	/************************************************************************/
	/* 初始化
	_eAlgorithmType type	算法类型
	wf						输入音频格式
	sampleRate				采样率，必须为11025
	minFreq					需要算法精确到的最小检测音高频率
	timerInterval			评分间隔
	/************************************************************************/
	virtual void init( _eAlgorithmType type, const _tWaveFormat& wf, float sampleRate = 11025, float minFreq = 80, unsigned int timerInterval = 50 ) = 0;

	/************************************************************************/
	/* 反初始化
	/************************************************************************/
	virtual void uninit() = 0;

	/************************************************************************/
	/* 设置是否开启静音检测
	bDetectSilent				是否开启静音检测
	/************************************************************************/
	virtual void setDetectSilent( bool bDetectSilent ) = 0;

	/************************************************************************/
	/* 获取是否开启静音检测
	/************************************************************************/
	virtual bool getDetectSilent() = 0;

	/************************************************************************/
	/* 设置歌曲实时评分类型，类型不同只是分数上的不同，困难的分数相对低
	type						歌曲实时评分类型(分为困难，容易)
	/************************************************************************/
	virtual void setRealtimeScoreType( _eRealtimeScoreType type ) = 0;

	/************************************************************************/
	/* 获取歌曲实时评分类型，类型不同只是分数上的不同，困难的分数相对低
	/************************************************************************/
	virtual _eRealtimeScoreType getRealtimeScoreType() = 0;

	/************************************************************************/
	/* 设置是否开启 分数开方乘以100
	bSqrtX100					是否开启 分数开方乘以100
	/************************************************************************/
	virtual void setSqrtX100( bool bSqrtX100 ) = 0;

	/************************************************************************/
	/* 获取是否开启 分数开方乘以100
	/************************************************************************/
	virtual bool getSqrtX100() = 0;

	/************************************************************************/
	/* 设置是否开启自动对调
	auto_adjust_tone			是否开启自动对调
	/************************************************************************/
	virtual void setAutoAdjustTone( bool auto_adjust_tone ) = 0;

	/************************************************************************/
	/* 获取是否开启自动对调
	/************************************************************************/
	virtual bool getAutoAdjustTone() = 0;

	/************************************************************************/
	/* 设置是否开启Combo
	if_combo					是否开启Combo
	/************************************************************************/
	virtual void setIfCombo( bool if_combo ) = 0;

	/************************************************************************/
	/* 获取是否开启Combo
	/************************************************************************/
	virtual bool getIfCombo() = 0;

	/************************************************************************/
	/* 加载歌曲信息
	pwszMDMOrDatFileName		以'\0'结尾的MDM文件或者DAT文件的文件名
	song_info					返回的歌曲信息
	/************************************************************************/
	virtual bool loadSongInfo( const wchar_t * pwszMDMOrDatFileName, _tSongInfo & song_info ) = 0;

	/************************************************************************/
	/* 卸载歌曲信息
	song_info					需要处理的歌曲信息
	/************************************************************************/
	virtual void unloadSongInfo( _tSongInfo & song_info ) = 0;

	/************************************************************************/
	/* 设置简版歌曲时间段
	nBeginTimeSecond			简版歌曲开始时间
	nEndTimeSecond				简版歌曲结束时间
	/************************************************************************/
	virtual void setRange( int nBeginTimeSecond, int nEndTimeSecond ) = 0;

	/************************************************************************/
	/* 实时评分及句评分
	buffer						采样率为44100，单声道，采样点位数为16位即2字节
	buffer_size					buffer大小，以字节为单位
	cur_timeMs					当前时间
	pitch						返回的音高
	pitchdiff					与标准音高的差值
	realtime_score				每个评分点的实时评分
	rank						每个评分点的分数等级
	sentence_index				句索引
	accumulated_sentence_score	每句的累积评分
	switch_sentence				当前时间是否切换了句索引
	fComboFactor				Combo加成
	fLyricScore					歌词评分（每句返回一个歌词评分）
	eSentenceRank				句评分等级
	bPreProcess					是否对输入的音频数据预处理
	/************************************************************************/
	virtual void realtimeEval( const unsigned char * buffer, unsigned int sample_size, unsigned int cur_timeMs, float & pitch, 
						  float & pitchdiff, float & realtime_score, int & rank, unsigned int & sentence_index, 
						  float & accumulated_sentence_score, bool & switch_sentence, float & fComboFactor, float & fLyricScore
						  , _eSentenceGradeLevel & eSentenceRank, bool bPreProcess ) = 0;

	/************************************************************************/
	/* 获取总分
	total_score 总分
	/************************************************************************/
	virtual void getTotalScore( float & total_score ) = 0;


	/************************************************************************/
	/* 设置是否开启录音
	bInputRecord		输入音频保存录音文件
	bLyricEvalRecord	歌词评分音频保存录音文件
	bPitchEvalRecord	音高评分音频保存录音文件
	/************************************************************************/
	virtual void enableRecord( bool bInputRecord, bool bLyricEvalRecord, bool bPitchEvalRecord ) = 0;

	/************************************************************************/
	/* 是否已经开启换气修正
	/************************************************************************/
	virtual bool longNoteCorrect() = 0;
	
	/************************************************************************/
	/* 开启/关闭 换气修正
	bCorrect		是否开启换气修正
	/************************************************************************/
	virtual void longNoteCorrect( bool bCorrect ) = 0;

	/************************************************************************/
	/* 设置自动对调类型以及其他参数
	autotone_type			自动对调类型，默认为BEGINING
	autotone_mindiff		自动对调范围大于几度，默认1度，最小相差一度的时候才开始自动对调
	autotone_start_length	参数autotone_type为BEGINING时有效，表示以歌曲开始时间长度为自动对调的依据，默认15秒即15000毫秒
	/************************************************************************/
	virtual void setAutoToneType( AUTOTONE_TYPE autotone_type = BEGINNING, float autotone_mindiff = 1, float autotone_start_timems= 15000 ) = 0;

	/************************************************************************/
	/* 开启/关闭 歌词评分
	bLyricEval		是否开启歌词评分
	/************************************************************************/
	virtual void lyricEval( bool bLyricEval ) = 0;

	/************************************************************************/
	/* 设置评分参数
	fCutNoise		静音阈值，范围？
	/************************************************************************/
	virtual void SetParaCutNoise( float fCutNoise ) = 0;
};


/************************************************************************/
/* 创建并返回一个评分模块对象指针
/************************************************************************/
EVAL_DLLAPI IEval * createEval();

/************************************************************************/
/* 销毁一个评分模块对象指针(销毁前如果没有调用uninit，此函数会在删除pEval对象前调用uninit)
/************************************************************************/
EVAL_DLLAPI void destroyEval( IEval * pEval );

#endif // __EVAL_H__