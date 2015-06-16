#include "timer_200.h"

  int sig=SIGUSR1;
  sigset_t set;
  struct sigevent timer_event;
  timer_t timer_id;
  struct itimerspec  itime;

//The following constant is defined in header that is why it's commented
// const unsigned int _SYSTEM_CLK=200; //Main Timer frequency:

int InitMainClk()
{
	
	sigemptyset(&set);
	sigaddset(&set, sig);

	sigprocmask(SIG_SETMASK, &set, NULL);
	
//!!!!!!!! QNX:
#ifdef __QNX__
	 SIGEV_SIGNAL_INIT(&timer_event,SIGUSR1);
//!!! END QNX

//!!!!!!!! Linux:
#else
	timer_event.sigev_notify=SIGEV_SIGNAL;
	timer_event.sigev_signo=sig;
#endif
//!!! END Linux


	if( timer_create(CLOCK_REALTIME, &timer_event, &timer_id )< 0 )
	{	
		printf("\nError while create timer...Errno");

		if (errno==EAGAIN) printf ("\nEAGAIN");
		if (errno==EINVAL) printf ("\nEINVAL");
		if (errno==ENOTSUP) printf ("\nENOTSUP");
	} 
 //printf("\nSucces create main Timer\n");
 	itime.it_value.tv_sec = 0;
	itime.it_value.tv_nsec = (long)500000000;
	itime.it_interval.tv_sec = 0;
	itime.it_interval.tv_nsec = (long)1000000000/_SYSTEM_CLK;

//!!!!!!!! QNX 
#ifdef __QNX__
	timer_settime(timer_id, 0, &itime, NULL);
//!!! END QNX
#else
//!!!!!!!! Linux
	timer_settime(&timer_id, 0, &itime, NULL);
#endif
//!!! END Linux

return 0;
}

int DoneMainClk()
{
//Linux version
	//timer_delete(&timer_id);
//QNX version
	timer_delete(timer_id);
return 0;
}


//sigwait(&set, &sig);


