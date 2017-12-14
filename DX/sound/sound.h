/*
	注释时间:2014-4-25
	author: glp
	基于bass音频库的声音管理类，
*/
#pragma  once

#include "bass.h"
#include "../Opengl.h"

void CALLBACK LoopSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user);

class CGLSound
{
public:
	CGLSound();
	virtual ~CGLSound();
public:
	//bool				IsSoundInit(){return m_bInit;}
	//加载声音文件
	bool				soundLoad(const char* file,bool bloop=true);
	//设置声道音量
	bool				soundSetChannelVolume(const float v);
	//设置全局音量
	bool				soundSetGobalVolume(const float v);
	//播放
	bool				soundPlay();
	//暂停
	bool				soundPause();
	bool				soundStop();
	bool				soundSetLoopStart(QWORD pos);
	bool				soundSetLoopEnd(QWORD pos);
private:
// 	bool					m_bInit;
	HSTREAM		m_hsam;
	DWORD			m_nchan;//音频channel句柄
	HSYNC			m_lsync;		// looping sync
	DWORD			m_bpp;
};
