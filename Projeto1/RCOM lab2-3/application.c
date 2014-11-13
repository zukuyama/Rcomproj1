//
//  application.c
//  RCOM lab2
//
//  Created by José Ricardo de Sousa Coutinho on 15/10/14.
//  Copyright (c) 2014 José Ricardo de Sousa Coutinho. All rights reserved.
//

#include "application.h"

static appLayer app;
static int conta;

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

int llopen(int porta, size_t flag)
{
    /*
     INICIO DO PROGRAMA
     */
    
    system("clear");
    
    
    switch (flag) {
        case TRANSMITTER:
            app.fileDescriptor = definirSettings(porta,&oldtio,&newtio,0,5,link.baudrate);
            break;
            
        case RECEIVER:
            app.fileDescriptor = definirSettings(porta,&oldtio,&newtio,0.1,0,link.baudrate);
            break;
        
        default:
            break;
    }
    
    definirSetUa(); //definir SET e UA
    
    while (conta < 3)
    {
        if(flag == 1)
        {
            alarm(3);
            enviarSet(&app.fileDescriptor);
            flag = 0;
            
            if(lerUA(&app.fileDescriptor) == 0)
            {
                break;
            }
        }
        
    }
    
    redefinirSettings(app.fileDescriptor,oldtio);
    close(app.fileDescriptor);
    
    return 0;
}

int llwrite(int id, char * buffer, int length);
int llread(int porta, size_t flag);
int llclose(int porta, size_t flag)
{
    redefinirSettings();
    close(app.fileDescriptor);
}



