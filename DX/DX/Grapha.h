/*
	注释时间:2014-4-25
	author: glp
	封装一个CGraph类，用于管理KTV mv的播放包含视频，音频的graph
	以及一切有关播放器的操作：播放，暂停，更改音调，音量，切换音轨等
*/
#pragma once
#include "vmrutil.h"
#include "../Opengl.h"
#include "iaudioswitch.h"
#include <map>
#include "../exception/CPlayerException.h"
class COpenGLWindow;

typedef std::map<std::wstring,HMODULE> MAPHMODULE;

enum eFileType
{
	avi,
	mp4,
	mpeg2,
};

class CRenderEngine;
class CGraph
{
public:
	CGraph(COpenGLWindow* pOwner);
	virtual ~CGraph(void);
	
	HRESULT	gStartGraph(HWND window,const wchar_t * fileName,const GSize &winsize,CRenderEngine* pallocator) throw(CPlayerException);

	HRESULT gCloseGraph();
	void errorUninit();
	/////////////////////////-----------0,成功；-1失败------------//////////////////////////////////
	//播放控制
	int ktvStartPlayer(bool bFirstAudio);

	int restartPlay();
	int startPlayer();
	int stopPlayer();
	int pausePlayer();
	int resumePlayer();
	//设置当前位置(毫秒)
	int setCurPosition(ulong nposition_ms,bool bAbsolute=true);
	//设置起始结束位置(毫秒)
	int setStartStopPosition(ulong nstart_ms,ulong nstop_ms);
	//获取当前位置(毫秒)
	int getPosition(ulong &nposition_ms);
	//获取媒体总长度:毫秒
	int getDuration(ulong &nduration_ms);
//////////////////////////////////////////////////////////////////////////
	//切换音轨会顿一下
	void switchAudio();
	//改善音轨切换体验
	void switchAudioEx(bool bFirstAudio);
	//改变音调
	void changeCurPitch(const int nPitch);
	//播放消息
	HRESULT getGraphEvent(LONG& evCode,LONG& evParam1,LONG evParam2);
	HRESULT freeGraphEvent(LONG evCode,LONG evParam1,LONG evParam2);
	//获得音量
	int getVolume(long &lVolume);
	//设置音量 -10000到0之间
	int setVolume(const long lVolume);
private:
	HRESULT setAllocatorPresenter( IBaseFilter* filter,CRenderEngine* pallocator );
	HRESULT initGraph(const wchar_t* filename,const GSize &winsize,CRenderEngine* pallocator) throw(CPlayerException);
	void			uinitGraph();

	HRESULT createObjectFromPath(REFCLSID clsid, IBaseFilter **ppUnk, const wchar_t* wPath);
	HRESULT createObjectFromPath(HMODULE hModule, REFCLSID clsid, IBaseFilter **ppUnk);
private:
	COpenGLWindow*				m_pOwner;
	ICaptureGraphBuilder2*	m_pBuilder;
	IGraphBuilder*					m_pGraph;

	MAPHMODULE					m_mapModule;

	IBaseFilter* 						m_pSourceFilter;
	IBaseFilter* 						m_pDemultiplexer ;
	IBaseFilter*							m_pAvisplitter;
	IBaseFilter* 						m_pVDecoder ;
	IBaseFilter* 						m_pADecoder1 ;
	IBaseFilter* 						m_pADecoder2 ;
	IBaseFilter* 						m_pAudioSwitcher ;
	IBaseFilter* 						m_pAudioRender;
	IBaseFilter*							m_pVMR9;//Video Mixing Render

	IFileSourceFilter*						m_pFileSourceFilter;
	IBasicAudio	*								m_pBasicAudio;
	IMediaControl*							m_pMediaControl;
	IMediaSeeking*							m_pMediaSeeking;
	IMediaEventEx*							m_pMediaEventEx;
	IAMStreamSelect*						m_pStreamSelect;
	IOS_AudioSwitch*						m_pOSAudioSwitch;
	IOS_ChangePitch*						m_pOSChangePitch;
	IVMRSurfaceAllocator9*			m_pAllocator;//
	//IVMRSurfaceAllocatorNotify9* m_lpIVMRSurfAllocNotify;

	bool												m_nStream;
	bool												m_bUseSameFilter;
	bool												m_binitGraph;
	HWND											m_hWnd;

public:
	// 初始化音轨
	void initAudioTrack(long nIndex);
};
