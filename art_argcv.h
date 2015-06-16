#ifndef ART_ARGCV_H
#define ART_ARGCV_H

#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <sstream>
using std::stringstream;

#include "art_string.h"


struct ArtArgcvOption
{
	ArtArgcvOption()
	{
		id = -1;
		exist = false;
	}
	int id;
	bool exist;
	string full_name;
	string short_name;
	vector<string> values;
	string comments;
};


//Command line parser
//Logic:
//Every option is designated with its name and short name (you can omit a short name but you can't omit a name)
//Every option then gets its id (when you use addOption() )
//The first value (right after the name of the program) may go without a name. It always has 0 id
//Every option may have several values. Every value has its num. 0 - is the last value. 
class ArtArgcv 
{
public: 
	ArtArgcv();
	ArtArgcv(int argc_, char * argv_[]);
	
	void setArgcv(int argc, char * argv[]);
	//Return: option id or -1 if the option is already exists
	//A short synonum for an option must be assigned manually. 
	//In case of short names overlap the short name is assigned to the last added option
	int addOption(const string& full_name_, string short_name = "", string help_str = "");
	
	//Return: bool - indicates if the option exists (0 - is the values that goes right after the program name, i.e. noname option)
	bool getOption(int opt_id_, int * val_num = 0);//Check if the option exists. Use val_num to retrieve number of values for this option
	
	//Return: bool - indicates if the opt val exists and can be properly converted 
	//opt_val_num_ is taken from the end (i.e. 0 - is the last (latest) element)
	bool getOptValStr  (int opt_id_, string   &input, int opt_val_num_ = 0);
	bool getOptValFloat(int opt_id_, float    &input, int opt_val_num_ = 0);
	bool getOptValInt  (int opt_id_, long int &input, int opt_val_num_ = 0);
	
	//Returns option's ID by its name
	int getID(const string &full_name_);
	
	//Searches for unknown options
	int getUnknOptNum();
	string printHelp ();
	string printStatistics ();
	
protected:
	int rawOptParser();
	bool findOptInRaw(int opt_id_);//tries to find option in the raw array and gets its values
	inline bool checkOptID(int opt_id_);
	vector<ArtArgcvOption> options;//Contain options. ID == 0 - header option that can have no names (neither full nor short)
	vector<ArtArgcvOption> raw_options;
	string progr_name;
	int argc;
	char **argv;
	
};



#endif