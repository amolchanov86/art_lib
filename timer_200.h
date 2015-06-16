

#ifndef TIMER_200_HPP
#define TIMER_200_HPP


#include <ctime>
#include <signal.h>
#include <cstdio>
#include <errno.h>



//!!!!!!!! QNX:
#ifdef __QNX__

#include <sys/siginfo.h>

#endif
//!!! END QNX.


extern int sig;
extern sigset_t set;
extern struct sigevent timer_event;
extern timer_t timer_id;
//Timer VARs & Constants
extern struct itimerspec  itime;



const unsigned int _SYSTEM_CLK=200; //Main Timer frequency:

int InitMainClk();

int DoneMainClk();


#endif
