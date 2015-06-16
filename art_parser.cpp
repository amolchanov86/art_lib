#include "art_parser.h"


//#define DEBUG

#ifdef DEBUG
	#define DEBUG_IF  if(1)
#else
	#define DEBUG_IF  if(0)
#endif


using std::vector;
using std::queue;
using std::pair;
using std:: cout;
using std:: endl;

//---------------- CONSTRUCTORS & DESTRUCTORS ---------------------------

ARTParser :: ARTParser ( unsigned char* mes, int len, unsigned char DS,  unsigned int id_max_size  )
{

	mode = 0;
	submode = 0;

	max_len=0;
	min_len=0;

	if (id_max_size <= 0)
	{
		mes_id_size = MES_ID_DEFAULT_SIZE;
	}
	else
	{
		mes_id_size = id_max_size;
	}

	addMessage (mes, len, DS);
	//Excess init


	//Main buffer initialisation	
	main_buff = new vector<unsigned char> ( mb_def_size  );
	mb_ipointer = 0; 
	exc_ipointer = 0; 
	
	//Options
	erase_found_messages = false;
	setQueueLimit();//Def limit for the main queue

}


ARTParser:: ~ARTParser ()
{

	vector<pair <unsigned char, int> ** >::iterator iter = messages_info.begin();
	vector<pair <unsigned char, int> ** >::iterator iter_end = messages_info.end();
	for ( ;iter != iter_end; iter++   )
	{
		for (int i=0; i < mes_id_size; i++)
		{
			delete (*iter)[i];	

		}
		
		delete [] *(iter);

	}

	for (int i=0; i< messages_queues.size() ; i++)
	{
		delete messages_queues [i];
	}	

	delete main_buff;	

}









//---------------------- PUBLIC FUNCTIONS -------------------------------------



//Return:
// 0 - ok
//-1 - bad input data
int ARTParser:: addMessage ( unsigned char* mes, int len , unsigned char DS)
{
	if ( (mes == 0) || (len <= 0) )
	{
return -1;		//<=return
	}

	messages_info.push_back ( new pair<unsigned char, int> * [ mes_id_size ] );

	for (int ind=0; ind< mes_id_size; ind++ )
	{
	
		(messages_info.back() )[ind] = 0;	

	}
	mes_len.push_back (len);



	int ind2=0;
	int cur_id_size=0;	
	while ( (ind2<len) && (cur_id_size < mes_id_size)   )
	{
		if (  mes [ind2] != DS  )//DS = data symbol, other symbols which repesent date, i.e. bytes in messages that are not a part of a header
		{
			cur_id_size++;
			(messages_info.back()  )[ cur_id_size - 1 ] = new  pair<unsigned char, int> ( mes[ind2], ind2  ); 


		}						


		ind2++;	
	}
	symbols_num.push_back (cur_id_size);


	max_len=findMaxMesLen();	
	min_len=findMinMesLen();	

	excess.resize ( max_len + 1  );

	mes_counter.push_back (0);

	queue < vector<unsigned char>  >  * temp_pqueue = new  queue < vector<unsigned char>  >;

	messages_queues.push_back (  temp_pqueue  ); 

	fl_search_mask.push_back (1);
	fl_search_result.push_back (0);


	queue_limit.push_back (0);
	setQueueLimit ( ART_PARSER_QUEUE_DEF_LIMIT, messages_queues.size() - 1  );


return messages_info.size() - 1;
}



bool ARTParser:: setSearchMessage (  int mes_id, bool search  )
{
	if ( (mes_id < 0)   ||   (mes_id > fl_search_mask.size() - 1) )
return false;

	if (search) fl_search_mask [mes_id] = 1;
	else fl_search_mask [mes_id] = 0;	

return true;
}


vector<unsigned char> ARTParser:: getSearchMask () const
{
return fl_search_mask; 
}


//Return pointer for the buffer array (YOU MUST DECLARE THE SIZE YOU GONNA USE)
//Use function correctBuffSize if the size
unsigned char * ARTParser:: loadBuff (int asked_size)
{
	if (asked_size < 0)
return 0;

	sizeControl ( asked_size);

	int prev_mb_ipointer = mb_ipointer;
	mb_ipointer +=asked_size;
	


	//vector<unsigned char >::iterator iter = main_buff->begin() + mb_ipointer;
	//printf ("\nIterator=%p\n", iter);

return ( unsigned char *) &( (*main_buff) [prev_mb_ipointer]);		
}


void ARTParser:: correctBuff (int size_correction)
{
	if (size_correction >= 0)
	{
		mb_ipointer -= size_correction;		

	}
	if (mb_ipointer < 0)
	{
		mb_ipointer = 0;
	}

}


void ARTParser:: copyBuff ( unsigned char * in_buff, int size  )
{


	if ( (in_buff != 0 )  &&  (size >0) )
	{

		sizeControl (size);


		for ( int i = 0 ; i< size  ; i++  )
		{
			(*main_buff) [mb_ipointer + i] = in_buff[i];	

		}
		
		mb_ipointer +=size;

	}

}

void ARTParser:: process ()
{
	switch (mode)
	{
	//Find Constant Length
	case 0:

		switch (submode)
		{
		//Find All
		case 0:
			processFindAllCL ();
			break;
		//Find Last
		case 1:
			processFindLastCL();
			break;


		}

		break;

	//Find Variable Length
	case 1:

		switch (submode)
		{
		//Find All
		case 0:
			processFindAllVL ();
			break;
		//Find Last
		case 1:
			processFindLastVL ();
			break;


		}

		break;

	}
}


//Return:
//num > 0 - the mode that has been set
//num = -1 - the mode doesn't exist
int ARTParser:: setMode (int new_mode )
{

	switch (new_mode)
	{
	case 0:
	case 1:
		mode = new_mode;
return mode;
		break;

	default:

return -1;	//<==return
		break;	


	}



}

//Return:
//num > 0 - the submode that has been set
//num = -1 - the submode doesn't exist
int ARTParser:: setSubMode (int new_submode )
{

	switch (new_submode)
	{
	case 0:
	case 1:
		submode = new_submode;
return submode;
		break;

	default:

return -1;	//<==return
		break;	


	}



}


//--------------------------- TEST FUNCTIONS -----------------------------------


void ARTParser::  printMesParameters ()
{

	int num=0;
	vector<pair <unsigned char, int> ** >::iterator iter = messages_info.begin();
	vector<pair <unsigned char, int> ** >::iterator iter_end = messages_info.end();
	for ( ;iter != iter_end; iter++   )
	{
		cout<<"Mes "<<num<<" : ";
		cout<<"len="<<mes_len[num]<<" : ";
		cout<<"symbols="<<symbols_num[num]<<" : ";
		for (int i=0; i < mes_id_size; i++)
		{
			if ( (*iter)[i] != 0  )	cout<<  ((*iter)[i])->first << "(" << ((*iter)[i])->second << ")  ";	
			else  break;
//			printf ( " %p"  ,(*iter)[i] ) ;	

		}
		cout << endl;		
		cout<< "Max length="<<max_len<<"    Min length="<<min_len<<endl;
		num++;

	}



}



int ARTParser:: mesIDSize () const
{
	return mes_id_size;

}


//When we found some message in the stream we erase it (make bytes == 0)
void ARTParser:: setEraseFoundMessages (bool erase)
{
	erase_found_messages = erase;

}


bool ARTParser:: setQueueLimit ( int limit, int mes_id   )
{
	if ( mes_id >=  (int) queue_limit.size()  )
return false;

	if (  limit < 0 ) limit = ART_PARSER_QUEUE_DEF_LIMIT;


	if (mes_id < 0)
	{
		main_queue_limit = limit;
		checkQueueLimit ( mes_id );	
	}
	else
	{
		queue_limit [mes_id] = limit;	
		checkQueueLimit ( mes_id );	
	}		
		

return true;
}


int ARTParser:: getQueueLimit ( int mes_id ) const
{
	if ( mes_id >=  (int) queue_limit.size()  )
return -1;

	if (mes_id < 0 )
return main_queue_limit;

return  queue_limit [mes_id];	
}


//---------------------------- PRIVATE FUNCTIONS --------------------------------

char ARTParser:: getSymb (int mes_num, int symb_num)
{
//	if (  (mes_num >= 0) && (mes_num < messages_info.size() )  )
//	{
//		if ( (symb_num >=0) && ( symb_num < symbols_num[mes_num] ) )
//		{
return  (messages_info[mes_num][symb_num] )->first;			

//		}
//
//	}
//	else
//return '\0';

}


int ARTParser:: getSymbPos (int mes_num, int symb_num)
{
//	if (  (mes_num >= 0) && (mes_num < messages_info.size() )  )
//	{
//		if ( (symb_num >=0) && ( symb_num < symbols_num[mes_num] ) )
//		{
return  (messages_info[mes_num][symb_num] )->second;			

//		}

//	}
//	else
//return '\0';

}


//void ARTParser:: sizeControl (  int asked_size  )
//{
	//Checking out the current size of the main buffer and increasing it in case it's too small
//	if (   ( main_buff->capacity() - (mb_ipointer + 1)  ) <= asked_size	)
//	{
//		vector<unsigned char> * prev_buff = main_buff;
//		main_buff = new vector<unsigned char> (  prev_buff->capacity() * 2 + asked_size  );
//	
//		for (int i = 0; i< mb_ipointer ; i++)
//		{
//			(*main_buff)[i] = (*prev_buff)[i];
//
//		}	
//
//		delete prev_buff;
//		prev_buff = 0;
//
//	}
//
//}

void ARTParser:: sizeControl (  int asked_size  )
{
      //Checking out the current size of the main buffer and increasing it in case it's too small
	if (   ( main_buff->capacity() - (mb_ipointer + 1)  ) <= asked_size	)
	{
		DEBUG_IF printf ("Size increased = %d\n", main_buff->capacity() * 2 + asked_size );
		
	
		#ifdef DEBUG	
		printf ("MAIN BUFFER BEFORE SIZE INCREASE (capacity=%d size=%d) :: ", main_buff->capacity(), main_buff->size()  );
		for ( int i = 0; i < mb_ipointer; i++   )
		{
			printf ("%d ", (*main_buff)[i] );
		}
		printf ("\n");
		#endif	
		
			main_buff->resize(main_buff->capacity() * 2 + asked_size  );

		#ifdef DEBUG
		printf ("MAIN BUFFER AFTER SIZE INCREASE (capacity=%d size=%d) :: ", main_buff->capacity(), main_buff->size() );
		for ( int i = 0; i < mb_ipointer; i++   )
		{
			printf ("%d ", (*main_buff)[i] );
		}
		printf ("\n");
		#endif
	
	}

}






int ARTParser:: findMaxMesLen ()
{
int max_size =0;

	if ( mes_len.empty() )
return 0;

	int messages_num = mes_len.size();
	for ( int i = 0; i < messages_num; i++   )
	{
		if (max_size < mes_len[i])
		{
			max_size = mes_len[i];

		}					

	}	
return max_size;

}

int ARTParser:: findMinMesLen ()
{
int min_size =0;

	if ( mes_len.empty() )
return 0;

	int messages_num = mes_len.size();
	min_size = mes_len[0];
	for ( int i = 0; i < messages_num; i++   )
	{
		if (min_size > mes_len[i])
		{
			min_size = mes_len[i];

		}					

	}	

return min_size;

}


//Return:
//-1 - no header
// 0 - end of main_buffer
// 1 - header found

//WARNING: the function DOES NOT CHECK message length 
int ARTParser:: headerCheck ( int mes_cur, int mb_cur_byte  )
{
int offset = 0;
	if (  mb_cur_byte <  messages_info[mes_cur][0]->second )
	{
	DEBUG_IF printf ("Headcheck: short word\n");
return -1;
	}

	if (  symbols_num[ mes_cur  ] == 1 )
return 1;

	for ( int mes_cur_byte = 1; mes_cur_byte < symbols_num[ mes_cur  ]; mes_cur_byte++  )
	{
		offset =   messages_info[mes_cur][mes_cur_byte]->second - messages_info[mes_cur][0]->second  ;
		if (  (mb_cur_byte + offset) >= mb_ipointer  )
return 0;
		if ( (*main_buff)[  mb_cur_byte + offset ] != messages_info[mes_cur][mes_cur_byte]->first   )
		{

	DEBUG_IF printf ("Headcheck: no header %d != %d offset=%d mes_cur_byte=%d\n" , (*main_buff)[  mb_cur_byte + offset ],   messages_info[mes_cur][mes_cur_byte]->first, offset, mes_cur_byte );
return -1;
		} 


	}
	
return 1;

}






//Return:
// 0 - can't finish length check because of the end of the main buffer
// 1 - length is ok
int ARTParser:: lengthCheckCL (int mes_cur, int mb_cur_byte )
{
	//if  ( ( mes_len[mes_cur] - (messages_info[mes_cur][0]->second  + 1)   )  >=  mb_ipointer )
	if  ( ( mes_len[mes_cur] - (messages_info[mes_cur][0]->second  + 1) + mb_cur_byte   )  >=  mb_ipointer )
	{

return 0;

	}

return 1;

}





//Return:
//-1 - no header
// 0 - end of main_buffer
// 1 - header found

//WARNING: the function DOES NOT CHECK message length 
int ARTParser:: headerCheckFLCL ( int mes_cur, int mb_cur_byte  )
{
int offset = 0;
	if (  mb_cur_byte <  messages_info[mes_cur][ symbols_num[mes_cur] - 1  ]->second )
	{
	DEBUG_IF printf ("Headcheck: short word\n");
return -1;
	}

	if (  symbols_num[ mes_cur  ] == 1 )
return 1;

	int mb_check_begining = mb_cur_byte - messages_info[mes_cur][ symbols_num[mes_cur] - 1  ]->second;
	for ( int mes_cur_byte = 0; mes_cur_byte < symbols_num[ mes_cur  ]; mes_cur_byte++  )
	{
		offset =   messages_info[mes_cur][mes_cur_byte]->second - messages_info[mes_cur][0]->second  ;
		if (  (mb_check_begining + offset) >= mb_ipointer  )
return 0;
		if ( (*main_buff)[  mb_check_begining + offset ] != messages_info[mes_cur][mes_cur_byte]->first   )
		{

	DEBUG_IF printf ("Headcheck: no header %d != %d offset=%d mes_cur_byte=%d\n" , (*main_buff)[  mb_check_begining + offset ],   messages_info[mes_cur][mes_cur_byte]->first, offset, mes_cur_byte );
return -1;
		} 


	}
	
return 1;

}




//Return:
// 0 - can't finish length check because of the end of the main buffer
// 1 - length is ok
int ARTParser:: lengthCheckFLCL (int mes_cur, int mb_cur_byte )
{

	//int mb_check_begining = mb_cur_byte - messages_info[mes_cur][ symbols_num[mes_cur] - 1  ]->second;
	if  ( ( mes_len[mes_cur] - (messages_info[mes_cur][ symbols_num[mes_cur] - 1   ]->second  + 1)  + mb_cur_byte  )  >=  mb_ipointer )
	{

return 0;

	}

return 1;

}





















bool ARTParser:: messageValidation (int mes_id)
{

return true;
}





bool ARTParser::  checkQueueLimit ( int mes_id  )
{
	if ( mes_id >=  (int) queue_limit.size()  )
return false;

	if (mes_id < 0)
	{
		while ( messages.size() > main_queue_limit   )
		{
			messages.pop();
		}	
	}
	else
	{
		while ( (messages_queues[mes_id])->size()    > queue_limit [mes_id]   )
		{
			(messages_queues[mes_id])->pop();
		}	
	}		

return true;
}


//Return:
//< 0 - if there is no such a type
long ARTParser:: getMessagesNum (int type ) const
{
	

	if (mes_counter.size() < (type+1) || (type < 0) )	
	{
return -1;
	}
return mes_counter[type];
}







////////////////////////////////////////////////////////////////////////
//-------------------------- PROCESSING --------------------------------
////////////////////////////////////////////////////////////////////////
void ARTParser:: processFindAllCL ()
{
int messages_num = messages_info.size();
//int save_excess = 0;

int mb_begin=0;

int header_check_result = -1;
int length_check_result = 0;

//int stop_search = 0;

vector <unsigned char> temp_mes;

int mes_cur_begin=0;
int mes_cur_end=0;
	
	temp_mes.resize (max_len);

	#ifdef DEBUG
		printf ("\n\n !!!!!!!!!!!!!!! NEW SEARCH !!!!!!!!!!!!!\n\n");
		
		printf ("MAIN BUFFER :: ");
		for ( int i = 0; i < mb_ipointer; i++   )
		{
			printf ("%d ", (*main_buff)[i] );
		}
		printf ("\n");
	#endif


	//for (int mes_count =0; mes_count < messages_num; mes_count++)
	//{
		//for ( int mb_cur_byte = mb_begin; mb_cur_byte < mb_ipointer; mb_cur_byte++   )
		for ( int mb_cur_byte = 0; mb_cur_byte < mb_ipointer; mb_cur_byte++   )
		{
			//stop_search = 0;	

		for (int mes_count =0; mes_count < messages_num; mes_count++)
		{
			if ( ! (fl_search_mask[mes_count]) ) continue;
			
			DEBUG_IF printf ("%d ", (*main_buff)[mb_cur_byte]   );

			if (  (*main_buff)[mb_cur_byte] == messages_info[mes_count][0]->first   )
			{
				header_check_result = headerCheck ( mes_count, mb_cur_byte );
				DEBUG_IF printf ("! Head_check=%d \n",header_check_result  );
				switch (header_check_result)
				{
				//Wrong header
				case -1:
					break;
				//Can't finish header check because of the end of the main_buffer
				case 0:
					//save_excess = 1;
					//stop_search = 1;
					break;

				//Header is ok
				case 1:
					length_check_result = lengthCheckCL ( mes_count,  mb_cur_byte  );									
					DEBUG_IF printf ("!! Length check res=%d\n", length_check_result);					
	
					if (length_check_result == 0)
					{
						//save_excess=1;
						//stop_search =1;
	
					}
					else
					{
						//Validation
						if ( messageValidation (mes_count) )
						{
							mb_begin = mb_cur_byte +  mes_len[mes_count] - (messages_info[mes_count][0]->second  + 1)  + 1 ;//Begin with the next pointer after the last symbol of the message
						
							//printf ( "\n---%d= %d+%d-%d ---\n" , mb_begin  ,mb_cur_byte ,mes_len[mes_count]  , messages_info[mes_count][0]->second  );
									
							messages.push (temp_mes);
							
							if ( !(messages.back().empty()) )
							{
								messages.back().clear();
							}

							mes_cur_begin = mb_cur_byte - messages_info[mes_count][0]->second  ;
							mes_cur_end = mes_cur_begin + mes_len[mes_count] -1 ;
							mb_cur_byte = mb_begin - 1;//Cause it will be increased by 1
							DEBUG_IF printf ("\n--- A message found first byte corrected = %d new_cur_byte=%d  ", mb_begin, mb_cur_byte );	
							for ( int ind=mes_cur_begin; ind <= mes_cur_end; ind++  )
							{
								messages.back().push_back( (*main_buff)[ind]  );
								if (erase_found_messages)  (*main_buff)[ind] = 0; 		
							}
							if ( queue_limit [mes_count] > 0  )
							{
								(messages_queues[mes_count])->push ( messages.back() );
								checkQueueLimit (mes_count);		
							}
							
							checkQueueLimit();
		
							mes_counter [mes_count] ++;						

		
						}											

	
					}
					break;

				}	
			


			}
			
			//if (stop_search)
			//{
			//	break;
			//}


		}
		}
		DEBUG_IF printf ("\n");

	//}	

	//Save excess
	int excess_begin = mb_begin;
	if ( (max_len + 1)  <  (mb_ipointer - mb_begin)   )
	{
		excess_begin = mb_begin + ( (mb_ipointer - mb_begin )  - ( max_len + 1) );
	}

	int temp_ind=0;
	for (int exc_ind = excess_begin; exc_ind < mb_ipointer ; exc_ind++  )
	{

		(*main_buff) [temp_ind] =  (*main_buff) [exc_ind];
		temp_ind++;								
	}
	DEBUG_IF printf ("MB IPOINTER = %d", temp_ind);
	mb_ipointer = temp_ind;
	DEBUG_IF printf ("\n\n\n");
	
}


void ARTParser:: processFindAllVL ()
{




}




////////////////// LAST MESSSAGES SEARCH !!!!!!!!!!!!!!!!!//////////////////////////

void ARTParser:: processFindLastCL ()
{
int messages_num = messages_info.size();
//int save_excess = 0;

int mb_begin=0;

int header_check_result = -1;
int length_check_result = 0;

int stop_search = 0;

vector <unsigned char> temp_mes;

int mes_cur_begin=0;
int mes_cur_end=0;


int excess_begin = 0;
	
	temp_mes.resize (max_len);

	#ifdef DEBUG
		//printf ("\n\n !!!!!!!!!!!!!!! NEW SEARCH !!!!!!!!!!!!!\n\n");
		
		printf ("MAIN BUFFER :: ");
		for ( int i = 0; i < mb_ipointer; i++   )
		{
			printf ("%d ", (*main_buff)[i] );
		}
		printf ("\n");
	#endif

	for (int ind=0; ind < fl_search_mask.size(); ind++ )
	{
		fl_search_result[ind] = 0;
	}
	

	stop_search = 0;	
	for ( int mb_cur_byte = mb_ipointer -1; mb_cur_byte >= 0; mb_cur_byte--   )
	{

		for (int mes_count =0; mes_count < messages_num; mes_count++)
		{
			if (  ! (fl_search_mask[ mes_count ])  ||  fl_search_result[ mes_count ]  ) continue;			
	
			DEBUG_IF printf ("%d ", (*main_buff)[mb_cur_byte]   );

			if (  (*main_buff)[mb_cur_byte] == messages_info[mes_count][  symbols_num[mes_count] -1  ]->first   )
			{
				header_check_result = headerCheckFLCL ( mes_count, mb_cur_byte );
				DEBUG_IF printf ("! Head_check=%d \n",header_check_result  );
				switch (header_check_result)
				{
				//Wrong header
				case -1:
					break;
				//Can't finish header check because of the end of the main_buffer
				case 0:
					break;

				//Header is ok
				case 1:
					length_check_result = lengthCheckFLCL ( mes_count,  mb_cur_byte  );									
					DEBUG_IF printf ("!! Length check res=%d\n", length_check_result);					
	
					if (length_check_result == 0)
					{
	
					}
					else
					{
						//Validation
						if ( messageValidation (mes_count) )
						{
							mb_begin = mb_cur_byte -  messages_info[mes_count][ symbols_num[mes_count] -1 ]->second -1 ;//Begin with the next pointer after the last symbol of the message
							if (mb_begin < 0) mb_begin = 0;	
							//printf ( "\n---%d= %d+%d-%d ---\n" , mb_begin  ,mb_cur_byte ,mes_len[mes_count]  , messages_info[mes_count][0]->second  );
								
							int new_excess_begin = mb_cur_byte  + mes_len[mes_count]  -  messages_info[mes_count][ symbols_num[mes_count] -1 ]->second ; 
							if (new_excess_begin >= mb_ipointer) excess_begin = mb_ipointer - 1;								

							if (excess_begin < new_excess_begin) excess_begin = new_excess_begin;

							fl_search_result [mes_count] = 1;

							
	
							messages.push (temp_mes);
							
							if ( !(messages.back().empty()) )
							{
								messages.back().clear();
							}

							mes_cur_begin = mb_cur_byte - messages_info[mes_count][ symbols_num[mes_count] -1   ]->second  ;
							mes_cur_end = mes_cur_begin + mes_len[mes_count] -1 ;
						
							mb_cur_byte = mb_begin + 1;//Cause it will be increased by 1
						
							DEBUG_IF printf ("\n--- A message found first byte corrected = %d new_cur_byte=%d  ", mb_begin, mb_cur_byte );	
							for ( int ind=mes_cur_begin; ind <= mes_cur_end; ind++  )
							{
								messages.back().push_back( (*main_buff)[ind]  );
								if (erase_found_messages)  (*main_buff)[ind] = 0; 		
							}
							if ( queue_limit [mes_count] > 0  )
							{
								(messages_queues[mes_count])->push ( messages.back() );
								checkQueueLimit (mes_count);		
							}
							
							checkQueueLimit();
		
							mes_counter [mes_count] ++;						

							stop_search =1;
							for (int ind=0; ind < fl_search_mask.size(); ind++ )
							{
								if (fl_search_mask[ind] != fl_search_result[ind] )
								{
									stop_search = 0;
								}
							}
		
						}											

	
					}
					break;

				}	
			


			}
	
			if (stop_search)
			{
				break;
			}
	
		

		}

		if (stop_search)
		{
			DEBUG_IF printf ("\n!!!! Search stopped\n");
			break;
		}

	}
	DEBUG_IF printf ("\n");


	//Save excess
	if (excess_begin >= mb_ipointer) excess_begin = mb_ipointer - 1;
	if ( ( mb_ipointer - excess_begin ) > max_len ) excess_begin = mb_ipointer - max_len - 1;
	if (excess_begin < 0) excess_begin = 0;


	int temp_ind=0;
	for (int exc_ind = excess_begin; exc_ind < mb_ipointer ; exc_ind++  )
	{

		(*main_buff) [temp_ind] =  (*main_buff) [exc_ind];
		temp_ind++;								
	}
	DEBUG_IF printf ("MB IPOINTER = %d", temp_ind);
	mb_ipointer = temp_ind;
	DEBUG_IF printf ("\n\n\n");
	
}




void ARTParser:: processFindLastVL ()
{




}


