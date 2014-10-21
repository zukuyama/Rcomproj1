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
    sleep(1); 
    return fd;
}

void llwrite(int fd, char * buffer, int length) {

	printf("Sending signal... ");
	
	tcflush(fd, TCOFLUSH); // Clean output buffer

    write(fd, buffer, length);

    printf("OK!!!\n");
}

int llread(int fd, char * buffer) {
   strcpy(buffer,"");// need to confirm if the buffer is allocated
   int STOPread = FALSE;// think in a better away just for testing

	printf("Waiting for message...\n");
    char buf [255];
    int res = 0;
   int cont = 0;
while (STOPread==FALSE) {       /* loop for input */
      res = read(fd,buf,255);   /* returns after 5 chars have been input */
printf("%d\n",res);
      
      if (buf[0]=='z' || buf[res-1]=='\0') STOPread=TRUE;// '\0' recebeu o fim da string
      buf[res]='\0';               /* so we can printf... */
            
    if(cont++ == 50)break;
      strcat(buffer,buf);             
      
      printf("packet received: \"%s\" -> (size = %d)\n", buf, res);
    }
    printf("Message received: \"%s\" -> (size = %d)\n\n", buffer, (int)strlen(buffer));
}

int llclose(int fd)
{
    close_nc(fd);
    
    return 0;
}
