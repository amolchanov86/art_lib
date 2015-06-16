
#ifndef COM_LIBS_H
#define COM_LIBS_H

//-----------------------------------------ART COM LIBRARIES VER 2.1 ----------------------------------------------
#include <iostream>
#include <cstdio>

#include <cstring>

#include <string>
#include <sstream>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <cstdlib>
#include <queue>

#include "art_misctools.h"

using std::queue;
using std::printf;
using std::malloc;
using std::free;
using std::strcat;
using std::string;
using std::stringstream;
using std::cerr;
using std::endl;


//Open serial port, where 'com_num' - number of the serial port to open


int open_com(int input_com_num = 1, bool verbose = true);
int open_com_8N1(int com_num = 1, long int  baud_rate = 115200  ,bool verbose=true); 
int open_ser_8N1(const char * device, long int  baud_rate = 115200  ,bool verbose=true);

//Return: file descriptor
//char * slave_device - here the function provides the name of the slave device that you should use (the function assigns the name itself; 
//you should just provide a char array with length "name_len")
//After acquiring the name of the slave device, you can use it as a casual serial port
//IMPORTANT: master_device is necessary for QNX only. In Linux you can set NULL pointer for the master
int open_pseudo_com_8N1(char * master_device, char * slave_device, int name_len,  long int baud_rate ,bool verbose = true);

//For just opening some device use open (char * dev, int mode) function, where mode = 2 for serial devices 

int open_com_old(unsigned int com_num);//An old version (may not work)

//Configure the opened serial port, 'fd' - file descriptor of the opened serial port, 'baud_rate' - the speed you need
long int com_conf(int fd, long int baud_rate);


//
int readall(int fd, unsigned char* buf);
//
int read_nlast(int fd, unsigned char* buf, unsigned int nlast);

//The class isn't finished yet
class ArtSerPort
{
public:
	ArtSerPort(long int baud=19200, unsigned int com_num=1 );
	~ArtSerPort();

	//Sending data
	//Return:a number of bytes really written
	long int WriteBlock(unsigned char out_data, long int data_len);

	//Set baud rate
	//Return: the baud rate set to the port or error code
	//Error codes:
	long int SetBaudRate(long int baud);

	//Clear all buffers
	//Return: 0 -if all is ok;
	int InClear();

	//The length of the data in the serial port
	//Return:a number of bytes read from the port
	long int ReadBlock (unsigned int in_data, long int data_len);

protected:
	
	//File descriptor for our port
	int fd;
	int serial_num;
	long int baud_rate;
	queue<unsigned char> internal_buffer;

};

#endif
