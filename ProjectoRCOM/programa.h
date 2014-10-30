#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <math.h>

#include "definicoes.h"

/*****************************************
	VARIAVEIS GLOBAIS
*****************************************/
int eEmissor;
int device;

char * nomeFicheiro = "";
int tamFicheiro = 0;
FILE * f; // ficheiro a ler e guardar dados

AppLayer app;
LinkLayer lnk;

struct termios oldtermios, newtermios;
int contador = 0; //vezes que sooa o alarme

unsigned int STOP = FALSE; // condicao de paragem nas tentativas de envio e leitura
unsigned int KEEP = 0;
unsigned int flag = 1;

/*****************************************
	FUNCOES GLOBAIS
*****************************************/
					/* ARRANQUE */

void setEmissorDevice(char * arg1, char * arg2)
{
	if (strcmp(arg1, "server") == 0) // servidor pc local
	{
		device = 4; // aparelho destino
	}
	else // cliente pc local
	{
		device = 0;
	}

	printf("PROGRAMA PRINCIPAL :: ");
	if (strcmp(arg2, "send") == 0) // pc local a querer ser o emissor
	{
		eEmissor = TRANSMITTER;
		
		if (device == 4)
			printf("SERVIDOR :: ");
		else
			printf("CLIENTE :: ");
			
		printf("EMISSOR\n");
	}
	else // pc local a querer ser o receptor
	{
		eEmissor = RECEIVER;
		
		if (device == 4)
			printf("SERVIDOR :: ");
		else
			printf("CLIENTE :: ");
			
		printf("RECEPTOR\n");
	}
	printf("==============================================\n");
}

void set_newsettings(int fd, float vtime, int vmin) // mete settings novos na port
{
    if ( tcgetattr(fd,&oldtermios) == -1) // save current port settings
    {
        perror("tcgetattr");
        exit(1);
    }
    
    bzero(&newtermios, sizeof(newtermios));
    newtermios.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtermios.c_iflag = IGNPAR;
    newtermios.c_oflag = 0; //OPOST;
    
    /* set input mode (non-canonical, no echo,...) */
    newtermios.c_lflag = 0;
    
    //newtermios[VTIME]    = 0.1;   /* inter-character timer unused */
    newtermios.c_cc[VTIME]    = vtime;//3   /* inter-character timer unused */
    newtermios.c_cc[VMIN]     = vmin; //5  /* blocking read until 5 chars received */
    
    /*
     VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
     leitura do(s) prÛximo(s) caracter(es)
     */
    
    tcflush(fd, TCIOFLUSH);
    
    if ( tcsetattr(fd,TCSANOW,&newtermios) == -1) // set new port settings
    {
        perror("tcsetattr");
        exit(2);
    }
}

void set_oldsettings(int fd) // repõe os settings antigos da port
{
    if ( tcsetattr(fd,TCSANOW,&oldtermios) == -1)
    {
        perror("tcsetattr");
        exit(1);
    }
}

				/* LEITURA E ESCRITA DE DADOS */



int suDFA(unsigned char * trama, unsigned char Aesperado, unsigned char Cesperado)
{	
	int estado = 0;
	
	int i;
	for (i = 0; i < 5; i++)
	{
		switch(estado)
		{
			case 0:
				if (trama[i] == FLAG)
					estado = 1;	
				break;
			case 1:
				if (trama[i] == FLAG)
					estado = 1;
				else if (trama[i] == Aesperado)
					estado = 2;
				else
					estado = 0;	
				break;
			case 2:
				if (trama[i] == FLAG)
					estado = 1;
				else if (trama[i] == Cesperado)
					estado = 3;
				else
					estado = 0;	
				break;
			case 3:
				if (trama[i] == FLAG)
					estado = 1;
				else if (trama[i] == (Aesperado ^ Cesperado))
					estado = 4;
				else
					estado = 0;	
				break;
			case 4:
				if (trama[i] == FLAG)
					estado = 5;
				else
					estado = 0;
		}
	}	
	
	if (estado == 5)
	{
		return(0); // perfeito
	}
	else
	{
		return(-1);
	}
}

int iDFA(unsigned char * trama, int size, unsigned char Aesperado, unsigned char Cesperado)
{
	int estado = 0;
	
	int i;
	for (i = 0; i < 5; i++)
	{
		switch(estado)
		{
			case 0:
				if (trama[i] == FLAG)
					estado = 1;	
				break;
			case 1:
				if (trama[i] == FLAG)
					estado = 1;
				else if (trama[i] == Aesperado)
					estado = 2;
				else
					estado = 0;	
				break;
			case 2:
				if (trama[i] == FLAG)
					estado = 1;
				else if (trama[i] == Cesperado)
					estado = 3;
				else
					estado = 0;	
				break;
			case 3:
				if (trama[i] == FLAG)
					estado = 1;
				else if (trama[i] == (Aesperado ^ Cesperado))
					estado = 4;
				else
					estado = 0;	
				break;
		}
	}

	unsigned char bcc2 = xor(trama, 4, size);
	
	if (estado == 4) // bcc1 correcto
		if (bcc2 == trama[size - 2]) // bcc2 correcto
			return(0);
		else
			return(-2);
	else
		return(-1);
}

// le constantemente, ate obter uma trama, utilizada no dfa e verifica se se aceita ou nao

int lerComAlarme(int fd, unsigned char * trama)
{
	tcflush(fd, TCIFLUSH);
	
	STOP = FALSE;	
	
	unsigned char buf[5];
	int tamT = 0;
	int res = 0;
	
	//printf("Entrou no lerComAlarme()\n");
	
	//printf("%d %d\n", STOP, flag);
	
	strcpy(trama, "");
	while(STOP == FALSE && !flag)
	{
		//printf("ola\n");
		
		strcpy(buf, "");
		res = read(fd, buf, 1);
		
		//printf("cenas\n");
		
		if(res == -1)
		{
			printf("empacou\n");
			return(-1);
		}
		unsigned char LETRA = buf[0];
			
		if (tamT == 0) // nao ha conteudo no buf
		{
			if (LETRA == FLAG)
			{
				trama[tamT] = LETRA;
				tamT++;
			}
		}
		else
		{
			trama[tamT] = LETRA;
			tamT++;
			if (LETRA == FLAG)
			{
				STOP = TRUE;
			}
		}
		//printf("adeus\n");	
	}
	//printf("tera sido adeus?\n");
	
	if (flag == 1)
	{
		//printf("TIMEOUT\n");
		return(-1);
	}
		
	// uma vez lida a trama, verificar erros	
	return(tamT);
}

int lerSemAlarme(int fd, unsigned char * trama)
{
	tcflush(fd, TCIFLUSH);
	
	STOP = FALSE;	
	
	unsigned char buf[5];
	int tamT = 0;
	int res = 0;
	
	strcpy(trama, "");
	while(STOP == FALSE)
	{
		strcpy(buf, "");
		res = read(fd, buf, 1);
		unsigned char LETRA = buf[0];
			
		if (tamT == 0) // nao ha conteudo no buf
		{
			
			if (LETRA == FLAG)
			{
				//printf("ARRANCOU\n");
				trama[tamT] = LETRA;
				tamT++;
			}
		}
		else
		{
			//printf("%d\n", tamT);
			trama[tamT] = LETRA;
			tamT++;
			if (LETRA == FLAG)
			{
				//printf("STOP = TRUE\n");
				STOP = TRUE;
			}
		}	
	}
	return(tamT);
}

int enviar(int fd, unsigned char * frame, int size)
{
	tcflush(fd, TCOFLUSH);
	int res = write(fd, frame, size);
	//tcflush(fd, TCOFLUSH);
	
	return(res);
}

void alarm_handler() // handler para o sinal SIGALRM
{
	//STOP = FALSE;
	contador++;
	flag = 1;
	KEEP = 1;
	
	if (contador != (NUM_TRANSMITIONS))
	{
		printf("TENTATIVA %d\n", contador);
	}
}

// envia o pacote de dados
int enviarComAlarme(int fd, unsigned char * frame, int size, unsigned char * frameLido, unsigned char Aesperado, unsigned char Cesperado)
{
	(void) signal(SIGALRM, alarm_handler);  // instala rotina que atende interrupcao 
   contador = 0;
   
   int estadoEnviar = 0;
   int estadoReceber = 0;
   flag = 1;
	KEEP = 0;
	
	//printf("0x%X 0x%X\n", Aesperado, Cesperado);

   while ( contador < 3 )
   {
		//aqui
   	if ( flag == 1 )
   	{
			//printf("Alarme on depois do enter"); getchar();
   		alarm(TIMEOUT_LIMIT);
   		
   		estadoEnviar = enviar(fd, frame, size);
   		
   		/*
   		alarm(TIMEOUT_LIMIT);
   		
   		estadoEnviar = enviar(fd, frame, size);
   		
			flag = 0;
   		estadoReceber = lerComAlarme(fd, frameLido);
   		
   		if ( estadoReceber >= 0 )
   		{
   			alarm(0);
   			break;
   		}
   		
			*/
   		
   		KEEP = 0;
   		int estadoDFA = -2;
			//printf("Vai entrar no while do KEEP\n");
			
			flag = 0;
   		while(KEEP == 0)
   		{
   			KEEP = 0;
   			//printf("Ola %d %d\n", flag, KEEP);
				
   			estadoReceber = lerComAlarme(fd, frameLido);

				//printf("Estado Receber: %d\n", estadoReceber);

   			if ( estadoReceber >= 0 ) // se leu alguma coisa
   			{ 
					//printf("ESTADO >= 0\n");
  			
   				if (frameLido[0] == frameLido[4]) // frame SU
   				{
						//printf("FRAME SU\n");
   					estadoDFA = suDFA(frameLido, Aesperado, Cesperado);
   					//printf("Estado DFA: %d\n", estadoDFA);

						//verDados(frameLido, estadoReceber-1);

						//printf("0x%X 0x%X\n", frameLido[1], frameLido[2]);

   					if (estadoDFA == 0)
   					{
							//printf("alarm off\n");
   						KEEP = 1;
   						alarm(0); // termina o alarme
   					}
   					else
   					{
   						KEEP = 0; // volta a ler	
   					}
   				}
   				else // frame I
   				{
   					printf("FRAME I\n");
   					estadoDFA = iDFA(frameLido, estadoReceber, Aesperado, Cesperado);
   					
   					if (estadoDFA == 0)
   					{
   						KEEP = 1;
   						alarm(0);
   					}
   					else if (estadoDFA == -1) // bcc1 erro
   					{
   						KEEP = 0;
   					}
   					else //rej -revoltar a mandar
   					{
							alarm(0);
							flag = 1;
   						KEEP = 1;
   					}
   				}
   			}
   			else
   			{
   				if (flag == 1) //timeout limit
					{
						break;	
					}
					else
					{
						KEEP = 0;	
					}
   			}
   		} // fim do ciclo while
   		
   		if (estadoDFA == 0)
   			break;
   		
			//printf("Vai sair fora do ciclo WHILE do KEEP\n");
   	}
   }
   
   if (contador == (NUM_TRANSMITIONS))
   {
   	return -2; //-1 corresponde ao timeout limit
  	}
   
   return(estadoReceber); // tamanho da trama SU de resposta
}

int enviarSemAlarme(int fd, unsigned char * frame, int size)
{	
	return(enviar(fd, frame, size));
}

/*****************************************
	FUNCOES DA APLICACAO A UTILIZAR
*****************************************/
int llopen(int porta, size_t flag);
int llwrite(int fd, unsigned char * buffer, int length);
int llread(int fd, unsigned char * buffer);
int llclose(int fd);
