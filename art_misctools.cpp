//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <termios.h>
//#include <stdlib.h>


#include "art_misctools.h"

//------------------------------------------------------------------------------------------------------------------------
//-------------------------------- NEW CLASSES ---------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------




//-------- FreqEstimator--------------------------------------------------------------------------------------------------


FreqEstimator:: FreqEstimator (  float init_base_freq, float init_est_time )
{
	freq_estim = 0;
	full_est_time = 0;
	est_counter = 0;
	refresh_sum=0;

	if (init_base_freq <= 0 ) base_freq = FREQ_ESTIM_DEF_BASE_FREQ;
	else base_freq = init_base_freq;	

	setEstimTime( init_est_time  );

			
	setAutoClearCounter (false);

	reinit ();
}



void FreqEstimator:: tick ( int *refreshes   )
{
	est_counter++;
	full_est_time += 1.0/base_freq;

	if (refreshes != 0)
	{
		if ( *refreshes >= 0 ) refresh_sum += *refreshes;
		if ( auto_clear_ref_count ) *refreshes = 0;
	}
	
	if ( est_count_limit > 0 )
	{
		if ( est_counter >= est_count_limit  )
		{
			//if (refresh_sum > 1) refresh_sum --;
			freq_estim = (float)refresh_sum / est_time;
			est_counter = 0;
			refresh_sum=0;
		}
	}
	else
	{
		freq_estim = (float)refresh_sum / full_est_time;

	}

}      

int FreqEstimator:: setBaseFreq (  float new_freq  )
{
	if (new_freq <= 0)
return -1;	

	base_freq = new_freq;

	reinit ();
return 0;
}

int FreqEstimator:: setTd (  float new_Td )
{
	if (new_Td <= 0)
return -1;	

	base_freq = 1.0 / new_Td;

	reinit ();
return 0;
}

int FreqEstimator:: setEstimTime (float new_est_time)
{

	est_time = new_est_time;



	refresh_sum=0;
	full_est_time = 0.000000000000001;

	reinit ();
return 0;
}

void FreqEstimator:: setAutoClearCounter (bool clear_rc)
{
	auto_clear_ref_count = clear_rc;
}

void FreqEstimator:: reinit ()
{
	est_counter = 0;

	if ( (1.0/base_freq >= est_time) && (est_time > 0 ) )
	{
		est_time = 1.0/base_freq;
	}

	if ( est_time > 0 ) refresh_sum=0;

	est_count_limit = base_freq * est_time;


}

float FreqEstimator:: getBaseFreq ()const
{	
	return base_freq;
}

float FreqEstimator:: getEstTime () const
{
	return est_time;
}

float FreqEstimator:: getFreq () const
{
	return freq_estim ;
}

float FreqEstimator:: getRefreshSum () const
{
	return	refresh_sum;
}




//--------------------- FREQ VALUATOR -------------------------------------------------------------

FreqValuator:: FreqValuator (float base_freq )
{
	setAutoClearCounter (false);

	feInt = new FreqEstimator (base_freq, -1);
	fe1sec = new FreqEstimator (base_freq, 1);
	fe5sec = new FreqEstimator (base_freq, 5);

	feInt->setAutoClearCounter(false);
	fe1sec->setAutoClearCounter(false);
	fe5sec->setAutoClearCounter(false);

}

FreqValuator::	FreqValuator ( const FreqValuator &rhs )
{
	feInt = new FreqEstimator (  *(rhs.feInt)  );
	fe1sec =  new FreqEstimator (  *(rhs.fe1sec)  );
	fe5sec = new FreqEstimator (  *(rhs.fe5sec)  );

	clear_ref_counter = rhs.clear_ref_counter;
}

FreqValuator:: ~FreqValuator ()
{
	delete feInt;
	delete fe1sec;
	delete fe5sec;
}

void FreqValuator:: tick ( int * new_ref_count )
{
	feInt->tick (new_ref_count);
	fe1sec->tick (new_ref_count);
	fe5sec->tick (new_ref_count);

	if (clear_ref_counter)
	{
		new_ref_count = 0;
	}
}

void FreqValuator:: setAutoClearCounter ( bool new_clear_count )
{
	clear_ref_counter = new_clear_count;
}

bool FreqValuator:: setBaseFreq ( float new_base_freq )
{
bool result=true;
	result = feInt->setBaseFreq(new_base_freq);
	fe1sec->setBaseFreq(new_base_freq);
	fe5sec->setBaseFreq(new_base_freq);

	result = feInt->setEstimTime(-1);
	fe1sec->setEstimTime(1);
	fe5sec->setEstimTime(5);

return result;
}

float FreqValuator:: getFreqInt () const
{
	return feInt->getFreq();
}
float FreqValuator:: getFreq1sec () const
{
	return fe1sec->getFreq();
}
float FreqValuator:: getFreq5sec () const
{
	return fe5sec->getFreq();
}
















//-------------------------------------------------------------------------------------------------------------------------
//--------------------------------- OLD FUNCTIONS -------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

unsigned char cs_calc_ver1(unsigned char* packet,unsigned int packet_length )
{
unsigned char cs='\0';
int i=0;
	for(i=0;i<packet_length-2;i++)
	{
		cs+=packet[i];
	}
	cs=~cs+1;
return cs;

}

/*cs_calc version2*/
unsigned char cs_calc(unsigned char* packet,unsigned int string_length,unsigned int packet_length )
{
unsigned char cs='\0';
int i=string_length-1-2;
	for ( i=string_length-1-2; i>(int)string_length-1-(int)packet_length ; i--)
	{
		cs+=packet[i];
	}
	cs=~cs+1;
return cs;

}


int uchar_cpy(unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int length1, unsigned int length2)
{
int i;
	if ( length2==0)
	{
		return length1;
	}
	for (i=0;i<length2;i++)
	{
		uchar_array1[i+length1]=uchar_array2[i];
	}
	return length1+length2;
}

int uchar_cut(unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int pos1, unsigned int pos2)
{
//uchar_array1 - cut from
//uchar_array2 - cut to
//pos1 - begining symbol  in uchar_array1
//pos2 - end symbol in uchar_array1
int i;
	
	for (i=pos1;i<=pos2;i++)
	{
		uchar_array2[i-pos1]=uchar_array1[i];
	}
	return pos2-pos1+1;
}

int uchar_inv_cpy (unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int length1)
{
int i;
	if (length1==0)
	{
		return 0;
	}
	for (i=0;i<length1;i++)
	{
		uchar_array1[i]=uchar_array2[length1-1-i];
	}
	return length1;

}

int uchar_cpy_tail(unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int length1, unsigned int length2, unsigned int tail)
{
int i;
unsigned int corrected_tail=tail;
	if (tail>length2) corrected_tail=length2;
	if ( tail==0)
	{
		return length1;
	}
	for (i=0;i<corrected_tail;i++)
	{
		uchar_array1[i+length1]=uchar_array2[i+length2-corrected_tail];
	}
	return length1+corrected_tail;

}



int print_char_array(unsigned char* char_array, unsigned int arr_length)
{
	int i=0;
        for (i=0;i<arr_length;i++)
        {
                printf("%u ",char_array[i]);
        }

        return 0;
}

int printerr_char_array(unsigned char* char_array, unsigned int arr_length)
{
	int i=0;
        for (i=0;i<arr_length;i++)
        {
                fprintf(stderr, "%u ",char_array[i]);
        }

        return 0;
}




int print_char_array_2d (unsigned char in_array[][10], unsigned int *length_array, unsigned int length)
{
int i=0;
	for (i=0;i<length;i++)
	{
		print_char_array(in_array[i],length_array[i]);
		printf("\n");
	}
return length;
}

//lb_converter is little to big endian converter (little <-> big), if the in_number is in little endian the function converts it in big endian and otherwise. 
//Return: errore code : 
//	0 - everything is OK;
//	-1 - error: wrong number capacity value;
//	-2 - error: zero pointer is given;
//Parameters: 
//	in_number - the number you wanna convert (it must be represented like unsigned char array, so you need to use reinterpret_cast<>() to remake pointer to unsigned char )
//	type - capacity of in_number: 16, 32, 64 bits

int lb_converter (unsigned char* in_number, unsigned int type )
{
//---VARS
//Temp
unsigned char temp[8];
unsigned int i=0;
unsigned int bytes=0;


//---START
	
	if ( in_number == 0) return -2;//Error! Zero pointer!
	
	switch (type)
	{
	case 16:
		bytes =2;
		break;
	case 32:
		bytes=4;
		break;
	case 64:
		bytes=8;
		break;
	default:
		return -1;//Error! Wrong number capacity value!	
	}

	for (i=0;i<bytes; i++)
	{
		temp[bytes-1-i]=in_number[i];
	}
	for (i=0;i<bytes; i++)
	{
		in_number[i]=temp[i];
	}

return 0;
}

//A function to calculate CRC-16, using the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1). My version. 
unsigned int crc16_calc_my (unsigned char * data, unsigned long int len )
{
unsigned int crc=0;
unsigned long int index =0;	

	while (len--)
	{
		crc = (crc >> 8) | (crc<<8);
		crc ^= data [index++];
		crc ^= (crc & 0xff) >> 4;
		crc ^= (crc <<8) << 4;
		crc ^= (( crc & 0xff) << 4 ) <<1;
	}
return crc;
}

//A function to calculate CRC-16, using the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1). TCMXB version 
unsigned int crc16_calc (void * data, unsigned long int len )
{
unsigned char * dataPtr = (unsigned char*)data;
unsigned int crc=0;
unsigned long int index =0;	

	while (len--)
	{
		crc = (unsigned char) (crc >> 8) | (crc<<8);
		crc ^= dataPtr [index++];
		crc ^=(unsigned char) (crc & 0xff) >> 4;
		crc ^= (crc <<8) << 4;
		crc ^= (( crc & 0xff) << 4 ) <<1;
	}
return crc;
}


unsigned  int uchar_array_sum_16bit (unsigned char * array, unsigned int len  )
{
unsigned int i=0;
unsigned short int sum=0;  

	for (i=0; i< len; i++)
	{
		sum+=array[i];

	}
return sum;
}



float freq_valuation_func( unsigned long int * refreshed_counter, float base_freq, struct fvf_out_struct * fvf_out, float * condition_array, short int clear_condition, short int method, short int dont_clear_flag  )
{
int i=0;
//Initialisation
//condition_array[0] - first init flag;
//condition_array[1] - number of steps when the flag was refreshed;
//condition_array[2] - total number of steps;

//condition_array[3] - counter of flag updates for 1 sec valuation;
//condition_array[4] - number of sample times for 1 sec valuation;
//condition_array[5] - 1 sec valuation condition;
//condition_array[6] - number of sample times for 1 sec valuation;
//condition_array[7] - number of sample times have gone since last 1 sec valuation recalculation;

//condition_array[9] - previous base frequency;
//

//	printf("%f\n",condition_array[6]);
	if (condition_array[0] !=33)
	{
		fvf_out->integral_fv=0;
		fvf_out->one_sec_fv=0;


	}
	
	if (condition_array[0] != 33 || clear_condition==1 || (base_freq != condition_array[9]  )  )
	{
		condition_array[0]=33;
		for (i=1;i<=9;i++ )
			condition_array[i]=0;

		if (base_freq > 1) condition_array[6]=(int)base_freq;
		else condition_array[6]= (int)(1.0/base_freq + 1);

	}
	
	if (refreshed_counter ==0) return 0;
	
	//Integral frequenxy valuation
	condition_array[1]+=*refreshed_counter;
	condition_array[2]+=1;


	fvf_out->integral_fv=(condition_array[1]/condition_array[2])*base_freq;

	//1 sec freq valuation
	switch (method)
	{
	case 0:	
		condition_array[7]+=1;
		condition_array[5]+=*refreshed_counter;

		if (condition_array[7] == condition_array[6] )
		{
	//		printf("%f\n",condition_array[6]);
			condition_array[7]=0;
			fvf_out->one_sec_fv= (condition_array[5]/condition_array[6]) * base_freq;
			condition_array[5]=0;
		}

		break;

	case 1:

		break;
	}

	if (dont_clear_flag==0) *refreshed_counter=0;
	condition_array[9] = base_freq;

return 0;
}



unsigned char bit_invert ( unsigned char * byte, short int bit )
{
if (byte == NULL) return 0;
if ( bit > 7 || bit < 0) return 0;

unsigned char bitmasks[8]={ 1,2,4,8,16,32,64,128 };
unsigned char result_mask = !(*byte) & bitmasks[bit];

	*byte &= !bitmasks[bit];
	*byte |= result_mask;



return *byte;
}



