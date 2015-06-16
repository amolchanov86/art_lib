#include "art_timer.h"


//------------------------------------------------------------------
//---------------- ART TIMER ----------------------------------
//------------------------------------------------------------------

ArtTimer::ArtTimer(float freq_init, bool verbose_init, int priority_init, int policy_init)
{
	verbose = verbose_init;
	errors.error = 0;
	current_interval = 0;
	timer_created = false;
	setItimerSpec(timerconf, 0);
	
	setThreadAttr(attr, priority_init, policy_init);

	//Signal event tuning 
	clock_id = CLOCK_REALTIME;

	sigval val;
	val.sival_ptr = this;
	
	
	ArtSignals:: createEventThread(sigev, thread_fun , &attr, val );
	
 	//Create timer
 	int create_result = -1;
 	
 	create_result= timer_create(clock_id, &sigev, &timer_id );
 	//fprintf(stderr, "Errno=%s   result=%d  Signo=%d \n", strerror(errno), create_result, sigev.sigev_signo);
 		
 	if (create_result)
 	{
		errors.CREATING_ERROR = 1;
 		if(verbose) fprintf(stderr, "\nWARNING :: ArtTimer :: Can't create timer \n");
		timer_created = false;
		stop();
 	}
	else
	{
		timer_created = true;
		errors.CREATING_ERROR = 0;
	}	
	
	if (timer_created && freq_init > 0)
	{
		startFreq(freq_init);
	}
	//cout<<"Timer :: Timer started :: "<<timer_created<<" "<<freq_init<<endl;	
}

void ArtTimer::setItimerSpec(itimerspec& timerconf, long int nsec, long int delay_nsec)
{	
	timerconf.it_interval.tv_sec = nsec / 1000000000;
	timerconf.it_interval.tv_nsec = nsec % 1000000000;
	timerconf.it_value.tv_sec = delay_nsec / 1000000000;	
	timerconf.it_value.tv_nsec = delay_nsec % 1000000000 + 1;
	
	//fprintf (stderr, "\nArtTimer :: \nSec=%ld  \nNSec=%ld \nDelay_Sec=%ld \nDelay_NSec=%ld", (long int)timerconf.it_interval.tv_sec,  (long int) timerconf.it_interval.tv_nsec, (long int)timerconf.it_value.tv_sec, (long int) timerconf.it_value.tv_nsec );
}




void ArtTimer::startNano(long int nsec)
{
	if (nsec >= 0) current_interval = nsec;
	setItimerSpec(timerconf, current_interval);
	
	int settime_res = -1;
	settime_res = timer_settime( timer_id, 0, &timerconf, 0 );

	if(settime_res)
	{
		if(verbose) fprintf(stderr, "\nWARNING :: ArtTimer :: Can't set time while Starting \n");
		//fprintf(stderr, "Errno=%s   result=%d  Signo=%d \n", strerror(errno), settime_res, sigev.sigev_signo);
		errors.SETTIME_ERROR = 1;
	}
	else
	{
		errors.SETTIME_ERROR = 0;
	}
}


void ArtTimer::stop()
{
	if(!timer_created)
return;		
	setItimerSpec(timerconf, 0);

	int settime_res = -1;
	settime_res = timer_settime( timer_id, 0, &timerconf, 0 );
	
	if(settime_res)
	{
		if(verbose) fprintf(stderr, "\nWARNING :: ArtTimer :: Can't set time while Stopping \n");
		errors.SETTIME_ERROR = 1;
	}
	else
	{
		errors.SETTIME_ERROR = 0;
	}
	
}


void ArtTimer::thread_fun(sigval val)
{
	ArtTimer * obj = (ArtTimer *) val.sival_ptr;
	obj->run();
}


void ArtTimer::run()
{
	cout<<"ArtTimer :: Works !!!"<<endl;
}

void ArtTimer::setThreadAttr(pthread_attr_t& attr_init, int priority, int policy)
{
	pthread_attr_init ( &attr_init );//Initialisation for the thread's attributes by default (inherited from the thread creater)
	pthread_attr_setinheritsched(&(attr_init), PTHREAD_EXPLICIT_SCHED);
	
	if (priority >= 0)
	{
sched_param param;
		param.sched_priority = priority;
		pthread_attr_setschedparam (&attr_init, &param);
	}
	
	if (policy >= 0)
	{
		pthread_attr_setschedpolicy(&attr_init, policy);	
	}
}



//------------------------------------------------------------------
//---------------- ART WAIT TIMER ----------------------------------
//------------------------------------------------------------------



ArtWaitTimer::ArtWaitTimer( int type_init, bool verbose_init)
{
	type = type_init;
	verbose = verbose_init;
	errors.error = 0;
	current_interval = 0;
	timer_created = false;
	setItimerSpec(timerconf, 0);
	sigemptyset(&sigmask_set);
}
	
ArtWaitTimer::~ArtWaitTimer()
{
	stop();
}


void ArtWaitTimer:: startNano(long int nsec)
{	
	if (!timer_created)
	{
		//Signal event tuning 
		clock_id = CLOCK_REALTIME;
		//pid = pthread_self();
		pid = getpid();
		
		switch(type)
		{
		case TYPE_PROCESS: 
			ArtSignals::createEventSignal(sigev);
			break;
		default:
			ArtSignals::createEventThreadSignal(sigev, pid);
		}
		current_interval = nsec;
			
		//Create timer
		int create_result = -1;
		
		create_result= timer_create(clock_id, &sigev, &timer_id );
		//fprintf(stderr, "Errno=%s   result=%d  Signo=%d \n", strerror(errno), create_result, sigev.sigev_signo);
			
		if (create_result)
		{
			errors.CREATING_ERROR = 1;
			if(verbose) fprintf(stderr, "\nWARNING :: ArtTimer :: Can't create timer \n");
			timer_created = false;
		}
		else
		{
			errors.CREATING_ERROR = 0;
			timer_created = true;
		}
	}

	ArtSignals:: setSigMaskAddSig(type, sigmask_set, sigev.sigev_signo );
	
	if (nsec >= 0) current_interval = nsec;
	setItimerSpec(timerconf, current_interval);
	
	int settime_res = -1;
	#ifdef __QNX__
	settime_res = timer_settime( timer_id, 0, &timerconf, 0 );
	#else
	settime_res = timer_settime( timer_id, 0, &timerconf, 0 );
	#endif
	
	if(settime_res)
	{
		if(verbose) fprintf(stderr, "\nWARNING :: ArtTimer :: Can't set time while Starting \n");
		fprintf(stderr, "Errno=%s   result=%d  Signo=%d \n", strerror(errno), settime_res, sigev.sigev_signo);
		errors.SETTIME_ERROR = 1;
	}
	else
	{
		errors.SETTIME_ERROR = 0;
	}
}


void ArtWaitTimer::pause()
{
	if(!timer_created)
return;		
	setItimerSpec(timerconf, 0);

	int settime_res = -1;
	#ifdef __QNX__
	settime_res = timer_settime( timer_id, 0, &timerconf, 0 );
	#else
	settime_res = timer_settime( timer_id, 0, &timerconf, 0 );
	#endif
	
	if(settime_res)
	{
		if(verbose) fprintf(stderr, "\nWARNING :: ArtTimer :: Can't set time while Pausing \n");
		errors.SETTIME_ERROR = 1;
	}
	else
	{
		errors.SETTIME_ERROR = 0;
	}
	
}

void ArtWaitTimer::stop()
{
	if(!timer_created)
return;
	timer_delete( timer_id );
	timer_created = false;
}

void ArtWaitTimer::wait()
{
	sigwait(&sigmask_set, &(sigev.sigev_signo) );
}

void ArtWaitTimer::setItimerSpec(itimerspec& timerconf, long int nsec, long int delay_nsec)
{	
	timerconf.it_interval.tv_sec = nsec / 1000000000;
	timerconf.it_interval.tv_nsec = nsec % 1000000000;
	timerconf.it_value.tv_sec = delay_nsec / 1000000000;	
	timerconf.it_value.tv_nsec = delay_nsec % 1000000000 + 1;
}

int ArtWaitTimer::getSignNo()
{
return sigev.sigev_signo;
}

sigset_t ArtWaitTimer::getSigMask()
{
return sigmask_set;
}

sigset_t ArtWaitTimer::setSigMask(int type_init)
{
sigset_t cur_mask;
	ArtSignals:: setSigMaskAddSig(type_init, cur_mask, sigev.sigev_signo);
return cur_mask;
}


