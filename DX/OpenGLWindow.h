/*
	注释时间:2014-4-25
	author: glp
	COpenGLWindow 所有对象的最终管理者，保存播放器所有的状态，还有界面窗口，保存应用程序的一些基本信息，配置文件等所有。
	处理点歌台所有的业务，以及给予回应。相应请求。
	还有可能处理星光传递过来的视频，评分数据
*/
#pragma once

#include <string>
#include <string.h>
#include <stdio.h>

#include "Florid/GLFlorid.h"
#include "TimerEx.h"
#include "DrawListener.h"
#include "staff/image/ImgsetMgr.h"
#include "sound/soundmgr.h"
#include "progress/ShareMemoryThreadMgr.h"
#include "wave/WaveUnit.h"
#include "GLListDef.h"
#include "DX/Grapha.h"
#include "exception/CPlayerException.h"

typedef BOOL (WINAPI *GetSystemPowerStatusFunc)(LPSYSTEM_POWER_STATUS);

DWORD WINAPI VThread(LPVOID pParam);//绘画线程

typedef struct _MonitorStruct
{
	HMONITOR h;
	MONITORINFO mi;
}MonitorStruct;

struct CRenderTargetList
{
	int					width;
	int					height;
	IDirect3DTexture9*	pTex;
	IDirect3DSurface9*	pDepth;
	CRenderTargetList*	next;
};

struct CTextureList
{
	HTEXTURE			tex;
	int						width;
	int						height;
	CTextureList*		next;
};


struct CStreamList
{
	HSTREAM				hstream;
	void*						data;
	CStreamList*			next;
};
 LRESULT	CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

 enum MSG_GL_WINDOW
 {
	 MSGGL_CENTER_FIRST = MSG_INVALID+1,
	 //切换歌曲，第一次可以当成播放
	 //wparam歌曲名，包含完整路径和后缀名mpg/avi(char*)
	 //lparam曲谱高度(int)
	 MSGGL_DXPLAYER_SWITCHSONG,
	 //继续播放
	 MSGGL_DXPLAYER_CONTROL_RESUME,
	 //暂停
	 MSGGL_DXPLAYER_CONTROL_PAUSE,
	 //切换评分状态，
	 //wparam(int)0,关闭，1单人模式，2双人pk模式，3对唱模式
	 MSGGL_DXPLAYER_GRADE ,
	 //切换伴奏状态，开启/关闭
	 MSGGL_DXPLAYER_ACCOMPANY ,
	 //歌曲信息
	 //wparam歌手名(char*),lparam歌名(char*)
	 MSGGL_DXPLAYER_SONGINFO ,
	 //歌手图片 ！！！！！！（需要在CMD_DXPLAYER_SONGINFO之前发送）
	 //wparam歌手图片(包含路径,后缀)
	 MSGGL_DXPLAYER_SINGERPIC ,
	 //设置歌曲的起始秒跟结束秒
	 //wparam起始(int)lparam结束(int)
	 MSGGL_DXPLAYER_SONGSTARTEND ,
	 //获取当前播放器音量
	 MSGGL_DXPLAYER_GETMUSICVOLUME ,
	 //设置播放器声音 -10000到0: -10000最小，0最大.(静音传-10000)
	 //wparam音量(int)
	 MSGGL_DXPLAYER_SETMUSICVOLUME,
	 //获取播放器麦克风音量
	 MSGGL_DXPLAYER_GETMICVOLUME,
	 //设置播放器音量wparam(int)
	 MSGGL_DXPLAYER_SETMICVOLUME,
	 //获取播放器音调
	 MSGGL_DXPLAYER_GETTONEVALUE,
	 //加1音调
	 MSGGL_DXPLAYER_ADDTONEVALUE,
	 //减1音调
	 MSGGL_DXPLAYER_SUBTONEVALUE,
	 //通知下一首的歌曲名wparam(char*)
	 MSGGL_DXPLAYER_NEXT_SONGNAME,
	 //打开幻影
	 MSGGL_DXPLAYER_OPENHY,
	 //关闭幻影
	 MSGGL_DXPLAYER_CLOSEHY,
	 //音乐音量范围
	 MSGGL_DXPLAYER_MUSIC_RANGE,
	 //麦克风音量范围
	 MSGGL_DXPLAYER_MIC_RANGE,
	 //音调范围
	 MSGGL_DXPLAYER_TONE_RANGE,
	 //打开静音
	 MSGGL_DXPLAYER_MUTE_OPEN,
	 //关闭静音
	 MSGGL_DXPLAYER_MUTE_CLOSE,
	 //公告,弹幕
	 MSGGL_DXPLAYER_MESSAGE,
	 //评分超过90的广播
	 MSGGL_DXPLAYER_OVERNINITY,
	 //火警
	 MSGGL_DXPLAYER_FIREWARNING,
	 MSGGL_DXPLAYER_NOFIREWARNING,
	 //音效提示
	 MSGGL_DXPLAYER_EFFECTNOTIFY,
	 //气氛
	 MSGGL_DXPLAYER_ATMOSPHERE,
	 //是否联网
	 MSGGL_DXPLAYER_ONLINE,
	 //视频信息
	 MSGGL_VIDEO_INFO,
	 //唱歌排行
	 MSGGL_RANKINGLIST,
	 //互联网用户实时评测
	 MSGGL_OTHERGRADE,
	 //播放进度
	 MSGGL_PROGRESS,
	 //选歌分类
	 MSGGL_CATEGORY,
	 //控制选歌状态
	 MSGGL_SELECTSTATE,
	 //更新包厢金币信息
	 MSGGL_UPDATEMONEY,
	 //兑换礼物通知
	 MSGGL_GIFTEXCHANGE,
	 //礼物送礼通知
	 MSGGL_GIFTSEND,
	//门店排名
	 MSGGL_SHOPTOP,
	 //排行榜更新
	 MSGGL_STUPDATE,
	 //美女主播连接状态
	 MSGGL_PERISTATUS,
	 //扫码信息
	 MSGGL_SCAN,
	 //初始化开房
	 MSGGL_INITOPEN,
 };

class COS_HeadMessage;
class COS_NotifyMessage;
class WaveRender;
class CScoreFrame;
class SceneMgr;
class CFloridMgr;
class ImgsetMgr;
class CSwfMgr;

typedef struct _CurPlayInfo{
	/*
	对于MV，AD简单来讲，
		当MV为True的时候，无论AD为何值，都是当成唱歌的MV播放的，有两路音频
		当MV为False的时候，AD为True则当成是门店要求播放任意视频，只有一路音频
										，AD为Fasle则是播放美女主播，没有任何播放器的元素在里面
	*/
	bool												m_isMV;//MV ,定义播放演唱视频,主要是提供ktv用户唱歌的，不是MV的时候不需要评分等
	bool												m_bAdvertisement;//广告 ，定义播放一段门店需要的视频，只有一路音轨
	bool												m_bAntiphonalSinging;//是否对唱
	bool												m_bReverseAudio;//是否音轨对调
	wchar_t										m_strMdmName[260];//mdm 文件名
	int												m_nStaffHeight1;//第一曲谱高度 方向：自上到下
	int												m_nStaffHeight2;//第二曲谱高度
	bool												m_bGradeSong;//是否评分歌曲
	bool												m_bGrade;//是否显示最后的评测结果
	bool												m_bJump;//是否跳过头尾
	wchar_t										m_cur_singerpic[260];//歌手图片的路径（包含后缀）
	uint												m_song_start_ms;//歌曲起始时间
	uint												m_song_end_ms;//歌曲结束时间，0为默认播放整首
	wchar_t										m_cur_songFile[260];//当前歌曲文件名

	int												m_nScoreFirst;//第一个人的演唱分数
	int												m_nScoreSecond;//第二个人的演唱分数

	wchar_t										m_cur_singerName[260];//当前歌手名字
	wchar_t										m_cur_songName[260];//当前歌曲名字
	wchar_t										m_cur_backupSinger[260];//后备歌手名字

	unsigned int								m_notify_get_score_ms;//提醒去获取最后的分数，以及开始掉落金币

	bool												m_bNotifyScore;//通知最后的得分
}CurPlayInfo;

class COpenGLWindow : public CThreadMgr
{
	friend class CDrawListener;
private:
	COpenGLWindow(ITimerListener* pTimerListener=NULL);
	~COpenGLWindow(void);

	//处理点歌台发过来的信息，并给予回应
	virtual void ProcessMessage(int id,void* pData);
	virtual void ProcessIdle();
	//创建单独的视频绘画线程，保证效率。
	bool				createVideoEvalThread();
public:
	static COpenGLWindow*	getWindowSingleton();
	static void							releaseWindowSingleton();

	void						waitForRenderThread();
	//处理切换歌曲
	void						dealSwitchSong(CurPlayInfo* pCPI,bool bMy=false) throw(CPlayerException);
	//切换歌曲资源回收
	void						uninitSwitchSong();
	//切换到选歌
	bool						switchSelect();
	//切歌，可能正在播放一首歌曲，切换到新的歌曲。
	bool						switchSong(const wchar_t* fileName,const wchar_t* mdmFile,const int nSPH1,const int nSPH2) throw(CPlayerException);
	bool 					initTimerListener(HWND hWnd,CGLVideo* pglVideo);
	//更新星光过来的视频数据，歌手视频跟主持人视频
	void						updateVideo2Texture();
	//调整窗口布局
	void						adjustWindow(HWND hWnd,bool bWindowed,bool needNotify=true);
	void						setHYWindow(HWND hWnd,bool bTop);
	//virtual BOOL		PreTranslateMessage(MSG* msg);
	//virtual LRESULT  WindowProc(UINT message,WPARAM wParam,LPARAM lParam);
	//一些属性设置获取函数
	void						setHYHwndRect(HWND hHY,RECT& rect){m_hHY=hHY;m_rectHY=rect;}
	void						setTimerRunning(bool bTimerRunning){m_bTimerRunning = bTimerRunning;}
	bool						getTimeRunning(){return m_bTimerRunning;}
	CTimerEx*			&getTimerEx(){return m_pcTimeEx;}//获取定时器
	CRenderEngine*	getAllocator(){m_pDrawListener->AddRef();return (CRenderEngine*)m_pDrawListener;}
	void						setTimerListener(ITimerListener* pTimerListener){m_pcTimeEx->setListener(pTimerListener);}//设置监听器
	ITimerListener*	getTimerListener(){return m_pcTimeEx->getListener();}//获取监听器
	ImgsetMgr*		getImgsetMgr(){return m_pImgsetMgr;}
	SceneMgr*			getSceneMgr(){return m_pSceneMgr;}
	void						setWinSize(const GSize& winsize){m_winSize = winsize;setWinStyleRect();}
	GSize					getWinSize(){return m_winSize;}
	int						getWinWidth(){return m_winSize.width;}
	int						getWinHeight(){return m_winSize.height;}
	void						setWinStyleRect();
	void						setMonitor(HMONITOR h){m_hMonitor=h;}
	void						setWinRect(const RECT rectFS);
	void						postRealEndSongMessage();
	CGLSoundMgr*	getSoundMgr(){return m_pSoundMgr;}

	bool initMgr(HWND hwnd);
	//随机数
	virtual void				Random_Seed(int seed=0);
	virtual int				Random_Int(int min, int max);
	virtual float				Random_Float(float min, float max);
	//获取filter配置文件
	char*					getFilterIniFile(){return m_szIniFilter;}
	//获取应用程序资源配置文件
	char*					getszIniFile()  {return m_szIniFile;}
	wchar_t*				getszIniFileW(){return m_szIniFileW;}
	//获取应用程序目录
	char*					getAppDir()  {return m_currentDir;}
	wchar_t*				getAppDirW(){return m_currentDirW;}
	//获取filter相关目录
	wchar_t*				getFilterDllDir(){return m_wsFilterDir;}
private:
	static COpenGLWindow* cls_window;

	void				_InitPowerStatus();
	void				_UpdatePowerStatus();
	void				_DonePowerStatus();
private:
	unsigned int								m_seed;//随机种子
	CCritSec										m_csSleep;
	HWND											m_hHY;
	// Audio
	int												nSampleRate;
	int												nFXVolume;
	int												nMusVolume;
	int												nStreamVolume;

	bool												bUseSound;
	HINSTANCE								hBass;
	bool												bSilent;
	CStreamList*								streams;

	// Power
	int												nPowerStatus;
	HMODULE									hKrnl32;
	GetSystemPowerStatusFunc		lpfnGetSystemPowerStatus;//电源状态检测函数
	//info file
	char						m_currentDir[260];//当前目录
	wchar_t				m_currentDirW[260];
	char						m_szIniFile[260];//config.ini
	wchar_t				m_szIniFileW[260];//config.ini
	char						m_szIniFilter[260];//filter.ini
	wchar_t				m_wsFilterDir[260];//

	bool						m_bTimerRunning;//定时器状态，以前测试使用到
	CTimerEx	*			m_pcTimeEx;//定时器
public:
	CDrawListener*							m_pDrawListener;//保存一份，只保存地址，不需要释放(如果异常就释放)。

	CGLSoundMgr*							m_pSoundMgr;//声音管理器
	SceneMgr*									m_pSceneMgr;//场景对象管理器
	CFloridMgr*								m_pFloridMgr;//效果管理器
	ImgsetMgr*								m_pImgsetMgr;//纹理管理器
	CSwfMgr*									m_pSwfMgr;//swf特效管理器

	CShareMemoryThreadMgr		m_smtHPMgr;//共享内存管理器

	//待完善的歌曲随机选择功能，根据分类.rsv文件
	std::string									m_strCurCategory;	//当前分类											
	MAPCATEGORY							m_mapCategory;//分类map
	bool												m_bSelectSong;//是否处于选择歌曲状态
	HMONITOR								m_hMonitor;//保存当前的监视器
	CGraph*										m_pGraph;//多媒体管理器
	int												m_nRankingSleep;//随机睡眠时间
	int												m_nSoundVolume;//默认声效音量
	//声效
	CGLSound*									m_pSoundFireWarning;//火警声效
	CGLSound*									m_pSoundCheer;//欢呼声效
	CGLSound*									m_pSoundApplaud;//鼓掌声效
//////////////////////////////////////////////////////////////////////////
	MAPVIDEOTYPESWITCH			m_mapVideoControl;//视频控制字典
	MAPVIDEONAME						m_mapVideoName;//视频名字字典
	bool												m_bUpdateVideo;//是否更新视频数据
	CCritSec										m_csVideo;//视频数据更新保护锁

	//二维码
	std::wstring									m_downloadUrl;
	std::wstring									m_enterRoomUrl;

	typedef struct _VideoData{
		bool changed;
		unsigned char* p;
		_VideoData():changed(false),p(NULL){}
	}VideoData;
	VideoData									m_sHostVideo;//星光主持人视频数据
	VideoData									m_sSingVideo;//星光演唱者视频数据
	GSize											m_nVideoSize[2];//星光视频大小,前面2个主播w*h 后面2个演唱w*h
	uint												m_nBytesPerPixel;//星光视频像素大小
	uint												m_nVideoBytes[2];//星光视频一帧的大小
// 	byte*											m_nVideoBuffer;

	HANDLE										m_hEvent;//
	CCritSec										m_ObjectLock;
	bool												m_isFinish;
	//歌曲评分时长 用于插播金币特效
	ulong											m_uRealtimeGradeMs;
	//歌曲播放时长
	ulong											m_nDurationMs;
	//实时评分数据，来自星光
	unsigned int								m_nOhterCurTime;
	LISTGLREALTIMEGRADE			m_listOtherRG;
	LISTGLSENTENCEGRADE			m_listOtherSG;
	CCritSec										m_otherRGLock;//实时评分数据保护锁
	//用来阻止一些相关的操作
	bool												m_bGettingGold;//金币特效

	CCritSec										m_endScoreLock;//
	bool												m_bShowEndScore;//是否已经进入最后的评分画面
	//实时属性，需要播放的时候实时来获取的数据
	bool												m_bOnline;//ktv是否在线
	bool												m_bFireWarning;//是否需要显示火警信息
	bool												m_isNeedResetAccompany;//是否需要重置伴唱
	bool												m_isNeedResetShowType;//是否需要重置评分模式
	bool												m_isForceShowEndEval;//是否强制显示最后的评分
	bool												m_bAccompany;//是否伴唱
	bool												m_bMute;//是否静音
	eShowWave								m_eShowType;//显示模式
	bool												m_bChangedShowType;
	std::wstring									m_next_songName;//下一首歌曲名字
	bool												m_bChangedNextSN;

	StructSound								m_cSound;//声音结构信息
	//播放当前歌曲的一些配置信息，直到切换到下一首才改变
	std::wstring									m_cur_singerName;
	std::wstring									m_cur_backupSinger;
	std::wstring									m_cur_songName;
	std::wstring									m_cur_singerpic;
	CurPlayInfo									m_gCurPlayInfo;
	bool												m_isShowWaitingPeri;

	//窗口参数
	GSize											m_winSize;//窗口大小
	bool												m_bWindowed;//是否窗口模式
	RECT											m_rectFS;//全屏参数
	RECT											m_rectHY;//幻影窗口
	LONG											m_styleFS;
	RECT											m_rectW;//窗口参数
	LONG											m_styleW;
	std::vector<MonitorStruct>		m_vectmonitor;

	//录音设备
	VECTMIXERCAPS						m_vect_mxc;
	MAPSTRDEVID							m_mapmx_str_id;
	//记录上榜通知的字符串 礼物兑换通知信息
	CCritSec										m_csSpecial;
	LISTSPECIALINFO						m_listSpecialMsg;
	//通知公告信息
	CCritSec										m_csLeftMsg;
	LISTSTRING									m_listLeffMsg;
	//弹幕信息
	CCritSec										m_csBarrage;
	LISTBARRAGEINFO						m_listBarrage;
	//包厢金币信息
	CCritSec										m_csMoney;
	unsigned int								m_nMoney;
	//Swf特效,嵌入到DX的视频中
	CCritSec										m_csSwfEffect;
	LISTINT										m_listSwfEffect;
	bool												m_bInitOpenRoom;
};


extern COpenGLWindow* cls_gl;
