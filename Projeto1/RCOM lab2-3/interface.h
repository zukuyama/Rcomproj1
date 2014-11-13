//
//  interface.h
//  RCOM lab2
//
//  Created by José Ricardo de Sousa Coutinho on 15/10/14.
//  Copyright (c) 2014 José Ricardo de Sousa Coutinho. All rights reserved.
//

#ifndef __RCOM_lab2__interface__
#define __RCOM_lab2__interface__

#include "include.h"

int definirSettings(const char * port, struct termios * oldtio, struct termios * newtio, float vtime, int vmin, int baudrate);
void redefinirSettings(int fd, struct termios * oldtio);


#endif /* defined(__RCOM_lab2__interface__) */
