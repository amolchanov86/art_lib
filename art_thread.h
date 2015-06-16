#ifndef ART_THREAD
#define ART_THREAD

#include <iostream>
using std::cout;
using std::endl;
#include <cstdio> //printf(); scanf();fprintf()
#include <unistd.h> //usleep();
#include <pthread.h> //For threads

//!!!!!!!! Linux
#ifndef __QNX__
#include <sched.h>
#endif
//!!! END Linux

#include <string>
using std::string;
#include <errno.h>

#define ART_THREAD_DEF_NAME  "ArtThread"


struct ArtThreadParam
{
	int policy;
	int priority;
};



class ArtThreadBase 
{
public:
	ArtThreadBase ( string name_init, int priority_init = -1, int policy_init = -1, bool verbose_init = true);
	//Priority == -1 - inherit from the root
	//Policy   == -1 - inherit from the root_param
	
	virtual ~ArtThreadBase ();
	
	//---Errors
	union 
	{	
		unsigned char error;
		struct 
		{
			unsigned char CREATING_ERROR: 1;
		};
	} errors;
	
	
	//---Thread control
	void start();
	void stop(bool force = false);
	//Stops the thread (requires reimplementation of the exit() function )
	bool isRunning() ;
	
	//---Parameters and attributes
	//(WARNING: set parameters before you create the thread or restart it 
	pthread_t getPID ();
	static int getParentPriority ();
	static int getParentParam (ArtThreadParam &root_param);
	//Return:
	// ==0 - OK
	// !=0 - a bad day :)	

	int setPriority (int priority_init );
	//Return: 
	// ==0 - ok
	// !=0 - an error occured (see errno)
	//Parameters: 
	//priority_init < 0 : use the priority of the parent thread
	//To determine Max(the highest) and Min (the lowest) priorities use:
	//int sched_get_priority_max(int policy);
	//int sched_get_priority_min(int policy); 
	
	int getPriority ();

	int getPolicy ();
	//Return: 
	// ==0 - OK; 
	// !=0 - you got a bad day :)
	
	int setPolicy (int policy_init = -1);
	
	void setVerbose (bool verbose_ = true) ;
	
	int priorityMin();
	static int priorityMin(int policy_init);
	int priorityMax();
	static int priorityMax(int policy_init) ;

	string name;
	
protected:	
	//---Functions that require reimplementation
	virtual void run();
	virtual int exit();
	//The function is used to make all necessary procedures before destruction of the thread. 
	//The function is called everytime before the thread is destroyed and must cause the run() function to return (exit)
	//If you don't care about an appropriate termination of the function use force = true when calling the stop() function

private:
	static void * thread_function(void * par);
	
	bool verbose;
	int priority;//It is not latest priority but the one we wanted to set
	int policy;
	bool is_running;
	pthread_t pid;//thread's id 	
	
	//Test
	bool run_thread;
};


//--- An old version
class ArtThread 
{
public:
	ArtThread (void * (*thread) (void * arg), void * arg_init = NULL, int priority = -1 , char * name_init = 0, bool verbose = true)
	{
			pid = -1;
			errors.error = 0;
			pthread_attr_init ( &attr );//Initialisation for the thread's attributes by default (inherited from the thread creater)
			
			if (name_init == 0) name = ART_THREAD_DEF_NAME;
			else
			{
				name = name_init;
				//fprintf (stderr, "NAME = %s \n", name.c_str());
			}
			
			if (thread == 0)
			{
				if (verbose) fprintf(stderr, "\nERROR!!! :: %s :: No thread function is set !!!\n", name.c_str());
				errors.NO_THREAD_FUNCTION = 1;
	return;
			}

			if (priority >= 0 )//If we gave priority < 0 then use inherited priority
			{
				pthread_attr_setinheritsched(&(attr), PTHREAD_EXPLICIT_SCHED);//Making schedule policy = inherited from the thread creater
				//!!!!!!!! QNX	
				#ifdef __QNX__	
				attr.param.sched_priority = priority;//Determination of the priority
				#endif
				//!!! END QNX
			}

			int create_return = pthread_create ( &pid, &attr, thread, arg_init ); //Thread creation

			//!!!!! PRINTING A MESSAGE
			if (create_return)
			{
				if (verbose) fprintf (stderr, "\nERROR!!! :: %s :: Can't creat thread !!!\n", name.c_str());
				errors.CREATING_ERROR = 1;
			}
			else
			{

				//!!!!!!!! Linux
				#ifndef __QNX__
				if (priority >=0)
				{
					param.sched_priority = priority;
					sched_setparam ( pid, &param );
					//attr.sched_param.sched_priority = priority;//Determination of the priority
				}
				#endif
				//!!! END Linux
			}
	}
	
	~ArtThread ()
	{
		pthread_cancel(pid);
		pthread_join(pid, NULL);
	}
	
	//Errors
	union 
	{	
		unsigned char error;
		struct 
		{
			unsigned char NO_THREAD_FUNCTION: 1;
			unsigned char CREATING_ERROR: 1;
		};
	} errors;
	
	pthread_t getPID () {return pid;}
	pthread_attr_t getAttrib () {return attr;}
	
protected:
	//!!!!!!!! Linux
	#ifndef __QNX__	
	struct sched_param param;
	#endif	
	//!!! END Linux

	pthread_t pid;//thread's id 
	pthread_attr_t attr;//thread's attributes		

	string name;
};






#endif