//
//  protocol.h
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#ifndef __RCOM_PROJ1__protocol__
#define __RCOM_PROJ1__protocol__

#define F 0x7e
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

unsigned char set[5];
unsigned char ua[5];

void init_SET_and_UA();

#endif /* defined(__RCOM_PROJ1__protocol__) */
