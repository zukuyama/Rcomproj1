//
//  main.c
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#include "noncanon.h"

int main(int argc, const char * argv[])
{
    /*
     Verify if arguments are valid:
     - "/dev/ttyS0" is the server (computer on the right)
     - "/dev/ttyS4" is the client (computer on the left)
     */
    if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) ))
    {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS4\n");
        exit(1);
    }
    
    //send_nc(argv[1]);
    
    //receive_nc(argv[1]);
    
    return 0;
}
