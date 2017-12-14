#pragma once
#include <vector>
#include <Windows.h>

//-----------------------------------------------------------------------------
// 句评分等级
enum _eAvSentenceGradeLevel
{
	AVSENTENCEGRADELEVEL_NONE = 0,
	AVSENTENCEGRADELEVEL_MISS,
	AVSENTENCEGRADELEVEL_POOR,
	AVSENTENCEGRADELEVEL_GOOD,
	AVSENTENCEGRADELEVEL_GREAT,
	AVSENTENCEGRADELEVEL_PERFECT
};

//-----------------------------------------------------------------------------
// 伴奏声道信息
enum _eAvMusicTrack
{
	AVMUSICTRACK_LEFT = 0,
	AVMUSICTRACK_RIGHT,
	AVMUSICTRACK_ENGLISH,
	AVMUSICTRACK_CHINESE
};

//-----------------------------------------------------------------------------
// 单个音高点：每个音符对应起、止两个音高点。比如一个字有三个音符，那么这个字对应的音高组就包含6个音高点，分别对应这三个音符的起止时间。
struct _tAvPitch
{
	unsigned long	unit;
	unsigned long	tm;// 毫秒
	float			pitch;// 音高（介于_tAvFtoInfo::pitch_max和_tAvFtoInfo::pitch_min之间）
};

//-----------------------------------------------------------------------------
// 单个字的音高组
struct _tAvPitchGroup
{
	std::vector< _tAvPitch > pitch_list;// 音高列表
};

//-----------------------------------------------------------------------------
// 单个歌词数据（每个字开始的精确时间）
struct _tAvOneLyric
{
	unsigned long	tm;// 毫秒（以0起点）
	std::wstring	lyric;// 歌词（不一定是一个字，例如英文）
};

//-----------------------------------------------------------------------------
// 整首歌的音高信息
struct _tAvSongPitch
{
	std::vector< _tAvPitchGroup > pitchgroup_list;
};

//-----------------------------------------------------------------------------
// 整首歌的歌词信息
struct _tAvSongLyric
{
	std::vector< _tAvOneLyric >	lyric_list;
};

//-----------------------------------------------------------------------------
// 整首歌的句尾线信息
struct _tAvSongSentenceLine
{
	std::vector< unsigned long > sentenceline_list;// DWORD为每个句尾竖线的出现时间
};

//-----------------------------------------------------------------------------
// 整首歌曲的fto信息
struct _tAvFtoInfo
{
	float					pitch_max;			// 音高最大值
	float					pitch_min;			// 音高最小值
	unsigned long			begin_tm;			// 波形开始时间（毫秒）
	unsigned long			end_tm;				// 波形结束时间（毫秒）
	unsigned long			song_tm;			// 歌曲总时间
	_tAvSongPitch			song_pitch;			// 整首歌的音高信息
	_tAvSongLyric			song_lyric;			// 整首歌的歌词信息
	_tAvSongSentenceLine	song_sentenceline;	// 整首歌的句尾线信息
};

//-----------------------------------------------------------------------------
// 实时评分信息
struct _tAvRealtimeGrade
{
	unsigned long	cur_tm;				// 当前时间
	float			cur_pitch;			// 当前演唱者唱的音高
	int				sentence_index;		// 句序号（第一句序号为0）
	float			realtime_score;		// 当前句的实时得分
	float			accumulate_score;	// 当前句的累加得分
};

//-----------------------------------------------------------------------------
// 句评分信息
struct _tAvSentenceGrade
{
	int						sentence_index;	// 句序号（第一句序号为0）
	float					sentence_score;	// 句得分
	_eAvSentenceGradeLevel	sentence_level;	// 句评分等级
	float					combo_value;	// combo值
};

//-----------------------------------------------------------------------------
// 总评分信息
struct _tAvTotalGrade
{
	float total_score;// 总分
};


#define MAX_P2P_OUTBOUNDDATA_LEN	144

#ifndef MIC_EFFECT_NONE
#define MIC_EFFECT_NONE			0
#define MIC_EFFECT_WAVESREVERB	1
#endif

typedef struct  tagSDMP2PDataStruct				//P2P语音传输外带数据
{	
	int						nSequence;			//Seq序列
	_tAvRealtimeGrade		realTimeGradedData;	//当前实时评分数据
	_tAvSentenceGrade		sentenceGradedData;	//当前句评分数据
	_tAvTotalGrade			totalGradedData;	//歌曲总分数据
	int					    nInputTimeInMSec;	//当前播放时间
	BYTE				    outboundData[MAX_P2P_OUTBOUNDDATA_LEN]; //外带数据
}SDMP2PDataStruct;

//-----------------------------------------------------------------------------
// 评分回调类
class IAvGradeListener
{
public:
	// 实时评分回调函数
	virtual void onRealtimeGrade( _tAvRealtimeGrade * grade ) = 0;

	// 单句评分回调函数
	virtual void onSentenceGrade( _tAvSentenceGrade * grade ) = 0;

	// 总体评分回调函数
	virtual void onTotalGrade( _tAvTotalGrade * grade ) = 0;
};
//AvModule回调函数
class IAvStatusListener
{
public:
	//当对方音视频状态发生改变的时候会回调此接口
	//nUserID:用户的ID
	//bAudioStatus:音频是否打开，true表示打开，false表示关闭
	//bVideoStatus:视频是否打开，true表示打开，false表示关闭
	virtual void OnChangeOutputAVStatus(int nUserID, bool bAudioStatus, bool bVideoStatus) = 0;
	//input的视频数据
	virtual void OnInputVideoData(char *pBuffer, int nBufferLength) = 0;
	//output的视频数据
	virtual void OnOutputVideoData(int nUserID, char *pBuffer, int nBufferLength) = 0;
	//nValue:input的声音强度值，范围是0－100
	virtual void OnInputAuidoValue(int nValue) = 0;
	//nValue:output的声音强度值，范围是0－100
	//dwOffsetTime:播放的时间偏移值。
	virtual void OnOutputAuidoValue(int nUserID, int nValue, DWORD dwOffsetTime, BYTE *pBuffer = NULL, int nBufferLength = 0) = 0;
};

//-----------------------------------------------------------------------------
// 音频处理接口
// 所有接口均不返回错误，使用者无需关心返回值
// 错误会打印到音频模块自身的日志中
// 上个接口失败，不会导致下个接口崩溃
// 请求响应一类的接口，请求失败了，响应接口仍然返回
// 例如init失败了，再调用loadLyric不会出现错误，只是没有任何效果
class IAvSound
{
public:
	// 初始化语音处理模块，申请设备资源
	virtual void init( IAvGradeListener * grade_listener, IAvStatusListener * avstatus_listener, const std::string & traindata_path ) = 0;

	//释放设备资源，调用release后不用delete对象
	virtual	void release() = 0;

	// 准备播放歌曲
	// 演唱者调用
	virtual void preparePlaySong(const std::string & song_path, _eAvMusicTrack music_track ) = 0;

	// 请求加载歌词和fto文件信息
	// 演唱者、收听者都要使用
	// lyric_path：歌词文件绝对路径
	// fto_path：  fto文件绝对路径
	// dat_path：  dat文件绝对路径
	// out_avFtoInfo: 输出整首歌曲的fto信息
	virtual void loadLyricAndFto( const std::string & lyric_path, const std::string & fto_path, const std::string & dat_path, _tAvFtoInfo & out_avFtoInfo) = 0;

	//进入房间
	//room_idx：房间号
	//my_idx：  用户id
	virtual void enterRoom( int room_idx, int my_idx ) = 0;

	// 离开房间
	virtual void leaveRoom() = 0;

	// 开始演唱，并同步录音
	// 演唱者调用
	virtual void playSong() = 0;

	// 暂停演唱
	// 演唱者调用
	// 只能在练歌房中使用
	virtual void pauseSong() = 0;

	// 停止演唱
	// 演唱者调用
	virtual void stopSong() = 0;  

	// 是否开启原唱
	virtual bool isVoiceTrackEnable() = 0;

	// 开启原唱
	virtual void enableVoiceTrack( bool enable ) = 0;

	// 设置麦克风音效
	virtual void setMicEffectType( int nType ) = 0;

	// 获得当前麦克风音效
	virtual int getMicEffectType() = 0;

	// 设置麦克风音效参数
	virtual void setWavesReverbParam( float fInGain, float fReverbMix, float fReverbTime, float fRTRatio ) = 0;

	// 发送P2P音视频外带数据
	virtual void sendPacket( int packet_id, const char * buf, int buf_len ) = 0; 

	// 主音量是否为静音
	virtual bool getMasterMute() = 0;

	// 将主音量设置为静音
	virtual void setMasterMute( bool mute ) = 0;

	// 获取当前主音量值（0.0f~1.0f）
	virtual float getMasterCurrentVolume() = 0;

	// 设置当前主音量（0.0f~1.0f）
	virtual void setMasterCurrentVolume( float val ) = 0;

	// 获取当前麦克风音量（0.0f~1.0f）
	virtual float getMicCurrentVolume() = 0;

	// 设置当前麦克风音量（0.0f~1.0f）
	virtual void setMicCurrentVolume( float val ) = 0;

	// 伴奏音量是否为静音
	virtual bool getMusicMute() = 0;

	// 将伴奏音量设置为静音
	virtual void setMusicMute( bool mute ) = 0;

	// 获取当前伴奏音量（0.0f~1.0f）
	virtual float getMusicCurrentVolume() = 0;

	// 设置当前伴奏音量（0.0f~1.0f）
	virtual void setMusicCurrentVolume( float val )= 0;

	// 直接发送麦克风语音数据，纯语音无伴奏。演唱时系统自动发送语音与伴奏混合后的数据，无需调用此函数。
	virtual void startSendVoice() = 0;

	// 停止直接发送麦克风语音数据
	virtual void stopSendVoice() = 0;


	//=====================================
	//自己上麦
	virtual void InsertInput() = 0;	
	//自己下麦
	virtual void DeleteInput() = 0;
	//其他用户上麦
	virtual void InsertOutput(int nUserID) = 0;
	//其他用户下麦
	virtual void DeleteOutput(int nUserID) = 0;
	//设置自己的视频是否打开
	virtual void SetInputVideoStatus(bool bStatus) = 0;
	//设置自己的音频是否打开
	virtual void SetInputAudioStatus(bool bStatus) = 0;
	//设置其他用户的视频是否打开
	virtual void SetOutputVideoStatus(int nUserID, bool bStatus) = 0;
	//设置其他用户的音频是否打开
	virtual void SetOutputAudioStatus(int nUserID, bool bStatus) = 0;
};
