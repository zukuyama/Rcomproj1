#ifndef DEFINICOES_H
#define DEFINICOES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define TIMEOUT_LIMIT 3
#define NUM_TRANSMITIONS 3

#define TRANSMITTER 1
#define RECEIVER 0

#define FALSE 0
#define TRUE 1

#define PRONTO 0xff
#define DADOS 0xfe

#define FLAG 0x7e
#define ESC 0x7d
#define Fstuffing 0x5e
#define ESCstuffing 0x5d

// campos de endereco
#define A1 0x03
#define A2 0x01

#define C_S_0 0x00
#define C_S_1 0x40

#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0b

#define C_RR_0 0x05
#define C_RR_1 0x85
#define C_REJ_0 0x01
#define C_REJ_1 0x81

#define MFS 256
#define MTS 1024

#define Cpkg_dados 0x01
#define Cpkg_inicio 0x02
#define Cpkg_fim 0x03

/***********************************************************************************************
	ESTRUTURAS DE DADOS
***********************************************************************************************/

/*	Application */
typedef struct _app
{
    int fileDescriptor;
    int status;
} AppLayer;

void setAppLayer(AppLayer * app, int f, int s)
{
    app->fileDescriptor = f;
    app->status = s;
}

void initAppLayer(AppLayer * app)
{
	app->fileDescriptor = 0;
	app->status = 0;
}

/*	Link */
typedef struct _link
{
    char port[20]; // dispositivo /dev/ttySx , com x = {0,1,2,3,4,5} // esq: 4, dir: 0
    int baudrate; // velocidade de transmissao
    unsigned int sequenceNumber; // numero de sequencia da trama: 0 ou 1
    unsigned int timeout; // valor do temporizador (segundos): ex: 3
    unsigned int numTransmissions; //numero de tentativas em caso de falha
    char frame[MTS]; // trama
} LinkLayer;

void setLinkLayer(LinkLayer * link, char p[20], int b, unsigned int s, unsigned int t, unsigned int n, char f[MTS])
{
    strcpy(link->port, p);
    link->baudrate = b;
    link->sequenceNumber = s;
    link->timeout = t;
    link->numTransmissions = n;
    strcpy(link->frame, f);
}

void setLinkLayer2(LinkLayer * link, unsigned int s, char f[MTS])
{
	link->sequenceNumber = s;
	strcpy(link->frame, f);
}

void initLinkLayer(LinkLayer * link, char p[20])
{
    strcpy(link->port, p);
    link->baudrate = BAUDRATE;
    link->sequenceNumber = 0;
    link->timeout = TIMEOUT_LIMIT;
    link->numTransmissions = NUM_TRANSMITIONS;

    strcpy(link->frame, "");
}



/***********************************************************************************************
	MAQUINA DE ESTADOS EM TRABALHOS
 ***********************************************************************************************/



int indice,maxbuffer;
unsigned char * buffer;

indice = 0;
maxbuffer = 5;
buffer = malloc( sizeof(unsigned char) * maxbuffer );

unsigned char estado = PRONTO;
unsigned char bcc1   = 0;
unsigned char bcc2   = 0;

int dados_inicio, dados_fim;
dados_inicio = 4;
dados_fim = 4;

int is_comando(unsigned char x)
{
    return ( x == C_SET || x == C_DISC );
}

int is_resposta(unsigned char x)
{
    return ( x == C_RR_0 || x == C_RR_1 || x == C_REJ_0 || x == C_REJ_1 || x == C_UA );
}


int maquina_de_estados(unsigned char x)
{
    int flag = 0;
    
    switch ( estado )
    {
        // estado inicial
        case PRONTO :  if ( x == F ) { estado = F; flag=1; } break;
            
        // F
        case F      :  if ( x == A1 || x == A2 ) { estado=x; flag=1; } break;
            
        // A
        case A1
        case A2     :  if ( is_comando(x) ) { estado=x; bcc1=(buffer[1] ^ x); flag=1; } break;
            
        // C , falta incluir e concluir outros comandos/respostas
        case C_SET  :
        case C_DISC :  if ( x == bcc1 ) { estado=x; flag=1; } break;
            
        case C_RR_0 :
        case C_RR_1 :
        case C_REJ_0:
        case C_REJ_1:  break;
            
        // BCC1 , ??? este tem mais que se lhe diga, falta completar/corrigir
        case bcc1   :  if ( is_resposta(x) ) { estado=x; flag=1; } break;
            
        // DADOS , se entrou aqui então deve continuar a ler até F
        case DADOS  :  if ( x == F ) { estado=PRONTO; bcc2 = xor(buffer,dados_inicio,dados_fim-1); flag=(bcc2==buffer[dados_fim])?1:0; }
                       else { dados_fim++; }
                       break;

    }
    
    if ( flag || estado == DADOS ) // se avançar de estado ou estiver a ler dados
    {
        if ( indice >= maxbuffer ) // realocar mediante o crescimento da trama
        {
            maxbuffer = indice + 1;
            if (maxbuffer > 1024) /*free(buffer); exit();*/; // EXCEDE TAMANHO PERMITIDO: erro+termina ou dropTrama
            
            buffer = realloc( buffer, sizeof(unsigned char) * maxbuffer );
        }
        
        buffer[indice] = x;
        indice++;
        
        return TRUE;
    }
    else // se recuar de estado ou falhar
    {
        estado = F;
        
        // se falhar faz reset ao buffer e variáveis relativas
        indice = 0;
        maxbuffer = 5;
        buffer = realloc( buffer, sizeof(unsigned char) * maxbuffer );
        
        bcc1 = 0;
        bcc2 = 0;
        
        dados_inicio = 4;
        dados_fim = 4;
        
        return FALSE;
    }
    
    return FALSE;
}

void teste(int fd) // isto nao da para testar ainda, mas esta é a ideia de como usar a maquina de estados
{
    unsigned char x;
    
    while (  )
    {
        read(fd,&x,1);
        resultado = maquina_de_estados(x);
        processa_resultado(fd,resultado);    // processa e envia respostas ???
    }
}


/***********************************************************************************************
	FUNCOES AUXILIARES AO TRATAMENTO DE DADOS
***********************************************************************************************/
int tamanhoFicheiro(char * str)
{
	int r = 0;

	FILE * f;
	f = fopen(str, "rb");
	
	fseek(f, 0L, SEEK_END);
	r = ftell(f);

	fclose(f);
	return(r);
}

int numDigitos(int a)
{
	int s = 0;
	int v = a;
	while(v > 0)
	{
		v /= 10;
		s++;
	}
	return(s);
}

char * toString(int num) // contem caracteres latinos, terminando em '\0'
{
	int dig = numDigitos(num);
	char * r;
	r = (char*) malloc(sizeof(char) * (dig+1) );

	int i;
	int v = num;
	
	for(i = 0; i <= dig; i++)
	{
		r[i] = '\0';
	}

	for (i = (dig-1); i >= 0; i--)
	{
		int x = v % 10;
		v /= 10;

		r[i] = (char)x;
	}

	return(r);
}

void colocarFicheiro(FILE * ficheiro, unsigned char * content, int size) // ficheiro aberto em modo "w" ou "w+"
{
	int k;
	for (k = 0; k < size; k++)
	{
		fputc(content[k], ficheiro);		
	}
}

void verDados(unsigned char * str, int size)
{
	int i ;
	for (i = 0; i <= size; i++)
	{
		printf("0x%X ", str[i]);
		if ( (i + 1) % 25 == 0)
			printf("\n");
	} printf("\n\n");
}

unsigned char xor(unsigned char * dados, int xmin, int xmax) // obtem bcc2 da trama entre xmin e xmax
{
	unsigned char r = dados[xmin];
	int i;	
	for (i = (xmin+1); i <= xmax; i++)
	{
		r = (r ^ dados[i]);
	}
	return(r);
}

/***********************************************************************************************
	FUNCOES TRATAMENTO DE DADOS
***********************************************************************************************/
unsigned char * tramaI_toPacote(unsigned char * frame, int size, int * tamPacote);
unsigned char * tramaI(unsigned char * pkg, int size, int * tamTrama);
void            obterTramaSU(unsigned char * frame);
unsigned char * tramaSU(int emissor, int numSeq, int qualC); // set, ua, disc, rr, rej

void            despacotarControlo(unsigned char * pkg, int size);
unsigned char * pacotarControlo( char * nomeFicheiro, int tamFicheiro, int modo, int * tamPacote);
unsigned char * despacotarDados(unsigned char * pkg, int size, int * tamFrag);
unsigned char * pacotarDados(unsigned char * frag, int numSeq, int size, int * tamPacote);

/*======================================================
*		FUNCOES AO NIVEL DO LINK LAYER
=======================================================*/

unsigned char * tramaI_toPacote(unsigned char * frame, int size, int * tamPacote)
{
	printf("-------------------------\nFRAME I UNBUILDER\n-------------------------\n");
	
	unsigned char * pkg = 
		(unsigned char*) malloc(sizeof(unsigned char) * (size));
	
	//printf("Tamanho da trama: %d\n", size);
	//verDados(frame, size);
		
	unsigned char N = frame[0];
	unsigned char A = frame[1];
	unsigned char C = frame[2];
	unsigned char Bcc1 = (A ^ C);
	
	printf("[0x%X, 0x%X, 0x%X, 0x%X, ..., 0x%X, 0x%X]\n", N, A, C, Bcc1, frame[size-1], frame[size]);
	
	if (Bcc1 != frame[3])
		printf("errou\n");
	
	printf("Tamanho do trama I: %d\n", size);
	
	int limite = size - 2;
	int i;

	for (i = 4; i <= limite; i++)
	{
		pkg[i - 4] = frame[i];
	}
	*tamPacote = (limite - 4);
	
	printf("PKG[0]: 0x%X\n", pkg[0]);
	printf("PKG[1]: 0x%X\n", pkg[1]);
	printf("PKG[2]: 0x%X\n", pkg[2]);
	printf("PKG[3]: 0x%X\n", pkg[3]);
	printf("\n");
	
	//verDados(pkg, (*tamPacote));
	
	return(pkg);
}

// frag: [0..MFS], pkg: [0.. MFS+4[, 

unsigned char * tramaI(unsigned char * pkg, int size, int * tamTrama)
{
	unsigned char * trama = 
		(unsigned char*) malloc(sizeof(unsigned char) * (MTS));
		
	printf("-------------------------\nFRAME I BUILDER\n-------------------------\n");
	//printf("Tamanho do pacote: %d\n", size);
	//verDados(pkg, size);
	
	int si = 0; //size of trama

	trama[0] = FLAG;
	trama[1] = 0x03;

	if(pkg[0] == 0x01) // pacote de dados
	{
		int num = (int) pkg[1];
		if ( num % 2 == 0) // se pkg.N par
		{
			trama[2] = 0x00;	
		}
		else
		{
			trama[2] = 0x40;	
		}
	}
	else // pacote de controlo
	{
		trama[2] = 0x00;
	}
	
	// bcc1
	trama[3] = (trama[1] ^ trama[2]);

	int i;
	
	for (i = 0; i <= size; i++)
	{
		trama[i + 4] = pkg[i];
	}
	i = 5 + size;
		
	// bcc2
	trama[i] = xor(pkg, 0, (size+0));
	i++;
	trama[i] = FLAG;
	i++;
	
	*tamTrama = (i - 1);

	printf("Tamanho trama I: %d\n", (*tamTrama));

	//verDados(trama, *tamTrama);

	//frameUnbuilderI(trama, tamTrama);
	
	printf("[0x%X, 0x%X, 0x%X, 0x%X, ..., 0x%X, 0x%X]\n", trama[0], trama[1], trama[2], trama[3], trama[(*tamTrama)-1], trama[(*tamTrama)]);
	
	return(trama);
}

void obterTramaSU(unsigned char * frame)
{
	printf("-------------------------\nFRAME SU UNBUILDER\n-------------------------\n");
	unsigned char F = frame[0];
	unsigned char A = frame[1];
	unsigned char C = frame[2];
	unsigned char Bcc1 = (A ^ C);
	unsigned char Ffinal = frame[4];

	if (Bcc1 != (frame[1] ^ frame[2]))
		printf("Erro\n");

	printf("[0x%X, 0x%X, 0x%X, 0x%X, 0x%X]\n",
	F, A, C, Bcc1, Ffinal);
}

unsigned char * tramaSU(int emissor, int qualC, int numSeq) // tem sempre 5 bytes
{
	printf("-------------------------\nFRAME SU BUILDER\n-------------------------\n");
	unsigned char * trama = 
		(unsigned char*) malloc(sizeof(unsigned char) * 5);
	
	trama[0] = FLAG;
	
	switch(qualC)
	{
		case 0: // SET -> comando
			trama[2]	= 0x03;
			trama[1] = (emissor == 1) ? 0x03 : 0x01;
			break;
		case 1: // DISC -> comando
			trama[2]	= 0x0b;
			trama[1] = (emissor == 1) ? 0x03 : 0x01;		
			break;
		case 2: // UA -> resposta
			trama[2]	= 0x07;
			trama[1] = (emissor == 1) ? 0x01 : 0x03;		
			break;
		case 3: // RR -> resposta
			if ((numSeq % 2) == 0)
			{
				trama[2]	= 0x85;
			}
			else
			{
				trama[2]	= 0x05;			
			}
			trama[1] = (emissor == 1) ? 0x01 : 0x03;		
			break;
		case 4: // REJ -> resposta
			if ((numSeq % 2) == 0)
			{
				trama[2]	= 0x81;
			}
			else
			{
				trama[2]	= 0x01;			
			}
			trama[1] = (emissor == 1) ? 0x01 : 0x03;		
			break;
	}
	trama[3] = (trama[1] ^ trama[2]);
	trama[4] = FLAG;

	printf("[0x%X, 0x%X, 0x%X, 0x%X, 0x%X]\n",
		trama[0], trama[1], trama[2], trama[3], trama[4]);
	
	//verDados(trama, 5);
	
	return(trama);
}

/*======================================================
*		FUNCOES AO NIVEL DA APPLICATION LAYER
=======================================================*/
void despacotarControlo(unsigned char * pkg, int size)
{
	printf("-------------------------\nCONTROL UNPACKET\n-------------------------\n");
	
	unsigned char C = pkg[0];
	unsigned char T1 = 0x00, T2 = 0x00;
	int vec1 = 0, vec2 = 0;

	unsigned char nomeFicheiro[255];
	int tamFicheiro = 0;

	int i = 0, tf = 0, tn = 0;
	int ref = 0;

	T1 = pkg[1];
	vec1 = (int) pkg[2];
	
	ref = 3;
	for (i = 0; i < vec1; i++)
	{
		int t = (int) pkg[i + ref];
		tamFicheiro = tamFicheiro * 10 + t;
	}
	
	ref += vec1;
	T2 = pkg[ref];
	vec2 = (int) pkg[ref+1];
	ref += 2;
 
	for (i = 0; i < vec2; i++)
	{
		nomeFicheiro[i] = (char)pkg[i + ref];
	}
	nomeFicheiro[vec2] = '\0';

	printf("[0x%X, 0x%X, %d, ..., 0x%X, %d, ...]\n\n", C, T1, vec1, T2, vec2);
	printf("Tamanho do ficheiro: %d bytes\n", tamFicheiro);
	printf("Nome do ficheiro: %s\n", nomeFicheiro);

	// armazenar tamanho e nome do ficheiro no lado do llread() 
}

unsigned char * pacotarControlo( char * nomeFicheiro, int tamFicheiro, int modo, int * tamPacote)
{
	//controlPacket(int modo, char * str_file, int tam_file)

	printf("===========================\nCONTROL PACKET\n-------------------------\n");
	int comprimentoFile = strlen(nomeFicheiro);
	int comprimentoTamanho = numDigitos(tamFicheiro);

	int i, j;
	int total =
		1 +
		1 + 1 + comprimentoFile + 
		1 + 1 + comprimentoTamanho;	

	unsigned char * pkg = (unsigned char *)
		malloc(sizeof(unsigned char) * (total+1));

	strcpy(pkg, "");
	
	i = 0;
	if (modo == 2) // 0x01 -> inicio de transmissao
	{
		pkg[i] = 0x02;
		i++;
	}
	else if (modo == 3)  // 0x02 -> fim de transmissao
	{
		pkg[i] = 0x03;
		i++;
	}

	printf("[0x%X, ", pkg[i-1]);	
	
	pkg[i] = 0x00; // tamanho do ficheiro
	i++;

	printf("0x%X, ", pkg[i-1]);

	pkg[i] = (char) comprimentoTamanho;
	i++;
	
	printf("%d, ", comprimentoTamanho);

	char * s = toString(tamFicheiro);

	for (j = 0; j < comprimentoTamanho; j++)
	{
		pkg[i] = s[j];
		i++;
	}

	pkg[i] = 0x01; // nome do ficheiro
	i++;

	printf("..., 0x%X, ", pkg[i-1]);

	pkg[i] = (unsigned char) comprimentoFile;
	i++;

	for (j = 0; j < comprimentoFile; j++)
	{
		pkg[i] = nomeFicheiro[j];
		
		i++;
	}
	
	printf("%d, ...]\n", j);

	*tamPacote = i - 1;
	printf("Tamanho do pacote: %d\n\n", *tamPacote - 1);
		
	return(pkg);
}

unsigned char * despacotarDados(unsigned char * pkg, int size, int * tamFrag)
{
	printf("-------------------------\nDATA UNPACKET\n-------------------------\n");
	
	printf("Tamanho do pacote: %d\n", size);
	//verDados(pkg, size);
	
	unsigned char * data = (unsigned char *)
		malloc(sizeof(unsigned char) * (MFS+1));
	
	unsigned char C = pkg[0];
	unsigned char N = pkg[1];

	unsigned char L2 = pkg[2];
	unsigned char L1 = pkg[3];

	int i = 4, j = 0;

	int NN = N;
	int LL2 = (int) L2;
	int LL1 = (int) L1;

	printf("[C, N, L2, L1]: [0x%X, 0x%X, 0x%X, 0x%X]\n", C, NN, LL2, LL1);
	
	int tamFragmento = LL2*MFS + LL1;
	int tt = size - 4;

	printf("Tamanho do fragmento (aS): %d\n", tt);
	printf("Tamanho do fragmento teorico (LL2*MFS + LL1): %d\n", tamFragmento);

	for (i = 4; i <= size; i++)
	{
		if (pkg[i] == ESC)
		{
			i++;
			if (pkg[i] == 0x5e) // flag
			{
				data[j] = 0x7e;
				j++;
			}
			else if(pkg[i] == 0x5d) // esc
			{
				data[j] = 0x7d;
				j++;
			}
		}
		else
		{
			data[j] = pkg[i];
			j++;
		}
	}
	
	*tamFrag = (j - 1);
	
	printf("Tamanho do fragmento: %d\n\n", (*tamFrag));
	
	//verDados(data, *tamFrag);
	
	return(data);
}

unsigned char * pacotarDados(unsigned char * frag, int numSeq, int size, int * tamPacote)
{
	printf("===========================\nDATA PACKET\n-------------------------\n");
	printf("Tamanho do fragmento: %d\n", size);
	//verDados(frag, size);
	
	unsigned char C = Cpkg_dados;
	unsigned char N = numSeq;

	//verDados(frag, size+1);

	int LL2 = size / MFS;
	int LL1 = size - LL2 * MFS;
	unsigned char L2 = (char)(LL2);
	unsigned char L1 = (char)(LL1);
	
	printf("[C, N, L2, L1]: [0x%X, 0x%X, 0x%X, 0x%X]\n", C, N, L2, L1);

	unsigned char * pkg = (unsigned char *)
		malloc(sizeof(unsigned char) * (MFS + 10));
		
	strcpy(pkg, "");
	
	int a, b = 4;
	
	pkg[0] = C;
	pkg[1] = N;
	pkg[2] = L2;
	pkg[3] = L1;

	for (a = 0; a <= size; a++)
	{
		if (frag[a] == ESC) // ESC
		{
			pkg[b] = ESC;
			b++;
			pkg[b] = 0x5d;
			b++;
		}
		else
		{
			if (frag[a] == FLAG) // FLAG
			{
				pkg[b] = ESC;
				b++;
				pkg[b] = 0x5e;
				b++;
			}
			else
			{
				pkg[b] = frag[a];
				b++;
			}
		}
	}

	*tamPacote = (b - 1);
	
	printf("PKG[ultimo]: 0x%X\n\n", pkg[b-1]);
		
	printf("Tamanho do pacote: %d\n", *tamPacote);
	
	//for(b = 0; b < *tamPacote; b++)
	//{
		//printf("0x%X ", pacote[b]);
		//if (b % 10 == 0)
			//printf("\n");
	//} printf("\n\n");
	
	//verDados(pkg, *tamPacote) ;
	
	return(pkg);
}

#endif
