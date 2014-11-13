//
//  appLayer.h
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#ifndef __RCOM_PROJ1__appLayer__
#define __RCOM_PROJ1__appLayer__

#include "include.h"
#include "linkLayer.h"

typedef struct _app
{
    int fileDescriptor;
    int status;
} appLayer;

static appLayer app;

void setAppLayer(appLayer * app, int f, int s);

void initAppLayer(appLayer * app);

int llopen(int porta, size_t flag);
int llwrite(int fd, char * buffer, int length);
int llread(int fd, char * buffer);
int llclose(int fd);

#endif /* defined(__RCOM_PROJ1__appLayer__) */
