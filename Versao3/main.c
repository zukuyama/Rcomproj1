#include "noncanonical.h"

int main(int argc, char** argv)
{
   if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) ))
   {
       printf("Usage:\tserial SerialPort\n\tex: serial /dev/ttyS0\n");
       return 1;
   }
   
   noncanon_t * nct = (noncanon_t *) malloc( sizeof(noncanon_t) );
   if (nct == NULL)
   {
       printf("Error in malloc of noncanon_t type.\nEnding program.");
       return 2;
   }
    
   strcpy(nct->deviceName, argv[1]);

   printf("Input message:\n");
   gets(nct->buf);

   noncanon_open(nct);

   // envio/leitura , usar exemplos abaixo
   // noncanon_msg_send(nct);
   // noncanon_msg_receive(nct);

   noncanon_close(nct);

   freeNCT(nct);
   return 0;
}
