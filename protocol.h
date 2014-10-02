#ifndef PROTOCOL_H
#define PROTOCOL_H

#define F 0x7e
#define A 0x03
#define C 0x03

typedef unsigned char Byte;

Byte set[5];
Byte ua[5];
 


void resetGates()
{
	set[0] = F;
	set[1] = A;
	set[2] = C;
	set[3] = set[1]^set[2];
	set[4] = F;

	ua[0] = set[0];
	ua[1] = set[1];
	ua[2] = set[2];
	ua[3] = set[3];
	ua[4] = set[4];
}

#endif
