#ifndef ART_MATH_LOGIC_H
#define ART_MATH_LOGIC_H

#include "art_math_obj.h"

#include <iostream>
using namespace std;


//----------------------------------- LOGIC ELEMENTS ---------------------------------------------------

template <class ForD>
class ArtMathSimpleIntegrator: public ArtMathObject<ForD>
{
#define x this->x
#define Td this->Td
#define x_num_max this->x_num_max	
public:
	ArtMathSimpleIntegrator(ForD init_val , ForD Td_init, ForD * x_init=0 ) : ArtMathObject<ForD>( Td_init, x_init, 4 ){}
	ForD tick (ForD in)
	{

		//x0 - current out 
		//x1 - current in
		//x2 - previous in
		x[2] = x[1];
		x[1] = in;
		x[0] = x[0] + Td*(x[1]+x[2])/2;
		
	return x[0];

	}
	
	void reinit(ForD init_val = 0)
	{

		for (int i=0; i<x_num_max; i++)
		{
			x[i] = 0;
		}
		x[0] = init_val;

	}
	
	void reset()
	{
		reinit(0);	
	}
	
protected:	

#undef x
#undef Td
#undef x_num_max	
};



template <class ForD>
class ArtMathLatchDelay: public ArtMathObject<ForD>
{
#define x this->x
public:
	ArtMathLatchDelay(int mode = mode_latch, ForD delay_def_init = DELAY_DEF ,ForD Td = ArtMathObject<ForD>:: Td_def, ForD * x_init=0 ) : 
		ArtMathObject<ForD>( Td, x_init, 10 ), integr(0, Td)
	{
		if (delay_def_init <= 0)
		{
			delay_def = DELAY_DEF;	
		}
		else
		{
			delay_def = delay_def_init;	
		}
		setMode(mode);
		output[0] = false;
		output[1] = false;
	}
	static const ForD DELAY_DEF = 1.0; 
		
	int setMode(int mode_init = 0)
	{
		if (mode_init < mode_max_num)
		{
			mode = mode_init;
		}

	return mode;
	}
	enum Modes {mode_latch=0, mode_integr, mode_delay, mode_max_num };

	bool tick ( bool set )
	{
		tick (set, false, delay_def);
	}

	bool tick( bool set, bool reset, ForD delay = -1.0 )
	{
	ForD set_fl = set ? 1.0 : 0;
	
		if (delay <= 0) delay = delay_def; 
			
		output[1] = output[0];
			
		x[0] = set_fl;
		x[1] = set_fl / delay; 
		x[2] = integr.tick( x[1] );
		if ( x[2] > 1.0 ) output[0] = 1 ;
		else output[0] = 0;
		
		bool reset_condition = false;
		switch (mode)
		{
		case mode_latch:	
			reset_condition = (!set && !output[1]) || reset;
			break;
		case mode_integr:
			reset_condition = reset;
			break;
		case mode_delay:
			reset_condition = !set  || reset;
			break;
		//default:
		//	reset_condition = (!set && !output[1]) || reset;
		//	break;
		}
		
		if ( reset_condition )			
		{
			integr.reset();	
		}
		
		x[3] = output[0];
		//cout<<"Mode="<<mode<<endl;
		//cout<<"Reset="<<reset_condition<<endl;
	return output[0];	
	}

	void reset ()
	{
		integr.reset();	
	}

	bool getOut() {return output[0];}
	
	virtual ForD setTd(ForD Td_new = ArtMathObject<ForD>::Td_def)
	{ 
		(Td_new > 0) ? this->Td = Td_new : this->Td = ArtMathObject<ForD>::Td_def; 
		integr.setTd(this->Td);
	}

protected:
	ArtMathSimpleIntegrator<ForD> integr;
	int mode;
	bool output[2];
	float delay_def;
#undef x
};


//Front detector; Can detect all kinds of fronts (up, down, both)
//Current mode just determines the behaviour of the memebers by default
template <class ForD>
class ArtMathLogicFrontDetector: public ArtMathObject<ForD>
{
#define x this->x
public:
	//front - 
	ArtMathLogicFrontDetector(int front_mode_init = front_up, ForD Td=-1, ForD * x_init=0 ): 
		ArtMathObject<ForD>( Td, x_init, 10 ), prev_val(false)
	{
		setMode(front_mode_init);
		for (int i=0; i<mode_max_num; i++)
		{
			out[i] = false;
			counters[i] = 0;
			resetable_counters[i] = 0;
		}
		
	}
	enum Front_types {front_up=0, front_down, front_both,    mode_max_num};	
	
	bool tick(bool in)
	{

		if (in && !prev_val) 
		{
			counters[front_up]++;
			resetable_counters[front_up]++;
			out[front_up] = true;
		}
		if (!in && prev_val) 
		{	
			counters[front_down]++;
			resetable_counters[front_down]++;
			out[front_down] = true;
		}
		if ( in != prev_val ) 
		{	
			counters[front_both]++;
			resetable_counters[front_both]++;
			out[front_both] = true;
		}
		
		prev_val = in;
		setAllX();
		
	return out[mode];	
	}
	
	bool getOut(int in_mode = -1) {return out[normMode(in_mode)];} //Get out value (by default uses current mode)
	long int getCounter(int in_mode = -1) {return counters[normMode(in_mode)];}
	long int getTempCount (int in_mode = -1) {return resetable_counters[normMode(in_mode)];}

	void reset(int front = -1)
	{
		front = normMode(front);
		out[front] = false;
		resetable_counters[front]=0;

	}
	
	//Return:
	//currently set mode
	int setMode(int mode_init = 0)
	{
		if ((mode_init < (int)mode_max_num) && (mode_init>=0))
		{
			mode = mode_init;
		}

	return mode;
	}
	
protected:
	void setAllX()
	{
		x[0] = out[front_up];
		x[1] = out[front_down];
		x[2] = out[front_both];
		
		x[3] = counters[front_up];
		x[4] = counters[front_down];
		x[5] = counters[front_both];
		
		x[6] = resetable_counters[front_up];
		x[7] = resetable_counters[front_down];
		x[8] = resetable_counters[front_both];
		
	}
	
	int normMode(int in_mode)//Normalize the mode
	{
		if( (in_mode >= 0) &&  (in_mode < mode_max_num) ) ;
		else
		{
			in_mode = mode;	
		}
	return in_mode;	
	}
	
	bool prev_val;
	int mode;
	bool out[mode_max_num];
	long int counters[mode_max_num];
	long int resetable_counters[mode_max_num];
	
#undef x	
};







////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif // LE_LOGIC_H
