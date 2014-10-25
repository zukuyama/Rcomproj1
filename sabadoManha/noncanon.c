//
//  noncanon.c
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#include "noncanon.h"

/************************************************
 *							GLOBAL VARIABLES
 ************************************************/

/*
 is a qualifier that is applied to a variable when it is declared.
 It tells the compiler that the value of the variable may
 change at any time-without any action
 being taken by the code the compiler finds nearby.
 The implications of this are quite serious.
 */
volatile int STOP=FALSE;
volatile int estadoSET = FALSE;
volatile int estadoUA = FALSE;

int flag=1, tentativas=0;
struct termios oldtermios, newtermios;
int fd; // descritor de ficheiro

int open_nc(int * fd, const char * port) // abre descriptor da port em modo O_NOCTTY
{
    (*fd) = open(port, O_RDWR | O_NOCTTY ); // fd = number of file descriptor
    if ((*fd) < 0)
    {
        perror(port);
        return -1;
    }
    
    return 0;
}

void close_nc(int fd) // fecha descriptor
{
	 set_oldsettings(fd,&oldtermios);
    close_nc(fd);

    //return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
    //sleep (3);
}

void set_newsettings(int fd, struct termios * oldtermios, struct termios * newtermios) // mete settings novos na port
{
    if ( tcgetattr(fd,oldtermios) == -1) // save current port settings
    {
        perror("tcgetattr");
        exit(1);
    }
    
    bzero(newtermios, sizeof((*newtermios)));
    (*newtermios).c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    (*newtermios).c_iflag = IGNPAR;
    (*newtermios).c_oflag = 0; //OPOST;
    
    /* set input mode (non-canonical, no echo,...) */
    (*newtermios).c_lflag = 0;
    
    //newtermios[VTIME]    = 0.1;   /* inter-character timer unused */
    (*newtermios).c_cc[VTIME]    = 0.1;//3   /* inter-character timer unused */
    (*newtermios).c_cc[VMIN]     = 0; //5  /* blocking read until 5 chars received */
    
    /*
     VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
     leitura do(s) prÛximo(s) caracter(es)
     */
    
    tcflush(fd, TCIOFLUSH);
    
    if ( tcsetattr(fd,TCSANOW,newtermios) == -1) // set new port settings
    {
        perror("tcsetattr");
        exit(2);
    }
}

void set_newsettings_custom(int fd, struct termios * oldtermios, struct termios * newtermios, float vtime, int vmin) // mete settings novos na port
{
    if ( tcgetattr(fd,oldtermios) == -1) // save current port settings
    {
        perror("tcgetattr");
        exit(1);
    }
    
    bzero(newtermios, sizeof((*newtermios)));
    (*newtermios).c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    (*newtermios).c_iflag = IGNPAR;
    (*newtermios).c_oflag = 0; //OPOST;
    
    /* set input mode (non-canonical, no echo,...) */
    (*newtermios).c_lflag = 0;
    
    //newtermios[VTIME]    = 0.1;   /* inter-character timer unused */
    (*newtermios).c_cc[VTIME]    = vtime;//3   /* inter-character timer unused */
    (*newtermios).c_cc[VMIN]     = vmin; //5  /* blocking read until 5 chars received */
    
    /*
     VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
     leitura do(s) prÛximo(s) caracter(es)
     */
    
    tcflush(fd, TCIOFLUSH);
    
    if ( tcsetattr(fd,TCSANOW,newtermios) == -1) // set new port settings
    {
        perror("tcsetattr");
        exit(2);
    }
}

void set_oldsettings(int fd, struct termios * oldtermios) // repõe os settings antigos da port
{
    if ( tcsetattr(fd,TCSANOW,oldtermios) == -1)
    {
        perror("tcsetattr");
        exit(1);
    }
}

int read_cc(int fd, unsigned char cc) // lê um CC
{	 
	unsigned char bloco[5];
	bloco[0] = F;
   bloco[1] = A;
   bloco[2] = cc;
   bloco[3] = (bloco[1] ^ bloco[2]);
   bloco[4] = F;

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
		default : printf("Invalid CC\n");
	}

    int res = 0;
    unsigned char buf[5];
    unsigned char aux1[5];
    
    strcpy(buf, "");
    strcpy(aux1, "");
    
    tcflush(fd, TCIFLUSH);
    
    int i = 0;
    
    while (STOP==FALSE && !flag) 
    {
        res = read(fd, buf, 1);
        if (res != 0)
        {
            aux1[i] = buf[0];
            if (aux1[i] == bloco[4] && i != 0)
            {
                STOP = TRUE;
            }
            else
            {
                i++;
            }
        }
    }
    
    if (strlen(aux1) != 0)
    {
        printf("\nReceived: ");
        for (i = 0; i < strlen(aux1); i++)
        {
            printf("0x%x ", aux1[i]);
        } 
        printf("\n");
    }
    
    if(flag == 1)
    {
        return -1;
    }
    
    return 0;
}

int write_cc(int fd, unsigned char cc) // escreve o SET
{
	unsigned char bloco[5];
	bloco[0] = F;
   bloco[1] = A;
   bloco[2] = cc;
   bloco[3] = (bloco[1] ^ bloco[2]);
   bloco[4] = F;

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
		default : printf("Invalid CC\n");
	}

   tcflush(fd, TCOFLUSH);
    
   write(fd, bloco, 5);
    
   return 0;
}

void alarm_handler() // handler para o sinal SIGALRM
{
    char buf[255];
    
    //texto("Chamada atendida.\n");
    //getchar();
    flag = 1;
    STOP = FALSE;
    
    tentativas++;
    if (tentativas != 3)
    {
        //printf("\nTentativa %d...\n", tentativas);
        
        //strcpy(buf, "");
        //sprintf(buf, "\nTentativa %d...\n", tentativas);
        //write(STDOUT_FILENO, buf, sizeof(buf));
	
	printf("\nTentativa %d...\n", tentativas);
    }
}

int send_nc(int fd)
{
    (void) signal(SIGALRM, alarm_handler);  // instala rotina que atende interrupcao
    
    init_SET_and_UA(); // define SET e UA
    
    system("clear");
    
    //set_newsettings_custom(fd, &oldtermios, &newtermios, 0.1, 0); 
    set_newsettings_custom(fd, &oldtermios, &newtermios, 0.1, 0); // atribui o descritor do ficheiro de serial port
    
    while ( tentativas < 3 )
    {
        if ( flag == 1 )
        {
            alarm(3);
            write_cc(fd,C_SET);
            flag = 0;
            
            if ( read_cc(fd,C_UA) == 0 )
            {
                break;
            }
        }
    }
    
    if (tentativas == 3)
    {
        printf("\nExcedido o tempo limite\n");
        return -1;
    }
    
    printf("\nConexao estabelecida e garantida\n");
    
    return fd;
    
}

int receive_nc(int fd)
{
    int res=0;
    unsigned int x;
    unsigned char buf[5];
    
    init_SET_and_UA(); // define SET e UA
    
    system("clear");
    
    //set_newsettings_custom(fd, &oldtermios, &newtermios, 0, 1); 
    set_newsettings_custom(fd, &oldtermios, &newtermios, 0, 5); // atribui o descritor do ficheiro de serial port
    
    int phase=0;
    unsigned int pos = 0;
    
    // receber set
    printf("Receiving SET...\n");
    while ( STOP==FALSE ) /* loop for input */
    {
        res = read(fd,&buf,1);   /* returns after 5 chars have been input */
	
	unsigned char LETRA = buf[0];		
	
		switch(phase)
		{
		case 0:
                	if (LETRA == set[0]) 
		  	{ phase=1; }
                	break;
            	case 1: 
                	if (LETRA == set[1]) 
		  	{ phase=2; }
                	else if (LETRA == set[0]) 
		       	{ phase=1; }
		      	else 
		       	{ phase=0; }
                	break;
            	case 2: 
                	if (LETRA == set[2])
		  	{ phase=3; }
                	else if (LETRA == set[0])
			{ phase=1; }
                     else
                        { phase=0; }
                	break;
            	case 3:
               		if (LETRA == set[3])
                   	{ phase=4; }
                	else if(LETRA == set[0])
                       { phase=1; }
		      	else
                        { phase=0; }
               		break;
            	case 4:
               		if (LETRA == set[4])
			{
				phase = 6;
				STOP = TRUE;
			}
                	else
                    		{ phase=0; }
                	break;
        	
	}	
	
	printf("HEXACODE: 0x%x\n", LETRA);

	// chegou ao fim do maquina de estados
	if (phase == 6) 
	{
		// enviar ua
           	 printf("SET Received... Sending UA...\n");
            
            	tcflush(fd, TCOFLUSH);
            
            	//enviar ua
            	res = write(fd, ua, 5);
            	STOP = TRUE;
	}
    }
    
    printf("UA sent with %d Bytes.\n", res);
    tcflush(fd, TCOFLUSH);
    
    if (tentativas == 3)
    {
        printf("\nExcedido o tempo limite\n");
        return -1;
    }
    
    printf("\nConexao estabelecida e garantida\n");
    
    return 0;
    
}



