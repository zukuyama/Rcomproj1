#include "programa.h"

int main(int argc, char ** argv)
{
	system("clear");
	
	if (argc >= 1) // ha argumentos
	{
		if (strcmp(argv[2], "send") == 0) // servidor deve ter 3 argumentos
		{
			if (argc != 4)
			{
				printf("MAIN :: SERVIDOR DEVE TER 3 ARGUMENTOS\n");
				exit(1);
			}
			else
			{
				if (strcmp(argv[1], "server") != 0 && strcmp(argv[1], "client") != 0)
				{
					printf("MAIN :: %s NAO EXISTE\n", argv[2]);
					exit(1);
				}

				nomeFicheiro = argv[3];
				tamFicheiro = tamanhoFicheiro(nomeFicheiro);
				
				if (tamFicheiro == -1)
				{
					printf("FICHEIRO %s NAO EXISTE\n", nomeFicheiro);
					return(-1);
				}
			}
		}
		else if (strcmp(argv[2], "get") == 0)// cliente deve ter 2 argumentos
		{
			if (argc != 3)
			{
				printf("MAIN :: SERVIDOR DEVE TER 2 ARGUMENTOS\n");
				exit(1);
			}
			else
			{
				if (strcmp(argv[1], "server") != 0 && strcmp(argv[1], "client") != 0)
				{
					printf("MAIN :: %s NAO EXISTE\n", argv[2]);
					exit(1);
				}
			}
		}
	}
	else
	{
		printf("MAIN :: FALTAM ARGUMENTOS AO PROGRAMA PRINCIPAL\n");
		exit(1);
	}
	
	/****************************************
				INICIO DO PROGRAMA
	*****************************************/
	
   setEmissorDevice(argv[1], argv[2]);
	
	//initLinkLayer(&lnk, device);
  	initAppLayer(&app);
	
	app.status = (eEmissor == 1) ? TRANSMITTER : RECEIVER ;
  	app.fileDescriptor = llopen(device,app.status);

	if (app.fileDescriptor == -1)
	{
		return(-1);
	}
	else
	{
		printf("CONEXAO ESTABELECIDA E GARANTIDA\n");
	}	
	
	/****************************************
				TRATAMENTO DE DADOS
	*****************************************/
	
	if (app.status == TRANSMITTER)
	{
		// envio do pacote de controlo de inicio da transferencia do ficheiro
		sleep(1);
		int tamPacote = 0;
		unsigned char * informacaoFile = pacotarControlo(nomeFicheiro, tamFicheiro, Cpkg_inicio, &tamPacote);
		
		int estEnvio = 0;
		estEnvio = llwrite(app.fileDescriptor, informacaoFile, tamPacote);

		// enviar a informacao
		int linhaActual = 0, i = 0;
		f = fopen(nomeFicheiro, "rb");		
		
		unsigned char string[MFS];
		strcpy(string, "");

		unsigned char * pkg;
		unsigned char * data;
		int tamDados = 0;
		
		int res = 0;
			
		int aux = fgetc(f);
		while(aux != EOF)
		{
			string[i] = (unsigned char)aux;
			i++;
		
			if (i == MFS) //string[i] ja contem os MFS caracteres
			{	
				sleep(1);		
				//tamPacote = 0;
				pkg = pacotarDados(string, linhaActual, MFS, &tamPacote);
				printf("Tamanho do pacote: %d\n", tamPacote);
				/*
				int k;
				for(k = 0; k <= MFS; k++)
				{
					printf("0x%X ", string[k]);
					if ( (k+5) % 15 == 0)
						printf("\n");
				}
				*/
				res = llwrite(app.fileDescriptor, pkg, tamPacote);		
				if (res < 0)
				{
					printf("PROBLEMA AO ENVIAR A TRAMA... PROGRAMA TERMINOU\n");
					return(-1);
				}
				//colocarFicheiro(f2, data, tamDados);
						
				i = 0;
				linhaActual++;
				printf("Linha %d\n", linhaActual);
				strcpy(string, "");
			}
			aux = fgetc(f);
		}
		/*
		int k;
				for(k = 0; k <= i; k++)
				{
					printf("0x%X ", string[k]);
					if ( (k+5) % 15 == 0)
						printf("\n");
				}
		*/
		pkg = pacotarDados(string, linhaActual, i, &tamPacote);
		res = llwrite(app.fileDescriptor, pkg, tamPacote);

		//printf("Resultdo final: %d\n", res);

		linhaActual++;
		printf("Linha %d\n", linhaActual);

		fclose(f);

		sleep(1);

		// envio do pacote de controlo de inicio da transferencia do ficheiro
		//printf("\n\n\nULTIMO PACOTE DE CONTROLO\n");
		informacaoFile = pacotarControlo(nomeFicheiro, tamFicheiro, Cpkg_fim, &tamPacote);

		estEnvio = 0;
		estEnvio = llwrite(app.fileDescriptor, informacaoFile, tamPacote);
	}
	else
	{
		//unsigned char * * info = (unsigned char**) malloc( sizeof(unsigned char));
		unsigned char * informacaoFile = (unsigned char*) malloc( sizeof(unsigned char) * MFS );
		
		//printf("Endereco informacaoFILE: %d\n", &informacaoFile[0]);
		//printf("Endereco informacaoFILE: %d\n", &info[0][0]);

		//int tamFile = llread(app.fileDescriptor, &info);
		int tamFile = llread(app.fileDescriptor, informacaoFile);

		//printf("Endereco informacaoFILE apos llread(): %d\n", &informacaoFile[0]);

	//printf("Endereco informacaoFILE apos llread(): %d\n", &info[0][0]);

		//printf("Resultado do llread(): %d\n\n", tamFile);
		
		if (tamFile == -1)
		{
			printf("Erro na leitura\n");
			return(-1);
		}
		
		//verDados(informacaoFile, tamFile);
		//if (nomeFicheiro == NULL)
		nomeFicheiro = (char *)malloc(sizeof(char) * 255);
		tamFicheiro = 0;

		despacotarControlo(informacaoFile, (tamFile), nomeFicheiro, &tamFicheiro);
		
		//printf("Nome ficheiro: %s\n", nomeFicheiro);
		//printf("Tamanho do ficheiro: %d\n", tamFicheiro);
		
		if (tamanhoFicheiro(nomeFicheiro) == -1)
		{
			printf("FICHEIRO %s NAO EXISTE\n", nomeFicheiro);
		}
		
		//nomeFicheiro = "galinha.gif";

		// armazenar a informacao
		int tamActual = 0;
		f = fopen(nomeFicheiro, "wb+");		
		
		unsigned char * pkg = NULL;
		int tamPacote = 0;
		unsigned char * data = NULL;
		int tamDados = 0;
		
		int res = 0;
		int linha = 0;
		while (tamActual < tamFicheiro)
		{
			pkg = NULL;
			data = NULL;
			pkg = (unsigned char*) malloc( sizeof(unsigned char) * MFS );
			tamPacote = llread(app.fileDescriptor, pkg);
			
			linha++;
			//printf("LINHA %d\n", linha);

			//printf("Tamanho llread(): %d\n", tamPacote);

			data = despacotarDados(pkg, tamPacote, &tamDados);
			
			//printf("Tamanho dados: %d\n", tamDados);
			
			//verDados(data, tamDados);

			colocarFicheiro(f, data, tamDados);
			//printf("Tamanho lido: %d\n", tamDados);
			//if (pkg != NULL)
			//free(pkg);

			//if (data != NULL)
			//free(data);			
		tamActual += tamDados;

			float qqq = (100.0 * (1.0 * tamActual) / (1.0*tamFicheiro));

			//printf("---> TAMANHO ACTUAL FICHEIRO: %d %d [ ", tamActual, tamActual%MFS);
			//system("clear");
			//printf("\t\t\t\t\t[ %.3f%% ]\n", qqq);
			mostrarEstadoTransferencia(qqq);
		}
		
		fclose(f);

		// envio do pacote de controlo de inicio da transferencia do ficheiro		
		//printf("\n\n\nULTIMO PACOTE DE CONTROLO\n");
		free(informacaoFile);
		informacaoFile=NULL;//ADDED TO TEST

		informacaoFile = (unsigned char*) malloc( sizeof(unsigned char) * MFS );

		tamFile = llread(app.fileDescriptor, informacaoFile);

		if (tamFile == -1)
		{
			printf("Erro na leitura\n");
			return(-1);
		}
		
		char * fff = (unsigned char*) malloc( sizeof(unsigned char) * MFS );
		unsigned int ttt = 0;

		//nomeFicheiro = "pinguim.gif";

		despacotarControlo(informacaoFile, (tamFile), fff, &ttt);
		
		//printf("%s %d\n", fff, ttt);
		int rrr = strcmp(fff, nomeFicheiro);

		if (! (rrr == 0 && ttt == tamFicheiro) )
		{
			printf("ERRO NO PACOTE DE CONTROLO DE FIM DE TRANSFERENCIA\n");
			return(-1);
		}
		free(informacaoFile);
	}
	
	/****************************************
				TERMINAR O PROGRAMA
	*****************************************/
	
	llclose(app.fileDescriptor);
	
	printf("\nPROGRAMA TERMINOU CORRECTAMENTE\n");
	printf("==============================================\n");
	exit(0);
}

/************************************************************************************************************************
*														DEFINICAO DAS FUNCOES A IMPLEMENTAR
************************************************************************************************************************/
int llopen(int porta, size_t flaggg)
{
	printf("\n\t\tLLOPEN()\n");
	
	int r = 3; // diferente de 0 - read, 1 - write, 2 - erros
	char portaa[11] = "/dev/ttyS0";

	if (porta == 4)
	{
		portaa[9] = '4';
	}
	else if (porta != 0)
	{
		printf("LLOPEN() :: PORTA %d INVALIDA\n", porta);
		return(-1);
	}

	r = open(portaa, O_RDWR | O_NOCTTY);
	if (r == -1)
	{
		printf("LLOPEN() :: FALHOU A CONEXAO\n");
		return(-1);
	}
	
	if (flaggg == TRANSMITTER) // llopen do TRANSMITTER
	{
		set_newsettings(r, 0.1, 0); // 0.1 0 
		
		unsigned char * tramaSET = tramaSU(r, 0, 0); // tramaSU(int eEmissor, int qualCtrama, int 0 ou 1)
		
		//verDados(tramaSET, 4);
		
		unsigned char * tramaUA = (unsigned char*) malloc( sizeof(unsigned char) * MTS );
		
		int tamUA = enviarComAlarme(r, tramaSET, 5, tramaUA, A1, C_UA);
		
		if (tamUA < 0)
		{
			printf("LLOPEN() :: EMISSOR :: TIMEOUT LIMIT\n");
			return(-1);
		}
		
		// trama UA e 'tramaUA' com tamanho 'estado'
		
		//printf("Tamanho UA: %d\n", tamUA);
		
		if (tamUA != 5)
		{
			printf("LLOPEN() :: EMISSOR :: TAMANHO UA DIFERENTE DE 5. TAMANHO = %d\n", tamUA);
			return(-1);
		}		
	}
	else if (flaggg == RECEIVER) // llopen do RECEIVER
	{
		set_newsettings(r, 0.1, 0); // 0 5
		
		unsigned char * tramaSET = (unsigned char*) malloc( sizeof(unsigned char) * MTS );
		int tamSET = lerSemAlarme(r, tramaSET);
		
		//printf("Tamanho SET: %d\n", tamSET);
		
		if (tamSET < 0)
		{
			printf("LLOPEN() :: RECEPTOR :: ERRO NA OBTENCAO DA TRAMA SET\n");
			return(-1);
		}
		
		if (tamSET == 5)
		{
			printf("TRAMA SET RECEBIDA... A ENVIAR UA\n");
			
			unsigned char * tramaUA = tramaSU(r, 2, 1);
			//verDados(tramaUA, 4);
			
			enviar(r, tramaUA, 5);
			
		}
		else // invalido
		{
			printf("LLOPEN() :: RECEPTOR :: TAMANHO SET DIFERENTE DE 5. TAMANHO = %d\n", tamSET);
			return(-1);
		}
	}
	else
	{
		printf("LLOPEN() :: FLAG INVALIDA\n");
		return(-1);
	}

	return(r);
}

int llclose(int fd)
{
	printf("\n\t\tLLCLOSE()\n");
	
	int r = 0;
	
	if (app.status == TRANSMITTER) // codigo do Emissor
	{
		//printf("Codigo do emissor do llclose()\n");
		unsigned char * tramaDISC = tramaSU(app.status, 1, 0); // tramaSU(int eEmissor, int qualCtrama, int 0 ou 1)
		
		//verDados(tramaSET, 4);
		
		unsigned char * tramaDISC2 = (unsigned char*) malloc( sizeof(unsigned char) * MTS );
		
		printf("A ENVIAR TRAMA DISC\n");
		int tamDISC2 = enviarComAlarme(app.fileDescriptor, tramaDISC, 5, tramaDISC2, A2, C_DISC);
		
		//printf("TAMANHO DO TAMDISC2: %d\n", tamDISC2);

		if (tamDISC2 < 0)
		{
			printf("LLCLOSE() :: EMISSOR :: TIMEOUT LIMIT\n");
			return(-1);
		}
		printf("TRAMA DISC RECEBIDA... A ENVIAR UA\n");
		
		unsigned char * tramaUA = tramaSU(app.status, 2, 0); // tramaSU(int eEmissor, int qualCtrama, int 0 ou 1)
		int res = enviarSemAlarme(app.fileDescriptor, tramaUA, 5);
		
		//printf("TAMANHO DO TAMDISC2: %d\n", tamDISC2);
	}
	else // codigo do Receptor
	{
		//printf("Codigo do receptor do llclose()\n");
		unsigned char * tramaDISC = (unsigned char*) malloc( sizeof(unsigned char) * MTS );
		int tamDISC = lerSemAlarme(app.fileDescriptor, tramaDISC);
		
		//printf("TAMANHO DO TAMDISC: %d\n", tamDISC);

		//printf("Tamanho SET: %d\n", tamSET);
		
		if (tamDISC < 0)
		{
			printf("LLCLOSE() :: RECEPTOR :: ERRO NA OBTENCAO DA TRAMA DISC\n");
			return(-1);
		}
		
		if (tamDISC != 5)
		{
			printf("LLCLOSE() :: RECEPTOR :: TAMANHO DISC DIFERENTE DE 5. TAMANHO = %d\n", tamDISC);
			return(-1);
		}
		
		printf("TRAMA DISC RECEBIDA\nA ENVIAR SEGUNDA TRAMA DISC\n");
			
		unsigned char * tramaDISC2 = tramaSU(app.status, 1, 0);
			
		//verDados(tramaDISC2, 4);

		unsigned char * tramaUA = (unsigned char*) malloc( sizeof(unsigned char) * MTS );
		int tamUA = enviarComAlarme(app.fileDescriptor, tramaDISC2, 5, tramaUA, A2, C_UA);
			
		//printf("TAMANHO DO TAMUA: %d\n", tamUA);
		
		if (tamUA < 0)
		{
			printf("LLCLOSE() :: RECEPTOR :: ERRO NA OBTENCAO DA TRAMA UA\n");
			return(-1);
		}
		
		if (tamUA != 5)
		{
			printf("LLCLOSE() :: RECEPTOR :: TAMANHO UA DIFERENTE DE 5. TAMANHO = %d\n", tamUA);
			return(-1);
		}		
	}
	
	set_oldsettings(fd);
	close(fd);

	return(0);
}

int llwrite(int fd, unsigned char * buffer, int length) // (file, pck, tamPKG)
{
	printf("\n\t\tLLWRITE()\n");
	int r = 0;
	
	int tamTrama = 0;
	unsigned char * trama = tramaI(buffer, length, &tamTrama);
	
	unsigned char Cs = trama[2];
	unsigned char Cr = 0x00;
	if (Cs == C_S_0)
	{
		Cr = C_RR_1;
	}
	else if (Cs == C_S_1)
	{
		Cr = C_RR_0;
	}
	else
	{
		printf("Erro na trama: Caracter do C corresponde ao 0x%X\n", Cs);
		return(-1);
	}
	
	//printf("Cs Cr: 0x%X 0x%X\n", Cs, Cr);
	
	unsigned char * trama2 = (unsigned char*) malloc( sizeof(unsigned char) * MTS );
	// app.fileDescriptor, tramaDISC, 5, tramaDISC2, A2, C_DISC);
	int tamResposta = enviarComAlarme(app.fileDescriptor, trama, tamTrama+1, trama2, A1, Cr);
	
	if (tamResposta < 0)
		return(tamResposta);

	return(tamTrama);
}

int llread(int fd, unsigned char * buffer)
{
	printf("\n\t\tLLREAD()\n");

	//printf("Endereco buffer: %d\n", &buffer[0]);
	
	int r = 0;

	unsigned char * trama = (unsigned char*) malloc( sizeof(unsigned char) * MTS );

	int tamTrama = 0;
	tamTrama = lerSemAlarme(app.fileDescriptor, trama);
	//verDados(trama, tamTrama);
	
	unsigned char * auxx;

	auxx = tramaI_toPacote(trama, tamTrama-1, &r);
	
	copyPacote(auxx, buffer, r);
	
	free(auxx);

	//printf("Endereco buffer: %d\n", &buffer[0]);
	//verDados(buffer, r);

	//printf("galinha\n");
	unsigned char Cs = trama[2];
	unsigned char Cr = 0x00;
	
	int numSeq = 0;
	
	if (Cs == C_S_0) // Ns = 0
	{
		Cr = C_RR_1;
		numSeq = 0; // Nr = 1
	}
	else if (Cs == C_S_1) // Ns = 1
	{
		Cr = C_RR_0;
		numSeq = 1; // Nr = 0
	}
	
	//printf("Cs Cr: 0x%X 0x%X\n", Cs, Cr);
	
	unsigned char * trama2 = tramaSU(app.status, 3, numSeq);
	
	//verDados(trama2, 4);
	
	int res = enviarSemAlarme(app.fileDescriptor, trama2, 5);
	
	//printf("Endereco buffer: %d\n", &buffer[0]);
	
	return(r);
}

