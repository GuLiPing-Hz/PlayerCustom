#include "stdafx.h"
#include "Grapha.h"
#include "util.h"
#include "RenderEngine.h"
#ifdef TEST_PLAY
#include "glp.h"
#else
#include "../OpenGLWindow.h"
#endif
#include <InitGuid.h>
#include "uguid.h"

static char g_strFileType[][50] = {"AVI","MP4","MPEG2"};
static wchar_t g_strModuleNameSpliter[][50]={L"",L"MP4Splitter.ax",L"MpegSplitter.ax"};
static wchar_t g_strModuleVideoCodec[] = L"ffdshow.ax";
static wchar_t g_strModuleAudioCodec[] = L"ffdshow.ax";
static wchar_t g_strModuleAudioSwitcher[] = L"AudioSwitch.ax";
static char g_defaultGUID[] = "00000000-0000-0000-0000-000000000000";


DEFINE_GUID(CLSID_FILESourceAsync,0xE436EBB5,0x524F,0x11CE,0x9F,0x53,0x00,0x20,0xAF,0x0B,0xA7,0x70);

#ifdef _GWINXP
int g_bCountToCreateListener = 0;
bool g_bNeedToReleaseListener = false;
#endif

#ifdef FAILDE_RETURNNEGATIVE
#undef FAILDE_RETURNNEGATIVE
#endif

#define FAILDE_RETURNNEGATIVE(exp) \
if(FAILED(exp))\
{\
	return -1;\
}

//	define the prototype of the class factory entry point in a COM dll
typedef HRESULT (STDAPICALLTYPE *DllGetClassObjectT)(REFCLSID clsid, REFIID iid, LPVOID *ppv);

CGraph::CGraph(COpenGLWindow* pOwner)
:m_pOwner(pOwner)
,m_pBuilder(NULL)
,m_pGraph(NULL)
,m_pSourceFilter(NULL)
,m_pDemultiplexer(NULL)
,m_pVDecoder(NULL)
,m_pADecoder1(NULL)
,m_pADecoder2(NULL)
,m_pAudioSwitcher(NULL)
,m_pAudioRender(NULL)
,m_pVMR9(NULL)
,m_pFileSourceFilter(NULL)
,m_pBasicAudio(NULL)
,m_pMediaControl(NULL)
,m_pMediaSeeking(NULL)
,m_pMediaEventEx(NULL)
,m_pStreamSelect(NULL)
,m_pOSAudioSwitch(NULL)
,m_pOSChangePitch(NULL)
,m_pAllocator(NULL)
,m_nStream(true)
// ,m_lpIVMRSurfAllocNotify(NULL)
,m_hWnd(NULL)
,m_binitGraph(false)
,m_bUseSameFilter(false)
{
	
}

CGraph::~CGraph()
{
	setAllocatorPresenter(m_pVMR9,NULL);
	SAFE_RELEASE(m_pVMR9);
	errorUninit();

	//释放模块库
	MAPHMODULE::iterator it = m_mapModule.begin();
	for( ; it!=m_mapModule.end() ; it++)
	{
		if(it->second != NULL)
		{
			FreeLibrary(it->second);
			it->second = NULL;
		}
	}
	m_mapModule.clear();
}

void CGraph::errorUninit()
{
	//if(m_pVMR9)
	//setAllocatorPresenter(m_pVMR9,NULL);
	//SAFE_RELEASE(m_pVMR9);
	m_bUseSameFilter = false;//彻底释放
	//SAFE_RELEASE(m_pAllocator);
	gCloseGraph();
}

void CGraph::uinitGraph()
{
	RemoveAllFilter(m_pGraph);
	SAFE_RELEASE(m_pFileSourceFilter);
	SAFE_RELEASE(m_pSourceFilter);

	
	if (!m_bUseSameFilter)
	{
		SAFE_RELEASE(m_pBasicAudio);
		SAFE_RELEASE(m_pMediaSeeking);
		SAFE_RELEASE(m_pMediaControl);
		if (m_pMediaEventEx)
		{
			m_pMediaEventEx->SetNotifyWindow(NULL,0,0);
		}
		SAFE_RELEASE(m_pMediaEventEx);

		SAFE_RELEASE(m_pGraph);
		SAFE_RELEASE(m_pBuilder);

		SAFE_RELEASE(m_pStreamSelect);
		SAFE_RELEASE(m_pOSAudioSwitch);
		SAFE_RELEASE(m_pOSChangePitch);

		//SAFE_RELEASE(m_pVMR9);
// 		int glp = 0;
// 		if(m_pDemultiplexer)
// 		{
// 			glp = m_pDemultiplexer->Release();
// 			if(glp == 0)
// 				m_pDemultiplexer = NULL;
// 		}
		SAFE_RELEASE(m_pDemultiplexer);
		SAFE_RELEASE(m_pVDecoder);
		SAFE_RELEASE(m_pADecoder1);
		SAFE_RELEASE(m_pADecoder2);
		SAFE_RELEASE(m_pAudioSwitcher);
		SAFE_RELEASE(m_pAudioRender);
	}
}

HRESULT CGraph::initGraph(const wchar_t* filename,const GSize &winsize,CRenderEngine* pallocator) throw(CPlayerException)
{
	HRESULT hr = S_OK;
	std::wstring error_msg;

	if( !filename || filename[0]==0 )
	{
		hr = E_INVALIDARG;
		error_msg = std::wstring(L"file url error[") + std::wstring(filename) + std::wstring(L"]");
		goto failed;
	}
	
	eFileType eft;
	if (wcsstr(filename,L".avi"))
		eft = avi;
	else if(wcsstr(filename,L".mp4") || wcsstr(filename,L".mov") || wcsstr(filename,L".st4"))
		eft = mp4;
	else
		eft = mpeg2;

	if (!m_binitGraph)
	{
		//hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2,(void**)&m_pBuilder);
		hr = CoCreateInstance(CLSID_FilterGraph,NULL,/*CLSCTX_ALL*/CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&m_pGraph);
		//The Filter Graph Manager is provided by an in-process DLL, so the execution context is CLSCTX_INPROC_SERVER.
		if (FAILED(hr))
		{
			error_msg = L"create CLSID_FilterGraph Failed";
			goto failed;
		}
		//设置图表
		//hr = m_pBuilder->SetFiltergraph(m_pgraph);
		hr = m_pGraph->QueryInterface(IID_IBasicAudio,(void**)&m_pBasicAudio);
		if (FAILED(hr))
		{
			error_msg = L"IID_IBasicAudio Query Failed";
			goto failed;
		}
		hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pMediaControl);
		if (FAILED(hr))
		{
			error_msg = L"IID_IMediaControl Query Failed";
			goto failed;
		}
		hr = m_pGraph->QueryInterface(IID_IMediaEventEx,(void**)&m_pMediaEventEx);
		if (FAILED(hr))
		{
			error_msg = L"IID_IMediaEventEx Query Failed";
			goto failed;
		}
		hr = m_pGraph->QueryInterface(IID_IMediaSeeking,(void**)&m_pMediaSeeking);
		if (FAILED(hr))
		{
			error_msg = L"IID_IMediaSeeking Query Failed";
			goto failed;
		} 

		if (m_pMediaSeeking)
			FAIL_GOTOFAILED(m_pMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME));
		if (m_pMediaEventEx)
			FAIL_GOTOFAILED(m_pMediaEventEx->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0));

		/* new source filter*/
		IFileSourceFilter* pFileSourceFilter = NULL;
		hr = CoCreateInstance(CLSID_FILESourceAsync, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pSourceFilter);
		if (FAILED(hr))
		{
			error_msg =  L"SourceFileter Add Failed\r\n";
			goto failed;
		}
		hr = m_pSourceFilter->QueryInterface(IID_IFileSourceFilter,(void**)&pFileSourceFilter);
		if (FAILED(hr))
		{
			error_msg =  L"IID_IFileSourceFilter Query Failed\r\n";
			goto failed;
		}
		hr = pFileSourceFilter->Load(filename,NULL);
		if(FAILED(hr))
		{
			error_msg =  L"IID_IFileSourceFilter Load File Failed\r\n";
			pFileSourceFilter->Release();
			goto failed;
		}
		pFileSourceFilter->Release();

		GUID clsid_demultiplexer;
		hr = CLSIDFromString(Ini_GetStringW(m_pOwner->getFilterIniFile(),g_strFileType[eft],"DEMULTIPLEXER",g_defaultGUID),&clsid_demultiplexer);
		if (FAILED(hr))
		{
			error_msg =  L"read demultiplexer clsid failed";
			goto failed;
		}
#ifndef USEAX_AS_DLL
		hr = CoCreateInstance(clsid_demultiplexer, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pDemultiplexer);
#else
		wchar_t wDllPath[300] = {0};
		swprintf(wDllPath,L"%s\\%s",m_pOwner->getFilterDllDir(),g_strModuleNameSpliter[eft]);
		hr = createObjectFromPath(clsid_demultiplexer,&m_pDemultiplexer,wDllPath);
#endif
		if (FAILED(hr))
		{
			error_msg =  L"create CLSID_Demultiplexer Failed";
			goto failed;
		}
		GUID clsid_vdecoder;
		hr = CLSIDFromString(Ini_GetStringW(m_pOwner->getFilterIniFile(),g_strFileType[eft],"VIDEODECODER",g_defaultGUID),&clsid_vdecoder);
		if (FAILED(hr))
		{
			error_msg =  L"read vdecoder clsid failed";
			goto failed;
		}
#ifndef USEAX_AS_DLL
		hr = CoCreateInstance(clsid_vdecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pVDecoder);
#else
		swprintf(wDllPath,L"%s\\%s",m_pOwner->getFilterDllDir(),g_strModuleVideoCodec);
		hr = createObjectFromPath(clsid_vdecoder,&m_pVDecoder,wDllPath);
#endif
		if (FAILED(hr))
		{
			error_msg =  L"create CLSID_VideoDecoder Failed";
			goto failed;
		}
		GUID clsid_adecoder1;
		hr = CLSIDFromString(Ini_GetStringW(m_pOwner->getFilterIniFile(),g_strFileType[eft],"AUDIODECODER1",g_defaultGUID),&clsid_adecoder1);
		if (FAILED(hr))
		{
			error_msg =  L"read adecoder1 clsid failed";
			goto failed;
		}
#ifndef USEAX_AS_DLL
		hr = CoCreateInstance(clsid_adecoder1, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pADecoder1);
#else
		swprintf(wDllPath,L"%s\\%s",m_pOwner->getFilterDllDir(),g_strModuleAudioCodec);
		hr = createObjectFromPath(clsid_adecoder1,&m_pADecoder1,wDllPath);
#endif
		if (FAILED(hr))
		{
			error_msg =  L"create CLSID_AudioDecoder1 Failed";
			goto failed;
		}
		GUID clsid_adecoder2;
		hr = CLSIDFromString(Ini_GetStringW(m_pOwner->getFilterIniFile(),g_strFileType[eft],"AUDIODECODER2",g_defaultGUID),&clsid_adecoder2);
		if (FAILED(hr))
		{
			error_msg =  L"read adecoder2 clsid failed";
			goto failed;
		}
#ifndef USEAX_AS_DLL
		hr = CoCreateInstance(clsid_adecoder2, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pADecoder2);
#else
		swprintf(wDllPath,L"%s\\%s",m_pOwner->getFilterDllDir(),g_strModuleAudioCodec);
		hr = createObjectFromPath(clsid_adecoder2,&m_pADecoder2,wDllPath);
#endif
		if (FAILED(hr))
		{
			error_msg =  L"create CLSID_AudioDecoder2 Failed";
			goto failed;
		}
		GUID clsid_audioswitch;
		hr = CLSIDFromString(Ini_GetStringW(m_pOwner->getFilterIniFile(),g_strFileType[eft],"AUDIOSWITCH",g_defaultGUID),&clsid_audioswitch);
		if (FAILED(hr))
		{
			error_msg =  L"read audioswitch clsid failed";
			goto failed;
		}
#ifndef USEAX_AS_DLL
		hr = CoCreateInstance(clsid_audioswitch, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pAudioSwitcher);
#else
		swprintf(wDllPath,L"%s\\%s",m_pOwner->getFilterDllDir(),g_strModuleAudioSwitcher);
		hr = createObjectFromPath(clsid_audioswitch,&m_pAudioSwitcher,wDllPath);
#endif
		if (FAILED(hr))
		{
			error_msg =  L"create CLSID_AudioSwitcher Failed";
			goto failed;
		}
		hr = m_pAudioSwitcher->QueryInterface(IID_IAMStreamSelect,(void**)&m_pStreamSelect);
		if (FAILED(hr))
		{
			error_msg =  L"IID_IAMStreamSelect Query Failed";
			goto failed;
		}
		hr = m_pAudioSwitcher->QueryInterface(IID_IOS_AudioSwitch,(void**)&m_pOSAudioSwitch);
		if (FAILED(hr))
		{
			error_msg =  L"IID_IOS_AudioSwitch Query Failed";
			goto failed;
		}
		hr = m_pAudioSwitcher->QueryInterface(IID_IOS_ChangePitch,(void**)&m_pOSChangePitch);
		if (FAILED(hr))
		{
			error_msg =  L"IID_IOS_ChangePitch Query Failed";
			goto failed;
		}
		hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pAudioRender);
		if (FAILED(hr))
		{
			error_msg =  L"CLSID_DSoundRender Failed";
			goto failed;
		}
		if (m_bUseSameFilter)//只创建一次filter
		{
			m_binitGraph = true;
		}
 	}

// 	hr = m_pGraph->AddSourceFilter(filename,L"File Source (Async.)",&m_pSourceFilter);//Fuck 这个函数有毒。。。
// 	if (FAILED(hr))
// 	{
// 		error_msg =  L"SourceFileter Add Failed\r\n";
// 		goto failed;
// 	}

	FAIL_GOTOFAILED(m_pGraph->AddFilter(m_pSourceFilter,L"FileSource"));

	FAIL_GOTOFAILED(m_pGraph->AddFilter(m_pDemultiplexer,L"Demultiplexer"));
	
	FAIL_GOTOFAILED(m_pGraph->AddFilter(m_pVDecoder,L"Video Decoder"));

	if(m_pOwner->m_gCurPlayInfo.m_isMV || m_pOwner->m_gCurPlayInfo.m_bAdvertisement)
	{
		FAIL_GOTOFAILED(m_pGraph->AddFilter(m_pADecoder1,L"Audio Decoder1"));
		if(m_pOwner->m_gCurPlayInfo.m_isMV)//如果是MV则需要连接第二条音轨
		{
			FAIL_GOTOFAILED(m_pGraph->AddFilter(m_pADecoder2,L"Audio Decoder2"));//先提供单音轨的demo
		}
		FAIL_GOTOFAILED(m_pGraph->AddFilter(m_pAudioSwitcher,L"Audio Switch"));
		FAIL_GOTOFAILED(m_pGraph->AddFilter(m_pAudioRender,L"DSound Render"));
	}

	if (!m_pVMR9)
	{
		hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, __uuidof(IBaseFilter), (void**)&m_pVMR9);
		if (FAILED(hr))
		{
			error_msg =  L"CLSID_VideoMixingRenderer9 Create Failed\r\n";
			goto failed;
		}

		IVMRFilterConfig9 *filterConfig = NULL;
		FAIL_GOTOFAILED( m_pVMR9->QueryInterface(IID_IVMRFilterConfig9, (void**)&filterConfig) );

		hr = filterConfig->SetRenderingMode( VMR9Mode_Renderless  );//VMR9Mode_Windowless
		//FAIL_GOTOFAILED( filterConfig->SetNumberOfStreams(1) );//VMR9AllocFlag_TextureSurface
		filterConfig->Release();
		FAIL_GOTOFAILED(hr);
	}

	if (FAILED(hr = setAllocatorPresenter( m_pVMR9,pallocator )))
	{
		error_msg =  L"setAllocatorPresenter Failed";
		goto failed;
	}
	if( FAILED(hr = m_pGraph->AddFilter(m_pVMR9, L"Video Mixing Renderer 9")) )
	{
		error_msg =  L"add vmr9 failsed";
		goto failed;
	}

	//FAIL_RET( SetAllocatorPresenter( m_pVMR9, winsize,pallocator ) );//+1
	{
		if( FAILED(hr = TryConnectFilter(m_pGraph,m_pSourceFilter,m_pDemultiplexer)))
		{
			error_msg =  L"connect filter SourceFilter Demultiplexer Failed";
			goto failed;
		}
		if( FAILED(hr = TryConnectFilter(m_pGraph,m_pDemultiplexer,m_pVDecoder)))
		{
			error_msg =  L"connect filter Demultiplexer VDecoder Failed";
			goto failed;
		}

		if(m_pOwner->m_gCurPlayInfo.m_isMV || m_pOwner->m_gCurPlayInfo.m_bAdvertisement)
		{
			if( FAILED(hr = TryConnectFilter(m_pGraph,m_pDemultiplexer,m_pADecoder1)))
			{
				error_msg =  L"connect filter Demultiplexer ADecoder1 failed";
				goto failed;
			}

			//goto failed; //for debug
			if(m_pOwner->m_gCurPlayInfo.m_isMV)//mv才需要连接第二条音轨
			{
				if( FAILED(hr = TryConnectFilter(m_pGraph,m_pDemultiplexer,m_pADecoder2)))
				{
					error_msg =  L"connect filter Demultiplexer ADecoder2 failed! The song may have only one audio line";
					goto failed;
				}
			}
		}
	}

	if (FAILED(hr = TryConnectFilter(m_pGraph,m_pVDecoder,m_pVMR9)))
	{
		error_msg =  L"connect filter VDecoder VMR9 Failed";
		goto failed;
	}

	if(m_pOwner->m_gCurPlayInfo.m_isMV || m_pOwner->m_gCurPlayInfo.m_bAdvertisement)
	{
		if( FAILED(hr = TryConnectFilter(m_pGraph,m_pADecoder1,m_pAudioSwitcher)))
		{
			error_msg =  L"connect filter ADecoder1 AudioSwitch Failed";
			goto failed;
		}
		if(m_pOwner->m_gCurPlayInfo.m_isMV)
		{
			if( FAILED(hr = TryConnectFilter(m_pGraph,m_pADecoder2,m_pAudioSwitcher)))
			{
				error_msg =  L"connect filter ADecoder2 AudioSwitch!";
				//m_pAllocator = NULL;
				//return hr;
				hr = S_OK;
			}
		}
		
		if( FAILED(hr = TryConnectFilter(m_pGraph,m_pAudioSwitcher,m_pAudioRender)))
		{
			error_msg =  L"connect filter AudioSwitch AudioRender Failed";
			goto failed;
		}
	}
	return hr;

failed:
	//m_pAllocator = NULL;
  	if(m_pVMR9)
  		setAllocatorPresenter( m_pVMR9,NULL );
 	SAFE_RELEASE(m_pVMR9);

	if(error_msg.empty())
		error_msg = L"common initGraph failed!";
	
	throw CPlayerException(error_msg.c_str());
	return hr;
}


HRESULT CGraph::gStartGraph(HWND window,const wchar_t * fileName,const GSize &winsize,CRenderEngine* pallocator) throw(CPlayerException)
{
	/*gCloseGraph(m_hWnd);*/
	m_hWnd = window;

	HRESULT hr = E_FAIL;

	try
	{
		hr = initGraph(fileName,winsize,pallocator);
		return hr;
	}
	catch (CPlayerException& ex)
	{
		throw ex;
		return E_FAIL;
	}
}

HRESULT CGraph::gCloseGraph()
{//433
	if( m_pMediaControl != NULL ) 
	{
		OAFilterState state;
		do {//1427 26-8
			m_pMediaControl->Stop();
			//1427-89
			m_pMediaControl->GetState(0, & state );
		} while( state != State_Stopped ) ;
	}

#ifdef _GWINXP
	if (g_bNeedToReleaseListener)
	{
#endif
		//SAFE_RELEASE(m_pallocator);
		if (m_pAllocator)
		{
// 			ulong nCount = m_pAllocator->Release();
// 			w riteLog("Allocator count : %d",nCount);
		}
#ifdef _GWINXP
		g_bNeedToReleaseListener = false;
	}
#endif
	//SAFE_RELEASE(m_pallocator);只有一个allocator
	uinitGraph();

	return S_OK;
}

HRESULT CGraph::setAllocatorPresenter( IBaseFilter* filter,CRenderEngine* pallocator )
{
	if( filter == NULL )
		return E_FAIL;

	HRESULT hr;

	IVMRSurfaceAllocatorNotify9*	 lpIVMRSurfAllocNotify = NULL;
	FAIL_RET( filter->QueryInterface(IID_IVMRSurfaceAllocatorNotify9, reinterpret_cast<void**>(&lpIVMRSurfAllocNotify)) );//9
	if (m_pAllocator)//
	{
		m_pAllocator->AdviseNotify(NULL) ;
		int nCount = m_pAllocator->Release();
		m_pAllocator = NULL;
	}
	// create our surface allocator
	m_pAllocator = pallocator;

	// let the allocator and the notify know about each other206
	if(m_pAllocator)
	{
		hr = m_pAllocator->AdviseNotify(lpIVMRSurfAllocNotify) ;
		if (FAILED(hr))
		{
			WLOGH_ERRO << L"AdviseNotify failed";
			goto failed;
		}
	}
	hr = lpIVMRSurfAllocNotify->AdviseSurfaceAllocator( (DWORD_PTR)this, m_pAllocator );
	//int n = lpIVMRSurfAllocNotify->Release();
failed:
	return hr;
}

void CGraph::switchAudioEx(bool bFirstAudio)
{
	if (m_pOSAudioSwitch)
	{
		m_pOSAudioSwitch->SwitchATrack(bFirstAudio);
	}
}

void CGraph::changeCurPitch(const int nPitch)
{
	int tmp_nPitch = nPitch;
	tmp_nPitch = tmp_nPitch<-5?-5:(tmp_nPitch>5?5:tmp_nPitch);
	if (m_pOSChangePitch)
	{
		m_pOSChangePitch->ChangeCurPitch(tmp_nPitch);
	}
}

void CGraph::switchAudio()
{
	if (m_pStreamSelect)
	{
		DWORD nStreams=1;
		m_pStreamSelect->Count(&nStreams);
		if (nStreams>1)
		{
			if (m_nStream)
			{
				m_pStreamSelect->Enable(1,AMSTREAMSELECTENABLE_ENABLE);//
				m_nStream = false;
			}
			else
			{
				m_pStreamSelect->Enable(0,AMSTREAMSELECTENABLE_ENABLE);//0
				m_nStream = true;
			}

		}
	}
}

int CGraph::restartPlay()
{
	if(0 != setCurPosition(0))
	{
		return -1;
	}
	return startPlayer();
}

int CGraph::ktvStartPlayer(bool bFirstAudio)
{
	if(m_pMediaControl)
	{
		switchAudioEx(bFirstAudio);//手动连接的时候，SwitchInit已经初始化完成，所以可以放在前面。
		return startPlayer();
	}
	return 0;
}

int CGraph::startPlayer()
{
	if (m_pMediaControl)
	{
		FAILDE_RETURNNEGATIVE(m_pMediaControl->Run());
	}
	return 0;
}

int CGraph::stopPlayer()
{
	if( m_pMediaControl ) 
	{
		OAFilterState state;
		do {
			m_pMediaControl->Stop();
			m_pMediaControl->GetState(0, & state );
		} while( state != State_Stopped ) ;
	}
	return 0;
}

int CGraph::pausePlayer()
{
	if (m_pMediaControl)
	{
		OAFilterState state;
		m_pMediaControl->GetState(0,&state);
		if(state == State_Running)
		{
			FAILDE_RETURNNEGATIVE(m_pMediaControl->Pause());
		}
	}
	return 0;
}

int CGraph::resumePlayer()
{
	if (m_pMediaControl)
	{
		OAFilterState state;
		m_pMediaControl->GetState(0,&state);
		if (state == State_Paused)
		{
			FAILDE_RETURNNEGATIVE(m_pMediaControl->Run());
		}
	}
	return 0;
}

//设置当前位置(毫秒)
int CGraph::setCurPosition(ulong nposition_ms,bool bAbsolute)
{
	if (m_pMediaSeeking)
	{
		LONGLONG nanosecond_unit = nposition_ms*10000;
		DWORD flagCur = bAbsolute?(AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame):(AM_SEEKING_RelativePositioning|AM_SEEKING_SeekToKeyFrame);
		FAILDE_RETURNNEGATIVE(m_pMediaSeeking->SetPositions(&nanosecond_unit, flagCur, 0, AM_SEEKING_NoPositioning));
	}
	return 0;
}

//设置起始结束位置(毫秒)
int CGraph::setStartStopPosition(ulong nstart_ms,ulong nstop_ms)
{
	if (m_pMediaSeeking)
	{
		LONGLONG nanosecond_start_unit = nstart_ms*10000;
		LONGLONG nanosecond_stop_unit = nstop_ms*10000;
		FAILDE_RETURNNEGATIVE(m_pMediaSeeking->SetPositions(&nanosecond_start_unit, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
			&nanosecond_stop_unit, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame));
	}
	return 0;
}
//获取当前位置(毫秒)
int CGraph::getPosition(ulong &nposition_ms)
{
	if (m_pMediaSeeking)
	{
		LONGLONG nanosecond_unit;
		FAILDE_RETURNNEGATIVE(m_pMediaSeeking->GetCurrentPosition(&nanosecond_unit));
		nposition_ms = ulong(nanosecond_unit/10000);//毫秒
	}
	return 0;
}

int CGraph::getDuration(ulong &nduration_ms)
{
	if (m_pMediaSeeking)
	{
		LONGLONG nanosecond_unit;
		FAILDE_RETURNNEGATIVE(m_pMediaSeeking->GetDuration(&nanosecond_unit));
		nduration_ms = ulong(nanosecond_unit/10000);//毫秒
	}
	return 0;
}


int CGraph::getVolume(long &lVolume)
{
	if (m_pBasicAudio)
	{
		FAILDE_RETURNNEGATIVE(m_pBasicAudio->get_Volume(&lVolume))
	}
	return 0;
}

int CGraph::setVolume(const long lVolume)
{
	if (m_pBasicAudio)
	{
		FAILDE_RETURNNEGATIVE(m_pBasicAudio->put_Volume(lVolume));
	}
	return 0;
}

HRESULT CGraph::getGraphEvent(LONG& evCode,LONG& evParam1,LONG evParam2)
{
	HRESULT hr = E_FAIL;
	if (m_pMediaEventEx)
	{
		hr = m_pMediaEventEx->GetEvent(&evCode,(LONG_PTR*)&evParam1,(LONG_PTR*)&evParam2,0);
	}
	return hr;
}

HRESULT CGraph::freeGraphEvent(LONG evCode,LONG evParam1,LONG evParam2)
{
	HRESULT hr = E_FAIL;
	if (m_pMediaEventEx)
	{
		hr = m_pMediaEventEx->FreeEventParams(evCode,evParam1,evParam2);
	}
	return hr;
}

void CGraph::initAudioTrack(long nIndex)
{
	if (m_pStreamSelect)
	{
		DWORD nStreams=1;
		m_pStreamSelect->Count(&nStreams);
		if (nStreams>1)
		{
			m_pStreamSelect->Enable(nIndex,AMSTREAMSELECTENABLE_ENABLE);
			if (nIndex == 0)
			{
				m_nStream = true;
			}
			else
			{
				m_nStream = false;
			}
		}
	}
}

HRESULT CGraph::createObjectFromPath(REFCLSID clsid, IBaseFilter **ppUnk, const wchar_t* wPath)
{
	if(!wPath || !ppUnk)
		return E_FAIL;

	std::wstring wstrPath = std::wstring(wPath);
	bool isFromLoad = false;
	HMODULE hModule = NULL;
	if(m_mapModule.find(wstrPath) == m_mapModule.end())
	{
		//	load the target DLL directly
		hModule = LoadLibraryW(wPath);
		isFromLoad = true;
	}
	else
		hModule = m_mapModule[wstrPath];

	if (hModule == NULL) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if(SUCCEEDED(createObjectFromPath(hModule, clsid, ppUnk)))
	{
		if(isFromLoad)
			m_mapModule.insert(std::make_pair(wstrPath,hModule));
		return S_OK;
	}
	else
	{
		//这里应该不会是map里面的HModule
		FreeLibrary(hModule);
		return E_FAIL;
	}
}

HRESULT CGraph::createObjectFromPath(HMODULE hModule, REFCLSID clsid, IBaseFilter **ppUnk)
{
	assert(hModule);

	//	the entry point is an exported function
	DllGetClassObjectT fn = (DllGetClassObjectT) GetProcAddress(hModule, "DllGetClassObject");
	if (fn == NULL) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	//	create a class factory
	IClassFactory* pClassFactory = NULL;
	HRESULT hr = fn(clsid, IID_IClassFactory, (LPVOID *) &pClassFactory);
	if (SUCCEEDED(hr)) 
	{
		if (pClassFactory == NULL) 
		{
			hr = E_NOINTERFACE;
		}
		else 
		{
			//	ask the class factory to create the object
			hr = pClassFactory->CreateInstance(NULL, IID_IBaseFilter, (LPVOID *) ppUnk);
		}
	}
	return hr;
}
