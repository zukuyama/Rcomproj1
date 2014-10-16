//
//  link.c
//  RCOM lab2
//
//  Created by José Ricardo de Sousa Coutinho on 15/10/14.
//  Copyright (c) 2014 José Ricardo de Sousa Coutinho. All rights reserved.
//

#include "link.h"

static linkLayer link;

void setLinkLayer(linkLayer * link, char p[20], int b, unsigned int s, unsigned int t, unsigned int n, char f[MAX_SIZE])
{
    strcpy(link->port, p);
    link->baudrate = b;
    link->sequenceNumber = s;
    link->timeout = t;
    link->numTransmissions = n;
    strcpy(link->frame, f);
}

void initLinkLayer(linkLayer * link)
{
    strcpy(link->port, "");
    link->baudrate = 0;
    link->sequenceNumber = 0;
    link->timeout = 0;
    link->numTransmissions = 1;
    strcpy(link->frame, "");
}
