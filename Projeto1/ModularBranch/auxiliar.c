//
//  auxiliar.c
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#include "auxiliar.h"

int print(char * string)
{
    write(STDOUT_FILENO, string, sizeof(string));
    
    return 0;
}