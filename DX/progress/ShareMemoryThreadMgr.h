/*
	注释时间:2014-4-25
	author: glp
	处理来自共享内存的消息线程类。
	用于与播放器和点歌台交互。
*/
#pragma once
#include "ThreadMgr.h"
#include "ShareMemory.h"

enum MSG_SHAEREMEMORY
{
	//打开共享内存 wparam (char*) lparam(NULL)
	MSG_OPENSHAREMEMORY = MSG_INVALID+1,
	//wparam(int*) lparam(int*)
	MSG_SCORESTARNUM,
	//歌曲结束 wparam(int*) lparam(NULL)
	MSG_ENDSONG,
	//写入当前播放器音乐音量 wparam(int*) lparam(NULL)
	MSG_MUSICVOLUME,
	//写入麦克风音量
	MSG_MICVOLUME,
	//写入音调
	MSG_TONEVALUE,
	//启用按钮
	MSG_ENABLEB,
	//禁用按钮 
	MSG_DISABLEB,
	//需要几条排名信息
	MSG_NEEDRANKINGNUM,
	//发送错误日志
	MSG_ERRORLOG,
	//位置移动
	MSG_NOTIFY_POS,
};
class COpenGLWindow;
class CShareMemoryThreadMgr : public CThreadMgr
{
public:
	CShareMemoryThreadMgr(void);
	~CShareMemoryThreadMgr(void);

	static CShareMemoryThreadMgr & getSingleton();

public:

public:
	// 消息处理 也就是写入共享内存
	//virtual void ProcessMessage( int id, void * wParam, void * lParam );
	virtual void ProcessMessage(int id,void* pData);

	// 空闲处理,也就是读取共享内存
	virtual void ProcessIdle();

protected:
	// 读取ShareMemory
	void ReadShareMemory();

	void informServer(){::SetEvent(m_hP2CEvent);}
	void informClient(){::SetEvent(m_hC2PEvent);}
private:
	CShareMemory	m_C2PShareMemory;
	CShareMemory	m_P2CShareMemory;
	bool			m_bC2PShareMemory;
	bool			m_bP2CShareMemory;
	HANDLE			m_hC2PEvent;
	HANDLE			m_hP2CEvent;
	CShareMemory  m_smVideo;
	bool						m_bSmVideo;

};
