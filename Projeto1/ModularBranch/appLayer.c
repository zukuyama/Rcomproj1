//
//  appLayer.c
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#include "appLayer.h"

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