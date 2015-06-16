#include "art_argcv.h"

ArtArgcv::ArtArgcv()
{
	setArgcv(0 , 0);
}

ArtArgcv::ArtArgcv(int argc_, char* argv_[])
{
	setArgcv(argc_, argv_);
}

int ArtArgcv::addOption(const string& full_name_, string short_name_, string help_str_)
{
bool fn_occupied = false;
bool sn_occupied = false;
int id = -1;

	for (int i=0; i< options.size(); i++)
	{
		if (options[i].full_name == full_name_ ) 
		{
			id = options[i].id;
			fn_occupied = true;
			if(id) options[id].short_name = short_name_;//if id == 0 don't assign the short name
			options[id].comments = help_str_;
		}
	}
	
	if (!fn_occupied ) 
	{
		ArtArgcvOption option;
		option.full_name = full_name_;
 		option.short_name = short_name_;
		for (int i=0; i< options.size(); i++)
		{
			if (options[i].short_name == short_name_ ) options[i].short_name = "";
		}		
		options.push_back(option);
		options.back().comments = help_str_;
		options.back().id = options.size() - 1;
		id = options.back().id;
		
		findOptInRaw( options.back().id );
		
// 		for (int i=1; i<raw_options.size(); i++)
// 		{
// 			if ( raw_options[i].full_name == full_name_  || raw_options[i].short_name == short_name_ )
// 			{
// 				options.back().exist = true;
// 				options.back().values = raw_options[i].values;
// 				raw_options[i].id = options.back().id;
// 			}
// 		}
		
	}
	
return 	id;
}



int ArtArgcv::rawOptParser()
{
	progr_name = argv[0];
	raw_options.resize(1);

	for (int i=1; i<argc; i++)
	{
		string temp (argv[i]);
		if(temp.length() > 2  && temp[0] == '-' && temp[1] == '-' )
		{
			temp.erase(temp.begin(), temp.begin()+2);
			raw_options.push_back( ArtArgcvOption() );
			raw_options.back().full_name = temp;
		}
		else if (temp.length() > 1  && temp[0] == '-')
		{
			temp.erase(temp.begin());
			raw_options.push_back( ArtArgcvOption() );
			raw_options.back().short_name = temp;
		}
		else
		{
			if(raw_options.size() == 0) raw_options.push_back( ArtArgcvOption() );
			raw_options.back().values.push_back(temp);
		}
	}
	
	options.resize(1);
	options[0].id = 0;
	if (raw_options[0].full_name.size() == 0   && raw_options[0].short_name.size() == 0)
	{
		options[0].values = raw_options[0].values;
		if(options[0].values.size()) options[0].exist = true;
	}
	
	
	//Debug
	/*
	for (int i=0; i<raw_options.size(); i++)
	{
		cout<<"---------------"<<endl;
		cout<<raw_options[i].full_name<<endl;
		cout<<raw_options[i].short_name<<endl;
		for (int j=0; j< raw_options[i].values.size(); j++)
		{
			cout<<raw_options[i].values[j]<<" ";
		}
		cout<<endl;
	}
	*/
return 0;	
}

bool ArtArgcv::findOptInRaw(int opt_id_)
{
bool result = false;	
	if ( !checkOptID(opt_id_) )
	{
return false;
	}
	
	for (int i=1; i<raw_options.size(); i++)
	{
		if ( raw_options[i].full_name == options[opt_id_].full_name  || raw_options[i].short_name == options[opt_id_].short_name )
		{
			options[opt_id_].exist = true;
			options[opt_id_].values = raw_options[i].values;
			raw_options[i].id = options[opt_id_].id;
		}
	}
	
return true;	
}

bool ArtArgcv::checkOptID(int opt_id_)
{
return 	(opt_id_ < options.size() && opt_id_ >= 0 );
}


int ArtArgcv::getID(const string& full_name_)
{	
	for (int i=0; i<options.size(); i++)
	{
		if (options[i].full_name == full_name_)
return i;
	}
return -1;	
}


bool ArtArgcv::getOption(int opt_id_, int * val_num)
{
	if ( checkOptID(opt_id_) )
	{
		if(val_num) *val_num = options[opt_id_].values.size();
return options[opt_id_].exist;
	}
	else
	{
		if(val_num) *val_num = 0;
return false;
	}
}

bool ArtArgcv::getOptValStr(int opt_id_, string& input, int opt_val_num_)
{
bool ok = true;	
	if ( checkOptID(opt_id_) )
	{
		if (options[opt_id_].exist && opt_val_num_ >= 0  && opt_val_num_ < options[opt_id_].values.size() && options[opt_id_].values.size() )
		{
			input = options[opt_id_].values[  options[opt_id_].values.size() - 1 - opt_val_num_  ];
		}
		else
		{
			//input = "";
			ok = false;
		}
	}
	else
	{
		ok = false;
	}
	
return ok;	
}

bool ArtArgcv::getOptValFloat(int opt_id_, float& input, int opt_val_num_)
{
string temp_str;
bool ok = false;
	ok = getOptValStr(opt_id_, temp_str, opt_val_num_);
	if (ok)
	{
		input =  ArtString::toDouble(temp_str, &ok); 
	}
return ok;
}


bool ArtArgcv::getOptValInt(int opt_id_, long int &input, int opt_val_num_)
{
string temp_str;
bool ok = false;
	ok = getOptValStr(opt_id_, temp_str, opt_val_num_);
	if (ok)
	{
		long int temp_input= 0;
		temp_input =  ArtString::toInt(temp_str, 0 ,&ok); 
		if(ok)
		{
			input = temp_input;
		}
	}
return ok;
}


int ArtArgcv::getUnknOptNum()
{
int counter = 0;
	for (int i=0; i<raw_options.size(); i++)
	{
		if (raw_options[i].id < 0) counter++;
	}
return counter;	
}


void ArtArgcv::setArgcv(int argc_, char* argv_[])
{
	argc = argc_;
	argv = argv_;
	
	if (options.size() <= 0)
	{
		options.resize(1);
		options[0].id = 0;	
	}
	
	if (argc_ > 0 && argv_)
	{
		progr_name = argv_[0];
		//cout<<progr_name<<endl;
		rawOptParser();
		for (int i=1; i< options.size(); i++)
		{
			findOptInRaw( options[i].id );	
		}
		
	}
}

string ArtArgcv::printHelp()
{
string help_str;
	help_str =  progr_name + " parameters : \n" ;
	if ( options[0].comments.size() ) 
		help_str +=  "   first (noname) param - " + options[0].comments + "\n";
	for (int i= 1; i< options.size(); i++)
	{
		help_str +=  "   --" + options[i].full_name + " (-" + options[i].short_name + ") - " + options[i].comments + "\n";
	}
	
return help_str;	
}

string ArtArgcv::printStatistics()
{
stringstream stat;

	for (int i= 0; i< options.size(); i++)
	{
		stat << "   --" << options[i].full_name << " (-" << options[i].short_name << ") - " << "\n";
		stat << "ID=" <<	 options[i].id << "\n";
		stat << "Exist=" <<  (int) options[i].exist  << "\n";
		stat << "Values=";
		for (int j=0; j< options[i].values.size(); j++)
		{
			 stat << " " << options[i].values[j];
		}
		stat << "\n\n";
	}	
	
return stat.str();
}

