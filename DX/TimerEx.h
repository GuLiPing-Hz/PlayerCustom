/*
	注释时间:2014-4-25
	author: glp
	定时器类，比较精确的
*/
#ifndef __TIMEREX_H__
#define __TIMEREX_H__

#include "Opengl.h"
#include "Timer.h"
#include <string>
class CFloridMgr;
class SceneMgr;
class ImgsetMgr;
//--------------------------------------------------------------------------
class ITimerListener
{
public:
	// 析构函数
	ITimerListener():m_bRunning(true){}
	virtual ~ITimerListener() {}
	//定时器相应事件处理
	virtual void onTimer( unsigned int elapsed_tm ) = 0;
	//设置开始时间
	virtual void setStarTime(unsigned int elapsed_ms)=0;
	//设置持续时间
	virtual void setLastTime(uint elapsed_ms)=0;
	//设置结束
	virtual void setEndTime(uint end_ms)=0;
	//设定停止的时间，
	virtual void setStopTime(unsigned int elapsed_ms)=0;
	//重置时间
	virtual void resetTime()=0;
	//结束评分线程
	virtual void endThread()=0;
public:
	bool				m_bRunning;
};

class CTimerEx:public COS_Timer
{
public:
	// 构造函数
	CTimerEx(const char* iniFile,DWORD dwFps = 60, bool bFrameSkip = true);

	// 析构函数
	~CTimerEx();

public:
	// 设置监听器
	void setListener( ITimerListener * listener );
	//获得监听器
	ITimerListener* getListener();

	// 触发定时器
	void processTimer();

	void startTimer();
	void stopTimer();
	void resetTimer();
	void closeTimer();

	void measure();
	void setFPS(DWORD time);
	// 设置屏幕刷新频率
	void setRefreshRate( unsigned int Numerator, unsigned int Denominator );
	void onFrameSkip(bool bFrameSkip);
	bool getDrawFlag();
	DWORD getSkipRate();
	DWORD getFrameRate();

protected:
	bool getRefreshRate( unsigned int screenWidth, unsigned int screenHeight, unsigned int &  Numerator, unsigned int & Denominator );

private:
	ITimerListener * m_pListener;
	DWORD m_dwLastFlipped;
	DWORD m_dwLastSecond;
	DWORD m_dwPrevFrameMSInSecond;
	DWORD m_dwPrevFrameMS;
	DWORD m_dwFrameRate;
	DWORD m_dwSkipRate;
	DWORD	m_dwFPS;
	double m_fFrameTime;
	bool m_bDrawFlag;
	bool m_bFrameSkip;

	bool m_bInit;
	DWORD m_dwFrameCount;
	DWORD m_dwSkipCount;

	DWORD m_unit_ms;

	std::string m_szIniFile;
};

#endif // __TIMEREX_H__