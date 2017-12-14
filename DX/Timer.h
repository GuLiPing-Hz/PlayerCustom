/*
	注释时间:2014-4-25
	author: glp
	用于精确测算函数的执行时间，for debug
*/
#ifndef TIMER_H_DEF
#define TIMER_H_DEF

#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

class COS_Timer
{
public:
	COS_Timer();                                    // default constructor
	~COS_Timer();                                   // default destructor

	void		start();                             // start timer
	void		start2();
	void		stop();                              // stop the timer
	void		stop2();
	double getElapsedTime();                    // get elapsed time in second
	double getElapsedTimeInSec();               // get elapsed time in second (same as getElapsedTime)
	double getElapsedTimeInMilliSec();          // get elapsed time in milli-second
	double getElapsedTimeInMicroSec();          // get elapsed time in micro-second

	double getLastElapsedTimerInMilliSec();
	double getLastElapsedTimeInMicroSec();

protected:


private:
	double startTimeInMicroSec;                 // starting time in micro-second
	double endTimeInMicroSec;                   // ending time in micro-second
	int    stopped;                             // stop flag 
#ifdef WIN32
	LARGE_INTEGER frequency;                    // ticks per second
	LARGE_INTEGER startCount;                   //
	LARGE_INTEGER endCount;                     //

	LARGE_INTEGER cur_startCount;
	LARGE_INTEGER cur_endCount;
#else
	timeval startCount;                         //
	timeval endCount;                           //
	timeval cur_startCount;                         //
	timeval cur_endCount;                           //
#endif
};

#endif // TIMER_H_DEF
