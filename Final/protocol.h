//
//  protocol.h
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#ifndef __RCOM_PROJ1__protocol__
#define __RCOM_PROJ1__protocol__

#define BAUDRATE B9600//B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define F 0x7e

#define A 0x03

#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0b


#include "include.h"

unsigned char set[5];
unsigned char ua[5];
unsigned char disc[5];

void init_SET_and_UA();

#endif /* defined(__RCOM_PROJ1__protocol__) */
