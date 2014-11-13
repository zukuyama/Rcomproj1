//
//  interface.c
//  RCOM lab2
//
//  Created by José Ricardo de Sousa Coutinho on 15/10/14.
//  Copyright (c) 2014 José Ricardo de Sousa Coutinho. All rights reserved.
//

#include "interface.h"

int definirSettings(const char * port, struct termios * oldtio, struct termios * newtio, float vtime, int vmin, int baudrate)
{
    int fd;
    
    fd = open(port, O_RDWR | O_NOCTTY ); // fd = number of file descriptor
    if (fd < 0)
    {
        perror(port);
        return -1;
    }
    
    if ( tcgetattr(fd,oldtio) == -1) { // save current port settings
        perror("tcgetattr");
        return -1;
    }
    
    bzero(&newtio, sizeof(newtio));
    newtio->c_cflag = baudrate | CS8 | CLOCAL | CREAD;
    newtio->c_iflag = IGNPAR;
    newtio->c_oflag = 0;
    
    /* set input mode (non-canonical, no echo,...) */
    newtio->c_lflag = 0;
    
    newtio->c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio->c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
    
    /*
     VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
     leitura do(s) próximo(s) caracter(es)
     */
    tcflush(fd, TCIOFLUSH);
    
    if ( tcsetattr(fd,TCSANOW,newtio) == -1) {
        perror("tcsetattr");
        return -1;
    }
    
    return fd;
}

void redefinirSettings(int fd, struct termios * oldtio)
{
    if ( tcsetattr(fd,TCSANOW,oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }
}
