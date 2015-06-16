#ifndef UDP_LIB_H
#define UDP_LIB_H


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
//#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/select.h>
//#include <netinet/tcp.h>

//#include <netdb.h>
//#include <unix.h>


struct udp_init_struct
{
char  in_addr[20];
short int in_port;
long int in_buf_size;


char out_addr[20];
short int out_port;
long int out_buf_size;


};



unsigned char udp_init_connect ( int * sock_in, int* sock_out,  struct udp_init_struct *  udp_init);

unsigned char udp_init ( int * sock_in, int* sock_out,  struct udp_init_struct *  udp_init, struct sockaddr_in * sock_out_addr );










#endif
