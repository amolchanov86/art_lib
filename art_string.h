#ifndef ART_STRING
#define ART_STRING

#include <stdlib.h>
#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;
#include <stdio.h>

class ArtString : public string
{
public:
    ArtString( const string & str_ ):string(str_){}
	
	static void toLowerCase(string &str);
	void toLowerCase();

	static void toUpperCase(string &str);
	void toUpperCase();
	
	//Deletes the set of symbols 
	static void delSymbol(string &str, const string &symb );
	void delSymbol(const string &symb);
	
	//Delete given set of symbols in the beginning and in the end of the string
	static void delEdges(string &str, const string &symb);
	void delEdges (const string &symb);

	//The function supports conversion from ASCII to symbols
	//The function works in 2 modes: 
	//	1) No ASCII string mode
	//	2) ASCII string mode. Begins after '<' symbol. 
	//In this mode spaces are considered to be deviders. The function analyse sequences of symbols between spaces
	//and tries to convert them into ASCII. If the procedure fails the function adds this sequences AS IS. 
	//The mode is finished with '>' symbol , but it is not necessary to put this symbol if the string is finished.
	//Examples:
	//"aaa<< 98 98 aaa >" => {97 97 97 60 98 98 97 97 97} in other words, the result is "aaa<bbaaa"
 	static string convertASCIIcodes( string &str );
	string convertASCIIcodes( );

	//flag:
	//=='b' - binar mode (convert from binar representation to a int)
	static long int toInt(const string &str_, char * flag = 0 , bool * conv_ok = 0);
	long int toInt (char * flag = 0 ,bool * conv_ok = 0);

	//Pattern for conversion: <+/-><num><.><num><e/E><+/-><num>, i.e.: +000.000e+000
	static double toDouble( const string &str_, bool * conv_ok = 0  );
	double toDouble(bool * conv_ok = 0);
	
};


  


#endif