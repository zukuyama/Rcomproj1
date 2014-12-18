#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 21

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct hostent HOSTENT;

void msg(char * str)
{
	printf("%s\n", str);
}

void msgErro(char * str)
{
	printf("ERROR: %s\n", str);
	exit(0);
}

int tamanhoFicheiro(char * str)
{
	int r = 0;

	FILE * f;
	f = fopen(str, "rb");
	
	if (f == NULL)
		return(-1);
	
	fseek(f, 0L, SEEK_END);
	r = ftell(f);

	fclose(f);
	return(r);
}

int toInt(char c)
{
	int num = 0;
	switch(c)
	{
		case '0':
			num = 0;
			break;
		case '1':
			num = 1;
			break;
		case '2':
			num = 2;
			break;
		case '3':
			num = 3;
			break;
		case '4':
			num = 4;
			break;
		case '5':
			num = 5;
			break;
		case '6':
			num = 6;
			break;
		case '7':
			num = 7;
			break;
		case '8':
			num = 8;
			break;
		case '9':
			num = 9;
			break;
	}
	return(num);
}

int getCodigo(char * str)
{
	int r = toInt(str[0])*100 + toInt(str[1])*10 + toInt(str[2]);
	
	return(r);
}
