#ifndef ART_SIGNALS_H
#define ART_SIGNALS_H

#include <iostream>
using std:: cout;
using std:: endl;
#include <signal.h>
#include <stdio.h>
#include <vector>
using std::vector;

//If "sigev_notify_thread_id" isn't declared then add this string to  /usr/include/bits/siginfo.h
//#define sigev_notify_thread_id _sigev_un._tid

class ArtRTSignals
{
public:	
	ArtRTSignals()
	{

	}
	
	static int getSignal(bool occupy = true)
	{
	int signo = SIGRTMIN_CORR;
		while( occupied_signals->operator[](signo-1) && (signo <= SIGRTMAX) ){ signo++; }
		occupied_signals->operator[](signo-1) = occupy;
	return signo;	
	}
	//Return: signal number or -1 if it is impossible to get a new signal
	//
	
	static int occupySignal(int signo)
	{
		if (!isOccupied(signo))
		{	
			occupied_signals->operator[](signo-1) = true; 
	return 0;		
		}
		else
	return -1;
	}
	//Return:
	// ==0 - OK
	// !=0 - a bad day
	
	static int freeSignal(int signo)
	{
		if( checkSignNum(signo) )
		{
			occupied_signals->operator[](signo-1) = false;
	return 0;
		}
		else
	return -1;
	}
	//Return:
	// ==0  - OK
	// !=0 - wrong signal num
	
	static inline bool checkSignNum (int signo)
	{
		return (signo >= SIGRTMIN_CORR) && (signo <= SIGRTMAX);
	}
	
	static inline bool isOccupied (int signo)
	{
		return ( !checkSignNum(signo) || occupied_signals->operator[](signo-1)  ); 
	}
	//The function returns whether the signal is already occupied or not
	//true  - occupied
	//false - not occupied
	
	static const int ALREADY_OCCUP = 3;
	static int SIGRTMIN_CORR; 
	
private:

	static vector<bool> * occupied_signals;
};


class ArtSignals
{
public:	
	ArtSignals(){}
	
	static const int TYPE_PROCESS = 0;
	static const int TYPE_THREAD  = 1;
	
	static void createEventThreadSignal(sigevent &event, pthread_t thread_id_init)
	{
	#ifdef __QNX__	
		SIGEV_SIGNAL_THREAD_INIT(&event, ArtRTSignals:: getSignal(), 0, SI_MINAVAIL);
	#else
		event.sigev_notify = SIGEV_THREAD_ID;
		event.sigev_signo = ArtRTSignals::getSignal();
		event.sigev_value.sival_int = 0;
		event.sigev_notify_thread_id = thread_id_init;
	#endif	
	return ;	
	}	
	
	static void createEventSignal(sigevent &event)
	{
	
	#ifdef __QNX__	
		SIGEV_SIGNAL_INIT(&event, ArtRTSignals:: getSignal() );
	#else
		event.sigev_notify = SIGEV_SIGNAL;
		event.sigev_signo = ArtRTSignals::getSignal();
		event.sigev_value.sival_int = 0;
	#endif
	return ;
	}
	
	static void createEventThread (sigevent &event, void  (*fun) (union sigval), pthread_attr_t *attr_init, union sigval sigval_init )
	{
	#ifdef __QNX__	
		SIGEV_THREAD_INIT( &event, fun, sigval_init.sival_ptr , attr_init );
	#else
		event.sigev_notify = SIGEV_THREAD;
		event.sigev_notify_function = fun;
		event.sigev_value = sigval_init;
		event.sigev_notify_attributes = attr_init;
		
	#endif
	return ;
		
	}
	
	static void setSigMaskAddSig(int type_init, sigset_t &cur_sigset, int signal_no)
	{		
	sigemptyset(&cur_sigset);
		switch(type_init)
		{
		case TYPE_PROCESS: 
			sigprocmask(SIG_SETMASK, 0, &cur_sigset);
			sigaddset(&cur_sigset, signal_no);
			sigprocmask(SIG_SETMASK, &cur_sigset, NULL);
			break;
		default:
			pthread_sigmask( SIG_SETMASK, 0, &cur_sigset );
			sigaddset(&cur_sigset, signal_no);
			pthread_sigmask( SIG_SETMASK, &cur_sigset, NULL );
		}
	}
	
	
	
protected:

};


#endif