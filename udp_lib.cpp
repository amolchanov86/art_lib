#include "udp_lib.h"




//struct sockaddr_in sock_in_addr, sock_out_addr;

//unsigned char udp_init ( int * sock_in, int* sock_out,  char * in_addr, int in_port, char * out_addr, int out_port  );
unsigned char udp_init_connect ( int * sock_in, int* sock_out,  struct udp_init_struct *  udp_init)
{
unsigned char error=0;
struct sockaddr_in sock_in_addr, sock_out_addr;

int sock_out_opt=0;
//int sock_in_opt=1;


	fprintf (stderr, "\nUDP LIB: %s %s\n", udp_init->in_addr, udp_init->out_addr);


	if (sock_in == 0  || sock_out == 0 || udp_init ==0  )
	{
		perror ("\nUDP: ERROR: Null pointer for some input parameter !!!\n");
	
	}
	

	
	*sock_in=socket (PF_INET,SOCK_DGRAM, 0);
	*sock_out=socket (PF_INET,SOCK_DGRAM, 0);

	if (*sock_in >0)
	{
		sock_in_addr.sin_family=PF_INET;
		sock_in_addr.sin_port=htons (udp_init->in_port);
	//	sock_in_addr.sin_port=htons (2000);
		sock_in_addr.sin_addr.s_addr= inet_addr (udp_init->in_addr)  ;

		sock_in_addr.sin_zero[0]='\0';	
		sock_in_addr.sin_zero[1]='\0';	
		sock_in_addr.sin_zero[2]='\0';	
		sock_in_addr.sin_zero[3]='\0';	
		sock_in_addr.sin_zero[4]='\0';	
		sock_in_addr.sin_zero[5]='\0';	
		sock_in_addr.sin_zero[6]='\0';	
		sock_in_addr.sin_zero[7]='\0';	
		//strcpy (sock_in_addr.sin_zero , "\0\0\0\0\0\0\0" );


	//	sock_in_addr.sin_addr.s_addr= inet_addr ("127.0.0.1")  ;
		//sock_in_addr.sin_addr.s_addr= inet_addr ("192.168.127.3")  ;
//		printf ("\nUDP_addr=%d\n",inet_addr (udp_init->in_addr));	
		
		
		if (    bind (*sock_in, (struct  sockaddr *) &sock_in_addr, sizeof (sock_in_addr)  )      < 0 )
		{
		
//			printf (" \n sock_in=%d Sock_addr_size_my=%d \n ", *sock_in, sizeof (sock_in_addr)   );	
			error |=4;

		}
		
		//sock_in_opt=1;	
		//setsockopt ( *sock_in, 6, TCP_NODELAY, (void*) &sock_in_opt, sizeof (sock_in_opt)   );	
			
		//sock_in_opt = udp_init->in_buf_size;	
		//setsockopt ( *sock_in, SOL_SOCKET, SO_RCVBUF, (void*) &sock_in_opt, sizeof (sock_in_opt)   );	
				

	}
	else
	{
		error |= 1;				

	}


	if (*sock_out >0)
	{
		sock_out_addr.sin_family=PF_INET;
		sock_out_addr.sin_port=htons (udp_init->out_port);
		sock_out_addr.sin_addr.s_addr= inet_addr (udp_init->out_addr)  ;
		sock_out_addr.sin_zero[0]='\0';	
		sock_out_addr.sin_zero[1]='\0';	
		sock_out_addr.sin_zero[2]='\0';	
		sock_out_addr.sin_zero[3]='\0';	
		sock_out_addr.sin_zero[4]='\0';	
		sock_out_addr.sin_zero[5]='\0';	
		sock_out_addr.sin_zero[6]='\0';	
		sock_out_addr.sin_zero[7]='\0';	


		sock_out_addr.sin_addr.s_addr= inet_addr (udp_init->out_addr)  ;
		
		if (    connect (*sock_out,(struct  sockaddr *)   &sock_out_addr, sizeof (sock_out_addr)  )      < 0 )
		{
			
			error |=8;

		}
		
		sock_out_opt = udp_init->out_buf_size;	
		setsockopt ( *sock_out, SOL_SOCKET, SO_SNDBUF, (void*) &sock_out_opt, sizeof (sock_out_opt)   );	

	}
	else
	{

		error |= 2;				

	}

	



return error;
}


unsigned char udp_init ( int * sock_in, int* sock_out,  struct udp_init_struct *  udp_init, struct sockaddr_in * sock_out_addr )
{
unsigned char error=0;
struct sockaddr_in sock_in_addr;

int sock_out_opt=0;
//int sock_in_opt=1;


	fprintf (stderr, "\nUDP LIB: %s %s\n", udp_init->in_addr, udp_init->out_addr);


	if (sock_in == 0  || sock_out == 0 || udp_init ==0  )
	{
		perror ("\nUDP: ERROR: Null pointer for some input parameter !!!\n");
	
	}
	

	
	*sock_in=socket (PF_INET,SOCK_DGRAM, 0);
	*sock_out=socket (PF_INET,SOCK_DGRAM, 0);

	if (*sock_in >0)
	{
		sock_in_addr.sin_family=PF_INET;
		sock_in_addr.sin_port=htons (udp_init->in_port);
	//	sock_in_addr.sin_port=htons (2000);
		sock_in_addr.sin_addr.s_addr= inet_addr (udp_init->in_addr)  ;

		sock_in_addr.sin_zero[0]='\0';	
		sock_in_addr.sin_zero[1]='\0';	
		sock_in_addr.sin_zero[2]='\0';	
		sock_in_addr.sin_zero[3]='\0';	
		sock_in_addr.sin_zero[4]='\0';	
		sock_in_addr.sin_zero[5]='\0';	
		sock_in_addr.sin_zero[6]='\0';	
		sock_in_addr.sin_zero[7]='\0';	
		//strcpy (sock_in_addr.sin_zero , "\0\0\0\0\0\0\0" );


	//	sock_in_addr.sin_addr.s_addr= inet_addr ("127.0.0.1")  ;
		//sock_in_addr.sin_addr.s_addr= inet_addr ("192.168.127.3")  ;
//		printf ("\nUDP_addr=%d\n",inet_addr (udp_init->in_addr));	
		
		
		if (    bind (*sock_in, (struct  sockaddr *) &sock_in_addr, sizeof (sock_in_addr)  )      < 0 )
		{
		
//			printf (" \n sock_in=%d Sock_addr_size_my=%d \n ", *sock_in, sizeof (sock_in_addr)   );	
			error |=4;

		}
		
		//sock_in_opt=1;	
		//setsockopt ( *sock_in, 6, TCP_NODELAY, (void*) &sock_in_opt, sizeof (sock_in_opt)   );	
			
		//sock_in_opt = udp_init->in_buf_size;	
		//setsockopt ( *sock_in, SOL_SOCKET, SO_RCVBUF, (void*) &sock_in_opt, sizeof (sock_in_opt)   );	
				

	}
	else
	{
		error |= 1;				

	}


	if (*sock_out >0)
	{
		sock_out_addr->sin_family=PF_INET;
		sock_out_addr->sin_port=htons (udp_init->out_port);
		sock_out_addr->sin_addr.s_addr= inet_addr (udp_init->out_addr)  ;
		sock_out_addr->sin_zero[0]='\0';	
		sock_out_addr->sin_zero[1]='\0';	
		sock_out_addr->sin_zero[2]='\0';	
		sock_out_addr->sin_zero[3]='\0';	
		sock_out_addr->sin_zero[4]='\0';	
		sock_out_addr->sin_zero[5]='\0';	
		sock_out_addr->sin_zero[6]='\0';	
		sock_out_addr->sin_zero[7]='\0';	


		sock_out_addr->sin_addr.s_addr= inet_addr (udp_init->out_addr);
		
//		if (    connect (*sock_out,(struct  sockaddr *)   &sock_out_addr-> sizeof (sock_out_addr->  )      < 0 )
//		{
			
//			error |=8;

//		}
		
		sock_out_opt = udp_init->out_buf_size;	
		setsockopt ( *sock_out, SOL_SOCKET, SO_SNDBUF, (void*) &sock_out_opt, sizeof (sock_out_opt)   );	

	}
	else
	{

		error |= 2;				

	}

	



return error;
}















