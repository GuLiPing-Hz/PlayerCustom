#ifndef OBJECTDEF__H__ 
#define OBJECTDEF__H__

typedef struct _AwakeStatistics{
	bool					bFirst;
	wchar_t			m_wcur_singer_name[260];
	wchar_t			m_wcur_song_name[260];
	int					m_star_number;
	int					m_ave_dest_score;
	eShowWave	eType;
}AwakeStatistics;

//������Ⱦ�Ⱥ�˳���壬���������޸�
#define COMOBJ "aĬ��"
#define SINGOBJF "����1"
#define SINGOBJS "����2"
#define STATISTICSOBJF "����1"
#define STATISTICSOBJS "����2"
#define XGVIDEOOBJ "��Ƶ"
#define NOTIFYOBJ "����"
#define FIROBJ "�ǹ��"

#endif// OBJECTDEF__H__
