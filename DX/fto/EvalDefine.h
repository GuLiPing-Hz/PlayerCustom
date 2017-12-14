#ifndef __EVALDEFINE_H__
#define __EVALDEFINE_H__

#pragma once

enum _eAlgorithmType
{
	ALGORITHM_TYPE_51MIKE = 0,			// 51mike
	ALGORITHM_TYPE_CUSTOM = 1,			// �Զ���
	ALGORITHM_TYPE_PERFORMOUS = 2,		// Performous
};

enum _eRealtimeScoreType
{
	REALTIME_SCORE_INVALID = -1,
	REALTIME_SCORE_EASY = 0,
	REALTIME_SCORE_HARD = 1,
	REALTIME_SCORE_MAX = 2
};

enum AUTOTONE_TYPE
{
	BEGINNING,				// ���ݸ�����ʼһ��ʱ��
	FULL					// ���׸����Զ��Ե�
};

struct _tWaveFormat
{
	short        wFormatTag;
	short        nChannels;
	unsigned int nSamplesPerSec;
	unsigned int nAvgBytesPerSec;
	short        nBlockAlign;
	short        wBitsPerSample;
	short        cbSize;
};

#endif // __EVALDEFINE_H__