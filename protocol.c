#include "protocol.h"

unsigned char set[5];

set[0] = F;
set[1] = A;
set[2] = C;
set[3] = set[1]^set[2];
set[4] = F;
