#include "noncanonical.h"

int noncanon_open(noncanon_t * nct)
{
   int i, res, sum = 0, speed = 0;

   // open serial port device for reading and writing and not as controlling tty because we don't want to get killed if linenoise sends CTRL-C.
   nct->fd = open(nct->deviceName, O_RDWR | O_NOCTTY );
   if (nct->fd < 0)
   {
      perror(nct->deviceName);
      exit(-1);
   }

   // save current port settings
   if ( tcgetattr(nct->fd, nct->oldtio) == -1 )
   {
      perror("tcgetattr");
      exit(-2);
   }

   bzero(nct->newtio, sizeof(nct->newtio));
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

   if ( tcsetattr(nct->fd,TCSANOW,nct->newtio) == -1 )
   {
      perror("tcsetattr");
      exit(-3);
   }

   printf("\nNew termios structure set.\nCommunication to '%s' is open.",nct->modemDevice);

   return 0;
}

int noncanon_close(noncanon_t * nct)
{
   // restore old port settings
   if ( tcsetattr(nct->fd,TCSANOW,nct->oldtio) == -1)
   {
      perror("tcsetattr");
      exit(-1);
   }

   printf("\nTermios structure reset.\nCommunication to '%s' is closed.",nct->modemDevice);

   close(nct->fd);

   return 0;
}

int noncanon_msg_send(noncanon_t * nct)
{

   // testing
   //nct->buf[25] = '\n';

   res = write(nct->fd, nct->buf, strlen(nct->buf));
   fsync(res);

   printf("%d bytes written\n", res);

   return 0;
}

int noncanon_msg_receive(noncanon_t * nct)
{
   while (STOP==FALSE)
   {
      res = read(nct->fd, nct->buf, 255);
      nct->buf[res] = '\0';

      printf("Received message:\n%s", nct->buf);

      if (buf[0] == 'z')
      {
         STOP = TRUE;
      }
   }

   strcpy(nct->buf,'\0');

   return 0;
}

void freeNCT(noncanon_t * nct)
{
   if (nct != null)
   {
      if (nct->modemDevice != NULL)
      {
         free(nct->modemDevice);
      }

      free(nct);
   }
}
