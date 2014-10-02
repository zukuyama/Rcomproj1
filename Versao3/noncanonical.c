#include "noncanonical.h"

volatile int STOP=FALSE;

void init_noncanon(noncanon_t * nct)
{  
   init_super(&(nct->s));
}

int open_noncanon(noncanon_t * nct)
{
   // open serial port device for reading and writing and not as controlling tty because we don't want to get killed if linenoise sends CTRL-C.      
   nct->fd = open(nct->device, O_RDWR | O_NOCTTY );
   if (nct->fd < 0)
   {
      perror(nct->device);
      exit(-1);
   }
   
   // save current port settings
   if ( tcgetattr(nct->fd, &(nct->oldtio)) == -1 )
   {
      perror("tcgetattr");
      exit(-2);
   }

   bzero(&(nct->newtio), sizeof(nct->newtio));
   nct->newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   nct->newtio.c_iflag = IGNPAR;
   nct->newtio.c_oflag = 0;

   // set input mode (non-canonical, no echo,...)
   nct->newtio.c_lflag = 0;

   nct->newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
   nct->newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

   /*
   VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
   leitura do(s) pr�ximo(s) caracter(es)
   */

   tcflush(nct->fd, TCIOFLUSH);

   printf("\nNew termios structure set.\nCommunication to '%s' is open.", nct->device);

   return 0;
}

int close_noncanon(noncanon_t * nct)
{
   // restore old port settings
   if ( tcsetattr(nct->fd,TCSANOW,&(nct->oldtio)) == -1)
   {
      perror("tcsetattr");
      exit(-1);
   }

   printf("\nTermios structure reset.\nCommunication to '%s' is closed.", nct->device);

   close(nct->fd);

   return 0;
}

int send_noncanon(noncanon_t * nct)
{
   int res;

   res = write(nct->fd, nct->s.set, 5 * sizeof(Byte));
   sleep(3);

   printf("\nSent %d bytes.", res);

   return 0;
}

int receive_noncanon(noncanon_t * nct)
{
   int res;
   
   unsigned char buf[255] = "\0";
   
   while (STOP==FALSE)
   {
      res = read(nct->fd, buf, 255);
      
      if (buf[0]=='z' || buf[res-1]=='\0') 
      {
	STOP = TRUE; // '\0' recebeu o fim da string
      }
      buf[res]='\0';  
      
      strcat((char *) nct->string,(char *) buf);
      
      int x = 0;
      for(; x<5; x++)
      {
	printf("%d -> [%x]\n", x, buf[x]);
      }
      
    }
    printf("Received message: \"%s\" -> (size = %d)\n\n", nct->string, (int) strlen((char *)nct->string));

    return 0;
}
