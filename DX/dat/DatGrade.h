#pragma once
#include "AvGrade.h"

#ifndef DATPITCHGROUP
#define DATPITCHGROUP
//-----------------------------------------------------------------------------
// �����ֵ�������
struct _tDatPitchGroup
{
	_tAvPitch* pitch; 
	int pitch_size;
};
#endif

#ifndef DATONELYRIC
#define DATONELYRIC
//-----------------------------------------------------------------------------
// ����������ݣ�ÿ���ֿ�ʼ�ľ�ȷʱ�䣩
struct _tDatOneLyric
{
	unsigned long	tm;// ���루��0��㣩
	char*		lyric;// ��ʣ���һ����һ���֣�����Ӣ�ģ�
	int				lyric_size;
};
#endif

//-----------------------------------------------------------------------------
// ���׸��������Ϣ
struct _tDatSongPitch
{
	std::vector< _tDatPitchGroup > pitchgroup_list;
};

//-----------------------------------------------------------------------------
// ���׸�ĸ����Ϣ
struct _tDatSongLyric
{
	std::vector< _tDatOneLyric >	lyric_list;
};
