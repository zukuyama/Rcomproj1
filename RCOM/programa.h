#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <math.h>

#include "definicoes.h"

// testing

 // declarar read_cc and write_cc functions
 
/*****************************************
	VARIAVEIS GLOBAIS
*****************************************/
int eEmissor;
//char device[11];
int device;

char * nomeFicheiro;
int tamFicheiro;

AppLayer app;
LinkLayer lnk;

// 
struct termios oldtio,newtio;// for the llopen and llclose to use xD

/*****************************************
	FUNCOES GLOBAIS
*****************************************/
void setEmissorDevice(char * arg1, char * arg2)
{
	if (strcmp(arg1, "server") == 0) // servidor pc local
	{
		device = 0;
		//strcpy(device, "/dev/ttyS0");
	}
	else // cliente pc local
	{
		device = 4;
		//strcpy(device, "/dev/ttyS4");
	}

	if (strcmp(arg2, "send") == 0) // pc local a querer ser o emissor
	{
		eEmissor = TRANSMITTER;
	}
	else // pc local a querer ser o receptor
	{
		eEmissor = RECEIVER;
	}
}

/*****************************************
	FUNCOES DA APLICACAO A UTILIZAR
*****************************************/
int llopen(int porta, size_t flag);
int llwrite(int fd, char * buffer, int length);
int llread(int fd, char * buffer);
int llclose(int fd);
