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

//按照渲染先后顺序定义，请勿随意修改
#define COMOBJ "a默认"
#define SINGOBJF "唱歌1"
#define SINGOBJS "唱歌2"
#define STATISTICSOBJF "结算1"
#define STATISTICSOBJS "结算2"
#define XGVIDEOOBJ "视频"
#define NOTIFYOBJ "提醒"
#define FIROBJ "星光火警"

#endif// OBJECTDEF__H__
