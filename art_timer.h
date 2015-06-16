#ifndef ART_TIMER_H
#define ART_TIMER_H

#include <pthread.h>
#include <ctime>
#include <stdio.h>
#include <vector>
using std::vector;
#include <iostream>
using std::cout;
using std::endl;
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "art_signals.h"

class ArtTime
{
public:	
	ArtTime()
	{
		time(&start_time);
	}
	
	double getSecondsGone() 
	{
		return difftime( time(NULL), start_time );
	}
	
private:
	time_t start_time;
};

class ArtTimer
{
public:	
	ArtTimer(float freq = -1, bool verbose_init = true, int priority_init = -1, int policy_init = -1);
	virtual ~ArtTimer (){};

	void startFreq (float freq_init = -1)
	{
		startNano( (long int)(1000000000.0 / freq_init) );
	}
	void start(long int msec = -1)
	{
		startNano((long int)msec * 1000000);	
	}
	void startNano (long int nsec = -1);
	//Creates the timer and starts the ticking
	//If interval_init < 0 then use previously set interval (continue)
	
	void stop ();
	//Deletes the current timer
	
	//---Errors
	union 
	{	
		unsigned char error;
		struct 
		{
			unsigned char CREATING_ERROR: 1;
			unsigned char SETTIME_ERROR : 1;
		};
	} errors;

	
protected:
	virtual void run();
	
private:	
	static void thread_fun( union sigval val);
	bool verbose;
	void setItimerSpec (itimerspec & timerconf, long int nsec, long int delay = 0);
	static void setThreadAttr( pthread_attr_t & attr_init ,int priority, int policy );
	
	timer_t timer_id;
	clockid_t clock_id;
	itimerspec timerconf;
	sigevent sigev;
	pthread_t pid;
	long int current_interval;
	pthread_attr_t attr;
	int priority;
	int policy;
	
	bool timer_created;
};









//ArtWaitTimer::
//Try not to use TYPE_PROCESS, cause in that case signals will be sent to the whole process (to every thread)
//Use TYPE_THREAD instead.
//In that case you can create an ArtWaitTimer object in any thread, but the start() function should be used 
//in the target thread !!!
class ArtWaitTimer
{
public:
	static const int TYPE_THREAD = 0; //This timer sends signals to the whole process, so it doesn't matter where to invoke start() and wait() functions (but create the timer before threads)
	static const int TYPE_PROCESS = 1;//This timer sends signals to the thread where the function start() was invoked
	//WARNING: When you use TYPE_PROCESS create the timer BEFORE you create any other thread, otherwise all threads will receive the timer's signals and will be terminated
	ArtWaitTimer( int type_init = TYPE_THREAD, bool verbose_init = true);
	~ArtWaitTimer ();

	void startFreq(float freq_init = -1)
	{
		startNano((long int)(1000000000.0 / freq_init));
	}
	void start(long int msec = -1)
	{
		startNano((long int)msec * 1000000);	
	}
	void startNano(long int msec = -1);
	//Creates the timer and starts the ticking
	//If interval_init < 0 then use previously set interval (continue)
	
	void stop ();
	//Deletes the current timer
	
	void pause ();
	
	void wait();
	//Wait for timer's signal
	
	int getSignNo();
	//Returns the number of a signal that is used in the timer
	
	sigset_t getSigMask();
	//Return sigmask_set of this timer
	
	sigset_t setSigMask(int type_init = TYPE_PROCESS );
	//This function is necessary when you use TYPE_PROCESS. 
	//Because if you create threads after you've created your timer you should set a signal mask for them
	//in order to avoid thread termination. So just call this function inside the newly created thread 
	//or use getSigMask() in order to acquire the sigmask. All these operations are not required if you
	//use TYPE_THREAD because signals are sent right to the threads that uses timers
	// type_init: if you use TYPE_PROCESS the mask will be set for the whole process and it doesn't matter
	//where you call the function. But if, for some reasons, you want to set mask for a particular process 
	//then use TYPE_THREAD and call this function in the thread you want to set the mask for 
	
	
	//---Errors
	union 
	{	
		unsigned char error;
		struct 
		{
			unsigned char CREATING_ERROR: 1;
			unsigned char SETTIME_ERROR : 1;
		};
	} errors;

private:
	bool verbose;
	void setItimerSpec (itimerspec & timerconf, long int nsec, long int delay = 0);
	
	int type;
	timer_t timer_id;
	bool timer_created;
	clockid_t clock_id;
	itimerspec timerconf;
	sigset_t sigmask_set;
	sigevent sigev;
	pthread_t pid;
	long int current_interval;
};




#endif