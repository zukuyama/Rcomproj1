/*Non-Canonical Input Processing*/
// in server xDD

/***********************************
*		Aula 1 : 25 Setembro 2014
***********************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

/***********************************
*		Aula 2 : 2 Outubro 2014
***********************************/
#include "protocol.h"

/***********************************
*		Aula 1 : 25 Setembro 2014
***********************************/
volatile int STOP=FALSE;

int main(int argc, char** argv)
{
	system("clear");
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS4", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS4\n");
      exit(1);
    }

  // &&&&&&&&&&&&&&&&&&&&&&&&&&&&  OPEN &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
      
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */


// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&& tcgetattr &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */


    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&     receive    &&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    printf("..:: New termios structure set ::..\n");
    printf("Waiting for message...\n");

    char frase[255] = "\0";
    while (STOP==FALSE) {       /* loop for input */
      res = read(fd,buf,255);   /* returns after 5 chars have been input */
      
      if (buf[0]=='z' || buf[res-1]=='\0') STOP=TRUE;// '\0' recebeu o fim da string
      buf[res]='\0';               /* so we can printf... */
            

      strcat(frase,buf);             
      
      printf("packet received: \"%s\" -> (size = %d)\n", buf, res);
    }
    printf("Message received: \"%s\" -> (size = %d)\n\n", frase, strlen(frase));

// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&      SEND     &&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    printf("..:: New termios structure set ::..\n");
    sleep(5);
    
    printf("Resending Message...\n");
   
	write(fd, set, 5);
    res = write(fd,frase,255);   
    printf("%d bytes written\n", res);
 
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&& tcsetattr &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    tcsetattr(fd,TCSANOW,&oldtio);
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& close &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 
    close(fd);
 
    return 0;
}
