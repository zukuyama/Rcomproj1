//
//  main.c
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#include "include.h"
#include "appLayer.h"

int main(int argc, const char * argv[])
{
  /*
    Verify if arguments are valid:
    - "/dev/ttyS0" is the server (computer on the right)
    - "/dev/ttyS4" is the client (computer on the left)
  */
  if ( (argc < 3) || ((strcmp("server", argv[1])!=0) && (strcmp("client"  , argv[1])!=0))
       || ((strcmp("send", argv[2])!=0) && (strcmp("get", argv[2])!=0))
       )
    {
      printf("4 ways to use:\n\texecutable server send\n\texecutable server get\n\texecutable client send\n\texecutable client send\n");
      exit(1);
    }
    
  char device[11]; 
  strcpy(device,"/dev/ttySX");
  int deviceN = 0;
    
  if ( strcmp(argv[1],"server")==0 )
    {
      device[9] = '0';
      deviceN   =  0;
    }
  else
    {
      device[9] = '4';
      deviceN   =  4;
    }
    
    
  initLinkLayer(&lnk);
  initAppLayer(&app);
    
  setLinkLayer(&lnk, device, BAUDRATE, 0, 0, 0, ""); // ver qual o valor para os outros valores a 0
    
  app.status = (strcmp(argv[2],"send")==0) ? TRANSMITTER : RECEIVER ;
  
  //--- llopen
  llopen(deviceN, app.status);
    
  if ( app.fileDescriptor == -1 ) // se o llopen falhar
  {
  		printf("Erro :: llopen :: Não foi possível estabelecer o protocolo de ligação.\n");
      exit(1);
  }
  
  int next =0;
  
  if (app.status == TRANSMITTER) 
  {  		
  		// constroi tramas de envio
  		
  		do 
  		{
  			// passa à proxima trama
  			if (next == 0) 
  			{
  				// trama++ 
  			}
  			
  			// llwrite
  			//// constroi a próxima trama para envio
  			//// envia trama com X tentativas e Y timeouts
  			//// 
  			//next = llwrite(app.fileDescriptor,trama,size_trama);
  			
  		} while (next >= 0);
  }
  else
  {
  		// constroi trama de recepcao
  		
  		do
  		{
  			// passa à proxima trama
  			if (next == 0) 
  			{
  				// trama++ 
  			}
  			
  			// llread
  			//next = llread(app.fileDescriptor,buffer);
  			
  		} while (next >= 0);
  }
	
  //--- llclose
  llclose(app.fileDescriptor);  
    
  return 0;
}
