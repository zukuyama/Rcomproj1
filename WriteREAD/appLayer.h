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

void setAppLayer(appLayer * app, int f, int s);

void initAppLayer(appLayer * app);

#endif /* defined(__RCOM_PROJ1__appLayer__) */
