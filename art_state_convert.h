#ifndef ART_STATE_CONVERT_H
#define ART_STATE_CONVERT_H

#include <iostream>
using std::cout;
using std::endl;


#include <string>
using std::string;



template<typename Type>
class ArtStateConvert
{
public:
	ArtStateConvert (Type state_init = 0) 
	{
			state = state_init;
			space_every = 4;
	}
	
	static string toString (  Type state_in, int space_every_in = 4  );
	string toString(){ return ArtStateConvert<Type>::toString( state, space_every );  };
	
	ArtStateConvert<Type>& operator= (const Type &obj) 
	{
		state = obj;
	return *this;	
	}
	
protected:
	typedef unsigned char uchar ;
	
	
private:
	Type state;
	int space_every;
	
	
};


template<typename Type>
string ArtStateConvert<Type>::toString(Type state_in, int space_every_in)
{
string state_str;
int size = sizeof (Type);
uchar * state_ar = (uchar *) &state_in;	

uchar cur_bit=0;
int bits_num = size * 8;
int first_space = (bits_num) % space_every_in;
int bit_counter=0;
int space_bit_counter = 0;

	cout<<size<<" "<<first_space<<endl;
	
	
	for (int byte=(size -1); byte>=0; byte--  )
	{
		for (int bit=7; bit>=0; bit--)
		{
			bit_counter++;
			space_bit_counter++;
			
			cur_bit = (state_ar[byte] >> bit) & (unsigned char)1;
			state_str += ( cur_bit ? '1':'0' );
			
			if (bit_counter==first_space  &&  first_space && bit_counter!=bits_num) 
			{	
				state_str+=' ';
				space_bit_counter = 0;
			}
			if (space_bit_counter == space_every_in && space_every_in && bit_counter!=bits_num) 
			{	
				state_str+=' ';
				space_bit_counter = 0;
			}
		}
	}
	
return state_str;	
}



#endif