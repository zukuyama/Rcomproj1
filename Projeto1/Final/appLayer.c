//
//  appLayer.c
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#include "appLayer.h"

appLayer app;

void setAppLayer(appLayer * app, int f, int s)
{
    app->fileDescriptor = f;
    app->status = s;
}

void initAppLayer(appLayer * app)
{
    app->fileDescriptor = 0;
    app->status = 0;
}

int llopen(int porta, size_t flag)
{
    int fd,error;
    char port[] = "/dev/ttyS0";
    port[9] = (porta == 4) ? '4' : '0' ;
    
    error = open_nc(&fd, port);
    
    if (error) { return -1; }
    
    // algures após isto é para meter o código do lab3
    // diagrama de estados
    
    int estado = 0;
    
    // app.status = flag;
    
    if (flag == TRANSMITTER)
    {
        estado = enviar(fd);
    }
    else
    {
        estado =  receber(fd);
    }
    
    if (estado == -1)
    {
    	return(-1);
    }
    
    return fd;
}


int llwrite(int fd, char * buffer, int length) 
{
 
      /* char FLAG = 0x7e;// need to define both
        char ESC = 0x7d; 
        int MFS = 200; 
   
        char C = 0x01;
        // need to divide the buffer in n parts xD 
        char N = 0;
	char L2 = MFS / 256;
        char L1 = MFS - L2 * 256;
        
        char newFrag [256];//maybe malloc ?

        // stuffing (PH)        
        newFrag[0] = C;
        newFrag[1] = N;
        newFrag[2] = L2;
	newFrag[3] = L1;

        int b = 4;
        for(int a = 0; a < length ; a++){
            if(buffer[a] == FLAG || buffer[a] == ESC)
               newFrag[b] == ESC;               
            else
               newFrag[b] = buffer[a];
            b++;
        }
 
       	printf("Sending signal... ");
	
	tcflush(fd, TCOFLUSH); // Clean output buffer

	//Pacote de dados
        write(fd, newFrag, 256);

        printf("[OK]\n");*/
    
    return 0;
}


int  llread(int fd, char * buffer)
{
	strcpy(buffer,"");// need to confirm if the buffer is allocated
   int STOPread = FALSE;// think in a better away just for testing

	printf("Waiting for message...\n");
    char buf [255];
    int res = 0;
   int cont = 0;
while (STOPread==FALSE) {       /* loop for input */
      res = read(fd,buf,255);   /* returns after 5 chars have been input */
      printf("res = %d\n",res);

      if(res < 0)continue;// try to change the vtime and vmin

      if (buf[res-1]=='\0') STOPread=TRUE;// '\0' recebeu o fim da string
      buf[res]='\0';               /* so we can printf... */
            
      strcat(buffer,buf);             
      
      printf("packet received: \"%s\" -> (size = %d)\n", buf, res);
    }
    printf("Message received: \"%s\" -> (size = %d)\n\n", buffer, (int)strlen(buffer));

   return 0;
}

int llclose(int fd)
{	
	int erro =0;

	if (app.status == TRANSMITTER) // TRANSMITTER
	{
		//// envia DISC
		erro = write_cc(fd,C_DISC);
		
		if (erro!=-1) 
		{
			//// recebe confirmacao de DISC
			erro = read_cc(fd,C_DISC);	
			
			if (erro!=-1) 
			{
				//// envia UA
				erro = write_cc(fd,C_UA);
			}
		}

		//// exit
		
	}
	else // RECEIVER
	{
		//// exit
	}
	
	close_nc(fd);
	
	return erro;
}
