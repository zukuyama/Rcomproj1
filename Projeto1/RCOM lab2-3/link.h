//
//  link.h
//  RCOM lab2
//
//  Created by José Ricardo de Sousa Coutinho on 15/10/14.
//  Copyright (c) 2014 José Ricardo de Sousa Coutinho. All rights reserved.
//

#ifndef __RCOM_lab2__link__
#define __RCOM_lab2__link__

#define MAX_SIZE 255


#include "include.h"
#include "protocol.h"
#include "interface.h"

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

#endif /* defined(__RCOM_lab2__link__) */
