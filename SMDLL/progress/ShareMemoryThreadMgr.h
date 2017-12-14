#pragma once
#include "ThreadMgr.h"
#include "ShareMemory.h"

enum MSG_SHAEREMEMORY
{
	//创建共享内存 wparam (char*) lparam(NULL)
	MSG_CREATESHAREMEMORY = MSG_INVALID+1,
	//关闭 wparam(NULL) lparam(NULL)
	MSG_DXPLAYER_CLOSE,
	//切歌 wparam(char*) lparam(NULL)
	MSG_SWITCH_SONG,//SwitchInfo
	//歌手图片 ！！！！！！（需要在CMD_DXPLAYER_SONGINFO之前发送）
	//wparam歌手图片(包含路径,后缀)
	MSG_SINGER_PIC,
	//播放/暂停  wparam(NULL) lparam(NULL)
	MSG_PLAY_CONTROL_RESUME,
	MSG_PLAY_CONTROL_PAUSE,
	//评分开关 wparam(NULL) lparam(NULL)
	MSG_PLAY_GRADE,
	//伴奏开关 wparam(NULL) lparam(NULL)
	MSG_PLAY_ACCOMPANY,
	//歌曲信息
	//歌曲信息 歌手名wparam(char*) 歌曲名lparam(char*)
	MSG_SONG_INFO,
	MSG_SONG_STARTEND,
	//调节音量 音量 wparam(int)
	MSG_SET_VOLUME,
	//获取音量
	MSG_GET_VOLUME,
	MSG_SET_MIC,
	//
	MSG_SET_ADD_TONE,
	MSG_SET_SUB_TONE,
	MSG_GET_TONE,
	//
	MSG_MUTE_OPEN,
	MSG_MUTE_CLOSE,
	//
	MSG_MESSAGE,
	MSG_OVERNINETY,
	MSG_FIREWARNING,
	MSG_NOFIREWARNING,
	MSG_NEXTSONG,
	MSG_EFFECTNOTIFY,
	MSG_ATOMOSPHERE,
	MSG_ONLINE,
	MSG_VIDEOINFO,
	MSG_RANKING,
	MSG_OTHERGRADE,
	MSG_PROGRESS,
	MSG_SELECTCATEGORY,
	MSG_SELECTSTATE,
	MSG_UPDATEMONEY,
	MSG_GIFTEXCHANGE,
	MSG_GIFTSEND,
	MSG_MUSICRANG,
	MSG_MICRANG,
	MSG_SHOPTOP,
	MSG_STUPDATE,
	MSG_PERISTATUS,
	MSG_SCAN,
	MSG_NOTIFYPOS,
	MSG_INITOPEN,
};

class CShareMemoryThreadMgr : public CThreadMgr
{
public:
	CShareMemoryThreadMgr(void);
	~CShareMemoryThreadMgr(void);

	static CShareMemoryThreadMgr & getSingleton();

public:

public:
	// 消息处理
	virtual void ProcessMessage( int id, void * pData );

	// 空闲处理
	virtual void ProcessIdle();

protected:
	// 读取ShareMemory
	void ReadShareMemory();

	void informServer(){::SetEvent(m_hP2CEvent);}
	void informClient(){::SetEvent(m_hC2PEvent);}
	void informNotify(){::SetEvent(m_hC2NEvent);}
private:
	CShareMemory	m_C2PShareMemory;
	CShareMemory	m_P2CShareMemory;
	CShareMemory  m_C2NShareMemory;

	bool			m_bC2PShareMemory;
	bool			m_bP2CShareMemory;
	bool			m_bC2NShareMemory;

	HANDLE			m_hC2PEvent;
	HANDLE			m_hP2CEvent;
	HANDLE			m_hC2NEvent;

	CShareMemory  m_smVideo;
	bool						m_bSmVideo;

	PROCESS_INFORMATION m_procinfoPlayer; //进程的信息
	PROCESS_INFORMATION m_procinfoNotify;
};
