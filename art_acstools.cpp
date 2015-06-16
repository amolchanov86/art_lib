#include "art_acstools.h"

// ---------------------- NONLINEAR ELEMENTS ---------------------------------------------

float art_sign (float val)
{
	if (val > 0) 
return 1;
	else if (val == 0)
return 0; 
	else
return -1;
}


float gainlim ( float signal, float gain, float saturation  )
{
float gained = signal*gain;

	if ( saturation >= 0  )
	{
		if ( fabs ( gained ) >= saturation   )
return   (  art_sign(signal)*saturation );
		else 
return gained;		
	}
	else //No saturation
	{
return gained;

	}

return 0;
}


//Asymmetric gain
//gains can have any sign; saturations must have proper signs of values, otherwise there is no saturation (no limit)
float asym_gainlim (float signal, float gain_pos, float gain_neg, float sat_pos, float sat_neg, bool bypass  )
{
	if (bypass)
return signal;

float gained = 0;
	if (signal >= 0) 
	{
		gained = signal*gain_pos;
		if ( (gained >=sat_pos) &&  (sat_pos >= 0)  )
		{
return sat_pos;				
		}
	}
	else
	{
		gained = signal*gain_neg;
		if ( (gained <=sat_neg) &&  (sat_neg <= 0)  )
		{
return sat_neg;				
		}
	}
	
return gained;
}









//------------------ EULER EQUATIONS -----------------------------------

//Wx,Wy,Wz ==> d(roll)/dt=der_roll (Roll derivative), d(yaw)/dt=der_yaw (Heading derivative), d(pitch)/dt=der_pitch (Pitch derivative)
//Units: Degrees
///Return: 
//	 0 - everything fine
//	-1 - zero pointers		

int euler_equations ( float Wx, float Wy, float Wz,  float roll, float pitch, float  *der_roll , float *der_yaw, float *der_pitch, bool bypass )
{
	if ( (der_yaw == 0)   || (der_roll == 0) || (der_pitch==0) )
return -1;

	if (bypass)
	{
		*der_roll  =  Wx;				
		*der_yaw   = -Wy;				
		*der_pitch =  Wz;				
return 0;
	}	

const float rtd = 57.295779513; //Conversion coeff - from radians to degrees
float roll_rad, pitch_rad;

	if (pitch == 90.0) pitch = 89.999;
	if (pitch == -90.0) pitch = -89.999;
	if (roll == 90.0) roll = 89.999;
	if (roll == -90.0) roll = -89.999;

	roll_rad=	roll/rtd	;
	pitch_rad=	pitch/rtd	;

	*der_roll 	=	Wx - tan(pitch_rad)*( Wy*cos(roll_rad) - Wz*sin(roll_rad) );  	
	*der_yaw	=	(1.0/cos(pitch_rad)) * (  -Wy*cos(roll_rad) +  Wz*sin(roll_rad)  );	
	*der_pitch	=	Wy*sin(roll_rad)	+ 	Wz*cos (roll_rad);

return 0;
}



//d(roll)/dt (Roll derivative), d(yaw)/dt (Heading derivative), d(pitch)/dt (Pitch derivative) ==>  Wx,Wy,Wz  
//Units: Degrees
///Return: 
//	 0 - everything fine
//	-1 - zero pointers		

int inverse_euler_equations ( float* Wx, float* Wy, float* Wz,  float roll, float pitch, float  der_roll , float  der_yaw, float der_pitch , bool bypass)
{
	if ( (Wx == 0) || (Wy == 0) || (Wz == 0) )
return -1;

	if ( bypass)
	{
		*Wx =  der_roll;
		*Wy = -der_yaw;
		*Wz =  der_pitch;		

	}

const float rtd = 57.295779513; //Conversion coeff - from radians to degrees
float roll_rad = roll / rtd, pitch_rad = pitch / rtd;

	if (pitch == 90.0) pitch = 89.999;
	if (pitch == -90.0) pitch = -89.999;
	if (roll == 90.0) roll = 89.999;
	if (roll == -90.0) roll = -89.999;

	*Wx=	der_roll - der_yaw*sin(pitch_rad)	;
	*Wy=	-der_yaw*cos(pitch_rad)*cos(roll_rad) + der_pitch*sin(roll_rad)  ;
	*Wz=	der_yaw*cos(pitch_rad)*sin(roll_rad) + der_pitch*cos(roll_rad);		

return 0;
}










// ----------------------  APERIODIC FILTERS ---------------------------------------------

ArtAperFilter:: ArtAperFilter ( float init_Tau, float init_Td, float init_data   )
{
	construct(init_Tau, init_Td);
	reinit (init_data);
}

ArtAperFilter::ArtAperFilter(float init_Tau, float init_Td)
{
	construct(init_Tau, init_Td);
	reinit(0);
	uninited = true;
}

void ArtAperFilter::construct(float init_Tau, float init_Td)
{
	Tau = DEF_Tau;
	Td = DEF_Td;
	
	setTau(init_Tau);
	setTd(init_Td);
	
	setBypass(false);
}


float ArtAperFilter:: tick( float new_data , bool bypass_temp )
{
	if (uninited) reinit(new_data);
	Data[0] = new_data;

	if (bypass || bypass_temp)
	{
	    DataF[0] = Data[0];
	    DataF[1] = DataF[0];
	    Data[1] = Data[0];
	}
	else
	{
	    filter();
	    //printf ("filtering  Tau=%5.2f  Td=%5.2f\n", Tau, Td);
	}
	
return DataF[0];	
}

void ArtAperFilter::setBypass(bool bypass_init)
{
    bypass = bypass_init;
}


void ArtAperFilter:: filter()
{
	DataF[0]=(1/(1+Par))*((Data[1]+Data[0])-((1-Par)*DataF[1]));
	Data[1]=Data[0]; DataF[1]=DataF[0];
}


void ArtAperFilter:: reinit (float reinit_data)
{
	Data[0] = reinit_data;
	DataF[0]=Data[0];
	DataF[1]=Data[0];
	Data[1] = Data [0];
	
	uninited = false;
	//printf ("reinit\n");
}

void ArtAperFilter::setUninited(bool uninited_in)
{
	uninited = uninited_in;
}


int  ArtAperFilter:: changeParameters (float new_Tau, float new_Td)
{
int result = 0;	
	if (setTau(new_Tau) < 0) result -= 1;
	if (setTd(new_Td) < 0) result -= 2;
return result;
}

int ArtAperFilter:: setTau (float new_tau)
{
	if (new_tau == Tau)
return 0;		
	
int result = 0;

	if (new_tau >= 0)
	{
		Tau = new_tau; 
	}
	else
	{
		result = -1;
return result;
	}
	recalc_parameters();	

return result;
}


int ArtAperFilter:: setTd (float new_Td)
{
	if (new_Td == Td)
return 0;		

int result = 0;	
	if (new_Td > 0)
	{
		Td = new_Td;
	}
	else
	{
		result = -1;
return result;
	}
	recalc_parameters();

return result;	
}



int ArtAperFilter:: setFreq (float new_freq)
{
int result = 0;	
	if (new_freq > 0)
	{
		Td = 1.0/new_freq;
	}
	else
	{
		result = -1;
return result;
	}
	recalc_parameters();

return result;
}

void ArtAperFilter::recalc_parameters()
{
	Par = 2*Tau/Td;
}


float ArtAperFilter:: getData ()
{
return DataF[0];
}

//------------------------------------------------------










// ----------------------  APERIODIC FILTERS WITH SPEED SATURATION---------------------------------------------

ArtAperFilterSat:: ArtAperFilterSat ( float init_Tau, float init_sat ,float init_Td, float init_data   ):
integr(0, init_Td)
{
	construct(init_Tau, init_Td);
	setSat(init_sat);
	reinit (init_data);
}

ArtAperFilterSat:: ArtAperFilterSat (float init_Tau, float init_Td):
integr(0, init_Td)
{
	construct(init_Tau, init_Td);
	reinit(0);
	uninited = true;
}

void ArtAperFilterSat::construct(float init_Tau, float init_Td)
{
	Tau = DEF_Tau;
	Td = DEF_Td;
	
	setTau(init_Tau);
	setSat(0, false);
	setTd(init_Td);
		
	setBypass(false);
	
	integr.setTd(Td);
}


float ArtAperFilterSat:: tick( float new_data , bool bypass_temp )
{
	if (uninited) reinit(new_data);
	Data[0] = new_data;

	if (bypass || bypass_temp)
	{
	    DataF[0] = Data[0];
	    DataF[1] = DataF[0];
	    Data[1] = Data[0];
	}
	else
	{
	    filter();
	    //printf ("filtering  Tau=%5.2f  Td=%5.2f\n", Tau, Td);
	}
	
return DataF[0];	
}




void ArtAperFilterSat:: filter()
{
	float integr_speed = (Data[0] - DataF[0])*(1.0/Tau);
	if ( sat_on &&  (fabs(integr_speed) > sat)  ) integr_speed = art_sign(integr_speed)*sat;
	DataF[0] = integr.tick(integr_speed);
	
	Data[1]=Data[0]; DataF[1]=DataF[0];
}

void ArtAperFilterSat::setBypass(bool bypass_init)
{
    bypass = bypass_init;
}


void ArtAperFilterSat:: reinit (float reinit_data)
{
	Data[0] = reinit_data;
	DataF[0]=Data[0];
	DataF[1]=Data[0];
	Data[1] = Data [0];
	
	integr.reinit(reinit_data);
	
	uninited = false;
	//printf ("reinit\n");
}

void ArtAperFilterSat::setUninited(bool uninited_in)
{
	uninited = uninited_in;
}

int ArtAperFilterSat:: setTau (float new_tau)
{
	if (new_tau == Tau)
return 0;		
	
int result = 0;

	if (new_tau >= 0)
	{
		Tau = new_tau; 
	}
	else
	{
		result = -1;
return result;
	}
	recalc_parameters();	

return result;
}

bool ArtAperFilterSat::getSatOn()
{
return sat_on;
}

float ArtAperFilterSat::getSatLevel()
{
return sat;
}

void ArtAperFilterSat::setSat(float sat_init, bool sat_on_init)
{
	sat_on = fabs(sat_init);
	sat = sat_init;
}


int ArtAperFilterSat:: setTd (float new_Td)
{
	if (new_Td == Td)
return 0;		

int result = 0;	
	if (new_Td > 0)
	{
		Td = new_Td;
		integr.setTd(Td);
	}
	else
	{
		result = -1;
return result;
	}
	recalc_parameters();

return result;	
}



int ArtAperFilterSat:: setFreq (float new_freq)
{
int result = 0;	
	if (new_freq > 0)
	{
		Td = 1.0/new_freq;
		integr.setTd(Td);
	}
	else
	{
		result = -1;
return result;
	}
	recalc_parameters();

return result;
}

void ArtAperFilterSat::recalc_parameters()
{

}


float ArtAperFilterSat:: getData ()
{
return DataF[0];
}

//------------------------------------------------------














ArtDerivative:: ArtDerivative ( float init_Td, float init_data )
{
	Td = DEF_Td;
	setTd(init_Td);

	in_data [0] = init_data;
	in_data [1] = in_data [0];

	out_data[0]=0;
	out_data [1] = out_data [0];

}

void ArtDerivative :: tick (float new_data)
{
	in_data[0] = new_data;

	out_data [1] = out_data[0];
	out_data [0] = (in_data[0] - in_data[1])/Td;	

	in_data[1] = in_data[0];
} 


int ArtDerivative:: setTd (float new_Td)
{
int result = 0;
	if (new_Td > 0  ) Td = new_Td;
	else result = -1;

return result;

}

int ArtDerivative:: setFreq (float new_freq)
{
int result = 0;
	if (new_freq > 0  ) Td = 1.0/new_freq;
	else result = -1;

return result;
}


float ArtDerivative:: getData ()
{

return out_data[0];
}

void ArtDerivative:: reinit( float  new_data  )
{
	in_data[0] = new_data;
	in_data[1] = in_data[0];
	out_data[1]=out_data[0];
}


//------------------------------------------------------


void art_aper_filter(float Tau, float Td, float *Data, float * DataF, int reinit_filter)
{// (1/(T*s+1)) - gde T-post vremeni fil'tra Td-shag diskretizaciiM
float Par;	

	Par= 2*Tau/Td;
	//printf("%5.5f", *(Data+1));M
	if (reinit_filter)
	{
		DataF[0]=Data[0];
		DataF[1]=Data[0];

	}
	
	DataF[0]=(1/(1+Par))*((Data[1]+Data[0])-((1-Par)*DataF[1]));
	Data[1]=Data[0]; DataF[1]=DataF[0];

}




