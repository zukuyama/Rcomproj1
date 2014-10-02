#include "procotol.h"

void init_super(super_t * s)
{
  s = (super_t *) malloc( sizeof(super_t) );
  if (s != NULL)
  {
    perror("Error in malloc super_t type");
    exit(1);
  }
  
  reset_super(s);
}

void reset_super(super_t * s)
{
  s->set[0] = F;
  s->set[1] = A;
  s->set[2] = C;
  s->set[3] = set[1]^set[2];
  s->set[4] = F;

  s->ua[0] = F;
  s->ua[1] = A;
  s->ua[2] = C;
  s->ua[3] = ua[1]^ua[2];
  s->ua[4] = F;
}