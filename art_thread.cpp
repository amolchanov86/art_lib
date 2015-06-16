#include "art_thread.h"

	ArtThreadBase:: ArtThreadBase ( string name_init, int priority_init, int policy_init ,bool verbose_init )
	{
			pid = -1;
			errors.error = 0;
			is_running = false;
			setPriority(priority_init);
			//cout<<"Priority="<<getPriority()<<endl;
			setPolicy(policy_init);
			setVerbose(verbose_init);
			
			if (name_init.length() == 0) name = ART_THREAD_DEF_NAME;
			else
			{
				name = name_init;
				//fprintf (stderr, "NAME = %s \n", name.c_str());
			}
				
	}
	
	ArtThreadBase::  ~ArtThreadBase ()
	{
		stop(true);
		if (verbose) fprintf(stderr, "WARNING :: %s :: The thread is deleted\n", name.c_str() );
	}
	
	
	void ArtThreadBase:: start()
	{  
	pthread_attr_t attr;	
		pthread_attr_init ( &attr );//Initialisation for the thread's attributes by default (inherited from the thread creater)
		pthread_attr_setinheritsched(&(attr), PTHREAD_EXPLICIT_SCHED);
		
	sched_param param;
		param.sched_priority = priority;
 		pthread_attr_setschedparam (&attr, &param);
		pthread_attr_setschedpolicy(&attr, policy);
		
		//return;
		run_thread = true;
		int create_return = 0; 
		//ArtThreadBaseContainer container(this);
		//create_return = pthread_create ( &pid, &attr, thread_function, &container ); //Thread creation
		//thread_function(this);
		create_return = pthread_create ( &pid, &attr, thread_function, this ); //Thread creation
		
		//cout<<"Create return="<<create_return<<endl;
	//return;	
		// !!!!! PRINTING A MESSAGE
		if (create_return)
		{
			if (verbose) fprintf (stderr, "\nERROR!!! :: %s :: Can't creat thread !!!\n", name.c_str());
			errors.CREATING_ERROR = 1;
		}
		else
		{
			setPriority(priority);
			setPolicy(policy);
			
			errors.CREATING_ERROR = 0;
			if (verbose) fprintf (stderr, "\n%s :: PID=%d :: The thread has been created!!!\n", name.c_str(), pid );
		}
	}
	
	//Stops the thread (requires reimplementation of the exit() function )
	void ArtThreadBase::  stop(bool force )
	{
		exit();
		if (force) pthread_cancel(pid);
		pthread_join(pid, NULL);
		if (verbose) fprintf (stderr, "WARNING :: %s :: The thread is stopped\n", name.c_str() );
	}
	
	bool ArtThreadBase::  isRunning() {return is_running;}
	
	int ArtThreadBase::  setPriority (int priority_init )
	{
	int return_val = 0;
		if (priority_init >=0)
		{
			priority = priority_init;
		}
		else
		{
			priority = getParentPriority();
		}
		//if (verbose) fprintf(stderr, "\n%s :: PID=%ld :: Current priority == %d !!!\n", name.c_str(), pid, getPriority() );
		
		//if ( isRunning() )
		//{
		//	struct sched_param param;
		//	sched_getparam(pid, &param);
		//	param.sched_priority = priority;
		//	
		//	return_val = sched_setparam ( pid, &param );	
		//	if (return_val &&  verbose) fprintf(stderr, "\nERROR!!! :: %s :: PID=%ld :: Can't set priority == %d. Current priority == %d !!!\n", name.c_str(), pid ,priority, getPriority() );
		//	else if (verbose) fprintf(stderr, "\n%s:: PID=%ld :: Current priority is == %d !!!\n", name.c_str(), pid, getPriority() );
		//}
	return return_val;
	}
	
	
	int ArtThreadBase::  getPriority ()
	{
		if (isRunning())
		{		
		int policy;
			struct sched_param sched_param;
			pthread_getschedparam (pid, &policy, &sched_param);
		return sched_param.sched_priority;	
		}
		else
		{
		return priority;	
		}
	}
	
	
	int ArtThreadBase:: getParentPriority ()
	{
		pthread_t pid_root;
		pthread_attr_t attr_root;
		int root_policy;
		struct sched_param root_sched_param;
		
		pid_root=pthread_self ();
		int result = pthread_getschedparam (pid_root, &root_policy, &root_sched_param);
	
		if (result) fprintf(stderr, "\nERROR!!! :: ArtThreadBase :: Can't get parent priority !!!\n" );	
		
	return root_sched_param.sched_priority;
	}

	int ArtThreadBase::getParentParam(ArtThreadParam &root_param)
	{
	int result = -1;	
	pthread_t pid_root;
	pthread_attr_t attr_root;
	int root_policy;
	struct sched_param root_sched_param;
		
		pid_root=pthread_self ();
		result = pthread_getschedparam (pid_root, &root_policy, &root_sched_param);
	
		if (result) fprintf(stderr, "\nERROR!!! :: ArtThreadBase :: Can't get parent priority !!!\n" );
		else
		{
			root_param.policy = root_policy;
			root_param.priority = root_sched_param.sched_priority;
		}
		
	return result;	
	}

	
	void ArtThreadBase::  setVerbose (bool verbose_ ) {verbose = true;}
	pthread_t ArtThreadBase::  getPID () {return pid;}//Using pid to change attributes of the thread isn't recommended
	
	
	int ArtThreadBase::  priorityMin()
	{
		return priorityMin( getPolicy() );
	}
	
	int ArtThreadBase::  priorityMin(int policy_init)
	{ 
		return sched_get_priority_min(  policy_init );
	}

	int ArtThreadBase::  priorityMax()
	{
		return priorityMax( getPolicy() );
	}
	//If policy < 0 the function returns max priority of the policy of the current thread
	int ArtThreadBase::  priorityMax(int policy_init) 
	{ 
		return sched_get_priority_max(  policy_init );
	}

	
	
	int ArtThreadBase::  setPolicy (int policy_init)
	{
	int result = -1;
	
		if(policy_init < 0)
		{
		ArtThreadParam root_param;	
			getParentParam(root_param);
			policy_init = root_param.policy;
		}
		
		policy = policy_init;		
// 		if (isRunning())
// 		{
// 			int policy_cur = -1;
// 			sched_param param;
// 			result = pthread_getschedparam(pid, &policy_cur, &param);
// 			
// 			if (result)
// 			{
// 				policy_cur = -1;
// 				if (verbose) fprintf(stderr, "\nERROR!!! :: %s :: PID=%ld :: Can't get current parameters. \n", name.c_str(), pid );
// 			}
// 			
// 			result = pthread_setschedparam(pid, policy_init, &param);
// 			if (result)
// 			{
// 				if (verbose) fprintf(stderr, "\nERROR!!! :: %s :: PID=%ld :: Can't set the policy == %d\n", name.c_str(), pid, policy_init );
// 			}
// 		
// 		}		
	return result;	
	}
	
	int ArtThreadBase::  getPolicy ()
	{
	int policy_out = -1;	
		if (isRunning())
		{
			sched_param param;
			int result = pthread_getschedparam(pid, &policy_out, &param);
			if (result)
			{
				policy_out = -1;
				if (verbose) fprintf(stderr, "\nERROR!!! :: %s :: PID=%ld :: Can't get policy.\n", name.c_str(), pid );
			}
		}
		else
		{
			policy_out = policy;
		}
	return policy_out;	
	}
	

	
	void ArtThreadBase:: run()
	{
		while (run_thread)
		{
			sleep(1);
			if(verbose) cout<<name<<": Works!"<<" Priorityy="<<getPriority()<<endl;
		}
	}
	

	int ArtThreadBase::  exit()
	{
		run_thread = false;
		pthread_join(pid, NULL);
	}

	

	void * ArtThreadBase::  thread_function(void * par)
	{
		//ArtThreadBase * obj = reinterpret_cast<ArtThreadBase *> (par);
		ArtThreadBase * obj = (ArtThreadBase *) par;
		//ArtThreadBaseContainer * cont = (ArtThreadBaseContainer *) par;		
		//ArtThreadBase * obj = cont->object;
		obj->is_running = true;
		obj->run();
		obj->is_running = false;
	}



