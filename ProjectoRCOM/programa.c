#include "programa.h"

int main(int argc, char ** argv)
{
	system("clear");
	initAppLayer(&app);
	
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

				//nomeFicheiro = argv[3];
				strcpy(nomeFicheiro, argv[3]);
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
	//return(0);
	int resultado = funcaoIO(); if (resultado < 0) return(-1);
		
	/****************************************
				TERMINAR O PROGRAMA
	*****************************************/
	//sleep(1);
	
	llclose(app.fileDescriptor);
	

	printf("\nPROGRAMA TERMINOU CORRECTAMENTE\n");
	printf("==============================================\n");
	exit(0);
}

int funcaoIO()
{
	if (app.status == TRANSMITTER)
	{
		// envio do pacote de controlo de inicio da transferencia do ficheiro
		sleep(1);
		int tamPacote = 0;
		
		//unsigned char * informacaoFile = pacotarControlo(nomeFicheiro, tamFicheiro, Cpkg_inicio, &tamPacote);
		limparPacote(PACOTE);
		tamPacote = controlToPackage(nomeFicheiro, tamFicheiro, Cpkg_inicio, PACOTE);
		
		int estEnvio = 0;
		estEnvio = llwrite(app.fileDescriptor, PACOTE, tamPacote);
		
		// enviar a informacao
		int linhaActual = 0, i = 0;
		f = fopen(nomeFicheiro, "rb");		
		
		unsigned char string[MFS];
		strcpy(string, "");
		
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
				//pkg = pacotarDados(string, linhaActual, MFS, &tamPacote);
				limparPacote(PACOTE);
				//setLinkLayer(&lnk, (lnk.sequenceNumber+1)%2);
				tamPacote = dataToPackage(string, linhaActual, MFS, PACOTE);
				//printf("Tamanho do pacote: %d\n", tamPacote);
				
				res = llwrite(app.fileDescriptor, PACOTE, tamPacote);		
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
		
		sleep(1);
		
		limparPacote(PACOTE);	
		tamPacote = dataToPackage(string, linhaActual, i, PACOTE);
		res = llwrite(app.fileDescriptor, PACOTE, tamPacote);

		printf("Resultdo llwrite(): %d\n", res);

		linhaActual++;
		printf("Linha %d\n", linhaActual);

		fclose(f);
		
		sleep(1);
		//setLinkLayer(&lnk, 0);
		// envio do pacote de controlo de inicio da transferencia do ficheiro
		//printf("\n\n\nULTIMO PACOTE DE CONTROLO\n");
		limparPacote(PACOTE);
		tamPacote = controlToPackage(nomeFicheiro, tamFicheiro, Cpkg_fim, PACOTE);

		estEnvio = 0;
		estEnvio = llwrite(app.fileDescriptor, PACOTE, tamPacote);
	}
	else
	{
		//unsigned char * * info = (unsigned char**) malloc( sizeof(unsigned char));
		
		//printf("Endereco informacaoFILE: %d\n", &informacaoFile[0]);
		//printf("Endereco informacaoFILE: %d\n", &info[0][0]);

		//int tamFile = llread(app.fileDescriptor, &info);
		//setLinkLayer(&lnk, 1);
		
		limparPacote(PACOTE);
		int tamFile = llread(app.fileDescriptor, PACOTE);
		
		//printf("Tamanho llread(): %d\n", tamFile);
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
		//nomeFicheiro = (char *)malloc(sizeof(char) * 255);
		limparNomeFicheiro(nomeFicheiro);
		tamFicheiro = 0;
		
		packageToControl(PACOTE, (tamFile), nomeFicheiro, &tamFicheiro);
		
		//printf("Nome ficheiro: %s\n", nomeFicheiro);
		//printf("Tamanho do ficheiro: %d\n", tamFicheiro);
		
		if (tamanhoFicheiro(nomeFicheiro) == -1)
		{
			printf("FICHEIRO %s NAO EXISTE\n", nomeFicheiro);
		}
		
		//nomeFicheiro = "galinha.gif";
		strcpy(nomeFicheiro, "galinha.gif");
		
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
			limparPacote(PACOTE);
			limparDados(DADOS);
			//setLinkLayer(&lnk, (lnk.sequenceNumber+1)%2);
			tamPacote = llread(app.fileDescriptor, PACOTE);
			
			linha++;
			printf("LINHA %d\n", linha);
			
			//verDados(PACOTE, tamPacote);
			//printf("Tamanho llread(): %d\n", tamPacote);
			
			int tamDados = packageToData(PACOTE, tamPacote, DADOS);
			//printf("Tamanho dados: %d\n", tamDados);
			
			//verDados(data, tamDados);

			colocarFicheiro(f, DADOS, tamDados);
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
		//setLinkLayer(&lnk, 1);
		
		// envio do pacote de controlo de inicio da transferencia do ficheiro		
		//printf("\n\n\nULTIMO PACOTE DE CONTROLO\n");
		limparPacote(PACOTE);
		tamFile = llread(app.fileDescriptor, PACOTE);

		if (tamFile == -1)
		{
			printf("Erro na leitura\n");
			return(-1);
		}
		
		//char * fff = (unsigned char*) malloc( sizeof(unsigned char) * MFS );
		char fff[30];
		limparNomeFicheiro(fff);
		
		unsigned int ttt = 0;

		//nomeFicheiro = "pinguim.gif";
		strcpy(nomeFicheiro, "pinguim.gif");
		
		packageToControl(PACOTE, (tamFile), fff, &ttt);
		
		//printf("%s %d\n", fff, ttt);
		int rrr = strcmp(fff, nomeFicheiro);

		if (! (rrr == 0 && ttt == tamFicheiro) )
		{
			printf("ERRO NO PACOTE DE CONTROLO DE FIM DE TRANSFERENCIA\n");
			return(-1);
		}
	}
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
	
	initLinkLayer(&lnk, portaa);
	
	r = open(portaa, O_RDWR | O_NOCTTY);
	if (r == -1)
	{
		printf("LLOPEN() :: FALHOU A CONEXAO\n");
		return(-1);
	}
	
	if (flaggg == TRANSMITTER) // llopen do TRANSMITTER
	{
		set_newsettings(r, 0.1, 0.0); // 0.1 0    ------> 0.1 0.1
		
		limparFrame(TRAMA);
		int tamSU = suFrame(1, 0, 0, TRAMA); // int numSeq, int qualC
		
		//verDados(TRAMA, tamSU);
		limparFrame(TRAMA2);
		int tamUA = enviarrComAlarme(r, TRAMA, tamSU, TRAMA2, A1, C_UA);
		
		//verDados(TRAMA2, tamUA);
		
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
		set_newsettings(r, 0.1, 1.0); // ------> 0.1 1.0
		contador = 0;
		
		int tamSET = 0;
		int estadoDFA = 0;
		
		do
		{
			limparFrame(TRAMA);
			tamSET = leerSemAlarme(r, TRAMA);

			verDados(TRAMA, tamSET);
			//TRAMA[1] = 0x45;
			estadoDFA = maquinaEstados(TRAMA, tamSET, A1, C_SET);
			contador++;
			//printf("Contador: %d\n", contador);
		}
		while(contador <= NUM_TRANSMITIONS && estadoDFA != 0);
		
		//verDados(TRAMA, tamSET);
		//printf("Tamanho SET: %d\n", tamSET);
		
		if (estadoDFA < 0)
		{
			return(-1);
		}
		
		if (tamSET < 0)
		{
			printf("LLOPEN() :: RECEPTOR :: ERRO NA OBTENCAO DA TRAMA SET\n");
			return(-1);
		}
		
		if (tamSET == 5)
		{
			printf("TRAMA SET RECEBIDA... A ENVIAR UA\n");
			
			limparFrame(TRAMA2);
			int tamUA = suFrame(0, 1, 2, TRAMA2);
			
			//verDados(TRAMA2, tamUA);
			
			enviarr(r, TRAMA2, tamUA);
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
		limparFrame(TRAMA);
		int tamDisc = suFrame(app.status, 0, 1, TRAMA); // tramaSU(int eEmissor, int qualCtrama, int 0 ou 1)
		
		//verDados(tramaSET, 4);		
		printf("A ENVIAR TRAMA DISC\n");
		limparFrame(TRAMA2);
		int tamDISC2 = enviarrComAlarme(app.fileDescriptor, TRAMA, 5, TRAMA2, A2, C_DISC);
		
		//printf("TAMANHO DO TAMDISC2: %d\n", tamDISC2);

		if (tamDISC2 < 0)
		{
			printf("LLCLOSE() :: EMISSOR :: TIMEOUT LIMIT\n");
			return(-1);
		}
		printf("TRAMA DISC RECEBIDA... A ENVIAR UA\n");
		
		limparFrame(TRAMA);
		int tamUA = suFrame(app.status, 0, 2, TRAMA); // tramaSU(int eEmissor, int qualCtrama, int 0 ou 1)
		int res = enviarrSemAlarme(app.fileDescriptor, TRAMA, 5);

		//printf("TAMANHO DO TAMDISC2: %d\n", tamDISC2);
	}
	else // codigo do Receptor
	{
		contador = 0;
		
		int tamDISC = 0;
		int estadoDFA = 0;
		//limparFrame(TRAMA);
		do
		{
			limparFrame(TRAMA);
			tamDISC = leerSemAlarme(app.fileDescriptor, TRAMA);

			//TRAMA[1] = 0x45;
			estadoDFA = maquinaEstados(TRAMA, tamDISC, A1, C_DISC);
			contador++;
			//printf("Contador: %d\n", contador);
		}
		while(contador <= NUM_TRANSMITIONS && estadoDFA != 0);
		
		if(estadoDFA != 0)
		{
			return(-1);
		}
		
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
		limparFrame(TRAMA);
		int tamDISC2 = suFrame(app.status, 0, 1, TRAMA);
			
		limparFrame(TRAMA2);
		int tamUA = enviarrComAlarme(app.fileDescriptor, TRAMA, 5, TRAMA2, A2, C_UA);
			
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
	
	sleep(1);
	set_oldsettings(fd);
	close(fd);

	return(0);
}

int llwrite(int fd,  char * buffer, int length) // (file, pck, tamPKG)
{
	
	printf("\n\t\tLLWRITE()\n");
	int r = 0;
	
	limparFrame(TRAMA);
	int tamTrama = iFrame(buffer, length, TRAMA);
	
	unsigned char Cs = TRAMA[2];
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
	limparFrame(TRAMA2);
	int tamResposta = enviarrComAlarme(app.fileDescriptor, TRAMA, tamTrama+1, TRAMA2, A1, Cr);
	
	if (tamResposta < 0)
		return(tamResposta);

	return(tamTrama);

	/*
	printf("\n\t\tLLWRITE()\n");
	int r = 0;
	int tamResposta = 0, tamTrama = 0;
	int estadoDFA = 0;

	do
	{
		limparFrame(TRAMA);
		tamTrama = iFrame(buffer, length, TRAMA);
	
		unsigned char Cs = TRAMA[2];
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
		limparFrame(TRAMA2);
		tamResposta = enviarrComAlarme(app.fileDescriptor, TRAMA, tamTrama+1, TRAMA2, A1, Cr); 
 	}
	while(tamResposta < 0);

	if (tamResposta < 0)
		return(tamResposta);

	return(tamTrama);*/
}

int llread(int fd,  char * buffer)
{
	printf("\n\t\tLLREAD()\n");

	//printf("Endereco buffer: %d\n", &buffer[0]);
	
	int tamPacote = 0, tamTrama = 0, tamTrama2 = 0, res = 0;

	int estadoDFA;
	int numSeq;

	contador = 0;
	//limparFrame(TRAMA);
	//tamTrama = leerSemAlarme(app.fileDescriptor, TRAMA);	
	
	unsigned char Cs, Cr;

	do
	{
		tamTrama = 0;
		numSeq = 0;

		limparFrame(TRAMA);
		tamTrama = leerSemAlarme(app.fileDescriptor, TRAMA);
		//verDados(TRAMA, tamTrama);

		//printf("bife\n");
		//printf("%d\n", tamTrama);
		//printf("0x%X\n", TRAMA [0]);
		
		Cs = TRAMA[2];
		Cr = 0x00;
		if (Cs == C_S_0)
		{
			Cr = C_RR_1;
			numSeq = 0;
		}
		else if (Cs == C_S_1)
		{
			Cr = C_RR_0;
			numSeq = 1;
		}
		
		//TRAMA[1] = 0x45;
		printf("0x%X 0x%X\n", A1, Cs);
		estadoDFA = maquinaEstados(TRAMA, tamTrama-1, A1, Cs);
		printf("Estado DFA llread(): %d\n", estadoDFA);
		//printf("Estado dfa: %d\n", estadoDFA);
		contador++;
		//printf("Contador: %d\n", contador);
	}

	while(contador <= NUM_TRANSMITIONS && estadoDFA != 0);
	//printf("aqui\n");
	//verDados(TRAMA, tamTrama);
	
	// acrescentado
	
	

	/*
	unsigned char Cs = TRAMA[2];
	unsigned char Cr = 0x00;
	if (Cs == C_S_0)
	{
		Cr = C_RR_1;
		numSeq = 0;
	}
	else if (Cs == C_S_1)
	{
		Cr = C_RR_0;
		numSeq = 1;
	}
	

	estadoDFA = maquinaEstados(TRAMA, tamTrama-1, A1, Cs);
	*/

	limparPacote(PACOTE);
	tamPacote = iFrameToPackage(TRAMA, tamTrama-1, PACOTE);
	
	limparFrame(TRAMA2);
	tamTrama2 = suFrame(app.status, numSeq, 3, TRAMA2); //rr
	//verDados(trama2, 4);
	
	res = enviarrSemAlarme(app.fileDescriptor, TRAMA2, 5);
	
	//printf("Endereco buffer: %d\n", &buffer[0]);
	
	return(tamPacote);
}

