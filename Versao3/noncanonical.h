#ifndef   NONCANON_H
#define   NONCANON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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
   char deviceName[10];
   
   super_t * s;
   
} noncanon_t;

void init_noncanon(noncanon_t * nct);

int open_noncanon(noncanon_t * nct);
int close_noncanon(noncanon_t * nct);

int send_noncanon(noncanon_t * nct);
int receive_noncanon(noncanon_t * nct);

void free_noncanon(noncanon_t * nct);

#endif
