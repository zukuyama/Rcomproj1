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
    
    // é necessário fechar aqui?
    close_nc(fd);
    
    return fd;
}

/*void llwrite(int fd, char * buffer, int length) {

	printf("Sending signal... ");
	
	tcflush(fd, TCOFLUSH); // Clean output buffer

    write(*fd, msg, length);

    printf("[OK]\n");
}*/

int llclose(int fd)
{
    close_nc(fd);
    
    return 0;
}
