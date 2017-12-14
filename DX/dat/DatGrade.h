#pragma once
#include "AvGrade.h"

#ifndef DATPITCHGROUP
#define DATPITCHGROUP
//-----------------------------------------------------------------------------
// 单个字的音高组
struct _tDatPitchGroup
{
	_tAvPitch* pitch; 
	int pitch_size;
};
#endif

#ifndef DATONELYRIC
#define DATONELYRIC
//-----------------------------------------------------------------------------
// 单个歌词数据（每个字开始的精确时间）
struct _tDatOneLyric
{
	unsigned long	tm;// 毫秒（以0起点）
	char*		lyric;// 歌词（不一定是一个字，例如英文）
	int				lyric_size;
};
#endif

//-----------------------------------------------------------------------------
// 整首歌的音高信息
struct _tDatSongPitch
{
	std::vector< _tDatPitchGroup > pitchgroup_list;
};

//-----------------------------------------------------------------------------
// 整首歌的歌词信息
struct _tDatSongLyric
{
	std::vector< _tDatOneLyric >	lyric_list;
};
