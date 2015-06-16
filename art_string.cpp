#include "art_string.h"

void ArtString::toLowerCase(string &str)
{
	for(int i=0; i< str.length(); i++  )
	{
		char case_bias = 'A' - 'a';
		if ( (str[i] >= 'A') && (str[i] <= 'Z') )
		{
			str[i]-=case_bias;
		}
	}		
}
void ArtString::toLowerCase()
{
	toLowerCase(*this);
}

void ArtString::toUpperCase(string &str)
{
	for(int i=0; i< str.length(); i++  )
	{
		char case_bias = 'A' - 'a';
		if ( (str[i] >= 'a') && (str[i] <= 'z') )
		{
			str[i]+=case_bias;
		}
	}		
}

void ArtString::toUpperCase()
{
	toUpperCase(*this);
}

//Deletes the set of symbols 
void ArtString::delSymbol(string &str, const string &symb )
{
string temp_str;
bool symb_found = false;
	for(int i=0; i< str.length(); i++  )
	{
		symb_found= false;
		for (int j =0; j< symb.length(); j++)
		{
			if ( str[i] == symb[j] ) symb_found = true; 
		}
		if(!symb_found)  temp_str.push_back(str[i]);
	}
	
	if (temp_str.size())
	{
		str = temp_str;
	}
}

void ArtString::delSymbol(const string &symb)
{
	delSymbol(*this, symb);
}

//Delete given set of symbols in the beginning and in the end of the string
void ArtString::delEdges(string &str, const string &symb)
{
	if(str.size() == 0  || symb.size() == 0)
return;

	int begin_symb = str.find_first_not_of(symb);
	int end_symb   = str.find_last_not_of(symb);
	if (  begin_symb == 0 && end_symb==(str.size() - 1)  )
	{
return;		
	}
	if ( ( begin_symb == string::npos ) || (end_symb == string::npos)  )
	{
		str.resize(0);
return;
	}
	
	str = str.substr(begin_symb, end_symb - begin_symb + 1);
	
}

void ArtString::delEdges (const string &symb)
{
	delEdges(*this, symb);
}


//The function supports conversion from ASCII to symbols
//The function works in 2 modes: 
//	1) No ASCII string mode
//	2) ASCII string mode. Begins after '<' symbol. 
//In this mode spaces are considered to be deviders. The function analyse sequences of symbols between spaces
//and tries to convert them into ASCII. If the procedure fails the function adds this sequences AS IS. 
//The mode is finished with '>' symbol.
//Examples:
//"aaa<< 98 98 aaa >" => {97 97 97 60 98 98 97 97 97} in other words, the result is "aaa<bbaaa"
string ArtString::convertASCIIcodes( string &str)
{
string num_str;	
string temp_str;
string edges =" ";
bool begin = false;
bool error =false;
int num = -1;

	for(int i=0; i< str.size(); i++)
	{
		switch(str[i])
		{
		case '<':	
			if (begin)
			{
				num_str.push_back(str[i]);
			}
			begin = true;	
			break;
		case '>':
			if (begin)
			{
				num = atoi( num_str.c_str() );
				if ( num>0 && num<=127 )  temp_str.push_back(num);
				else if (num == 0  && num_str.size()==1 && num_str[0]=='0') temp_str.push_back(num);
				else if(num_str.size())		temp_str += num_str;
				num_str.resize(0);
				begin = false;
			}
			else
			{
				temp_str.push_back(str[i]);
			}
			break;
			
		default:
			if (begin)
			{
				switch (str[i])
				{
				case ' ':
					num = atoi( num_str.c_str() );
					if ( num>0 && num<=255 )  temp_str.push_back(num);
					else if (num == 0  && num_str.size()==1 && num_str[0]=='0') temp_str.push_back(num);
					else if(num_str.size())temp_str += num_str;
					num_str.resize(0);
					
					break;	
				default:
					num_str.push_back(str[i]);
					
					break;
				}
				
			}
			else
			{
				temp_str.push_back(str[i]);	
			}					
		}
		cout<<temp_str.size()<<" ";		
	}
	
	num = atoi( num_str.c_str() );
	if ( num>0 && num<=255 )  temp_str.push_back(num);
	else if (num == 0  && num_str.size()==1 && num_str[0]=='0') temp_str.push_back(num);
	else if(num_str.size())		temp_str += num_str;
	
return temp_str;
}

string ArtString::convertASCIIcodes( )
{
	convertASCIIcodes( *this);
}

long int ArtString::toInt(const string &str_, char * flag_ , bool * conv_ok )
{
long int result = 0;
bool ok = true;	
string flag;
const int MODE_NORM = 0;//Convert decimal 
const int MODE_HEX = 1;
const int MODE_BIN = 2;
//const int MODE_BIN_SIGN = 3;
//const int MODE_BIN_COMP = 4;
int mode = MODE_NORM;

int scanres = 0;

ArtString str(str_);
	str.delEdges(" ");

	if (!str.length())	ok = false;
	
	
	if (flag_) flag = flag_;
	ArtString::toLowerCase(flag);
	
	if (flag == "b") mode = MODE_BIN;
	//else if (flag == "bs") mode = MODE_BIN_SIGN;
	//else if (flat == "bc") mode = MODE_BIN_COMP;
	if (str.length() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X' ))
	{
		mode = MODE_HEX;
	}
	
	if (ok)
	{
		switch (mode)
		{
		case MODE_BIN:
			ok = str.find_last_not_of("01") == string::npos;
			if (ok)
			{
				result = 0;
				long int mask = 1;
				int offset = 0;
				for (int i= str.length() - 1; i >= 0; i--)
				{
					if(str[i]=='1') result |= mask<<offset;
					offset++;
				}
			}
			break;
			
		case MODE_HEX:

			scanres = sscanf( str.c_str() , "%x", &result); 
			ok = (scanres != EOF) && scanres;
			
			break;	
			
		case MODE_NORM:
		default:		
			ok = (str.find_last_of( "+-") == 0 || str.find_last_of( "+-") == string::npos  ) && (str.find_last_not_of("0123456789+-") == string::npos);
			if (ok)
			{
				result = atoi( str.c_str() );
			}
			
			break;
		}	
	}
	
	if (conv_ok) *conv_ok = ok;
	
return result;
}


long int ArtString::toInt(char* flag, bool* conv_ok)
{
return toInt(*this, flag, conv_ok);	
}



//Pattern for conversion: <+/-><num><.><num><e/E><+/-><num>, i.e.: +000.000e+000
double ArtString::toDouble( const string &str_, bool * conv_ok )
{
bool ok = true;	
double result = 0;	
int check_step = 0;	

int sign = 1;
string mantissa;
double mantissa_num=0;

int exp_sign = 1;
string exponent;
double exp_num=0;

const int EXPONENT_STEP = 3;

ArtString str(str_);
	str.delEdges(" ");

	if (!str.length()) ok = false;

	for (int i = 0; i< str.length(); i++)
	{
		if (!ok) break;
		
		switch( check_step )
		{
		case 0:
			if (str[i] == '+' )
			{
				sign = 1;
				check_step++;
				mantissa.push_back('0');
			}
			else if (str[i] == '-' )
			{
				sign = -1;
				check_step++;
				mantissa.push_back('0');
			}
			else if (str[i] >= '0' && str[i] <= '9' )
			{
				sign = 1;
				check_step++;
				mantissa.push_back(str[i]);
			}
			else if(str[i] == 'e' || str[i] == 'E')
			{
				check_step = EXPONENT_STEP;
			}
			else
			{
				ok = false;
			}
			break;
			
		case 1:
			if (str[i] >= '0' && str[i] <= '9' )
			{
				mantissa.push_back(str[i]);
			}
			else if (str[i] == '.')
			{
				mantissa.push_back(str[i]);
				check_step++;
			}
			else if(str[i] == 'e' || str[i] == 'E')
			{
				check_step = EXPONENT_STEP;
			}
			else
			{
				ok = false;
			}
			break;
			
		case 2:
			if (str[i] >= '0' && str[i] <= '9' )
			{
				mantissa.push_back(str[i]);
			}
			else if(str[i] == 'e' || str[i] == 'E')
			{
				check_step = EXPONENT_STEP;
			}
			else
			{
				ok = false;
			}
			break;

		case EXPONENT_STEP:
			if (str[i] == '+' )
			{
				exp_sign = 1;
				check_step++;
				exponent.push_back('0');
			}
			else if (str[i] == '-' )
			{
				exp_sign = -1;
				check_step++;
				exponent.push_back('0');
			}
			else if (str[i] >= '0' && str[i] <= '9' )
			{
				exp_sign = 1;
				check_step++;
				exponent.push_back(str[i]);
			}
			else
			{
				ok = false;
			}
			break;
			
		case (EXPONENT_STEP + 1):
			if (str[i] >= '0' && str[i] <= '9' )
			{
				exponent.push_back(str[i]);
			}
			else
			{
				ok = false;
			}
			break;	
			
		}
	}
	
	if ( ok ) //Conversion
	{
		if (mantissa.length()) mantissa_num = sign * atof(mantissa.c_str());
		else mantissa_num = 1;
		if (exponent.length()) exp_num = atoi(exponent.c_str());
		else exp_num = 0;
		
		result = mantissa_num;
		for (int i=0; i<exp_num; i++)
		{
			(exp_sign > 0) ? result *= 10 : result /= 10;
		}
	}
	
	if (conv_ok) *conv_ok = ok;

return result;	
}


double ArtString::toDouble(bool * conv_ok )
{
	return toDouble(*this, conv_ok);
}