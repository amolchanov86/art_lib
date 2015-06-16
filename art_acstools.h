#ifndef ART_ACS_TOOLS_H
#define ART_ACS_TOOLS_H

#include <math.h>
#include <stdio.h>

//------------------------------ NONLINEAR FUNCTIONS ----------------------
float art_sign (float val);

//Return: If saturation < 0 - no limits
float gainlim ( float signal, float gain, float saturation = -1 );

//Return: If sat_pos < 0 then no positive limit; if sat_neg > 0 then no negaive limit
float asym_gainlim (float signal, float gain_pos, float gain_neg, float sat_pos=-1, float sat_neg=1 , bool bypass = false );





class ArtDerivative
{
public:
	ArtDerivative( float init_Td = DEF_Td, float init_data = 0  );
	void tick(float new_data);
	int setTd ( float Td  );
	int setFreq ( float new_freq );
	float getData ();
	void reinit (float new_data);

	static const float DEF_Td =0.01; 
	
protected:
	float Td;
	float in_data [2];
	float out_data [2];
};




template <class ForD = float>
class ArtSimpleIntegrator
{

public:
	ArtSimpleIntegrator(ForD init_val , ForD Td_init ) 
	{
		if ( !setTd(Td_init) )
		{
			Td = Td_DEF;
		}
		
		reinit(init_val);
		
		up_lim = 0;
		low_lim = 0;
		up_lim_on = false;
		low_lim_on = false;
	}
	
	bool setTd( ForD Td_init )
	{
		if (Td_init > 0)
		{
				Td = Td_init;
	return true;
		}
	return false;
	}
	
	ForD tick (ForD in)//Uses predifined limits
	{

		//x0 - current out 
		//x1 - current in
		//x2 - previous in
		x[2] = x[1];
		x[1] = in;
		x[0] = x[0] + Td*(x[1]+x[2])/2;
		
		if ( up_lim_on && (x[0] >  up_lim) ) x[0] =  up_lim;
		if (low_lim_on && (x[0] < low_lim) ) x[0] = low_lim;
		
	return x[0];
	}
	
	ForD tick (ForD  in, ForD lim_in)
	{
		//x0 - current out 
		//x1 - current in
		//x2 - previous in
		x[2] = x[1];
		x[1] = in;
		x[0] = x[0] + Td*(x[1]+x[2])/2;
		
		lim_in = fabs(lim_in);
		if (  fabs( x[0] ) >  lim_in ) x[0] =  art_sign(x[0]) * lim_in;
		
	return x[0];	
	}
	
	void setLim (ForD lim_init, bool lim_on_init = true)
	{	
		lim_init = fabs(lim_init);
		up_lim = lim_init;
		low_lim = -lim_init;
		
		up_lim_on = lim_on_init;
		low_lim_on = lim_on_init;
		
		if (lim_on_init  && ( fabs(x[0]) > lim_init ) ) reinit(  art_sign(x[0]) * lim_init  );
		
	}
	
	void setUpperLim (ForD up_lim_init,  bool up_lim_on_init = true)
	{
		up_lim = up_lim_init;
		up_lim_on = up_lim_on_init; 
		if ( up_lim_on && (x[0] >  up_lim) ) reinit(up_lim);
	}
	
	void setLowerLim (ForD low_lim_init, bool low_lim_on_init = true)
	{
		low_lim = low_lim_init;
		low_lim_on = low_lim_on_init;
		if (low_lim_on && (x[0] < low_lim) ) reinit(low_lim);
	}
	
	void turnUpperLim (bool up_lim_on = true)
	{
		setUpperLim( up_lim, up_lim_on );
	}
	
	void turnLowerLim (bool low_lim_on = true)
	{
		setLowerLim( low_lim, low_lim_on);
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

	int getXnumMax () 
	{
	return x_num_max;
	}

	ForD getData()
	{
	return x[0];	
	}

protected:	
	static const int x_num_max=4;
	static const ForD Td_DEF = 0.001;
	ForD Td;
	ForD x[x_num_max];
	
	ForD up_lim;
	ForD low_lim;
	bool up_lim_on;
	bool low_lim_on;
	
};



//------------------------------ FILTERING --------------------------------
class ArtAperFilter 
{
public:
	ArtAperFilter(float Tau , float Td, float init_data);
	ArtAperFilter (float Tau = DEF_Tau, float Td = DEF_Td); //Set the filter uninited by default
	void construct(float Tau = DEF_Tau, float Td = DEF_Td);
	
	
	
	//~AperFilter();
	float tick( float new_data, bool bypass_temp = false);
	void reinit(float reinit_data = 0);
	void setUninited (bool uninited_in=true); //Means next time the func. tick() is called, the function reinit() will be called
	
	int changeParameters (float new_Tau, float new_Td = -1);
	int setTau (float newTau);
	int setTd (float Td);
	int setFreq (float new_freq);

	void setBypass(bool bypass_init=true);
	
	float getData ();

//Constants:	
	static const float DEF_Tau=0;
	static const float DEF_Td =0.01; 
	
protected:
	void filter();
	void recalc_parameters();
	
	float Data[2], DataF[2];
	float Tau;
	float Td;
	float Par;
	
	bool uninited;
	bool bypass;
};


class ArtAperFilterSat 
{
public:
	ArtAperFilterSat(float Tau , float Sat ,float Td , float init_data);
	ArtAperFilterSat (float Tau = DEF_Tau, float Td = DEF_Td); //Set the filter uninited by default
	void construct(float Tau = DEF_Tau, float Td = DEF_Td);
	
	
	
	//~AperFilter();
	float tick( float new_data, bool bypass_temp = false);
	void reinit(float reinit_data = 0);
	void setUninited (bool uninited_in=true); //Means next time the func. tick() is called, the function reinit() will be called
	
	int setTau (float new_Tau);
	void setSat ( float new_sat, bool sat_on=true );
	int setTd (float Td);
	int setFreq (float new_freq);

	void setBypass(bool bypass_init=true);
	
	float getData ();
	float getSatLevel ();
	bool getSatOn ();
	
//Constants:	
	static const float DEF_Tau=0;
	static const float DEF_Td =0.01; 
	
protected:
	void filter();
	void recalc_parameters();
	
	float Data[2], DataF[2];
	float Tau;
	float sat;
	bool sat_on;
	float Td;
	float Par;
	
	bool uninited;
	bool bypass;
	
	ArtSimpleIntegrator<float> integr;
};






void art_aper_filter(float Tau, float Td, float *Data, float * DataF, int reinit_filter);


//Different Vector conversions with use of Euler angles
//Heading is clockwise
//Body fixed frame: x_body - forward; y_body - up; z_body - to the right (thus Wy is counter clockwise) 
//rad == false == angles in degrees (angular speeds without any scale transformations)
template <class ForD = float>
class ArtEuler
{
public:
	ArtEuler(ForD roll_init = 0, ForD yaw_init = 0, ForD pitch_init = 0 );
	static const ForD RtD = 57.295779513; //Radians to Degrees: Degrees = RtD * Radians;
	
	//BODY=>GLOBAL FRAME
	static void equation ( ForD Wx, ForD Wy, ForD Wz, ForD &Wroll, ForD &Wyaw, ForD &Wpitch, ForD roll, ForD pitch, bool rad = false ,bool bypass = false )
	{
		if (bypass)
		{
			Wroll  =  Wx;				
			Wyaw   = -Wy;				
			Wpitch =  Wz;				
	return;
		}	

		ForD rtd = RtD;
		if (rad) rtd = 1.0;

		if ( cos(pitch/rtd) == 0.0  ) pitch -=0.001;

		ForD roll_rad=	roll/rtd	;
		ForD pitch_rad=	pitch/rtd	;
		
		Wroll 	=	Wx - tan(pitch_rad)*( Wy*cos(roll_rad) - Wz*sin(roll_rad) );  	
		Wyaw	=	(1.0/cos(pitch_rad)) * (  -Wy*cos(roll_rad) +  Wz*sin(roll_rad)  );	
		Wpitch	=	Wy*sin(roll_rad)	+ 	Wz*cos (roll_rad);

		
	}
	
	//GLOBAL=>BODY FRAME
	static void equationInv (  ForD &Wx,   ForD &Wy,  ForD &Wz, ForD Wroll, ForD Wyaw, ForD Wpitch, ForD roll, ForD pitch, bool rad=false ,bool bypass = false  )
	{
		if ( bypass)
		{
			Wx =  Wroll;
			Wy = -Wyaw;
			Wz =  Wpitch;		
	return;
		}

		ForD rtd = RtD;
		if (rad) rtd = 1.0;

		ForD roll_rad = roll / rtd;
		ForD pitch_rad = pitch / rtd;
		
		Wx=	 Wroll - Wyaw*sin(pitch_rad)	;
		Wy=	-Wyaw*cos(pitch_rad)*cos(roll_rad) + Wpitch*sin(roll_rad)  ;
		Wz=	 Wyaw*cos(pitch_rad)*sin(roll_rad) + Wpitch*cos(roll_rad);		
	}
	
	//BODY=>GLOBAL FRAME
	static void rotation ( ForD x_body, ForD y_body, ForD z_body, ForD &X, ForD &Y, ForD &Z, ForD roll, ForD yaw ,ForD pitch, bool rad = false ,bool bypass = false )
	{
		if (bypass)
		{
			X = x_body;
			Y = y_body;
			Z = z_body;
	return;		
		}
		
		ForD rtd = RtD;
		if (rad) rtd = 1.0;
		
		ForD roll_rad = roll / rtd;
		ForD yaw_rad = yaw / rtd;
		ForD pitch_rad = pitch / rtd;
		
		ForD A[3][3] = { { 0, 0 ,0 } , { 0 ,0 ,0 }, { 0, 0, 0 } };
		
		rotationCoef(A, roll_rad, yaw_rad, pitch_rad);
		
		X = A[0][0] * x_body  + A[0][1] * y_body  + A[0][2] * z_body;
		Y = A[1][0] * x_body  + A[1][1] * y_body  + A[1][2] * z_body;
		Z = A[2][0] * x_body  + A[2][1] * y_body  + A[2][2] * z_body; 
	}
	
	//GLOBAL=>BODY FRAME
	static void rotationInv ( ForD &x_body, ForD &y_body, ForD &z_body, ForD X, ForD Y, ForD Z, ForD roll, ForD yaw, ForD pitch, bool rad = false ,bool bypass = false )
	{
		if (bypass)
		{
			x_body = X;
			y_body = Y;
			x_body = Z;
	return;		
		}
		
		ForD rtd = RtD;
		if (rad) rtd = 1.0;
		
		ForD roll_rad = roll / rtd;
		ForD yaw_rad = yaw / rtd;
		ForD pitch_rad = pitch / rtd;
		
		ForD A[3][3] = { { 0, 0 ,0 } , { 0 ,0 ,0 }, { 0, 0, 0 } };

		rotationCoef(A, roll_rad, yaw_rad, pitch_rad);
		
		x_body = A[0][0] * X  + A[1][0] * Y  + A[2][0] * Z;
		y_body = A[0][1] * X  + A[1][1] * Y  + A[2][1] * Z;
		z_body = A[0][2] * X  + A[1][2] * Y  + A[2][2] * Z; 		
	}

	static void rotationCoef (ForD A[3][3], ForD roll_rad, ForD yaw_rad, ForD pitch_rad)
	{
		A[0][0] =  cos(yaw_rad )*cos(pitch_rad);
		A[0][1] = -sin(roll_rad)*sin(yaw_rad) - cos(roll_rad)*cos(yaw_rad)*sin(pitch_rad);
		A[0][2] = -cos(roll_rad)*sin(yaw_rad) + sin(roll_rad)*cos(yaw_rad)*sin(pitch_rad);
		
		A[1][0] =  sin(pitch_rad);
		A[1][1] =  cos(roll_rad)*cos(pitch_rad);
		A[1][2] = -sin(roll_rad)*cos(pitch_rad);
		
		A[2][0] =  sin (yaw_rad)*cos(pitch_rad);
		A[2][1] =  sin(roll_rad)*cos(yaw_rad) - cos(roll_rad)*sin(yaw_rad)*sin(pitch_rad);
		A[2][2] =  cos(roll_rad)*cos(yaw_rad) + sin(roll_rad)*sin(yaw_rad)*sin(pitch_rad);
	}

protected:
};

//------------------ EULER EQUATIONS -----------------------------------
//Wy - right-handed ("+" - against a time arrow (looking from a top of a ROV) )
//der_yaw - left handed ("+" - along a time arrow  (looking from a top of a ROV) )

//------ !!!! NOT TESTED
//Wx,Wy,Wz ==> d(roll)/dt=der_roll (Roll derivative), d(yaw)/dt=der_yaw (Heading derivative), d(pitch)/dt=der_pitch (Pitch derivative)
//Units: Degrees
//Return: 
//	 0 - everything fine
//	-1 - zero pointers		
int euler_equations ( float Wx, float Wy, float Wz,  float roll, float pitch, float * der_roll , float * der_yaw, float* der_pitch, bool bypass = false );

//------ !!!! NOT TESTED
//dGamma/dt (Roll derivative), dPsi/dt (Heading derivative), dTeta/dt (Pitch derivative) ==>  Wx,Wy,Wz  
//Units: Degrees
///Return: 
//	 0 - everything fine
//	-1 - zero pointers		
int inverse_euler_equations ( float* Wx, float* Wy, float* Wz,  float Gamma, float Teta, float  pGamma , float  pPsi, float pTeta );

	













#endif
