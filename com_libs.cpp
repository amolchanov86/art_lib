//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <termios.h>
#include "com_libs.h"

//Return:
//>0 - the device has been opened and configured successfully
//-1 - an error while opening the device
//-2 - an error while configuring the device 
//-3 - no device has been specified
//Other - some error :) 
int open_ser_8N1(const char * device, long int baud_rate ,bool verbose)
{
//--------VARS:
int fd = -1; // File descriptor for a serial port
int open_mode=2; //Mode for opening a serial port


	if (device == 0)
	{
		if (verbose) cerr<<"ERROR !!!: COM_LIB: a null pointer for the device name !\n";
return -3; 	
	}

	fd=open(device, open_mode);

	if ( fd < 0)
	{
		if (verbose) cerr<<"ERROR !!!: COM_LIB:  Can't open the device: "<<device<<endl;
	}
	else
	{
	//It's some sort of com port configuration 	
		fcntl(fd,F_SETFL,0);
		if ( verbose ) cerr<<"COM_LIB: The device "<<device<<" has been opened."<<endl;
		
		int conf_res = com_conf( fd, baud_rate );
		
		if ( conf_res >= 0) 
		{
			if (verbose ) cerr<<"COM_LIB: The device "<<device<<" has been successfully configured for 8N1 mode."<<endl;
		}
		else 
		{
			if (verbose) cerr<<"ERROR !!!: COM_LIB: A problem has occured while configuring the device "<<device<<" "<<endl;
		}
	}

return (fd);
}

//Return:
//>0 - the com_num has been opened and configured successfully
//-1 - an error while opening the com_num
//-2 - an error while configuring the com_num 
//Other - some error :) 
int open_com_8N1(int com_num, long int baud_rate ,bool verbose)
{
//--------VARS:
int fd = -1; // File descriptor for a serial port

	fd=open_com( com_num, verbose );

	if ( fd < 0)
	{
		if (verbose) cerr<<"ERROR !!!: COM_LIB:  Can't open the port "<<com_num<<endl;
	}
	else
	{
	//It's some sort of com port configuration 	
		fcntl(fd,F_SETFL,0);
		if ( verbose ) cerr<<"COM_LIB: The port "<<com_num<<" has been opened."<<endl;
		
		int conf_res = com_conf( fd, baud_rate );
		
		if ( conf_res > 0) 
		{
			if (verbose ) cerr<<"COM_LIB: The port "<<com_num<<" has been successfully configured for 8N1 mode."<<endl;
		}
		else 
		{
			fd = -2;
			if (verbose) cerr<<"ERROR !!!: COM_LIB: A problem has occured while configuring the port "<<com_num<<" "<<endl;
		}
	}

return (fd);
}





int open_com (int input_com_num, bool verbose)
{
//--------VARS:
string com_num(""); // number for a serial port in string variable
int fd = -1; // File descriptor for a serial port
//!!!!!!!! QNX
#ifdef __QNX__
string com_path ("/dev/ser") ;//Base serial port path for QNX
//!!! END QNX
#else
//!!!!!!!! Linux
string com_path=("/dev/ttyS");//Base serial port path for Linux
#endif
//!!! END Linux

int open_mode=2; //Mode for opening a serial port
int wrong_com_num=0; //This variable(flag) indicates that the function got a wrong type of serial port number
int i=0,j=0;//Variables for diffrent cycles

stringstream ss;

//vars for number conversion

	//Checking the input serial port number for validity
	if (input_com_num<=0 || input_com_num >=100)
	{
		wrong_com_num=1;
		fd=-2;
	}
	else
	{
//!!!!!!!! Linux
#ifndef __QNX__
		input_com_num--;	
#endif
//!!! END Linux
	}

	if (!wrong_com_num)
	{
		ss<<input_com_num;			
		ss>>com_num;

		com_path+=com_num;

		//Opening the serial port
		fd=open(com_path.c_str(), open_mode);
	}

	if ( fd < 0)
	{
		if (verbose) cerr<<"ERROR !!!: COM_LIB:  Can't open the device: "<<com_path<<endl;
	}
	else
	{
	//It's some sort of com port configuration 	
		fcntl(fd,F_SETFL,0);
		if ( verbose ) cerr<<"COM_LIB: The device "<<com_path<<" has been opened."<<endl;
	}

return (fd);
}



int open_pseudo_com_8N1(char * master_device, char * slave_device, int slave_dev_name_len ,long int baud_rate ,bool verbose)
{
int fd = -1;
#ifndef __QNX__ //---- Linux

	fd = posix_openpt(O_RDWR);//Must be >=0

	if ( (fd < 0) )
	{
		if (verbose) cerr<<"ERROR !!! :: PSEUDO_COM ::  Can't open the Master Pseudo Terminal"<<endl;
	}
	else
	{
		//It's some sort of com port configuration 	
		fcntl(fd,F_SETFL,0);
		
		if ( grantpt(fd) != 0 )
		{
			if(verbose) cerr<<"ERROR !!! :: PSEUDO_COM ::  Can't change a mode of the PseudoTerm. The descriptor will be closed "<<endl;
			close (fd);
return -3;
		}
		
		if ( unlockpt(fd) )
		{
			if(verbose) cerr<<"ERROR !!! :: PSEUDO_COM ::  Can't unlock the PseudoTerm. The descriptor will be closed "<<endl;
			close (fd);
return -4;			
		}

		//Configure the opened port
		if ( com_conf( fd, baud_rate ) >= 0  )
		{
			if (verbose) cerr<<"PSEUDO_COM :: The Master device has been successfully opened and configured for 8N1 mode."<<endl;
		}
		else
		{
			if (verbose) cerr<<"ERROR !!! :: PSEUDO_COM ::  Can't configure the PseudoTerm. The file descriptor will be closed."<<endl;
			close (fd);
return -5;
		}

		if ( (slave_device == 0) || (slave_dev_name_len <= 0) )
		{
			if (verbose) cerr<<"WARNING !!! :: PSEUDO_COM ::  Buffer for the slave dev name isn't correct ! Acquire a slave name manually with the ptsname() function !"<<endl;
		}
		else
		{
			ptsname_r(fd, slave_device, slave_dev_name_len);	
		}
		//fprintf (stderr, "Slave=%s\n", slave_device);
	}
#else //---- QNX
	if (master_device == 0)
	{
		if(verbose) cerr<<"ERROR !!! :: PSEUDO_COM ::  No master dev name provided (null pointer)!"<<endl;
return -6;
	}

	fd = open_ser_8N1( master_device, baud_rate, verbose );
	
	if ( (fd < 0) )
	{
		if (verbose) cerr<<"ERROR !!! :: PSEUDO_COM ::  Can't open the Master Pseudo Terminal"<<endl;
	}
	else
	{ 
		if ( (slave_device == 0) || (slave_dev_name_len <= 0) )
		{
			if (verbose) cerr<<"WARNING !!! :: PSEUDO_COM ::  Buffer for the slave dev name isn't correct (a zero pointer or wrong length)! Choose a slave dev name manually !"<<endl;
		}
		else if ( strlen (master_device) > slave_dev_name_len )	
		{
			if (verbose) cerr<<"WARNING !!! :: PSEUDO_COM ::  Buffer for the slave dev name isn't correct (too short) ! Choose a slave dev name manually !"<<endl;
		}
		else 
		{
			strcpy (slave_device, master_device);
			char * position = strstr (slave_device, "pty");
			if (position)
			{
				position[0] = 't';
			}
			else
			{
				position = strstr (slave_device, "tty");
				if (position)
				{
					position[0] = 'p';
				}
				else
				{
					if (verbose) cerr<<"WARNING !!! :: PSEUDO_COM :: Can't identify a type of the slave device! Choose a slave dev name manually !"<<endl;
					strcpy (slave_device, "");
				}
			}
		}
	}


#endif

return fd;
}


int open_com_old(unsigned int input_com_num)
{

//--------VARS:
char com_num[3]; // number for a serial port in string variable
int fd; // File descriptor for a serial port
//!!!!!!!! QNX
#ifdef __QNX__
char com_path[11]="/dev/ser";//Base serial port path for QNX
//!!! END QNX
#else
//!!!!!!!! Linux
char com_path[11]="/dev/ttyS";//Base serial port path for Linux
#endif
//!!! END Linux

char error_message[40]="Open_com: Unable to open ";
int open_mode=2; //Mode for opening a serial port
int wrong_com_num=0; //This variable(flag) indicates that the function got a wrong type of serial port number
int com_num_array[2]; //Here i save high and low decades
int i=0,j=0;//Variables for diffrent cycles

//vars for number conversion

	//Checking the input serial port number for validity
	if (input_com_num<=0 || input_com_num >=100)
	{
		wrong_com_num=1;
		fd=-2;
	}
	else
	{
//!!!!!!!! Linux
#ifndef __QNX__
		input_com_num--;	
#endif
//!!! END Linux
	}

	if (!wrong_com_num)
	{
	//Start conversion the serial port number from integer to string 
		com_num_array[0]=input_com_num / 10;
		com_num_array[1]=input_com_num % 10;
		
		for (i=0; i<2; i++)
		{
			switch (com_num_array[i])
			{
				case 0:
					if (i==1) 
					{
						com_num[j]='0';
						j++;
					}
					break;
				case 1:
					com_num[j]='1';
					j++;
					break;
				case 2:
					com_num[j]='2';
					j++;

					break;
				case 3:
					com_num[j]='3';
					j++;

					break;
				case 4:
					com_num[j]='4';
					j++;

					break;
				case 5:
					com_num[j]='5';
					j++;

					break;
				case 6:
					com_num[j]='6';
					j++;

					break;
				case 7:
					com_num[j]='7';
					j++;

					break;
				case 8:
					com_num[j]='8';
					j++;

					break;
				case 9:
					com_num[j]='9';
					j++;

					break;

			}

		}
		com_num[j]='\0';//Everi C string must be ended with '\0' symbol
		//End conversion the serial port number from int to C string

		strcat(com_path,com_num);// Create a real path to the serial port
		
		//Opening the serial port
		fd=open(com_path,open_mode);
		if (fd == -1)
		{	
		//I don't think this error messages are necessary
	/*		strcat(error_message,com_path);
			strcat(error_message," - ");
			perror (error_message);
	*/	}
		else
		//It's some sort of com port configuration 	
			fcntl(fd,F_SETFL,0);
	}
	
	return (fd);
}






//Configer for serial port, where fd - file descriptor ,
//Return: baud_rate set or error code
long int com_conf(int fd, long int baud_rate )
{
struct termios com_mode;
int result=0; //Flag to return. If result >= 0 then everything is ok, else - something wrong:
// result == -1 - can't correct and set the inputed baud rate.Baud rate has been set by default	(19200).		


/*Initialisation*/

com_mode.c_lflag=0;
com_mode.c_cflag=0;
com_mode.c_iflag=0;
com_mode.c_oflag=0;
		/*CONTROL OPTIONS*/
/*Speed configuiration*/
/*tcgetattr - helps to preserv options that you don't want to change*/
/*	tcgetattr(fd,&com_mode); */

//Correction for baud_rate
	if (baud_rate>0 && baud_rate <= 69) baud_rate=50;
	if (baud_rate>69 && baud_rate <= 99) baud_rate=75;
	if (baud_rate>99 && baud_rate <= 120) baud_rate=110;
	if (baud_rate>120 && baud_rate <= 139) baud_rate=134;
	if (baud_rate>139 && baud_rate <= 179) baud_rate=150;
	if (baud_rate>179 && baud_rate <= 269) baud_rate=200;
	if (baud_rate>269 && baud_rate <= 539) baud_rate=300;
	if (baud_rate>539 && baud_rate <= 999) baud_rate=600;
	if (baud_rate>999 && baud_rate <= 1599) baud_rate=1200;
	if (baud_rate>1599 && baud_rate <= 1999) baud_rate=1800;
	if (baud_rate>1999 && baud_rate <= 3999) baud_rate=2400;
	if (baud_rate>3999 && baud_rate <= 7999) baud_rate=4800;
	if (baud_rate>7999 && baud_rate <= 14999) baud_rate=9600;
	if (baud_rate>14999 && baud_rate <= 29999) baud_rate=19200;
	if (baud_rate>29999 && baud_rate <= 49999) baud_rate=38400;
	if (baud_rate>49999 && baud_rate <= 69999) baud_rate=57600;
//	if (baud_rate>69999 && baud_rate <= 99999) baud_rate=76800;
	if (baud_rate>69999) baud_rate=115200;

//Setup the speed
	switch (baud_rate)
	{
	case 0:
		cfsetispeed(&com_mode,B0);
		cfsetospeed(&com_mode,B0);
		break;
	case 50:
		cfsetispeed(&com_mode,B50);
		cfsetospeed(&com_mode,B50);
		break;
	case 75:
		cfsetispeed(&com_mode,B75);
		cfsetospeed(&com_mode,B75);
		break;
	case 110:
		cfsetispeed(&com_mode,B110);
		cfsetospeed(&com_mode,B110);
		break;
	case 134:
		cfsetispeed(&com_mode,B134);
		cfsetospeed(&com_mode,B134);
		break;

	case 150:
		cfsetispeed(&com_mode,B150);
		cfsetospeed(&com_mode,B150);
		break;
	case 200:
		cfsetispeed(&com_mode,B200);
		cfsetospeed(&com_mode,B200);
		break;
	case 300:
		cfsetispeed(&com_mode,B300);
		cfsetospeed(&com_mode,B300);
		break;
	case 600:
		cfsetispeed(&com_mode,B600);
		cfsetospeed(&com_mode,B600);
		break;
	case 1200:
		cfsetispeed(&com_mode,B1200);
		cfsetospeed(&com_mode,B1200);
		break;
	case 1800:
		cfsetispeed(&com_mode,B1800);
		cfsetospeed(&com_mode,B1800);
		break;
	case 2400:
		cfsetispeed(&com_mode,B2400);
		cfsetospeed(&com_mode,B2400);
		break;
	case 4800:
		cfsetispeed(&com_mode,B4800);
		cfsetospeed(&com_mode,B4800);
		break;
	case 9600:
		cfsetispeed(&com_mode,B9600);
		cfsetospeed(&com_mode,B9600);
		break;
	case 19200:
		cfsetispeed(&com_mode,B19200);
		cfsetospeed(&com_mode,B19200);
		break;
	case 38400:
		cfsetispeed(&com_mode,B38400);
		cfsetospeed(&com_mode,B38400);
		break;
	case 57600:
		cfsetispeed(&com_mode,B57600);
		cfsetospeed(&com_mode,B57600);
		break;
/*	case 76800:
		cfsetispeed(&com_mode,B76800);
		cfsetospeed(&com_mode,B76800);
		break;
*/
	case 115200:
		cfsetispeed(&com_mode,B115200);
		cfsetospeed(&com_mode,B115200);
		break;
	default:
		result=-1;//Can't find the baud_rate
		cfsetispeed(&com_mode,B19200);
		cfsetospeed(&com_mode,B19200);
		break;

	}	
	
	com_mode.c_cflag |= (CLOCAL | CREAD);


/*Configure data byte size*/
	com_mode.c_cflag &= ~CSIZE;
	com_mode.c_cflag |= CS8;

/*Configure parity bit*/
	com_mode.c_cflag &= ~PARENB;

/*Configure lenght of the stop bit*/
	com_mode.c_cflag &= ~CSTOPB;

/*Configure of the RTS/CTS data flow control*/
/*Note it's commented as it's possible that Linux doesn't suuport these functions*/
/*
	com_mode.c_cflag &= ~CNEW_RTSCTS;

 */

/*Also possible*/
/*
 	EXTA - External rate clock (EXTB is the same by mean)
	PARODD - Use odd parity instead of even
	HUPCL - Hangup(drop DTR) on last close
	LOBLK - Block job control output
 
 */
/*
	com_mode.c_cflag &= ~EXTA;
	com_mode.c_cflag &= ~EXTB;
	com_mode.c_cflag &= ~PARODD;
	com_mode.c_cflag &= ~HUPCL;
	com_mode.c_cflag &= ~LOBLK;
 	

 */

		/*LOCAL OPTIONS*/

/*Non canonical input*/
	com_mode.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);	
/*Canonical input*/
/*	com_mode.c_lflag &= ~(ICANON | ECHO | ECHOE);	*/

/*Also possible*/
/*
 	ISIG - Allow SIGINTR,SIGSUSP,SIGDSUSP,SIGQUIT signals
	ICANON - Allow canonical input
	ECHO - Allow echo of inputed simbols
	ECHOE - Echo erase simbol (like BS-SP-BS)
	ECHOK - Echo NL after KILL simbol
	ECHONL - Echo NL
	NOFLSH - Block flush of the receiver's buffer after interrupt simbol or exit
	IEXTEN - Allow extended options
	ECHOCTL - Echo of control simbols 
	ECHOPRT - Echo erased character as character erased
	ECHOKE - BS-SP-BS entire line on line kill
	FLUSHO - Output being flushed
	PENDIN - Retype pending input at next read or input char
	TOSTOP - Receiving SIGTTOU for background output	
 */


		/*INPUT OPTIONS*/
/*Disable parity check*/
	com_mode.c_iflag &= ~(INPCK | PARMRK );
	com_mode.c_iflag |= IGNPAR;
/*Disable CR to NL convert*/
	com_mode.c_iflag &= ~(ICRNL);
/*Enable CR to NL convert*/
/*	com_mode.c_iflag |= ICRNL;	*/



/*Enable parity check*/
/*
 	com_mode.c_iflag |= INPCK
 	com_mode.c_iflag |= ISTRIP 
	com_mode.c_iflag &= ~PARMRK;
	com_mode.c_iflag &= ~IGNPAR;
*/
/*!!!! Strip parity bit - enable this option ONLY then you eneble patity check - to strip parity bit after checking otherwise (if parity checking is disabled) it would strip MSB in every transmitted byte, so you wouldn't able to transmit numbers > 128 */
/*	com_mode.c_iflag |= ISTRIP; */

/*Software flow control */
/*Disable software flow control*/
	com_mode.c_iflag &= ~(IXON | IXOFF | IXANY);

/*Enable software flow control*/
/*	com_mode.c_iflag |= (IXON | IXOFF | IXANY);*/

/*Also possible*/
/*
	INPCK - Enable parity check
	IGNPAR - Ignore parity errors
	PARMRK - Mark parity errors(if IGNPAR  then we receive NUL=(000) else we receive DEL=(177) and NUL before every byte with error   )
	ISTRIP - Strip parity bits
	IXON - Enable software flow control(outgoing)
	IXOFF -Enable software flow control (incoming)
	IXANY - Allow any character to start flow again
	IGNBRK - Ignore break condition
	BRKINT - Send a SIGINT when a break condition is detected 
	INLCR - Map NL to CR
	IGNCR - Ignore CR
	ICRNL - Map CR to NL
	IUCLC - Map uppercase to lowercase
	IMAXBEL - Echo BEL on input line too long 
 */
		/*OUTPUT OPTIONS*/
/*Non canonical output*/
	com_mode.c_oflag &= ~OPOST;
/*Canonical output*/
/*	com_mode.c_oflag |= OPOST; */

/* Then ~OPOST all these options are ignored: 
	OPOST - Postprocess output (not set = raw otput)
	OLCUC - Map lowercase to uppercase
	ONLCR - Map NL to CR-NL
	OCRNL - Map CR to NL
	NOCR - No CR output at column 0
	ONLRET - NL performs CR function
	OFILL - Use fill characters for delay
	OFDEL - Fill character is DEL
	NLDLY - Mask for delay time needed between lines
		NL0 - no delay for NLs
		NL1 - delay futher output after newline for 100 milliseconds
	CRDLY - Mask for delay time needed to return carriage to left column
		CR0 - no delay for CRs
		CR1 - delay after CRs depending on current column position
		CR2 - Delay 100 milliseconds after sending CRs
		CR3 - Delay 150 milliseconds after sending CRs
	TABDLY - Mask for delay time needed after TABs
		TAB1 - delay after TABs depending on current column position
		TAB2 - delay 100 milliseconds after sending TABs
		TAB3 - Expand TAB characters to spaces
	BSDLY - Mask for delay time needed after BSs 
		BS0 - no delay for BSs
		BS1 - delay 50 milliseconds after BSs
	VTDLY - Mask for delay time needed after VTs 
		VT0 - no delay for VTs
		VT1 - delay 2 second after sending VTs
	FFDLY - Mask for delay time needed after FFs
		FF0 - No delay for FFs
		FF1 - Delay 2 seconds after sending FFs
 
 */
		
		/*CONTROL CHARACTERS*/
/*Here they are:
	Constant  *  Description          * Keys combination		
***************************************************************	
	VINT         Interrupt             ctrl+C   
	VQUIT        Quit                  ctrl+Z
   	VERASE       Erase                 Backspace
	VKILL        Kill-line             ctrl+U
	VEOF         End-of-file           ctrl+D
	VEOL         End-of-line           Carriage return
	VEOL2        Second EOL            Line feed (LF)
	VMIN         Minimum number of characters to read
	VTIME        Time to wait for data (tenths of seconds)

	VSTART       XON (Software flow control) 
	VSTOP	     XOFF (Software flow control)
		   
*/
/*Configure num of data and the time to wait*/
	
/*	com_mode.c_cc[VMIN]=14;
	com_mode.c_cc[VTIME]=100;*/
	com_mode.c_cc[VMIN]=0;
	com_mode.c_cc[VTIME]=0;
/*Saving configured options */
	
	tcflush(fd,TCIFLUSH);

	tcsetattr (fd,TCSANOW, &com_mode);

 return result;
}

int readall(int fd, unsigned char* buf)
{

/*CONSTANTS*/
const int print_messages=1;

/*VARS*/

int all_read_flag=0;
int read_res=-1;
unsigned int buf_length=0;
unsigned int bytes_to_read=500;
unsigned char temp_buf[500];

/*PROGRAM IS BEGINING*/
if (print_messages) printf("\n --Function readall--\n ");

temp_buf[0]='\0';

	while (!all_read_flag)
	{
		fcntl(fd,F_SETFL,0);
		read_res=read(fd,temp_buf,bytes_to_read);
/*Print num of read bytes*/
		if (print_messages) printf("\n	Readall: bytes_read=%i\n",read_res);

		if (read_res<500) all_read_flag=1;
		if (read_res>0)
		{
			buf_length=uchar_cpy(buf,temp_buf,buf_length,read_res);  	
		}
	}
	if (print_messages) printf("\n --END Func Readall--\n");
return buf_length;
}

int read_nlast(int fd, unsigned char* buf, unsigned int nlast)
{

const int print_messages=0;

int all_read_flag=0;
int read_res=-1;
unsigned int last_buf_length=0;
unsigned int cur_buf_length=0;
unsigned int min_bytes_to_read=500;
unsigned int bytes_to_read=min_bytes_to_read;
unsigned char *current_buf;
unsigned char *last_buf;
int excess=0;
unsigned int byte_from_lbuf=0;
unsigned int byte_from_cbuf=0;
int bytes_read_from_com=0;

	if (print_messages) printf(" --Function read_nlast--");
	if (nlast>min_bytes_to_read) bytes_to_read=nlast;
        current_buf=(unsigned char*)malloc(bytes_to_read*sizeof(unsigned char));	
        last_buf=(unsigned char*)malloc(nlast*sizeof(unsigned char));	
	while (!all_read_flag)
	{	
		last_buf_length=uchar_cpy_tail(last_buf,current_buf,0,cur_buf_length,nlast);
		fcntl(fd,F_SETFL,0);
		cur_buf_length=read(fd,current_buf,bytes_to_read);
		if(print_messages) printf("\n	Read_nlast: bytes_read=%i\n",cur_buf_length);
		if (cur_buf_length<bytes_to_read) all_read_flag=1;
	
	}
	
	excess=cur_buf_length-nlast;
	if (excess>=0) 
	{
		byte_from_lbuf=0;
		byte_from_cbuf=nlast;
	}
	if (excess<0) 
	{
		byte_from_lbuf=nlast-cur_buf_length;
		byte_from_cbuf=cur_buf_length;
	}	
	
	if (print_messages)
	{
		printf("	Read_nlast: cbuf_len=%u lbuf_len=%u get_cbuf=%u, get_lbuf=%u\n",cur_buf_length,last_buf_length,byte_from_cbuf,byte_from_lbuf);
	}
		bytes_read_from_com=uchar_cpy_tail(buf,last_buf,0,last_buf_length,byte_from_lbuf);
/*		printf("	Read_nlast: first res=%u\n",bytes_read_from_com);
*/		
		bytes_read_from_com=uchar_cpy_tail(buf,current_buf,bytes_read_from_com,cur_buf_length,byte_from_cbuf);
/*		printf("	Read_nlast: second res=%u\n",bytes_read_from_com);
 */		
	
	if(print_messages) printf("\n --END Read_nlast Function\n");
	free(current_buf);
	free(last_buf);
return bytes_read_from_com;
}

/*
//---------------------class SerPort implementation

SerPort::SerPort(long int baud, unsigned int com_num)
{

	fd=open_com (com_num);
	if (fd>0)
	{
		serial_num=com_num;	
		baud_rate=com_conf(fd,baud);
		
	}
	else
	{
		serial_num=(-1)*com_num; // serial_num <0 means an error occured
		baud_rate=(-1)*baud;// baud<0 means an error occured
	}

}

SerPort::~SerPort()
{
	if (fd>0) close (fd);

	fd=-1;
	com_num=-1;
	baud_rate=-1;
}



//---------------------END SerPort implementation
*/
