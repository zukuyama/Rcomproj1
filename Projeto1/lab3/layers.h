#ifndef APPLICATONLAYER_H
#define APPLICATONLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_SIZE 255

typedef struct _app
{
	int fileDescriptor;
	int status;
} appLayer;

typedef struct _link
{
	char port[20]; // dispositivo /dev/ttySx , com x = {0,1,2,3,4,5} // esq: 4, dir: 0
	int baudrate; // velocidade de transmissao
	unsigned int sequenceNumber; // numero de sequencia da trama: 0 ou 1
	unsigned int timeout; // valor do temporizador (segundos): ex: 3
	unsigned int numTransmissions; //numero de tentativas em caso de falha
	char frame[MAX_SIZE]; // trama
} linkLayer;

void setAppLayer(appLayer * app, int f, int s)
{
	app->fileDescriptor = f;
	app->status = s;
}

void initAppLayer(appLayer * app)
{
	app->fileDescriptor = 0;
	app->status = 0;
}

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

#endif
