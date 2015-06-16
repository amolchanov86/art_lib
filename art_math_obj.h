#ifndef ART_MATH_OBJ
#define ART_MATH_OBJ

template <class ForD> //So you can use Float or Double for the template
class ArtMathObject
{
public:
	ArtMathObject( ForD Td_init, ForD * x_init =0, long int x_num_max_init=0)
	{
		x_num_max = x_num_max_init;
		x = 0;
		x_ref = 0;
		
		setTd(Td_init);
		setXarray(x_init, true);

	}

	virtual ~ArtMathObject()
	{
		deleteXarray();
	}


	ForD getX( long int x_num ) //Access to the state variables
	{ 
		if ((x_num > 0) && (x_num < x_num_max)) 
	return x[x_num]; 
		else   
	return 0;	
	}
	
	long int getXnumMax(){ return x_num_max; }
	
	virtual ForD setTd(ForD Td_new = Td_def){ (Td_new > 0) ? Td = Td_new : Td = Td_def; }
	ForD getTd();

	//Return:
	//true - external array is used
	//false - internal array is used
	//Description:
	//Sets new array as the storage of the state of the object.
	//x_init == 0 - clears up all previously set arrays: begin to use internal storage
	//x_init != 0; reflect_only = true: use the external array just for as a reflection of the internal state (can't influence)
	//x_init != 0; reflect_only = false: use the external array as the internal storage of the state (uses less memory)
	bool setXarray (ForD * x_init = 0, bool reflect_only = true);//Sets new external array as a state storage.

	//Return:
	//true - the reflection is set
	//false - no reflection
	bool setReflectOnly (bool reflect=true);

	//---Static CONSTANTS
	const static ForD Td_def = 0.01;	//The default value for the time step

protected:

	virtual void deleteXarray();//Delete the internal X array; I made it virtual in case I would like to use containers

	//Return:
	//true - fine
	//false - null pointer
	bool copyXarray( ForD * in_ar, ForD * out_ar );//Copy from one array to another (uses x_num_max)
	void forceXarrayOwn();	//If you are not the owner of the X array it makes you the owner (doesn't influence the reflection)


	//---Private variables
	ForD Td;		//The Time step
	bool own_x;		//This one determines who creates and manages the X array
	long int x_num_max;	//The max number of state variables



	//---Heap
	ForD * x;	//The State variables array
	ForD * x_ref;	//The array for the reflection

};

template<class ForD>
void ArtMathObject<ForD>::deleteXarray()
{
	if (own_x && (x != 0) )
	{
		delete [] x;
	}
	x = 0;

}

template<class ForD>
bool ArtMathObject<ForD>::copyXarray(ForD *in_ar, ForD *out_ar)
{
	if ( (in_ar ==0) || (out_ar == 0))
	{
return false;
	}
	else
	{
		for (int i = 0; i< x_num_max; i++)
		{
			out_ar[i] = in_ar[i];
		}
return true;
	}

}


template<class ForD>
void ArtMathObject<ForD>::forceXarrayOwn()
{
	if (!own_x || (x==0) )
	{
		own_x = true;
		if (x !=0 )
		{
			ForD * x_tmp = new ForD [x_num_max];
			copyXarray( x, x_tmp );
			x = x_tmp;
		}
		else
		{
			x = new ForD [x_num_max];
		}

	}

}

template<class ForD>
bool ArtMathObject<ForD>:: setXarray ( ForD * x_init, bool reflect_only )
{
	if (x_init != 0)
	{
		if(reflect_only)
		{
			x_ref = x_init;
			copyXarray (x, x_ref);
			forceXarrayOwn();
		}
		else
		{
			x_ref = 0;
			copyXarray(x, x_init);
			deleteXarray();
			x = x_init;
			own_x = false;
		}

	}
	else
	{
		x_ref = 0;
		forceXarrayOwn();
	}

}


template<class ForD>
bool ArtMathObject<ForD>::setReflectOnly(bool reflect)
{
	if (reflect)
	{
		if ( (x_ref == 0) && (!own_x) )
		{
			x_ref = x;
			forceXarrayOwn();
		}

	}
	else
	{
		if ( (x_ref !=0) && (own_x))
		{
			copyXarray( x, x_ref );
			deleteXarray();
			own_x = false;
			x = x_ref;
		}

	}

	if (x_ref == 0)
	{
return false;
	}
	else
	{
return true;
	}

}



#endif