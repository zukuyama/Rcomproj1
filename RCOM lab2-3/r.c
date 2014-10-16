/*Non-Canonical Input Processing*/
/*********************************************************
*	COMPILAR: 				gcc r.c -o r
* COMPILAR AMBOS: 	gcc r.c -o r | gcc s.c -o s
*	EXECUTAR: 				./r /dev/ttySx , x = {0, 4}
*	x = 4: servidor
*	x = 0: cliente
* ------------------------------------------------------
*						set | tentativa1
*		emissor --------------> receptor (programa)
*
*						set | tentativa2
*		emissor --------------> receptor (programa)
*
*							(caso sucesso)
*
*									ua
*		emissor <-------------- receptor (programa)
*********************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "protocol.h"
#include "application.h"

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

/************************************************
*							GLOBAL VARIABLES
************************************************/

/*
is a qualifier that is applied to a variable when it is declared.
It tells the compiler that the value of the variable may
change at any time-without any action
being taken by the code the compiler finds nearby. 
The implications of this are quite serious.
*/
volatile int STOP=FALSE;

static int flag=1, conta=0;

struct termios oldtio, newtio;
int fd; // descritor de ficheiro

/************************************************
*							MAIN FUNCTION
************************************************/
int main(int argc, char** argv)
{
	/*
	Verify if arguments are valid:
	- "/dev/ttyS0" is the server (computer on the right)
	- "/dev/ttyS4" is the client (computer on the left)
	*/
	if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) ))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS4\n");
		exit(1);
	}

	/*
	VARIAVEIS LOCAIS	
	*/
	int c, res;
	unsigned int x;
	unsigned char buf[5];

	/*
	INICIO DO PROGRAMA	
	*/
	system("clear");
	definirSettings(argc, argv); //atribuir o descritor do ficheiro de serial port
	definirSetUa(); //definir SET e UA


	// receber set
	printf("Receiving SET...\n");
	while (STOP==FALSE) /* loop for input */
	{       
		res = read(fd,buf,5);   /* returns after 5 chars have been input */
		printf("HEXACODES: ");
		for (x = 0; x < 5; x++)
		{
			printf("0x%x ", buf[x] );
		} printf("\n");

		if (strcmp(buf, set) == 0)
		{
			// enviar ua
			printf("SET Received... Sending UA...\n");

			tcflush(fd, TCOFLUSH);

			//enviar ua
			res = write(fd, ua, 5);
            STOP = TRUE;
		}
	}

	printf("UA sent with %d Bytes.\n", res);
	tcflush(fd, TCOFLUSH);

	redefinirSettings();
	close(fd);
	return (0);
}
