#ifndef __FTOTYPE_H__
#define __FTOTYPE_H__

//-----------------------------------------------------------------------------
// 句评分等级
enum _eSentenceGradeLevel
{
	SENTENCEGRADELEVEL_NONE = 0,
	SENTENCEGRADELEVEL_MISS,
	SENTENCEGRADELEVEL_POOR,
	SENTENCEGRADELEVEL_GOOD,
	SENTENCEGRADELEVEL_GREAT,
	SENTENCEGRADELEVEL_PERFECT
};

//-----------------------------------------------------------------------------
// 单个音高点
typedef struct tagPitch
{
	unsigned int			begin_tm;					// 开始 毫秒（以0起点）
	unsigned int			end_tm;						// 结束 毫秒（以0起点）
	float							pitch;						// 音高（介于_tFtoFtoInfo::pitch_max和_tFtoFtoInfo::pitch_min之间）
}_tPitch;

//-----------------------------------------------------------------------------
// 单个字的音高组
typedef struct tagPitchGroup
{
	unsigned int		pitch_size;					// 音高列表项总数
	_tPitch *				pitch_list;					// 音高列表
}_tPitchGroup;

//-----------------------------------------------------------------------------
// 单个歌词数据（每个字开始的精确时间）
typedef struct tagWordInfo
{
	unsigned int			begin_tm;					// 开始 毫秒（以0起点）
	unsigned int			end_tm;						// 结束 毫秒（以0起点）
	unsigned int			lyric_size;					// 一个单词的字数，不包含'\0'结尾
	char*						lyric;						// 歌词（不一定是一个字，例如英文）
	//////////////////////////////////////////////////////////////////////////2012-12-14 glp
	bool							benglish;//歌词是否是英文
	char*						pingyin;//中文歌词的拼音(包含'\0')，如果是英文歌词则为NULL
}_tWordInfo;

//-----------------------------------------------------------------------------
//句子信息
typedef struct tagSentenceInfo
{
	unsigned int			beginMsec;					//开始毫秒
	unsigned int			endMsec;					//结束毫秒
	unsigned int			word_size;					// 单个歌词 个数
	_tWordInfo *			word_list;					// 歌词列表
	unsigned int			pitchgroup_size;			// 音高组个数
	_tPitchGroup *		pitchgroup_list;			// 音高组列表
}_tSentenceInfo;


//-----------------------------------------------------------------------------
// 整首歌曲的fto信息
typedef struct tagSongInfo
{
	float					pitch_max;					// 音高最大值
	float					pitch_min;					// 音高最小值
	unsigned int			begin_tm;					// 波形开始时间（毫秒）
	unsigned int			end_tm;						// 波形结束时间（毫秒）
	unsigned int			song_tm;					// 歌曲总时间
	unsigned int			sentence_size;				// 整首歌的歌曲总句数
	_tSentenceInfo *		sentence_info;				// 整首歌的多句歌曲
}_tSongInfo;

//-----------------------------------------------------------------------------
// 实时评分信息
typedef struct tagRealtimeGrade
{
	unsigned int			cur_tm;						// 当前时间
	float					cur_pitch;					// 当前演唱者唱的音高
	int						sentence_index;				// 句序号（第一句序号为0）
	float					realtime_score;				// 当前句的实时得分
	float					accumulate_score;			// 当前句的累加得分
}_tRealtimeGrade;

//-----------------------------------------------------------------------------
// 句评分信息
typedef struct tagSentenceGrade
{
	int						sentence_index;				// 句序号（第一句序号为0）
	float					sentence_score;				// 句得分
	_eSentenceGradeLevel	sentence_level;				// 句评分等级
	float					combo_value;				// combo值
}_tSentenceGrade;

//-----------------------------------------------------------------------------
// 总评分信息
typedef struct tagTotalGrade
{
	float					total_score;				// 总分
}_tTotalGrade;

#endif // __FTOTYPE_H__