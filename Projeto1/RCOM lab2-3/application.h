//
//  application.h
//  RCOM lab2
//
//  Created by José Ricardo de Sousa Coutinho on 15/10/14.
//  Copyright (c) 2014 José Ricardo de Sousa Coutinho. All rights reserved.
//

#ifndef __RCOM_lab2__application__
#define __RCOM_lab2__application__

#include "link.h"

#define TRANSMITTER 1
#define RECEIVER 0

typedef struct _app
{
    int fileDescriptor;
    int status;
} appLayer;


void setAppLayer(appLayer * app, int f, int s);
void initAppLayer(appLayer * app);


int llopen(int porta, size_t flag);
int llwrite(int id, char * buffer, int length);
int llread(int porta, size_t flag);
int llclose(int porta, size_t flag);

#endif /* defined(__RCOM_lab2__application__) */
