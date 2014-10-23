//
//  linkLayer.c
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#include "linkLayer.h"

void setLinkLayer(linkLayer * link, char p[20], int b, unsigned int s, unsigned int t, unsigned int n, char f[MAX_SIZE])
{
    strcpy(link->port, p);
    link->baudrate = b;
    link->sequenceNumber = s;
    link->timeout = t;
    link->numTransmissions = n;
    strcpy(link->frame, f);
}

void initLinkLayer(linkLayer * link)
{
    strcpy(link->port, "");
    link->baudrate = 0;
    link->sequenceNumber = 0;
    link->timeout = 0;
    link->numTransmissions = 1;
    strcpy(link->frame, "");
}

int llopen(int porta, size_t flag)
{
    int fd,error;
    char port[] = "/dev/ttyS0";
    port[9] = (porta == 4) ? '4' : '0' ;
    
    error = open_nc(&fd, port);
    
    if (error) { return -1; }
    
    // algures após isto é para meter o código do lab3
    // diagrama de estados
    
    if (flag == TRANSMITTER)
    {
        send_nc(fd);
    }
    else
    {
        receive_nc(fd);
    }
    
    return fd;
}

int llwrite(int fd, char * buffer, int length) {
        char FLAG = 0x7e;// need to define both
        char ESC = 0x7d; 
        int MFS = 200; 
   
        char C = 0x01;
        // need to divide the buffer in n parts xD 
        char N = 0;
	char L2 = MFS % 256:
        char L1 = MFS - L2 * 256;
        
        char newFrag [256];//maybe malloc ?

        // stuffing (PH)        
        newFrag[0] = C;
        newFrag[1] = N;
        newFrag[2] = L2;
	newFrag[3] = L1;

        int b = 4;
        for(int a = 0; a < length ; a++){
            if(buffer[a] == FLAG || buffer[a] == ESC)
               newFrag[b] == ESC;               
            else
               newFrag[b] = buffer[a];
            b++;
        }
 
       	printf("Sending signal... ");
	
	tcflush(fd, TCOFLUSH); // Clean output buffer

	//Pacote de dados
        write(fd, newFrag, 256);

        printf("[OK]\n");
}


int  llread(int fd, char * buffer){
   strcpy(buffer,"");// need to confirm if the buffer is allocated
   int STOPread = FALSE;// think in a better away just for testing

	printf("Waiting for message...\n");
    char buf [255];
    int res = 0;
   int cont = 0;
while (STOPread==FALSE) {       /* loop for input */
      res = read(fd,buf,255);   /* returns after 5 chars have been input */
      printf("res = %d\n",res);

      if(res < 0)continue;// try to change the vtime and vmin

      if (buf[res-1]=='\0') STOPread=TRUE;// '\0' recebeu o fim da string
      buf[res]='\0';               /* so we can printf... */
            
      strcat(buffer,buf);             
      
      printf("packet received: \"%s\" -> (size = %d)\n", buf, res);
    }
    printf("Message received: \"%s\" -> (size = %d)\n\n", buffer, (int)strlen(buffer));

   return 0;
}

int llclose(int fd)
{
    close_nc(fd);
    
    return 0;
}
