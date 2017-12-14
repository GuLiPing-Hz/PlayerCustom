#pragma once
#include <vector>
#include <Windows.h>

//-----------------------------------------------------------------------------
// �����ֵȼ�
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
// ����������Ϣ
enum _eAvMusicTrack
{
	AVMUSICTRACK_LEFT = 0,
	AVMUSICTRACK_RIGHT,
	AVMUSICTRACK_ENGLISH,
	AVMUSICTRACK_CHINESE
};

//-----------------------------------------------------------------------------
// �������ߵ㣺ÿ��������Ӧ��ֹ�������ߵ㡣����һ������������������ô����ֶ�Ӧ��������Ͱ���6�����ߵ㣬�ֱ��Ӧ��������������ֹʱ�䡣
struct _tAvPitch
{
	unsigned long	unit;
	unsigned long	tm;// ����
	float			pitch;// ���ߣ�����_tAvFtoInfo::pitch_max��_tAvFtoInfo::pitch_min֮�䣩
};

//-----------------------------------------------------------------------------
// �����ֵ�������
struct _tAvPitchGroup
{
	std::vector< _tAvPitch > pitch_list;// �����б�
};

//-----------------------------------------------------------------------------
// ����������ݣ�ÿ���ֿ�ʼ�ľ�ȷʱ�䣩
struct _tAvOneLyric
{
	unsigned long	tm;// ���루��0��㣩
	std::wstring	lyric;// ��ʣ���һ����һ���֣�����Ӣ�ģ�
};

//-----------------------------------------------------------------------------
// ���׸��������Ϣ
struct _tAvSongPitch
{
	std::vector< _tAvPitchGroup > pitchgroup_list;
};

//-----------------------------------------------------------------------------
// ���׸�ĸ����Ϣ
struct _tAvSongLyric
{
	std::vector< _tAvOneLyric >	lyric_list;
};

//-----------------------------------------------------------------------------
// ���׸�ľ�β����Ϣ
struct _tAvSongSentenceLine
{
	std::vector< unsigned long > sentenceline_list;// DWORDΪÿ����β���ߵĳ���ʱ��
};

//-----------------------------------------------------------------------------
// ���׸�����fto��Ϣ
struct _tAvFtoInfo
{
	float					pitch_max;			// �������ֵ
	float					pitch_min;			// ������Сֵ
	unsigned long			begin_tm;			// ���ο�ʼʱ�䣨���룩
	unsigned long			end_tm;				// ���ν���ʱ�䣨���룩
	unsigned long			song_tm;			// ������ʱ��
	_tAvSongPitch			song_pitch;			// ���׸��������Ϣ
	_tAvSongLyric			song_lyric;			// ���׸�ĸ����Ϣ
	_tAvSongSentenceLine	song_sentenceline;	// ���׸�ľ�β����Ϣ
};

//-----------------------------------------------------------------------------
// ʵʱ������Ϣ
struct _tAvRealtimeGrade
{
	unsigned long	cur_tm;				// ��ǰʱ��
	float			cur_pitch;			// ��ǰ�ݳ��߳�������
	int				sentence_index;		// ����ţ���һ�����Ϊ0��
	float			realtime_score;		// ��ǰ���ʵʱ�÷�
	float			accumulate_score;	// ��ǰ����ۼӵ÷�
};

//-----------------------------------------------------------------------------
// ��������Ϣ
struct _tAvSentenceGrade
{
	int						sentence_index;	// ����ţ���һ�����Ϊ0��
	float					sentence_score;	// ��÷�
	_eAvSentenceGradeLevel	sentence_level;	// �����ֵȼ�
	float					combo_value;	// comboֵ
};

//-----------------------------------------------------------------------------
// ��������Ϣ
struct _tAvTotalGrade
{
	float total_score;// �ܷ�
};


#define MAX_P2P_OUTBOUNDDATA_LEN	144

#ifndef MIC_EFFECT_NONE
#define MIC_EFFECT_NONE			0
#define MIC_EFFECT_WAVESREVERB	1
#endif

typedef struct  tagSDMP2PDataStruct				//P2P���������������
{	
	int						nSequence;			//Seq����
	_tAvRealtimeGrade		realTimeGradedData;	//��ǰʵʱ��������
	_tAvSentenceGrade		sentenceGradedData;	//��ǰ����������
	_tAvTotalGrade			totalGradedData;	//�����ܷ�����
	int					    nInputTimeInMSec;	//��ǰ����ʱ��
	BYTE				    outboundData[MAX_P2P_OUTBOUNDDATA_LEN]; //�������
}SDMP2PDataStruct;

//-----------------------------------------------------------------------------
// ���ֻص���
class IAvGradeListener
{
public:
	// ʵʱ���ֻص�����
	virtual void onRealtimeGrade( _tAvRealtimeGrade * grade ) = 0;

	// �������ֻص�����
	virtual void onSentenceGrade( _tAvSentenceGrade * grade ) = 0;

	// �������ֻص�����
	virtual void onTotalGrade( _tAvTotalGrade * grade ) = 0;
};
//AvModule�ص�����
class IAvStatusListener
{
public:
	//���Է�����Ƶ״̬�����ı��ʱ���ص��˽ӿ�
	//nUserID:�û���ID
	//bAudioStatus:��Ƶ�Ƿ�򿪣�true��ʾ�򿪣�false��ʾ�ر�
	//bVideoStatus:��Ƶ�Ƿ�򿪣�true��ʾ�򿪣�false��ʾ�ر�
	virtual void OnChangeOutputAVStatus(int nUserID, bool bAudioStatus, bool bVideoStatus) = 0;
	//input����Ƶ����
	virtual void OnInputVideoData(char *pBuffer, int nBufferLength) = 0;
	//output����Ƶ����
	virtual void OnOutputVideoData(int nUserID, char *pBuffer, int nBufferLength) = 0;
	//nValue:input������ǿ��ֵ����Χ��0��100
	virtual void OnInputAuidoValue(int nValue) = 0;
	//nValue:output������ǿ��ֵ����Χ��0��100
	//dwOffsetTime:���ŵ�ʱ��ƫ��ֵ��
	virtual void OnOutputAuidoValue(int nUserID, int nValue, DWORD dwOffsetTime, BYTE *pBuffer = NULL, int nBufferLength = 0) = 0;
};

//-----------------------------------------------------------------------------
// ��Ƶ����ӿ�
// ���нӿھ������ش���ʹ����������ķ���ֵ
// ������ӡ����Ƶģ���������־��
// �ϸ��ӿ�ʧ�ܣ����ᵼ���¸��ӿڱ���
// ������Ӧһ��Ľӿڣ�����ʧ���ˣ���Ӧ�ӿ���Ȼ����
// ����initʧ���ˣ��ٵ���loadLyric������ִ���ֻ��û���κ�Ч��
class IAvSound
{
public:
	// ��ʼ����������ģ�飬�����豸��Դ
	virtual void init( IAvGradeListener * grade_listener, IAvStatusListener * avstatus_listener, const std::string & traindata_path ) = 0;

	//�ͷ��豸��Դ������release����delete����
	virtual	void release() = 0;

	// ׼�����Ÿ���
	// �ݳ��ߵ���
	virtual void preparePlaySong(const std::string & song_path, _eAvMusicTrack music_track ) = 0;

	// ������ظ�ʺ�fto�ļ���Ϣ
	// �ݳ��ߡ������߶�Ҫʹ��
	// lyric_path������ļ�����·��
	// fto_path��  fto�ļ�����·��
	// dat_path��  dat�ļ�����·��
	// out_avFtoInfo: ������׸�����fto��Ϣ
	virtual void loadLyricAndFto( const std::string & lyric_path, const std::string & fto_path, const std::string & dat_path, _tAvFtoInfo & out_avFtoInfo) = 0;

	//���뷿��
	//room_idx�������
	//my_idx��  �û�id
	virtual void enterRoom( int room_idx, int my_idx ) = 0;

	// �뿪����
	virtual void leaveRoom() = 0;

	// ��ʼ�ݳ�����ͬ��¼��
	// �ݳ��ߵ���
	virtual void playSong() = 0;

	// ��ͣ�ݳ�
	// �ݳ��ߵ���
	// ֻ�������跿��ʹ��
	virtual void pauseSong() = 0;

	// ֹͣ�ݳ�
	// �ݳ��ߵ���
	virtual void stopSong() = 0;  

	// �Ƿ���ԭ��
	virtual bool isVoiceTrackEnable() = 0;

	// ����ԭ��
	virtual void enableVoiceTrack( bool enable ) = 0;

	// ������˷���Ч
	virtual void setMicEffectType( int nType ) = 0;

	// ��õ�ǰ��˷���Ч
	virtual int getMicEffectType() = 0;

	// ������˷���Ч����
	virtual void setWavesReverbParam( float fInGain, float fReverbMix, float fReverbTime, float fRTRatio ) = 0;

	// ����P2P����Ƶ�������
	virtual void sendPacket( int packet_id, const char * buf, int buf_len ) = 0; 

	// �������Ƿ�Ϊ����
	virtual bool getMasterMute() = 0;

	// ������������Ϊ����
	virtual void setMasterMute( bool mute ) = 0;

	// ��ȡ��ǰ������ֵ��0.0f~1.0f��
	virtual float getMasterCurrentVolume() = 0;

	// ���õ�ǰ��������0.0f~1.0f��
	virtual void setMasterCurrentVolume( float val ) = 0;

	// ��ȡ��ǰ��˷�������0.0f~1.0f��
	virtual float getMicCurrentVolume() = 0;

	// ���õ�ǰ��˷�������0.0f~1.0f��
	virtual void setMicCurrentVolume( float val ) = 0;

	// ���������Ƿ�Ϊ����
	virtual bool getMusicMute() = 0;

	// ��������������Ϊ����
	virtual void setMusicMute( bool mute ) = 0;

	// ��ȡ��ǰ����������0.0f~1.0f��
	virtual float getMusicCurrentVolume() = 0;

	// ���õ�ǰ����������0.0f~1.0f��
	virtual void setMusicCurrentVolume( float val )= 0;

	// ֱ�ӷ�����˷��������ݣ��������ް��ࡣ�ݳ�ʱϵͳ�Զ���������������Ϻ�����ݣ�������ô˺�����
	virtual void startSendVoice() = 0;

	// ֱֹͣ�ӷ�����˷���������
	virtual void stopSendVoice() = 0;


	//=====================================
	//�Լ�����
	virtual void InsertInput() = 0;	
	//�Լ�����
	virtual void DeleteInput() = 0;
	//�����û�����
	virtual void InsertOutput(int nUserID) = 0;
	//�����û�����
	virtual void DeleteOutput(int nUserID) = 0;
	//�����Լ�����Ƶ�Ƿ��
	virtual void SetInputVideoStatus(bool bStatus) = 0;
	//�����Լ�����Ƶ�Ƿ��
	virtual void SetInputAudioStatus(bool bStatus) = 0;
	//���������û�����Ƶ�Ƿ��
	virtual void SetOutputVideoStatus(int nUserID, bool bStatus) = 0;
	//���������û�����Ƶ�Ƿ��
	virtual void SetOutputAudioStatus(int nUserID, bool bStatus) = 0;
};
