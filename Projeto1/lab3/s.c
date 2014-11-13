/*Non-Canonical Input Processing*/
/*********************************************************
*	COMPILAR: 	gcc s.c -o s
* COMPILAR AMBOS: 	gcc r.c -o r | gcc s.c -o s
*	EXECUTAR: 	./s /dev/ttySx , x = {0, 4}
*	x = 4: servidor
*	x = 0: cliente
* ------------------------------------------------------
*										set | tentativa1
*		emissor(programa) --------------> receptor
*
*										set | tentativa2
*		emissor(programa) --------------> receptor
*
*							(caso sucesso)
*
*														ua
*		emissor(programa) <-------------- receptor
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
#include "layers.h"

#define BAUDRATE B9600//B38400
#define MODEMDEVICE "/dev/ttyS1"
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
volatile int estadoSET = FALSE;
volatile int estadoUA = FALSE;

int flag=1, conta=0;
struct termios oldtio, newtio;
int fd; // descritor de ficheiro

/************************************************
*							GLOBAL FUNCTIONS
************************************************/

void atende() // atende alarme
{
	//texto("Chamada atendida.\n");
	//getchar();
	flag = 1;
	STOP = FALSE;

	conta++;
	if (conta != 3)
		printf("\nTentativa %d...\n", conta);
}

/*
Open serial port device for reading and writing and not as controlling tty
because we don't want to get killed if linenoise sends CTRL-C.
*/
void definirSettings(int argc, char** argv)
{
	fd = open(argv[1], O_RDWR | O_NOCTTY ); // fd = number of file descriptor
	if (fd < 0)
	{
		perror(argv[1]);
		exit(-1);
	}

	if ( tcgetattr(fd,&oldtio) == -1) { // save current port settings
		perror("tcgetattr");
		exit(-1);
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0; //OPOST;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	//newtio.c_cc[VTIME]    = 0.1;   /* inter-character timer unused */
	newtio.c_cc[VTIME]    = 0.1;//3   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 0; //5  /* blocking read until 5 chars received */

	/* 
	VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
	leitura do(s) próximo(s) caracter(es)
	*/

	tcflush(fd, TCIOFLUSH);
	if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
		perror("tcsetattr");
		exit(-1);
	}
}

void redefinirSettings()
{
	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1)
	{
		perror("tcsetattr");
		exit(-1);
	}
}

void enviarSet(int * f)
{
	printf("A enviar SET...\n");
	tcflush(*f, TCOFLUSH);

	write(*f, set, 5);
}

int lerUA(int * f)
{
	int res = 0;
	unsigned char buf[5];
	unsigned char aux1[5];

	strcpy(buf, "");
	strcpy(aux1, "");    

    	tcflush(*f, TCIFLUSH);

	int i = 0;
	printf("A receber UA... \n");
	

	i = 0;
	while (STOP==FALSE && !flag) // leitura do UA vindo do Receptor
	{
		res = read(*f, buf, 1);
		if (res != 0)
		{
			aux1[i] = buf[0];
			if (aux1[i] == F && i != 0)
			{
				STOP = TRUE;
			}
			else
			{
				i++;
			}
		}
	}
	
	if (strlen(aux1) != 0)// why strlen()
	{
		printf("UA recebido: ");
		for (i = 0; i < strlen(aux1); i++)
		{
			printf("0x%x ", aux1[i]);
		} printf("\n");
	}
	if(flag == 1)
	{
		//STOP = FALSE;
		return -1;
	}

	
		

	return 0;
}

/************************************************
*								MAIN FUNCTION
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
	ATRIBUICAO DE SINAIS DEVE SER FEITA AO INICIO DE UM MAIN
	*/
	(void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao

	/*
	VARIAVEIS LOCAIS	
	*/
	int c, res;
	int i, sum = 0, speed = 0;

	/*
	INICIO DO PROGRAMA	
	*/
	system("clear");
	definirSettings(argc, argv); //atribuir o descritor do ficheiro de serial port
	definirSetUa(); //definir SET e UA
	
	while (conta < 3)
	{
		if(flag == 1)
		{
    	    		alarm(3);
    	    		enviarSet(&fd);
    			flag = 0;
			
    			if(lerUA(&fd) == 0)
    				break;
    		}

	}
	
	redefinirSettings();
	close(fd);

	if (conta == 3)
	{
		printf("\nExcedido o tempo limite\n");
		return(-1);
	}
	else
	{
		printf("\nConexao estabelecida e garantida\n");
		return(0);
	}
}

