#ifndef __RSDATEXTRACTOR__
#define __RSDATEXTRACTOR__

#include "AvGrade.h"
//typedef float TWZ_REAL;

#ifndef __RSFE_SENTENCEINFO__
#define __RSFE_SENTENCEINFO__
#define _RS_MAXLYRICNUM					1024
//��ʵľ���ʼ�����ʱ��
struct TSongSentenceStamp
{
	float	tStartTime;
	float	tEndTime;
};

//�˽ṹ�����ڳ�ʼ��ʱ����xml��ʻ�����Ϣ
struct LyricStruct
{
	TSongSentenceStamp vSentences[_RS_MAXLYRICNUM];//�޶�ÿ�׸���������1024��
};
#endif //__RSFE_SENTENCEINFO__


#ifndef DATPITCHGROUP
#define DATPITCHGROUP
struct _tDatPitchGroup
{
	_tAvPitch* pitch; 
	int pitch_size;
};
#endif

#ifndef DATONELYRIC
#define DATONELYRIC
//
struct _tDatOneLyric
{
	unsigned long	tm;// ���루��0��㣩
	char*		lyric;// ��ʣ���һ����һ���֣�����Ӣ�ģ�
	int				lyric_size;
};
#endif

//��ʼ���ø�����DAT·�����ɹ�����0��dat���ɷ��ʣ�����1������dat���󷵻�2����ȡ������Ϣʧ�ܷ���3����ʱĿ¼��ȡʧ�ܣ�����4
EXTERN_C __declspec(dllimport) int InitDATInfo(const char* pchInFileName,bool isOutXML = false);

//��ȡ����������������Ϣ���ɹ�����true,ʧ�ܷ���false
EXTERN_C __declspec(dllimport) bool GetSongPitch(_tDatPitchGroup*& fDatSongPitch,int& iDatSongPitchLength);

//��ȡ���������и����Ϣ���ɹ�����true,ʧ�ܷ���false
EXTERN_C __declspec(dllimport) bool GetSongLyric(_tDatOneLyric*& fDatSongLyric,int& iDatSongLyricLength);

//��ȡ���������о���Ϣ���ɹ�����true,ʧ�ܷ���false
EXTERN_C __declspec(dllimport) bool GetSongSentenceLine( unsigned long*& fAvSongSentenceLine,int& iAvSongSentenceLineLength);

//��ȡ��������ʱ�䣬��λ����
EXTERN_C __declspec(dllimport)  unsigned long __cdecl GetSongDuration();

//��ȡ���������и��ʱ����Ϣ,�ɹ�����0,�ļ�Ϊ�շ���1,�ļ����ܷ���Ϊ2,�����ļ�����Ϊ3
EXTERN_C __declspec(dllimport) int GetLyricStruct(LyricStruct& outSentenceInfo);

EXTERN_C __declspec(dllimport) int SetDatUnit(unsigned long ulUnit,long ulOffsetUnit,unsigned char* text);

//EXTERN_C __declspec(dllimport) void GetOrigFetureElement(int& iLen,int& iAlgType,TWZ_REAL& tSongOrigStartTime, TWZ_REAL*& ptVolume,TWZ_REAL*& ptPitch,TWZ_REAL*& ptTempo);
//
//EXTERN_C __declspec(dllimport) void ClearOrigFetureElement();

//�ͷ����ָ��
EXTERN_C __declspec(dllimport) void ClearDatInfo();
#endif