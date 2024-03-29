//
//  linkLayer.h
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#ifndef __RCOM_PROJ1__linkLayer__
#define __RCOM_PROJ1__linkLayer__

#include "include.h"
#include "noncanon.h"

#define MAX_SIZE 255
#define TRANSMITTER 1
#define RECEIVER 0


typedef struct _link
{
    char port[20]; // dispositivo /dev/ttySx , com x = {0,1,2,3,4,5} // esq: 4, dir: 0
    int baudrate; // velocidade de transmissao
    unsigned int sequenceNumber; // numero de sequencia da trama: 0 ou 1
    unsigned int timeout; // valor do temporizador (segundos): ex: 3
    unsigned int numTransmissions; //numero de tentativas em caso de falha
    char frame[MAX_SIZE]; // trama
} linkLayer;


void setLinkLayer(linkLayer * link, char p[20], int b, unsigned int s, unsigned int t, unsigned int n, char f[MAX_SIZE]);
void initLinkLayer(linkLayer * link);

int  llopen(int porta, size_t flag);
int  llwrite(int fd, char * buffer, int length);
int  llread(int fd, char * buffer);
int  llclose(int fd);


#endif /* defined(__RCOM_PROJ1__linkLayer__) */
