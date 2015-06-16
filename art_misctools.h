#ifndef ART_MISCTOOLS_H
#define ART_MISCTOOLS_H

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>





//------------------------ NEW FUNCTIONS & CLASSES --------------------------------------------



class FreqEstimator
{
public:
	FreqEstimator (  float base_freq = FREQ_ESTIM_DEF_BASE_FREQ, float estimation_time = FREQ_ESTIM_DEF_EST_TIME   );
	//~FreqEstimator();
	
	void tick (int *refreshed_data_counter);

	int setBaseFreq (float new_freq);
	int setTd (float new_Td);
	int setEstimTime  ( float est_time );
	void setAutoClearCounter (bool clear_counter = true);

        void reinit ();


	float getFreq () const	;
	float getBaseFreq () const;
	float getEstTime () const;

	//Auxilary functions
	float getRefreshSum () const;

	//Constants
	static const float FREQ_ESTIM_DEF_BASE_FREQ=200.0;
	static const float FREQ_ESTIM_DEF_EST_TIME=1.0;

protected:

	float freq_estim;

	int refresh_sum;
	float base_freq;
	float est_time;
	float full_est_time;
	int est_counter;
	int est_count_limit;
	bool auto_clear_ref_count;


};



class FreqValuator
{
public:
	FreqValuator ( float base_freq = DEF_BASE_FREQ );
	FreqValuator ( const FreqValuator &rhs );
	~FreqValuator ();
	void tick (int * new_ref_counter);

	float getFreqInt () const;
	float getFreq1sec () const;
	float getFreq5sec () const;

	void setAutoClearCounter ( bool new_clear_count );
	bool setBaseFreq ( float new_base_freq );

	static const float DEF_BASE_FREQ = 100;
	
protected:
	FreqEstimator * feInt;
	FreqEstimator * fe1sec;
	FreqEstimator * fe5sec;

	bool clear_ref_counter;

};











//------------------------ FUNCTIONS FOR OLD INTERFACE (FOR COMPATIBILITY) -----------------------------

//Frequency valuation function out structure
struct fvf_out_struct
{
	float integral_fv;
	float one_sec_fv;
	float five_sec_fv;//Not realised

};


unsigned char cs_calc_ver1(unsigned char* packet,unsigned int packet_length );

unsigned char cs_calc(unsigned char* packet,unsigned int string_length,unsigned int packet_length  );

int uchar_cpy(unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int length1, unsigned int length2);

int uchar_inv_cpy (unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int length1);

int uchar_cpy_tail(unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int length1, unsigned int length2, unsigned int tail);

int uchar_cut(unsigned char* uchar_array1, unsigned char * uchar_array2, unsigned int pos1, unsigned int pos2);

int print_char_array(unsigned char* char_array, unsigned int arr_length);

int printerr_char_array(unsigned char* char_array, unsigned int arr_length);

int print_char_array_2d (unsigned char in_array[][10], unsigned int *length_array, unsigned int length);

unsigned int buffer_analyser (unsigned char* in_array, unsigned int in_ar_length, unsigned char out_array[][10], unsigned int* lengths_array, unsigned char* message_excess, unsigned int *excess_length);

int lb_converter (unsigned char* in_number, unsigned int type );

unsigned int crc16_calc_my (unsigned char * data, unsigned long int len );

unsigned int crc16_calc (void * data, unsigned long int len );

unsigned int uchar_array_sum_16bit (unsigned char * array, unsigned int len  );


float freq_valuation_func( unsigned long int * refreshed_counter, float base_freq, struct fvf_out_struct * fvf_out, float * condition_array, short int clear_condition, short int method, short int clear_flag );


unsigned char bit_invert ( unsigned char * byte, short int bit );


#endif
