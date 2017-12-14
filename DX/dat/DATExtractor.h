#ifndef __RSDATEXTRACTOR__
#define __RSDATEXTRACTOR__

#include "AvGrade.h"
//typedef float TWZ_REAL;

#ifndef __RSFE_SENTENCEINFO__
#define __RSFE_SENTENCEINFO__
#define _RS_MAXLYRICNUM					1024
//歌词的句起始与结束时间
struct TSongSentenceStamp
{
	float	tStartTime;
	float	tEndTime;
};

//此结构用于在初始化时返回xml歌词基本信息
struct LyricStruct
{
	TSongSentenceStamp vSentences[_RS_MAXLYRICNUM];//限定每首歌曲歌词最多1024句
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
	unsigned long	tm;// 毫秒（以0起点）
	char*		lyric;// 歌词（不一定是一个字，例如英文）
	int				lyric_size;
};
#endif

//初始化该歌曲的DAT路径，成功返回0，dat不可访问，返回1；解析dat错误返回2；抽取歌曲信息失败返回3；临时目录获取失败，返回4
EXTERN_C __declspec(dllimport) int InitDATInfo(const char* pchInFileName,bool isOutXML = false);

//读取歌曲的所有音符信息，成功返回true,失败返回false
EXTERN_C __declspec(dllimport) bool GetSongPitch(_tDatPitchGroup*& fDatSongPitch,int& iDatSongPitchLength);

//读取歌曲的所有歌词信息，成功返回true,失败返回false
EXTERN_C __declspec(dllimport) bool GetSongLyric(_tDatOneLyric*& fDatSongLyric,int& iDatSongLyricLength);

//读取歌曲的所有句信息，成功返回true,失败返回false
EXTERN_C __declspec(dllimport) bool GetSongSentenceLine( unsigned long*& fAvSongSentenceLine,int& iAvSongSentenceLineLength);

//读取歌曲的总时间，单位毫秒
EXTERN_C __declspec(dllimport)  unsigned long __cdecl GetSongDuration();

//读取歌曲的所有歌词时间信息,成功返回0,文件为空返回1,文件不能访问为2,解析文件错误为3
EXTERN_C __declspec(dllimport) int GetLyricStruct(LyricStruct& outSentenceInfo);

EXTERN_C __declspec(dllimport) int SetDatUnit(unsigned long ulUnit,long ulOffsetUnit,unsigned char* text);

//EXTERN_C __declspec(dllimport) void GetOrigFetureElement(int& iLen,int& iAlgType,TWZ_REAL& tSongOrigStartTime, TWZ_REAL*& ptVolume,TWZ_REAL*& ptPitch,TWZ_REAL*& ptTempo);
//
//EXTERN_C __declspec(dllimport) void ClearOrigFetureElement();

//释放相关指针
EXTERN_C __declspec(dllimport) void ClearDatInfo();
#endif