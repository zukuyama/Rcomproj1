#ifndef   NONCANON_H
#define   NONCANON_H

#include "include.h"
#include "protocol.h"

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

typedef struct noncanon_type
{
   int fd;
   struct termios oldtio;
   struct termios newtio;
   unsigned char string[255];
   char device[10];
   
   super_t s;
   
} noncanon_t;

void init_noncanon(noncanon_t * nct);

int open_noncanon(noncanon_t * nct);
int close_noncanon(noncanon_t * nct);

int send_noncanon(noncanon_t * nct);
int receive_noncanon(noncanon_t * nct);

#endif
