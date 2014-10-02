#include "noncanonical.h"

int main(int argc, char** argv)
{
  
   if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) ))
   {
       printf("Usage:\tserial SerialPort\n\tex: serial /dev/ttyS0\n");
       return 1;
   }
   
   noncanon_t * nct;
   init_noncanon(nct);
    
   strcpy(nct->deviceName, argv[1]);

   printf("Input message:\n");
   gets(nct->buf);
   
   open_noncanon(nct);

   // envio/leitura , usar exemplos abaixo
   // send_noncanon(nct);
   // receive_noncanon(nct);

   close_noncanon(nct);
   
   free_noncanon(nct);
   
   return 0;
}
