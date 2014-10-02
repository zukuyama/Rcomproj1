#ifndef PROTOCOL_H
#define PROTOCOL_H

#define F 0x7e
#define A 0x03
#define C 0x03

typedef unsigned char Byte;

typedef struct supervision_t
{
    Byte set[5];
    Byte ua[5];
} super_t;

void init_super(super_t * s);
void reset_super(super_t * s);

#endif
