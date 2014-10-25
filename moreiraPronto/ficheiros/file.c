/**
*	RCOM: programa para ler ficheiro.extensao, enviar ou guardar para outro ficheiro
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * f;
FILE * f2;

int MFS = 256; // max fragment size 256
int MTS = 768; // max trama size: MFS * 3

unsigned char FLAG = 0x7e;
unsigned char Cdados = 0x01;
unsigned char Cinicio = 0x02;
unsigned char Cfim = 0x03;

unsigned char ESC = 0x7d;

void frameUnbuilderI(unsigned char * frame, int size);
void frameBuilderI(unsigned char * pkg, int size);
void frameUnbuilderSU(unsigned char frame[5]);
void frameBuilderSU(int emissor, int qualC, int num_frag);

void controlUnpacket(unsigned char * str, int tam);
void controlPacket(int modo, char * str_file, int tam_file);
void dataUnpacket(unsigned char * str, int tam);
void dataPacket(unsigned char * str, int pos, int size);

/*
	FUNCOES GENERICAS
*/
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

char * toString(int num)
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

/*======================================================
*		FUNCOES AO NIVEL DO LINK LAYER
=======================================================*/
// obtem bcc2 da trama
unsigned char xor(unsigned char * dados, int xmin, int xmax)
{
	unsigned char r = dados[xmin];
	int i;	
	for (i = (xmin+1); i <= xmax; i++)
	{
		r = (r ^ dados[i]);
	}
	return(r);
}

/*******************************************************
	Trama SU: codificacao e descodificacao
*******************************************************/
// obtem os dados da trama I recebida pelo read()
void frameUnbuilderI(unsigned char * frame, int size)
{
	unsigned char N = frame[0];
	unsigned char A = frame[1];
	unsigned char C = frame[2];
	unsigned char Bcc1 = (A ^ C);
	if (Bcc1 != frame[3])
		printf("errou\n");
	
	int limite = size - 2;
	int i;

	unsigned char pkg[limite];

	for (i = 4; i <= limite; i++)
	{
		pkg[i - 4] = frame[i];
	}
	int tamPacote = limite - 4;
	dataUnpacket(pkg, tamPacote);
}

// constroi a trama I para enviar pelo write()
/*
	pkg.C = 0x01 (dados)?
		Ns = (pkg.N % 2)
			espera-se do receptor Nr = ((pkg.N+1) % 2)
	pkg.C > 0x01 (controlo)?
		Ns = 0
			espera-se do receptor Nr = 1	
	
	
*/
void frameBuilderI(unsigned char * pkg, int size)
{
	unsigned char trama[MTS];
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

	int tamTrama = (i - 1);
	// write(fd, trama, tamTrama);

	//for (i = 0; i <= tamTrama; i++)
	//{
	//	printf("0x%X ", trama[i]);
	//	if (i % 10 == 0)
	//		printf("\n");
	//}printf("\n");

	frameUnbuilderI(trama, tamTrama);
}

// obtem os dados da trama SU recebida pelo read()
void frameUnbuilderSU(unsigned char frame[5])
{
	unsigned char F = frame[0];
	unsigned char A = frame[1];
	unsigned char C = frame[2];
	unsigned char Bcc1 = (A ^ C);
	unsigned char Ffinal = frame[4];

	if (Bcc1 != (frame[1] ^ frame[2]))
		printf("Errado\n");

	printf("[0x%X, 0x%X, 0x%X, 0x%X, 0x%X]\n",
	F, A, C, Bcc1, Ffinal);
}

// quem esta a enviar? emissor(1) ou receptor(0)?
// qualC = 0, SET
// qualC = 1, DISC
// qualC = 2, UA
// qualC = 3, RR --> 0x05 quando num_frag par, 0x85 fragmento impar 
// qualC = 4, REJ --> 0x01 quando num_frag par, 0x81 fragmento impar 

// qualC = SET, DISC and emissor envia, A = 0x03 
// qualC = SET, DISC and receptor envia, A = 0x01
// qualC = UA, RR, REJ and emissor envia, A = 0x01 
// qualC = UA, RR, REJ and receptor envia, A = 0x03 

// num_frag: variavel global na camada Link (0 ou 1)
// se pacote controlo, tanto faz
// senao pacote de dados, alternar entre 0 e 1 por cada frag. 
void frameBuilderSU(int emissor, int qualC, int num_frag)
{
	unsigned char trama[5];
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
			if ((num_frag % 2) == 0)
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
			if ((num_frag % 2) == 0)
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
		trama[0], trama[1], trama[2], trama[3], trama[4]
	);

	// trama SU preparada
	// write(fd, trama, 5);

	frameUnbuilderSU(trama);
}

/*======================================================
*		FUNCOES AO NIVEL DA APPLICATION LAYER
=======================================================*/

/*******************************************************
	Pacote de controlo: codificacao e descodificacao
*******************************************************/
// descodificador
void controlUnpacket(unsigned char * str, int tam) // str terminado com '\0'
{
	printf("-------------------------\nCONTROL UNPACKET\n-------------------------\n");
	unsigned char C = str[0];
	unsigned char T1 = 0x00, T2 = 0x00;
	int vec1 = 0, vec2 = 0;

	unsigned char nomeFicheiro[255];
	int tamFicheiro;

	int i = 0, tf = 0, tn = 0;
	int ref = 0;

	T1 = str[1];
	vec1 = (int) str[2];
	
	ref = 3;
	for (i = 0; i < vec1; i++)
	{
		int t = (int) str[i + ref];
		tamFicheiro = tamFicheiro * 10 + t;
	}

	ref += vec1;
	T2 = str[ref];
	vec2 = (int) str[ref+1];
	ref += 2;
 
	for (i = 0; i < vec2; i++)
	{
		nomeFicheiro[i] = (char)str[i + ref];
	}
	nomeFicheiro[vec2] = '\0';

	printf("[0x%X, 0x%X, %d, ..., 0x%X, %d, ...]\n\n", C, T1, vec1, T2, vec2);
	printf("Tamanho do ficheiro: %d bytes\n", tamFicheiro);
	printf("Nome do ficheiro: %s\n", nomeFicheiro);

	// armazenar tamanho e nome do ficheiro no lado do llread() 
}

// codificador
void controlPacket(int modo, char * str_file, int tam_file)
{
	printf("===========================\nCONTROL PACKET\n-------------------------\n");
	int comprimentoFile = strlen(str_file);
	int comprimentoTamanho = numDigitos(tam_file);

	int i, j;
	int total =
		1 +
		1 + 1 + comprimentoFile + 
		1 + 1 + comprimentoTamanho;	

	char pacote[total+1];
	strcpy(pacote, "");
	
	i = 0;
	if (modo == 2) // 0x01 -> inicio de transmissao
	{
		pacote[i] = 0x02;
		i++;
	}
	else if (modo == 3)  // 0x02 -> fim de transmissao
	{
		pacote[i] = 0x03;
		i++;
	}

	printf("[0x%X, ", pacote[i-1]);	
	
	pacote[i] = 0x00; // tamanho do ficheiro
	i++;

	printf("0x%X, ", pacote[i-1]);

	pacote[i] = (char) comprimentoTamanho;
	i++;
	
	printf("%d, ", comprimentoTamanho);

	char * s = toString(tam_file);

	for (j = 0; j < comprimentoTamanho; j++)
	{
		pacote[i] = s[j];
		i++;
	}

	pacote[i] = 0x01; // nome do ficheiro
	i++;

	printf("..., 0x%X, ", pacote[i-1]);

	pacote[i] = (unsigned char) comprimentoFile;
	i++;

	for (j = 0; j < comprimentoFile; j++)
	{
		pacote[i] = str_file[j];
		i++;
	}
	
	printf("%d, ...]\n", j);


	int tamanhoPacote = (i - 1);
	printf("Tamanho do pacote: %d\n\n", tamanhoPacote);

	//for (i = 0; i <= tamanhoPacote; i++)
	//	printf("0x%x\n", pacote[i]);

	// llwrite(fd, pacote, tamanhoPacote);
	//controlUnpacket(pacote, tamanhoPacote);
	frameBuilderI(pacote, tamanhoPacote);
}

/*******************************************************
		Pacote de dados: codificacao e descodificacao
*******************************************************/
// descodificador
void dataUnpacket(unsigned char * str, int tam)
{
	printf("-------------------------\nDATA UNPACKET\n-------------------------\n");
	unsigned char C = str[0];
	unsigned char N = str[1];

	unsigned char L2 = str[2];
	unsigned char L1 = str[3];

	int i = 4, j = 0;

	int NN = N;
	int LL2 = (int) L2;
	int LL1 = (int) L1;

	printf("[C, N, L2, L1]: [0x%X, 0x%X, 0x%X, 0x%X]\n", C, NN, LL2, LL1);
	
	int tamFragmento = LL2*MFS + LL1;
	int tt = tam - 4;

	printf("Tamanho do pacote: %d\n", tam);
	printf("Tamanho do fragmento (aS): %d\n", tt);
	printf("Tamanho do fragmento teorico (LL2*MFS + LL1): %d\n", tamFragmento);

	unsigned char fragmento[tamFragmento + 1];
	for (i = 4; i <= tam; i++)
	{
		if (str[i] == ESC)
		{
			i++;
		}
		fragmento[j] = str[i];
		j++;
	}
	
	tamFragmento = (j - 1);

	printf("Tamanho do fragmento: %d\n", tamFragmento);
	// fragmento obtido
	int k;
	for (k = 0; k < tamFragmento; k++)
	{
		fputc(fragmento[k], f2);		
	}
	printf("\n");
}

// codificador
void dataPacket(unsigned char * str, int pos, int size)
{
	printf("===========================\nDATA PACKET\n-------------------------\n");
	unsigned char C = Cdados;
	unsigned char N = pos;

	int LL2 = size / MFS;
	int LL1 = size - LL2 * MFS;
	unsigned char L2 = (char)(LL2);
	unsigned char L1 = (char)(LL1);
	
	printf("[C, N, L2, L1]: [0x%X, 0x%X, 0x%X, 0x%X]\n", C, N, L2, L1);

	unsigned char pacote[MTS];
	strcpy(pacote, "");

	int a, b = 4;

	pacote[0] = C;
	pacote[1] = N;
	pacote[2] = L2;
	pacote[3] = L1;

	for (a = 0; a <= size; a++)
	{
		if (str[a] == ESC || str[a] == FLAG)
		{
			pacote[b] = ESC;
			b++;
		}
		pacote[b] = str[a];
		b++;
	}
	int tamanhoPacote = (b-1);

	printf("Tamanho do pacote: %d\n", tamanhoPacote);
	
	for(b = 0; b <= tamanhoPacote; b++)
	{
		printf("0x%X ", pacote[b]);
		if (b % 10 == 0)
			printf("\n");
	} printf("\n\n");

	// llwrite(fd, pacote, tamanhoPacote);
	//dataUnpacket(pacote, tamanhoPacote);
	frameBuilderI(pacote, tamanhoPacote);
}

/*======================================================
*				PROGRAMA PRINCIPAL DE TESTE
=======================================================*/
int main(int a, char ** b)
{
	system("clear");
	unsigned char * src;
	unsigned char * dst;
	
	if (a > 2)
	{
		src = b[1];
		dst = b[2];	
	}
	else
	{
		src = "file.txt";
		dst = "novo.txt";
	}
	
	// verificacao de erros
	//unsigned char trama[3] = {0x7e, 0x85, 0x03};
	//unsigned char bcc2 = xor(trama, 0, 2);
	//printf("0x%X\n", bcc2);	
	
	//void frameBuilderSU(int emissor, int qualC, int num_frag)
	
	//frameBuilderSU(1, 3, 1);		

	/* // control packet
	int tamFicheiro = tamanhoFicheiro(src);
	controlPacket(2, src, tamFicheiro);
	
	*/
	//return(0);
	
	unsigned char string[MFS];
	strcpy(string, "");

	
	f = fopen(src, "rb");
	f2 = fopen(dst, "wb+");
	
	int aux = fgetc(f);
	int i = 0, j = 0;
	int k;
	
	while(aux != EOF)
	{
		string[i] = (unsigned char)aux;
		i++;
		
		if (i == MFS)
		{		
			//string[i] ja contem os MFS caracteres
			dataPacket(string, j, MFS);

			i = 0;
			j++;
			strcpy(string, "");
		}
		aux = fgetc(f);
	}
	dataPacket(string, j, i); // ultimo fragmento de dados
	
	/**
	*		Informacao sobre a leitura de dados
	*		- tamanho do ficheiro: (LINHAS - 1)* MFS + colunasUltLinha;
	*/
	int LINHAS = j+1;
	int colunasUltLinha = i;
	
	printf("#####################################\nFicheiro src: %s\n", src);
	printf("Ficheiro dst: %s\n", dst);
	printf("Numero de linhas: %d\n", LINHAS);
	printf("Numero de caracteres por linha: %d\n", MFS);
	printf("Numero de caracteres da ultima linha: %d\n", colunasUltLinha);
	printf("Tamanho do ficheiro src: %d\n", (LINHAS-1) * MFS + colunasUltLinha);
	
	fclose(f);
	fclose(f2);
	printf("\n");
	return(0);
}
