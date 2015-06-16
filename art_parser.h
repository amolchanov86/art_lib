#ifndef ART_PARSER_H
#define ART_PARSER_H

#include <iostream>
#include <cstdio>

#include <vector>
using std::vector;
#include <utility>
using std::pair;
#include <queue>
using std::queue;

#define ART_PARSER_QUEUE_DEF_LIMIT 	100
#define ART_PARSER_MB_DEF_LIMIT 	300
#define ART_PARSER_DEF_DS		'\0'



//------------------------------ ART PARSER VER 1.0 -------------------------------------
//LITTLE HELP :)
//This parser takes strings of symbols, searches user defined messages and collect them in queues (one queue for every type of message)
//Every message is determined by its identity symbols (aka a HEADER) which can be placed in any position of a message.
//You determine the message (its length and all the identity symbols by a string of bytes. 
//This string must have length of the searched message, and the string must contain all the identity symbols of the message in order they appear in the message.
//All other symbols (which are not the identity symbols and represent the usefull information of the message) must be designeted with a special symbol, which I call the Data Symbol (DS).
//The DS might be determined by the user of the parser, by default this symbol is '\0'.
//An example of the message identification string (MIS): "AAA\0\0\0A". That means that the message has  AAA as a header, A as a tail; the message has the length of 7 symbols, 3 symbols of which represent some variable usefull data.
//The order in which you add your MIS will determine their priority while you search them.   



class ARTParser
{
public:
	//--- CONSTRUCTORS AMD DESTRUCTORS

	//Parameters:
	//mes - message identification string (MIS)
	//len - lenght of the MIS
	//DS  - data symbol (symbol that represents bytes that are not a part of the header of this kind of the message) 
	ARTParser(unsigned char*  mes=0, int len =0, unsigned char DS = ART_PARSER_DEF_DS  ,unsigned int id_max_size = 0);
	~ARTParser (); 


	//--- MAIN FUNCTIONALITY ----
	//Return: 
	// <0 - an error; no MIS has been added
	//>=0 - the id of the message, which you can use to access the queue of the this type of message
	//Parameters:
	//mes - message identification string (MIS)
	//len - lenght of the MIS 
	int addMessage ( unsigned char * mes, int len, unsigned char DS = ART_PARSER_DEF_DS );



	//Use the function in case you need to use a straight pointer
	//WARNING !!!: This function returns a pointer to an array that the parser creates. It handles creation and destruction of this array (actually this array is a vector), so DO NOT delete[] this array !!! 
	unsigned char * loadBuff (int asked_size); 
	//Use the function in case you didn't use all the size you asked with the function loadBuff
	//Size correction is a difference between asked size and really used size
	void correctBuff (int size_correction);



	//Use the function in case you know the real size of the input buffer beforehand and the function loadBuff() won't make the copying faster
	void copyBuff ( unsigned char * in_buff, int size  );



	//The main function (processing the data, that has been loaded by loadBuff() or copyBuff() )
	void process ();





	//--- PROCESSING ABSTRACTION
	bool messageValidation (int mes_id);



	//--- MAIN OPTIONS
	//Mode:
	//0 - search messages with static size
	//1 - search messages with variable size
	int setMode (int new_mode);
	//Submode:
	//0 - find all messages
	//1 - find the last one
	int setSubMode (int new_submode);



	//--- ADDITIONAL OPTIONS
	void setEraseFoundMessages (bool erase=true);//When we found some message in the stream we erase it (make bytes == 0)

	//If queue_id < 0 - then it's the main queue
	//If limit < 0 set default limit
	//Return:
	//ok = true/false
	bool setQueueLimit ( int limit=ART_PARSER_QUEUE_DEF_LIMIT, int mes_id = -1  );
	//Return:
	//-1 - no such mes id
	//other - length of a queue
	int getQueueLimit ( int mes_id = -1 ) const;

	bool setSearchMessage ( int mes_id=0, bool search = true );
	vector <unsigned char> getSearchMask () const;


	//--- TEST PURPOSES
	void printMesParameters ();
	int mesIDSize () const;




	//--- OUTPUT	
	queue < vector<unsigned char>  > messages; //Common queue (for all messages)
	long getMessagesNum (int type = 0) const;
	vector< queue< vector< unsigned char > > * > messages_queues;//Queues for particular types of messages



protected:

	//Mode:
	//0 - search messages with static size
	//1 - search messages with variable size
	int mode;

	//Submode:
	//0 - find all messages
	//1 - find the last one
	int submode;

	static const int mb_def_size = ART_PARSER_MB_DEF_LIMIT ;
	static const int exc_def_size = 1;
	vector <unsigned char> excess; 
	unsigned int exc_ipointer;
	vector <unsigned char> * main_buff;
	unsigned int mb_ipointer; //Internal pointer. Points to a free cell


	vector < pair <unsigned char, int> **  > messages_info;//An array that contains pairs of identify symbols and their positions in the message 		
	vector <int> symbols_num;//Array that contains number of identify symbols (symbols that identify this particular kind of message)
	vector <int> mes_len;//Array that contains messages lengths
	vector <long> mes_counter;
	int max_len;
	int min_len;

	static const unsigned int MES_ID_DEFAULT_SIZE = 15;
	unsigned int mes_id_size;


	//Find LAST parameters
	vector <unsigned char> fl_search_mask;
	vector <unsigned char> fl_search_result;


	//Other options
	bool erase_found_messages; //Erase found messages in the main buffer

	//Queue length limits
	vector <int> queue_limit;
	int main_queue_limit;



	//---Auxillary Private Functions
	char getSymb (int mes_num, int symb_num);
	int getSymbPos (int mes_num, int symb_num);

	void sizeControl (int asked_size);

	int findMaxMesLen ();
	int findMinMesLen ();

	unsigned char & mergedArraByOffset ( int offset );

	//These functions are for the Find All Const Length mode
	//Return:
	//-1 - no header
	// 0 - end of main_buffer
	// 1 - header found
	int headerCheck ( int message_num, int mb_cur_byte  );
	int lengthCheckCL (int message_num, int mb_cur_byte );

	//These functions are for the Find Last Const Length mode
	//Return:
	//-1 - no header
	// 0 - end of main_buffer
	// 1 - header found
	int headerCheckFLCL ( int message_num, int mb_cur_byte  );
	int lengthCheckFLCL (int message_num, int mb_cur_byte );





	bool checkQueueLimit ( int mes_id = -1 );

	//Processors
	void processFindAllCL ();
	void processFindLastCL ();

	void processFindAllVL ();
	void processFindLastVL ();




 
};









#endif
