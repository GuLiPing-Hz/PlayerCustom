#ifndef __FTOTYPE_H__
#define __FTOTYPE_H__

//-----------------------------------------------------------------------------
// �����ֵȼ�
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
// �������ߵ�
typedef struct tagPitch
{
	unsigned int			begin_tm;					// ��ʼ ���루��0��㣩
	unsigned int			end_tm;						// ���� ���루��0��㣩
	float							pitch;						// ���ߣ�����_tFtoFtoInfo::pitch_max��_tFtoFtoInfo::pitch_min֮�䣩
}_tPitch;

//-----------------------------------------------------------------------------
// �����ֵ�������
typedef struct tagPitchGroup
{
	unsigned int		pitch_size;					// �����б�������
	_tPitch *				pitch_list;					// �����б�
}_tPitchGroup;

//-----------------------------------------------------------------------------
// ����������ݣ�ÿ���ֿ�ʼ�ľ�ȷʱ�䣩
typedef struct tagWordInfo
{
	unsigned int			begin_tm;					// ��ʼ ���루��0��㣩
	unsigned int			end_tm;						// ���� ���루��0��㣩
	unsigned int			lyric_size;					// һ�����ʵ�������������'\0'��β
	char*						lyric;						// ��ʣ���һ����һ���֣�����Ӣ�ģ�
	//////////////////////////////////////////////////////////////////////////2012-12-14 glp
	bool							benglish;//����Ƿ���Ӣ��
	char*						pingyin;//���ĸ�ʵ�ƴ��(����'\0')�������Ӣ�ĸ����ΪNULL
}_tWordInfo;

//-----------------------------------------------------------------------------
//������Ϣ
typedef struct tagSentenceInfo
{
	unsigned int			beginMsec;					//��ʼ����
	unsigned int			endMsec;					//��������
	unsigned int			word_size;					// ������� ����
	_tWordInfo *			word_list;					// ����б�
	unsigned int			pitchgroup_size;			// ���������
	_tPitchGroup *		pitchgroup_list;			// �������б�
}_tSentenceInfo;


//-----------------------------------------------------------------------------
// ���׸�����fto��Ϣ
typedef struct tagSongInfo
{
	float					pitch_max;					// �������ֵ
	float					pitch_min;					// ������Сֵ
	unsigned int			begin_tm;					// ���ο�ʼʱ�䣨���룩
	unsigned int			end_tm;						// ���ν���ʱ�䣨���룩
	unsigned int			song_tm;					// ������ʱ��
	unsigned int			sentence_size;				// ���׸�ĸ����ܾ���
	_tSentenceInfo *		sentence_info;				// ���׸�Ķ�����
}_tSongInfo;

//-----------------------------------------------------------------------------
// ʵʱ������Ϣ
typedef struct tagRealtimeGrade
{
	unsigned int			cur_tm;						// ��ǰʱ��
	float					cur_pitch;					// ��ǰ�ݳ��߳�������
	int						sentence_index;				// ����ţ���һ�����Ϊ0��
	float					realtime_score;				// ��ǰ���ʵʱ�÷�
	float					accumulate_score;			// ��ǰ����ۼӵ÷�
}_tRealtimeGrade;

//-----------------------------------------------------------------------------
// ��������Ϣ
typedef struct tagSentenceGrade
{
	int						sentence_index;				// ����ţ���һ�����Ϊ0��
	float					sentence_score;				// ��÷�
	_eSentenceGradeLevel	sentence_level;				// �����ֵȼ�
	float					combo_value;				// comboֵ
}_tSentenceGrade;

//-----------------------------------------------------------------------------
// ��������Ϣ
typedef struct tagTotalGrade
{
	float					total_score;				// �ܷ�
}_tTotalGrade;

#endif // __FTOTYPE_H__