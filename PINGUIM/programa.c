#include "programa.h"

int main(int argc, char ** argv)
{
	system("clear");
	initAppLayer(&app);
	initLinkLayer(&lnk);

	if (argc >= 1) // ha argumentos
	{
		if (strcmp(argv[2], "send") == 0) // servidor deve ter 3 argumentos
		{
			if (argc < 4)
			{
				printf("MAIN :: SERVIDOR DEVE TER MINIMO 3 ARGUMENTOS\n");
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
				
				if (argc >= 5) // baudrate
				{				
					int n = toInt(argv[4]);
					lnk.baudrate = (n == 0) ? 1 : n;
				}
				if (argc >= 6) // timeout
				{				
					int n = toInt(argv[5]);
					lnk.timeout = (n == 0) ? 1 : n;
				}
				if (argc >= 7) // num_trans
				{				
					int n = toInt(argv[6]);
					lnk.numTransmissions = (n == 0) ? 1 : n;
				}
			}
		}
		else if (strcmp(argv[2], "get") == 0)// cliente deve ter 2 argumentos
		{
			if (argc < 3)
			{
				printf("MAIN :: SERVIDOR DEVE TER MINIMO 2 ARGUMENTOS\n");
				exit(1);
			}
			else
			{
				if (strcmp(argv[1], "server") != 0 && strcmp(argv[1], "client") != 0)
				{
					printf("MAIN :: %s NAO EXISTE\n", argv[2]);
					exit(1);
				}
				
				if (argc >= 4) // baudrate
				{				
					int n = toInt(argv[3]);
					lnk.baudrate = (n == 0) ? 1 : n;
				}
				if (argc >= 5) // timeout
				{				
					int n = toInt(argv[4]);
					lnk.timeout = (n == 0) ? 1 : n;
				}
				if (argc >= 6) // num_trans
				{				
					int n = toInt(argv[5]);
					lnk.numTransmissions = (n == 0) ? 1 : n;
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
	//printf("%d %u %u\n", lnk.baudrate, lnk.timeout, lnk.numTransmissions);
	//printf("App.STATUS: %d | eEmissor: %d | DISPOSITIVO: %d\n", app.status, eEmissor, device);
	//return(0);

	//initLinkLayer(&lnk, device);
  
	app.status = (eEmissor == 1) ? TRANSMITTER : RECEIVER ;
  	app.fileDescriptor = llopen(device,app.status);

	if (app.fileDescriptor == -1)
	{
		return(-1);
	}
	else
	{
		printf(" :: LIGADO COM SUCESSO\n");
	}	
	(void) signal(SIGINT, ctrlC_handler);  // instala rotina que atende interrupcao	

	/****************************************
				TRATAMENTO DE DADOS
	*****************************************/
	if (funcaoIO() < 0) return(-1);
		
	/****************************************
				TERMINAR O PROGRAMA
	*****************************************/
	//sleep(1);
	
	llclose(app.fileDescriptor);

	printf(" :: DESLIGADO COM SUCESSO\n");	

	estatistica();
	
	exit(0);
}

int funcaoIO()
{
	printf("\nINICIANDO TRANSMISSAO :: ");

	if (app.status == TRANSMITTER)
	{
		//printf(" :: EMISSOR :: ");

		// envio do pacote de controlo de inicio da transferencia do ficheiro
		sleep(1);
		int tamPacote = 0;
		
		limparPacote(PACOTE);
		tamPacote = controlToPackage(nomeFicheiro, tamFicheiro, Cpkg_inicio, PACOTE);
		
		int estEnvio = 0;
		estEnvio = llwrite(app.fileDescriptor, PACOTE, tamPacote);
		
		if (estEnvio < 0)
		{
			printf("ERRO , ENVIO DO PACOTE DE CONTROLO com 0x02\n");
			return(-1);
		}

		printf("SUCESSO , ENVIO DO PACOTE DE CONTROLO INICIAL\n");
		
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
				//printf("ola\n");
				limparPacote(PACOTE);
				tamPacote = dataToPackage(string, linhaActual, MFS, PACOTE);
				//printf("Tamanho do pacote: %d\n", tamPacote);
				//printf("ola 1.5\n");
				res = llwrite(app.fileDescriptor, PACOTE, tamPacote);		
				if (res < 0)
				{
					printf("ERRO NO ENVIO DA TRAMA DE INFORMACAO NUMERO %d\n", linhaActual);
					return(-1);
				}
				
				tamTotal += MFS;
				numTramasI_IO++;
						
				i = 0;
				linhaActual++;
				//printf("Linha %d\n", linhaActual);
				printf(" :: DADOS #%2.d\n", linhaActual);
				strcpy(string, "");
			}
			aux = fgetc(f);
		}
		
		limparPacote(PACOTE);	
		tamPacote = dataToPackage(string, linhaActual, i, PACOTE);
		res = llwrite(app.fileDescriptor, PACOTE, tamPacote);

		//printf("Resultdo llwrite(): %d\n", res);
		
		tamTotal += i;
		
		linhaActual++;
		printf(" :: DADOS #%2.d\n", linhaActual);
		numTramasI_IO++;
		
		fclose(f);
		
		sleep(1);
	
		// envio do pacote de controlo de inicio da transferencia do ficheiro
		limparPacote(PACOTE);
		tamPacote = controlToPackage(nomeFicheiro, tamFicheiro, Cpkg_fim, PACOTE);

		estEnvio = 0;
		estEnvio = llwrite(app.fileDescriptor, PACOTE, tamPacote);
		
		if (estEnvio < 0)
		{
			printf("ERRO , ENVIO DO PACOTE DE CONTROLO 0x03\n");
			return(-1);
		}
		printf("SUCESSO , ENVIO DO PACOTE DE CONTROLO FINAL\n");
	}
	else
	{
		printf(" :: RECEPTOR :: ");

		limparPacote(PACOTE);
		int tamFile = llread(app.fileDescriptor, PACOTE);

		//printf("Resultado do llread(): %d\n\n", tamFile);
		
		if (tamFile == -1)
		{
			printf("ERRO NA RECEPCAO DA TRAMA COM O PACOTE DE CONTROLO 0x02\n");
			return(-1);
		}
		
		printf("\nSUCESSO , RECEPCAO DO PACOTE DE CONTROLO");

		printf("\n");
		
		
		
		//verDados(informacaoFile, tamFile);
		//if (nomeFicheiro == NULL)
		//nomeFicheiro = (char *)malloc(sizeof(char) * 255);
		limparNomeFicheiro(nomeFicheiro);
		tamFicheiro = 0;
		
		
		packageToControl(PACOTE, (tamFile), nomeFicheiro, &tamFicheiro);
		
		//printf("Nome ficheiro: %s\n", nomeFicheiro);
		//printf("Tamanho do ficheiro: %d\n", tamFicheiro);
		
		//if (tamanhoFicheiro(nomeFicheiro) == -1) printf("FICHEIRO %s NAO EXISTE\n", nomeFicheiro);
		
		
		//nomeFicheiro = "galinha.gif";
		//strcpy(nomeFicheiro, "galinha.gif");
		
		// armazenar a informacao
		f = fopen(nomeFicheiro, "wb+");		
		int tamPacote = 0;
		int tamDados = 0;
		float qqq = 0.0;

		int res = 0;
		int linha = 0;
		while (tamTotal < tamFicheiro)
		{
			limparPacote(PACOTE);
			limparDados(DADOS);
			//setLinkLayer(&lnk, (lnk.sequenceNumber+1)%2);
			tamPacote = llread(app.fileDescriptor, PACOTE);
			
			if (tamPacote == -1)
			{
				printf("ERRO NA RECEPCAO DA TRAMA DE INFORMACAO NUMERO %d\n", linha+1);
				return(-1);
			}
			
			linha++;
			printf(" :: DADOS #%2.d :: ", linha);
			numTramasI_IO++;
			
			//verDados(PACOTE, tamPacote);
			//printf("Tamanho llread(): %d\n", tamPacote);
			
			tamDados = packageToData(PACOTE, tamPacote, DADOS);
			//printf("Tamanho dados: %d\n", tamDados);
			
			//verDados(data, tamDados);

			colocarFicheiro(f, DADOS, tamDados);
			//printf("Tamanho lido: %d\n", tamDados);
			//if (pkg != NULL)
			//free(pkg);

			//if (data != NULL)
			//free(data);			
			tamTotal += tamDados;

			qqq = (100.0 * (1.0 * tamTotal) / (1.0*tamFicheiro));

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
			printf("ERRO NA RECEPCAO DA TRAMA COM O PACOTE DE CONTROLO 0x03\n");
			return(-1);
		}
		
		//char * fff = (unsigned char*) malloc( sizeof(unsigned char) * MFS );
		char fff[30];
		limparNomeFicheiro(fff);
		
		unsigned int ttt = 0;

		//nomeFicheiro = "pinguim.gif";
		//strcpy(nomeFicheiro, "pinguim.gif");
		
		packageToControl(PACOTE, (tamFile), fff, &ttt);
		
		//printf("%s %d\n", fff, ttt);
		int rrr = strcmp(fff, nomeFicheiro);

		if (! (rrr == 0 && ttt == tamFicheiro) )
		{
			printf("ERRO NO PACOTE DE CONTROLO DE FIM DE TRANSFERENCIA\n");
			return(-1);
		}
		printf("SUCESSO , RECEPCAO DO PACOTE DE CONTROLO FINAL\n");
	}

}

void estatistica()
{
	printf("\n\n =============[ INFORMACAO DA TRANSMISSAO E FICHEIRO ]=============\n\n");

	if (app.status == TRANSMITTER)
	{
		
		printf("NOME: %s\n", nomeFicheiro);
		printf("TAMANHO ORIGINAL: %d\n", tamFicheiro);
		//printf("TAMANHO RECEBIDO: %d\n", tamTotal);
		printf("nº TRAMAS I ENVIADAS: %d\n", numTramasI_IO);
		printf("nº TRAMAS I RENVIADAS: %d\n", numTramasI_re);
		printf("nº TRAMAS I REJEITADAS: %d\n", numIO_REJ);
		printf("nº TIMEOUTS: %d\n", numTimeouts);
	}
	else
	{	
		//strcpy(nomeFicheiro, "galinha.gif");
		printf("NOME: %s\n", nomeFicheiro);
		printf("TAMANHO ORIGINAL: %d\n", tamFicheiro);
		printf("TAMANHO RECEBIDO: %d\n", tamTotal);
		printf("TAMANHO CRIADO: %d\n", tamanhoFicheiro(nomeFicheiro));
		//strcpy(nomeFicheiro, "pinguim.gif");
		printf("nº TRAMAS I RECEBIDAS: %d\n", numTramasI_IO);
		printf("nº TRAMAS I REJEITADAS: %d\n", numIO_REJ);
		printf("nº TIMEOUTS: %d\n", numTimeouts);


	}

	printf("\t-------------------------------------------------------------\n");
}

/************************************************************************************************************************
*														DEFINICAO DAS FUNCOES A IMPLEMENTAR
************************************************************************************************************************/
int llopen(int porta, size_t flaggg)
{
	printf("\nESTABELECENDO LIGACAO");
	
	int r = 3; // diferente de 0 - read, 1 - write, 2 - erros
	char portaa[11] = "/dev/ttyS0";

	if (porta == 4)
	{
		portaa[9] = '4';
	}
	else if (porta != 0)
	{
		printf(" :: PORTA %d INVALIDA\n", porta);
		return(-1);
	}
	
	r = open(portaa, O_RDWR | O_NOCTTY);
	if (r == -1)
	{
		printf(" :: FALHOU A LIGACAO\n");
		return(-1);
	}
	
	if (flaggg == TRANSMITTER) // llopen do TRANSMITTER
	{
		sleep(1);
		//app.status = 1;
		printf(" :: EMISSOR :: ");

		set_newsettings(r, 0.1, 0.0); // 0.1 0 	

		limparFrame(TRAMA);
		int tamSU = suFrame(1, 0, 0, TRAMA); // int numSeq, int qualC
		
		//verDados(TRAMA, tamSU);
		limparFrame(TRAMA2);
		int tamUA = enviarrComAlarme(r, TRAMA, tamSU, TRAMA2, A1, C_UA);
		
		//verDados(TRAMA2, tamUA);
		
		if (tamUA < 0)
		{
			printf("TIMEOUT LIMIT\n");
			return(-1);
		}
		
		// trama UA e 'tramaUA' com tamanho 'estado'
		
		//printf("Tamanho UA: %d\n", tamUA);
		
		if (tamUA != 5)
		{
			printf("TAMANHO UA DIFERENTE DE 5. TAMANHO = %d\n", tamUA);
			return(-1);
		}		
	}
	else if (flaggg == RECEIVER) // llopen do RECEIVER
	{
		//app.status = 0;
		printf(" :: RECEPTOR :: ");

		set_newsettings(r, 0.1, 1.0); // ------> 0.1 1.0	
		
		num_trans = 0;
		
		int tamSET = 0;
		int estadoDFA = 0;
		
		do
		{
			limparFrame(TRAMA);
			tamSET = leerSemAlarme(r, TRAMA);

			//verDados(TRAMA, tamSET);
			//TRAMA[1] = 0x45;
			estadoDFA = maquinaEstados(TRAMA, tamSET, A1, C_SET);
			num_trans++;

			//printf("Contador: %d\n", contador);
		}
		while(num_trans <= lnk.numTransmissions && estadoDFA != 0);
		
		if (num_trans > lnk.numTransmissions)
		{
			printf("NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
			return(-1);
		}
		
		//verDados(TRAMA, tamSET);
		//printf("Tamanho SET: %d\n", tamSET);
		
		if (estadoDFA < 0)
		{
			return(-1);
		}
		
		if (tamSET < 0)
		{
			printf("ERRO NA OBTENCAO DA TRAMA SET\n");
			return(-1);
		}
		
		if (tamSET == 5)
		{
			printf("SET RECEBIDO , ENVIANDO UA\n");
			
			limparFrame(TRAMA2);
			int tamUA = suFrame(0, 1, 2, TRAMA2);
			
			//verDados(TRAMA2, tamUA);
			
			enviarr(r, TRAMA2, tamUA);
		}
		else // invalido
		{
			printf("SET INVALIDO , TAMANHO DIFERENTE  %d != 5\n", tamSET);
			return(-1);
		}
	}
	else
	{
		printf(" :: FLAG INVALIDA\n");
		return(-1);
	}

	return(r);
}

int llclose(int fd)
{
	printf("\nDESLIGANDO LIGACAO");
	
	int r = 0;
	
	if (app.status == TRANSMITTER) // codigo do Emissor
	{
		printf(" :: EMISSOR :: ");
				
		//printf("Codigo do emissor do llclose()\n");
		limparFrame(TRAMA);
		int tamDisc = suFrame(app.status, 0, 1, TRAMA); // tramaSU(int eEmissor, int qualCtrama, int 0 ou 1)
		
		//verDados(tramaSET, 4);		
		printf("ENVIANDO DISC\n");
		limparFrame(TRAMA2);
		int tamDISC2 = enviarrComAlarme(app.fileDescriptor, TRAMA, 5, TRAMA2, A2, C_DISC);
		
		//printf("TAMANHO DO TAMDISC2: %d\n", tamDISC2);

		if (tamDISC2 < 0)
		{
			printf("TIMEOUT LIMIT\n");
			return(-1);
		}
		printf(":: DISC RECEBIDO , ENVIANDO UA\n");
		
		limparFrame(TRAMA);
		int tamUA = suFrame(app.status, 0, 2, TRAMA); // tramaSU(int eEmissor, int qualCtrama, int 0 ou 1)
		int res = enviarrSemAlarme(app.fileDescriptor, TRAMA, 5);

		//printf("TAMANHO DO TAMDISC2: %d\n", tamDISC2);
	}
	else // codigo do Receptor
	{
		printf(" :: RECEPTOR :: ");		

		num_trans = 0;
		
		int tamDISC = 0;
		int estadoDFA = 0;
		//limparFrame(TRAMA);
		do
		{
			limparFrame(TRAMA);
			tamDISC = leerSemAlarme(app.fileDescriptor, TRAMA);

			//TRAMA[1] = 0x45;
			estadoDFA = maquinaEstados(TRAMA, tamDISC, A1, C_DISC);
			num_trans++;
			//printf("Contador: %d\n", contador);
		}
		while(num_trans <= lnk.numTransmissions && estadoDFA != 0);
		
		if (num_trans > lnk.numTransmissions)
		{
			printf("NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
			return(-1);
		}
		
		if(estadoDFA != 0)
		{
			return(-1);
		}
		
		//printf("TAMANHO DO TAMDISC: %d\n", tamDISC);

		//printf("Tamanho SET: %d\n", tamSET);
		
		if (tamDISC < 0)
		{
			printf("ERRO NA OBTENCAO DA TRAMA DISC\n");
			return(-1);
		}
		
		if (tamDISC != 5)
		{
			printf("DISC INVALIDO , TAMANHO DIFERENTE  %d != 5\n", tamDISC);
			return(-1);
		}
		
		printf("DISC RECEBIDO , A ENVIAR DISC\n");
		limparFrame(TRAMA);
		int tamDISC2 = suFrame(app.status, 0, 1, TRAMA);
			
		limparFrame(TRAMA2);
		int tamUA = enviarrComAlarme(app.fileDescriptor, TRAMA, 5, TRAMA2, A2, C_UA);
			
		//printf("TAMANHO DO TAMUA: %d\n", tamUA);
		
		if (tamUA < 0)
		{
			printf("ERRO NA OBTENCAO DA TRAMA UA\n");
			return(-1);
		}
		
		if (tamUA != 5)
		{
			printf("TAMANHO UA DIFERENTE DE 5. TAMANHO = %d\n", tamUA);
			return(-1);
		}
		
		printf(" :: UA RECEBIDO\n");		
	}
	
	sleep(1);
	set_oldsettings(fd);
	close(fd);

	return(0);
}

int llwrite(int fd,  char * buffer, int length) // (file, pck, tamPKG)
{
	int r = 0;
	
	limparFrame(TRAMA);
	int tamTrama = iFrame(buffer, length, TRAMA);
	int tamResposta = 0;
	num_trans = 0;
	contador = 0;
	
	unsigned char Cs = TRAMA[2];
	unsigned char Crr = 0x00, Crej = 0x00;

	do
	{
		sleep(1);
		if (Cs == C_S_0)
		{
			Crr = C_RR_1;
			Crej = C_REJ_1;
			r = 0;
			//printf("llwrite() :: Ns = 0!!\n");
		}
		else if (Cs == C_S_1)
		{
			Crr = C_RR_0;
			Crej = C_REJ_0;
			r = 1;
			//printf("llwrite() :: Ns = 1!!\n");
		}
		else
		{
			printf("Erro na trama: Caracter do C corresponde ao 0x%X\n", Cs);
			return(-1);
		}
		if (contador > 0)
			printf("RETRANSMISSAO DA TRAMA PELA %do VEZ\n", contador);
	
		limparFrame(TRAMA2);
		r = 0; //verificar se rr ou rej ou ignorar
		
		tamResposta = enviarTramaI(app.fileDescriptor, TRAMA, tamTrama+1, &r, TRAMA2, Crr, Crej);
		
		//printf("tamResposta: %d\n", tamResposta);
		
		if (tamResposta < 0) // -> r = -1
			return(tamResposta);

		if (r == 1) //rej
		{
			//num_trans++;
			numIO_REJ++;
			contador++;
		}
	}
	while( contador <= lnk.numTransmissions && r == 1); //rej ou contador
	
	if (contador > lnk.numTransmissions)
	{
		printf("NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
		return(-1);
	}
	
	return(tamTrama);
}

int llread(int fd,  char * buffer)
{
	
	int tamPacote = 0, tamTrama = 0, tamTrama2 = 0, res = 0;
	int estadoDFA = 0, numSeq = 0;

	num_trans = 0;
	contador = 0;

	unsigned char Cs, Cr;

	do
	{
		tamTrama = 0;
		numSeq = 0;

		limparFrame(TRAMA);
		tamTrama = leerSemAlarme(app.fileDescriptor, TRAMA);
		
		limparFrame(TRAMA2);
		copiarPara(TRAMA, TRAMA2, tamTrama); //copiar temporariamente para TRAMA2 para testar erros de bcc
		
		//verDados(TRAMA, tamTrama);
		
		//descomentar codigo abaixo para testar erro bcc1 / bcc2
		//if (num_trans < 3)
			//TRAMA2[7] = 0x23;
		
		//printf("bife\n");
		//printf("%d\n", tamTrama);
		//printf("0x%X\n", TRAMA [0]);
		
		Cs = TRAMA[2];
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
		
		//printf("0x%X 0x%X\n", A1, Cs);
		estadoDFA = maquinaEstados(TRAMA2, tamTrama-1, A1, Cs);
		//printf("Estado DFA llread(): %d\n", estadoDFA);
		

		//limparPacote(PACOTE);
		tamPacote = iFrameToPackage(TRAMA2, tamTrama-1, buffer); // <-- alterei aqui
		
		limparFrame(TRAMA2);
		if (estadoDFA == 0) // rr
		{
			//printf("llread() :: RR!!\n");
			tamTrama2 = suFrame(app.status, numSeq, 3, TRAMA2); //rr
		}
		else if (estadoDFA == -2) // rej
		{
			//printf("llread() :: REJ!!\n");
			tamTrama2 = suFrame(app.status, numSeq, 4, TRAMA2); //bcc2 erro -> rej
			//num_trans++;
			contador++;

			numIO_REJ++;
			numTramasI_re++;
			
			//int numTramasI_IO = 0, numTramasI_re = 0, numTimeouts = 0, numIO_REJ = 0;
		}
		else
		{
			//num_trans++; // bcc1 erro
			contador++;
		}
	
		res = enviarrSemAlarme(app.fileDescriptor, TRAMA2, 5);
	}
	while(contador < lnk.numTransmissions && estadoDFA != 0);
	
	if (contador >= lnk.numTransmissions)
	{
		printf(" :: NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
		return(-1);
	}
	
	return(tamPacote);
}

