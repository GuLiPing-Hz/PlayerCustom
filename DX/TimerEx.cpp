#include "stdafx.h"
#include "TimerEx.h"
#include "processstat.h"
#include "dxgi.h"


/*#pragma comment(lib,"winmm.lib")*/

//-----------------------------------------------------------------------------
// 构造函数
CTimerEx::CTimerEx(const char* iniFile,DWORD dwFps/* = 60*/, bool bFrameSkip/* = true*/)
: m_pListener( NULL )
, m_dwLastFlipped( 0 )
, m_dwLastSecond( 0 )
, m_dwPrevFrameMSInSecond( 0 )
, m_dwPrevFrameMS()
, m_dwFrameRate( 0 )
, m_dwSkipRate( 0 )
, m_fFrameTime( 0 )
, m_bDrawFlag( false )
, m_bFrameSkip( bFrameSkip )
, m_bInit( true )
, m_dwFrameCount( 0 )
, m_dwSkipCount( 0 )
, m_dwFPS(0)
, m_unit_ms(1000)
, m_szIniFile(iniFile)
{
	setFPS(dwFps);
	unsigned int Numerator = 0,Denominator = 0;
// 	getRefreshRate(1920,1080,Numerator,Denominator);
// 	if ( 0 == Denominator )
// 	{
// 		Denominator = 1;
// 	}
// 	setRefreshRate(Numerator,Denominator);
}

//-----------------------------------------------------------------------------
// 析构函数
CTimerEx::~CTimerEx()
{
	//SAFE_DELETE(m_pListener);//add by glp
}

//-----------------------------------------------------------------------------
bool CTimerEx::getRefreshRate( unsigned int screenWidth, unsigned int screenHeight, unsigned int &  Numerator, unsigned int & Denominator )
{
// 	HRESULT hr;
// 	IDXGIFactory* factory;
// 	IDXGIAdapter* adapter;
// 	IDXGIOutput* adapterOutput;
// 	unsigned int numModes, i;
// 	DXGI_MODE_DESC* displayModeList;
// 	//DXGI_SWAP_CHAIN_DESC swapChainDesc;
// 
// 	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
// 	if(FAILED(hr))
// 	{
// 		return false;
// 	}
// 
// 	// Use the factory to create an adapter for the primary graphics interface (video card).
// 	hr = factory->EnumAdapters(0, &adapter);
// 	if(FAILED(hr))
// 	{
// 		return false;
// 	}
// 
// 	// Enumerate the primary adapter output (monitor).
// 	if (adapter->EnumOutputs(1, &adapterOutput) == DXGI_ERROR_NOT_FOUND)//first find the second monitor
// 	{
// 		hr = adapter->EnumOutputs(0, &adapterOutput);//find the primary monitor
// 		if(FAILED(hr))
// 		{
// 			return false;
// 		}
// 	}
// 
// 	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
// 	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
// 	if(FAILED(hr))
// 	{
// 		return false;
// 	}
// 
// 	// Create a list to hold all the possible display modes for this monitor/video card combination.
// 	displayModeList = new DXGI_MODE_DESC[numModes];
// 	if(!displayModeList)
// 	{
// 		return false;
// 	}
// 
// 	// Now fill the display mode list structures.
// 	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
// 	if(FAILED(hr))
// 	{
// 		return false;
// 	}
// 
// 	// Now go through all the display modes and find the one that matches the screen width and height.
// 	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
// 	for(i=0; i<numModes; i++)
// 	{
// 		if ( screenWidth == displayModeList[i].Width && screenHeight == displayModeList[i].Height )
// 		{
// 			Numerator = displayModeList[i].RefreshRate.Numerator;
// 			Denominator = displayModeList[i].RefreshRate.Denominator;
// 			return true;
// 		}
// 	}
// 
// 	SAFE_DELETE(displayModeList);
// 	SAFE_RELEASE(adapterOutput);
// 	SAFE_RELEASE(adapter);
// 	SAFE_RELEASE(factory);

	return false;
}

//-----------------------------------------------------------------------------
// 设置监听器
void CTimerEx::setListener( ITimerListener * listener )
{
// 	if (m_pListener)
// 	{
// 		delete m_pListener;
// 		m_pListener = NULL;
// 	}
	m_pListener = listener;
}

ITimerListener* CTimerEx::getListener()
{
	return m_pListener;
}

void CTimerEx::startTimer()
{
	timeBeginPeriod(1);
	if (m_pListener)
	{
		m_pListener->setStarTime(timeGetTime());
	}
	
}

void CTimerEx::resetTimer()
{
	//timeEndPeriod(1);
	m_dwLastFlipped = 0;
	m_dwLastSecond = 0;
	m_dwPrevFrameMSInSecond = 0;
	m_dwPrevFrameMS = 0;
	m_dwFrameRate = 0;
	m_dwSkipRate = 0;
	m_bDrawFlag = false;
	m_dwFrameCount = 0;
	m_bInit = true;
	m_dwSkipCount = 0;

	if (m_pListener)
	{
		m_pListener->resetTime();
	}
}

void CTimerEx::stopTimer()
{
	if (m_pListener)
	{
		m_pListener->setStopTime(timeGetTime());
	}
	timeEndPeriod( 1 );
}

void CTimerEx::closeTimer()
{
	timeEndPeriod(1);
	resetTimer();
}
//-----------------------------------------------------------------------------
// 触发定时器
void CTimerEx::processTimer()
{
	DWORD elapsed_ms = 0;

	++m_dwFrameCount;

	if ( m_bInit )
	{
		m_dwLastSecond = timeGetTime();
		//m_dwPrevFrameMSInSecond = 0;
		m_dwPrevFrameMS = m_dwLastSecond;
		m_bInit = false;
		m_bDrawFlag = true;
	}
	else
	{
		DWORD dwTime = 0;
		if ( m_dwFrameCount == m_dwFPS )
		{
			dwTime = m_dwLastSecond + m_unit_ms;
		}
		else
		{
			dwTime = (DWORD)( m_dwFrameCount * m_fFrameTime + m_dwLastSecond );
		}
		DWORD dwCurTime = 0;
		if ( m_bFrameSkip && (dwCurTime=timeGetTime())  > dwTime/*(DWORD)( (m_dwFrameCount + 1) * m_fFrameTime + m_dwLastMinitues )*/ )
		{
			m_bDrawFlag = false;
			++m_dwSkipCount;
		}
		else
		{
			while ( (dwCurTime=timeGetTime()) < dwTime )
			{
				Sleep( 1 );
			}

			if ( dwCurTime > dwTime ) {
				OutputDebugStringA("dwCurTime > dwTime\n");
			}
			elapsed_ms = dwCurTime - m_dwPrevFrameMS;
			m_dwPrevFrameMS = dwCurTime;
// 			DWORD dw_tmp = dwCurTime - m_dwLastSecond;
// 			elapsed_tm = dw_tmp - m_dwPrevFrameMSInSecond;
// 			m_dwPrevFrameMSInSecond = dw_tmp;

			m_bDrawFlag = true;
		}

		if ( (dwCurTime - m_dwLastSecond) >= m_unit_ms )
		{
			if ( (dwCurTime - m_dwLastSecond) > m_unit_ms ) {
				//OutputDebugStringA("(dwCurTime - m_dwLastMinitues) > m_uMSec\n");
			}
			m_dwLastSecond = dwCurTime;
			//m_dwPrevFrameMSInSecond = 0;

			//w riteLog("当前帧率:%d,跳帧:%d",m_dwFrameRate,m_dwSkipCount);
			//LOG_IFN2(m_dwFrameRate,m_dwSkipCount);
// 			int cpu = get_cpu_usage();
// 			if (cpu != -1)
// 			{
// 				//w riteLog("当前CPU: %d",cpu);
					//LOG_IFN1(cpu);
// 			}
			m_dwFrameRate = m_dwFrameCount;//保存当前帧率
			m_dwSkipRate = m_dwSkipCount;//保存当前跳帧
			m_dwFrameCount = 0;
			m_dwSkipCount = 0;
		}
	}


	if ( m_bDrawFlag )
	{
		if ( m_pListener ) 
		{
			m_pListener->onTimer( elapsed_ms );
		}
	}
}

//-----------------------------------------------------------------------------
// 
void CTimerEx::measure()
{
	++m_dwFrameCount;
	m_bDrawFlag = true;
	if(timeGetTime() - m_dwLastSecond >= m_unit_ms)
	{
		m_dwLastSecond = timeGetTime();
		m_dwPrevFrameMS = m_dwLastSecond;
		m_dwPrevFrameMSInSecond = 0;
		m_dwFrameRate = m_dwFrameCount;
		m_dwFrameCount = 0;
		m_dwSkipRate = m_dwSkipCount;
		m_dwSkipCount = 0;
	}
}

//-----------------------------------------------------------------------------
//
void CTimerEx::onFrameSkip(bool bFrameSkip)
{
	m_bFrameSkip = bFrameSkip;
}

//-----------------------------------------------------------------------------
// 设置帧率
void CTimerEx::setFPS(DWORD fps)
{
	float adjust = Ini_GetFloat(m_szIniFile.c_str(),"TIMER","ADJUST",0.0f);
	m_dwFPS = fps;
	m_fFrameTime = 1000.0*(1+adjust) / fps;
}

//-----------------------------------------------------------------------------
// 设置屏幕刷新频率
void CTimerEx::setRefreshRate( unsigned int Numerator, unsigned int Denominator )
{
	m_unit_ms = (DWORD)(1000 * Denominator *(1-0.008f));
	m_dwFPS = Numerator;
	m_fFrameTime = m_unit_ms*1.0f / m_dwFPS;
}

//-----------------------------------------------------------------------------
// 获取帧率
DWORD CTimerEx::getFrameRate()
{
	return m_dwFrameRate;
}

//-----------------------------------------------------------------------------
// 获取忽略帧率
DWORD CTimerEx::getSkipRate()
{
	return m_dwSkipRate;
}

//-----------------------------------------------------------------------------
// 获取是否允许绘画
bool CTimerEx::getDrawFlag()
{
	return m_bDrawFlag;
}
