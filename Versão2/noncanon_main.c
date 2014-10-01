#include "noncanonical.h"

int main(int argc, char** argv)
{
   noncanon_t * nct = malloc( nct, sizeof(noncanon_t) );
   if (nct == NULL)
   {
       printf("Error in malloc of noncanon_t type.\nEnding program.");
       return 1;
   }

   if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) ))
   {
       printf("Usage:\tserial SerialPort\n\tex: serial /dev/ttyS0\n");
       return 2;
   }

   nct->modemDevice = malloc( nct->modemDevice, sizeof(char) * strlen(argv[1]) );
   if (nct == NULL)
   {
       printf("Error in malloc of noncanon_t type.\nEnding program.");
       freeNCT(nct);
       return 3;
   }

   printf("Input message:\n");
   gets(nct->buf);

   noncanon_open(nct);

   // envio/leitura aqui
   noncanon_msg_send(nct);
   noncanon_msg_receive(nct);

   noncanon_close(nct);

   freeNCT(nct);
   return 0;
}
