#include "programa.h"
// testing
int read_cc(int fd,unsigned char A, unsigned char cc) // lê um CC
{	 
	unsigned char bloco[5];
	bloco[0] = FLAG;
   bloco[1] = A;
   bloco[2] = cc;
   bloco[3] = (bloco[1] ^ bloco[2]);
   bloco[4] = FLAG;

	switch (cc)
	{
		case C_UA:
			printf("--- Reading UA ---\n");						
			break;		 
		case C_SET:
			printf("--- Reading SET ---\n");			
			break; 
		case C_DISC:
			printf("--- Reading DISC ---\n");			
			break;
		default : 
			printf("Invalid CC\n");
         return -1;
	}

    int res = 0;
    unsigned char buf[1];
    unsigned char aux1[5];
    
    strcpy(buf, "");
    strcpy(aux1, "");
    
    tcflush(fd, TCIFLUSH);
    
    int i = 0;
     
    // state machine / reading ***************************************    
    while (i<5) 
    {
        if(read(fd, buf, 1) != -1){
           i++;
        }
        //  call state machine
        //i++ if state valid
    }
    
    if (i != 0)
    {
        printf("\nReceived: ");
        int x;
        for (x = 0; x < i ; x++)
        {
            printf("0x%x ", aux1[x]);
        } 
        printf("\n");
    }
    
    if(flag == 1)
    {
        return -1;
    }
    
    return 0;
}

int write_cc(int fd,unsigned char A , unsigned char cc) // escreve o SET
{ 
   int res = 0;

	unsigned char bloco[5];
	bloco[0] = FLAG;
   bloco[1] = A;
   bloco[2] = cc;
   bloco[3] = (bloco[1] ^ bloco[2]);
   bloco[4] = FLAG;

   switch (cc)
	{
		case C_UA:
			printf("--- Sending UA ---\n");
			break;		 
		case C_SET:
			printf("--- Sending SET ---\n");
		   break; 
		case C_DISC:
			printf("--- Sending DISC ---\n");			
			break;
		default :
         printf("Invalid CC\n");
         return -1;
	}

   tcflush(fd, TCOFLUSH);
    
   int x = 0;
   while(x < 5){
     write(fd, bloco[x++], 1)
   }    

   return 0;
}


int main(int argc, char ** argv)
{
	/*
    Verify if arguments are valid:
    - "/dev/ttyS0" is the server (computer on the right)
    - "/dev/ttyS4" is the client (computer on the left)
  */

	if (argc >= 1) // ha argumentos
	{
		if (strcmp(argv[2], "send") == 0) // servidor deve ter 3 argumentos
		{
			if (argc != 4)
			{
				printf("SERVIDOR DEVE TER 3 ARGUMENTOS\n");
				exit(1);
			}
			else
			{
				if (strcmp(argv[1], "server") != 0 && strcmp(argv[1], "client") != 0)
				{
					printf("%s NAO EXISTE\n", argv[2]);
					exit(1);
				}

				nomeFicheiro = argv[3];
			}
		}
		else if (strcmp(argv[2], "get") == 0)// cliente deve ter 2 argumentos
		{
			if (argc != 3)
			{
				printf("SERVIDOR DEVE TER 3 ARGUMENTOS\n");
				exit(1);
			}
			else
			{
				if (strcmp(argv[1], "server") != 0 && strcmp(argv[1], "client") != 0)
				{
					printf("%s NAO EXISTE\n", argv[2]);
					exit(1);
				}
			}
		}
	}
	else
	{
		printf("FALTAM ARGUMENTOS AO PROGRAMA PRINCIPAL\n");
		exit(1);
	}
   setEmissorDevice(argv[1], argv[2]);

	//initLinkLayer(&lnk, device);
  	initAppLayer(&app);
	
	app.status = (eEmissor == 0) ? TRANSMITTER : RECEIVER ;
   app.fileDescriptor = llopen(device,app.status);

	if (app.fileDescriptor == -1)
	{
		printf("LLOPEN() COM ERROS\n");
		return(-1);
	}
	
	// enviar ao receptor trama de pacote de controlo a indicar inicio da transferecia
	// para cada fragmento, construir pacote e llwrite, donde constroi trama e a envia para o receptor
	// enviar ao receptor trama de pacote de controlo a indicar fin da transferecia

	// se ocorrer NUM_TRANSMISSIONS, fazer llclose()

	return(0);
}

/************************************************
		DEFINICAO DAS FUNCOES A IMPLEMENTAR
************************************************/
int llopen(int porta, size_t flag)
{
	int fd = 3; // diferente de 0 - read, 1 - write, 2 - erros
	char porta[11] = "/dev/ttyS0";

	if (porta == 4)
	{
		porta[9] = '4';
	}
	else if (porta != 0)
	{
		printf("LLOPEN() :: PORTA %d INVALIDA\n", porta);
		return(-1);
	}
	
	int fd = open(port, O_RDWR | O_NOCTTY ); // fd = number of file descriptor
    if (fd < 0)
    {
        perror(port);
        return -1;
    }
    
    return 0;

	int r = open(porta, O_RDWR | O_NOCTTY);
	if (r == -1)
	{
		printf("LLOPEN() :: FALHOU A CONEXAO\n");
		return(-1);
	}

   if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0.1;   /* inter-character timer set to 0.1 */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 chars received */


   tcflush(fd, TCIOFLUSH);

	if (eEmissor == TRANSMITTER)
	{
				
	}
	else if (eEmissor == RECEIVER)
	{
				
	}
	else
	{
		printf("LLOPEN() :: FLAG INVALIDA\n");
	}
	return(r);
}

int llwrite(int fd, char * buffer, int length)
{
	int r = 0;

	return(r);
}

int llread(int fd, char * buffer)
{
	int r = 0;

	return(r);
}

int llclose(int fd)
{
	int r = 0;

	return(r);
}
