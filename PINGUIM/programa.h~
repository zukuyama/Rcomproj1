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
struct termios oldtermios, newtermios;

int eEmissor;
int device;

char nomeFicheiro[30];
int tamFicheiro = 0;
FILE * f; // ficheiro a ler e guardar dados

AppLayer app;
LinkLayer lnk;

int contador = 0; //vezes que sooa o alarme
int num_trans = 0; // numero de transmissoes

unsigned int STOP = FALSE; // condicao de paragem nas tentativas de envio e leitura
unsigned int KEEP = 0; // condicao de manter a trama ou nao, segundo o bcc1
unsigned int flag = 1;

unsigned char TRAMA[MTS];
unsigned char TRAMA2[MTS];
unsigned char PACOTE[MTS];
unsigned char DADOS[MFS+1];

int tamTotal = 0;
int numTramasI_IO = 0, numTramasI_re = 0, numTimeouts = 0, numIO_REJ = 0;

/*****************************************
	FUNCOES GLOBAIS
*****************************************/
					/* ARRANQUE */

void setEmissorDevice(char * arg1, char * arg2)
{
	if (strcmp(arg1, "server") == 0) // servidor pc local
	{
		device = 0; // aparelho destino
	}
	else // cliente pc local
	{
		device = 4;
	}

	printf("PROGRAMA PRINCIPAL :: ");
	if (strcmp(arg2, "send") == 0) // pc local a querer ser o emissor
	{
		eEmissor = TRANSMITTER;
		
		if (device == 4)
			printf("CLIENTE :: ");
		else
			printf("SERVIDOR :: ");
			
		printf("EMISSOR\n");
	}
	else // pc local a querer ser o receptor
	{
		eEmissor = RECEIVER;
		
		if (device == 4)
			printf("CLIENTE :: ");
		else
			printf("SERVIDOR :: ");
			
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
    newtermios.c_cflag = lnk.baudrate | CS8 | CLOCAL | CREAD;
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

int suuDFA(unsigned char trama[MTS], unsigned char Aesperado, unsigned char Cesperado)
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

int iiDFA(unsigned char trama[MTS], int size, unsigned char Aesperado, unsigned char Cesperado)
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

	unsigned char bcc2 = xoor(trama, 4, size-2);	
	
	//printf("bcc2: 0x%X\n", bcc2);
	//printf("BCC2: 0x%X\n", trama[size - 1]);
	if (estado == 4) // bcc1 correcto
		if (bcc2 == trama[size - 1]) // bcc2 correcto
			return(0);
		else
			return(-2);
	else
		return(-1);
}

int maquinaEstados(unsigned char trama[MTS], int size, unsigned char Aesperado, unsigned char Cesperado)
{
	if (size < 4)
	{
		printf("Tamanho inferior a 5!\n");
		//tcflush(app.fileDescriptor, TCIOFLUSH);
		return(-3);
	}
	
	if (trama[0] == trama[4]) // trama SU
		return(suuDFA(trama, Aesperado, Cesperado));
	else
		return(iiDFA(trama, size, Aesperado, Cesperado));
}

void ctrlC_handler() // handler para o sinal SIGALRM
{
	printf("\nCTRL C ACTIVADO\n");
	set_oldsettings(app.fileDescriptor);
	close(app.fileDescriptor);
	exit(1);
}

void alarm_handler() // handler para o sinal SIGALRM
{
	//STOP = FALSE;
	contador++;
	numTimeouts++;
	
	flag = 1;
	KEEP = 1;
	
	if (contador != (lnk.numTransmissions))
	{
		printf("RETRANSMISSAO DA TRAMA PELA %do VEZ\n", contador);
	}
}

// le constantemente, ate obter uma trama, utilizada no dfa e verifica se se aceita ou nao

int leerComAlarme(int fd, unsigned char trama[MTS])
{
	tcflush(fd, TCIFLUSH);
	
	STOP = FALSE;	
	
	unsigned char buf[5];
	unsigned char LETRA = 0x00;
	int tamT = 0;
	int res = 0;
	int aux = 0;
	
	//printf("Entrou no lerComAlarme()\n");
	
	//printf("%d %d\n", STOP, flag);
	
	limparFrame(trama);
	while(STOP == FALSE && !flag)
	{
		//printf("ola\n");
		
		strcpy(buf, "");
		res = read(fd, buf, 1);
		//printf("cenas\n");
		
		if(res == -1)
		{
			return(-1);
		}
		LETRA = buf[0];
			
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
				//printf("STOP = TRUE\n");
				if (tamT >= 4)
					STOP = TRUE;
				else
				{
					aux = 0;
					for (aux = 1; aux <= tamT; aux++)
					{
						trama[aux] = '\0';
						tamT = 1;
					}
				}
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

int leerSemAlarme(int fd, unsigned char trama[MTS])
{
	tcflush(fd, TCIOFLUSH);
	
	STOP = FALSE;	
	
	unsigned char buf[5];
	unsigned char LETRA;	
	int tamT = 0;
	int res = 0;
	int aux = 0;
	
	limparFrame(trama);	

	while(STOP == FALSE)
	{
		strcpy(buf, "");
		res = read(fd, buf, 1);
		LETRA = buf[0];
			
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
			//printf("0x%X ", LETRA);

			//printf("%d ", tamT);
			//if (tamT % 10 == 0) { printf("\n"); getchar(); }

			trama[tamT] = LETRA;
			tamT++;
			if (LETRA == FLAG)
			{
				//printf("STOP = TRUE\n");
				if (tamT >= 4)
					STOP = TRUE;
				else
				{
					for (aux = 1; aux <= tamT; aux++)
					{
						trama[aux] = '\0';
						tamT = 1;
					}
				}
			}
		}	
	}
	return(tamT);
}

int enviarr(int fd, unsigned char frame[MTS], int size)
{
	tcflush(fd, TCOFLUSH);
	int res = write(fd, frame, size);
	//tcflush(fd, TCOFLUSH);
	
	return(res);
}

int enviarTramaI(int fd, unsigned char frame[MTS], int size, int * rr_rej,
	unsigned char frameLido[MTS], unsigned char C_RResperado, unsigned char C_REJesperado)
{
	(void) signal(SIGALRM, alarm_handler);  // instala rotina que atende interrupcao 
   
   int estadoEnviar = 0;
   int estadoReceber = 0;
   flag = 1;
	KEEP = 0;
	
	//printf("0x%X 0x%X\n", Aesperado, Cesperado);
	int estadoDFA_RR = -2;
	int estadoDFA_REJ = -2;
	int estadoDFA = 0;

	numTramasI_re--;
   while ( contador < lnk.numTransmissions )
   {
   
   	numTramasI_re++;
		//aqui
   	if ( flag == 1 )
   	{
		//printf("Alarme activado\n");
   		alarm(lnk.timeout);
   		
   		estadoEnviar = enviarr(fd, frame, size);
   		
   		KEEP = 0;
   		
			//printf("Vai entrar no while do KEEP\n");
			
			flag = 0;
   		while(KEEP == 0) // -1
   		{
   			KEEP = 0;
   			//printf("Ola %d %d\n", flag, KEEP);
   			
				estadoDFA = -2;
				limparFrame(frameLido);
   			estadoReceber = leerComAlarme(fd, frameLido);
   			//printf("Ola %d %d\n", flag, KEEP);

				//verDados(frameLido, estadoReceber);
				//printf("Estado Receber: %d\n", estadoReceber);

   			if ( estadoReceber >= 0 ) // se leu alguma coisa
   			{
   				//printf("%d\n", estadoReceber);
					//printf("ESTADO >= 0\n");
  					//printf("0x%X 0x%X\n", frameLido[0], frameLido[4]);
  					
				 	estadoDFA_RR = maquinaEstados(frameLido, estadoReceber, A1, C_RResperado);
					estadoDFA_REJ = maquinaEstados(frameLido, estadoReceber, A1, C_REJesperado);				
					
					if (estadoDFA_RR == 0)
					{
						KEEP = 1;
   					alarm(0);
						*rr_rej = 0; // rr
						estadoDFA = 0;
					}
					else
					{
						if (estadoDFA_REJ == 0)
						{
							KEEP = 1;
   						alarm(0);
							*rr_rej = 1; // rej
							estadoDFA = 0;
						}
						else
						{
							KEEP = 0; // ignorar por serem -1
						}
					}
   			}
   			else
   			{
   				if (flag == 1) //timeout limit
					{
						//printf("aqui\n");
						break;	
					}
					else
					{
						printf("entras aqui ? \n"); //apagar esta linha antes de entregar o trabalho
						KEEP = 0;	
					}
   			}
   		} // fim do ciclo while   		
		
		if (estadoDFA == 0) break;

		//printf("Vai sair fora do ciclo WHILE do KEEP\n");
   	}
   }
   
   if (contador == (lnk.numTransmissions))
   {
   	printf("NUMERO DE RETRANSMISSOES COM TEMPORIZADOR EXCEDIDO\n");
		*rr_rej = -1;
   	return -2; //-1 corresponde ao timeout limit
  	}
   
   return(estadoReceber); // tamanho da trama SU de resposta
}

int enviarrComAlarme(int fd, unsigned char frame[MTS], int size,
	unsigned char frameLido[MTS], unsigned char Aesperado, unsigned char Cesperado)
{
	(void) signal(SIGALRM, alarm_handler);  // instala rotina que atende interrupcao 
   contador = 0;
   
   int estadoEnviar = 0;
   int estadoReceber = 0;
   flag = 1;
	KEEP = 0;
	
	//printf("0x%X 0x%X\n", Aesperado, Cesperado);
	int estadoDFA = -2;
	
   while ( contador < lnk.numTransmissions )
   {
		//aqui
   	if ( flag == 1 )
   	{
		//printf("Alarme activado\n");
   		alarm(lnk.timeout);
   		
   		estadoEnviar = enviarr(fd, frame, size);
   		
   		KEEP = 0;
   		
			//printf("Vai entrar no while do KEEP\n");
			
			flag = 0;
   		while(KEEP == 0)
   		{
   			KEEP = 0;
   			//printf("Ola %d %d\n", flag, KEEP);
   			
			estadoDFA = -2;
			limparFrame(frameLido);
   			estadoReceber = leerComAlarme(fd, frameLido);
   			//printf("Ola %d %d\n", flag, KEEP);

				//verDados(frameLido, estadoReceber);
				//printf("Estado Receber: %d\n", estadoReceber);

   			if ( estadoReceber >= 0 ) // se leu alguma coisa
   			{
   				//printf("%d\n", estadoReceber);
					//printf("ESTADO >= 0\n");
  					//printf("0x%X 0x%X\n", frameLido[0], frameLido[4]);
  					
  					estadoDFA = maquinaEstados(frameLido, estadoReceber, Aesperado, Cesperado);
  					
  					if (estadoDFA == 0)
  					{
  						KEEP = 1;
   						alarm(0);
						//printf("1\n");
  					}
  					else if (estadoDFA == -1)
  					{
						
  						KEEP = 0;
						//printf("2\n");
  					}
   			}
   			else
   			{
   				if (flag == 1) //timeout limit
					{
						//printf("aqui\n");
						break;	
					}
					else
					{
						printf("entras aqui ? \n");
						KEEP = 0;	
					}
   			}
   		} // fim do ciclo while   		
		
		if (estadoDFA == 0) break;

		//printf("Vai sair fora do ciclo WHILE do KEEP\n");
   	}
   }
   
   if (contador == (lnk.numTransmissions))
   {
   	printf("NUMERO DE RETRANSMISSOES COM TEMPORIZADOR EXCEDIDO\n");
   	return -2; //-1 corresponde ao timeout limit
  	}
   
   return(estadoReceber); // tamanho da trama SU de resposta
}

int enviarrSemAlarme(int fd, unsigned char frame[MTS], int size)
{	
	return(enviarr(fd, frame, size));
}

/*****************************************
	FUNCOES DA APLICACAO A UTILIZAR
*****************************************/
int llopen(int porta, size_t flag);
int llwrite(int fd,  char * buffer, int length);
int llread(int fd,  char * buffer);
int llclose(int fd);
int funcaoIO();
void estatistica();
