#ifndef PROTOCOL_H
#define PROTOCOL_H

#define F 0x7e
#define A 0x03
#define C_SET 0x03
#define C_UA 0x07

unsigned char set[5];
unsigned char ua[5];

void definirSetUa()
{
	set[0] = F;
	set[1] = A;
	set[2] = C_SET;
	set[3] = (set[1] ^ set[2]);
	set[4] = F;

	ua[0] = F;
	ua[1] = A;
	ua[2] = C_UA;
	ua[3] = (ua[1] ^ ua[2]);
	ua[4] = F;
}

void newline()
{
	printf("\n");
}

void texto(char * s)
{
	printf("%s\n", s);
}

#endif
