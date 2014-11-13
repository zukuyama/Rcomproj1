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
	
	printf("%d %u %u\n", lnk.baudrate, lnk.timeout, lnk.numTransmissions);	
	
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
		printf("CONEXAO ESTABELECIDA\n");
	}	
	
	/****************************************
				TRATAMENTO DE DADOS
	*****************************************/
	//return(0);
	if (funcaoIO() < 0) return(-1);
		
	/****************************************
				TERMINAR O PROGRAMA
	*****************************************/
	//sleep(1);
	
	llclose(app.fileDescriptor);
	
	estatistica();
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
		
		limparPacote(PACOTE);
		tamPacote = controlToPackage(nomeFicheiro, tamFicheiro, Cpkg_inicio, PACOTE);
		
		int estEnvio = 0;
		estEnvio = llwrite(app.fileDescriptor, PACOTE, tamPacote);
		
		if (estEnvio < 0)
		{
			printf("LLWRITE() :: ERRO NO ENVIO DA TRAMA COM O PACOTE DE CONTROLO 0X02\n");
			return(-1);
		}
		
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
				limparPacote(PACOTE);
				tamPacote = dataToPackage(string, linhaActual, MFS, PACOTE);
				//printf("Tamanho do pacote: %d\n", tamPacote);
				
				res = llwrite(app.fileDescriptor, PACOTE, tamPacote);		
				if (res < 0)
				{
					printf("LLWRITE() :: ERRO NO ENVIO DA TRAMA DE INFORMACAO NUMERO %d\n", linhaActual);
					return(-1);
				}
				
				tamTotal += MFS;
				numTramasI_IO++;
						
				i = 0;
				linhaActual++;
				printf("Linha %d\n", linhaActual);
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
		printf("Linha %d\n", linhaActual);
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
			printf("LLWRITE() :: ERRO NO ENVIO DA TRAMA COM O PACOTE DE CONTROLO 0X03\n");
			return(-1);
		}
	}
	else
	{
		limparPacote(PACOTE);
		int tamFile = llread(app.fileDescriptor, PACOTE);

		//printf("Resultado do llread(): %d\n\n", tamFile);
		
		if (tamFile == -1)
		{
			printf("LLREAD() :: ERRO NA RECEPCAO DA TRAMA COM O PACOTE DE CONTROLO 0X02\n");
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
				printf("LLREAD() :: ERRO NA RECEPCAO DA TRAMA DE INFORMACAO NUMERO %d\n", linha+1);
				return(-1);
			}
			
			linha++;
			printf("LINHA %d\n", linha);
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
			printf("LLREAD() :: ERRO NA RECEPCAO DA TRAMA COM O PACOTE DE CONTROLO 0X03\n");
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

void estatistica()
{
	if (app.status == TRANSMITTER)
	{
		printf("\n\t\t-----ESTATISTICA DOS DADOS ENVIADOS-----\n");
		
		printf("NOME DO FICHEIRO: %s\n", nomeFicheiro);
		printf("TAMANHO DO FICHEIRO ORIGINAL: %d\n", tamFicheiro);
		printf("NUMERO DE BYTES DO FICHEIRO ENVIADOS: %d\n", tamTotal);
		printf("NUMERO DE TRAMAS I ENVIADAS: %d\n", numTramasI_IO);
		printf("NUMERO DE TRAMAS I RENVIADAS: %d\n", numTramasI_re);
		printf("NUMERO DE TRAMAS I REJEITADAS: %d\n", numIO_REJ);
		printf("NUMERO DE TIMEOUT OCORRIDOS: %d\n", numTimeouts);
	}
	else
	{
		printf("\n\t\t-----ESTATISTICA DOS DADOS RECEBIDOS-----\n");
		
		strcpy(nomeFicheiro, "galinha.gif");
		printf("NOME DO FICHEIRO: %s\n", nomeFicheiro);
		printf("TAMANHO DO FICHEIRO ORIGINAL: %d\n", tamFicheiro);
		printf("NUMERO DE BYTES DO FICHEIRO RECEBIDOS: %d\n", tamTotal);
		printf("TAMANHO DO FICHEIRO RECEBIDO: %d\n", tamanhoFicheiro(nomeFicheiro));
		strcpy(nomeFicheiro, "pinguim.gif");
		printf("NUMERO DE TRAMAS I RECEBIDAS: %d\n", numTramasI_IO);
		printf("NUMERO DE TRAMAS I REJEITADAS: %d\n", numIO_REJ);
		printf("NUMERO DE TIMEOUT OCORRIDOS: %d\n", numTimeouts);
	}

	printf("\t-------------------------------------------------------------\n");
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
			printf("LLOPEN() :: NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
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
			printf("LLCLOSE() :: NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
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
			printf("LLCLOSE() :: RECEPTOR :: ERRO NA OBTENCAO DA TRAMA DISC\n");
			return(-1);
		}
		
		if (tamDISC != 5)
		{
			printf("LLCLOSE() :: RECEPTOR :: TAMANHO DISC DIFERENTE DE 5. TAMANHO = %d\n", tamDISC);
			return(-1);
		}
		
		printf("TRAMA DISC RECEBIDA... A ENVIAR SEGUNDA TRAMA DISC\n");
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
		
		printf("TRAMA UA RECEBIDA\n");		
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
		printf("LLWRITE() :: NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
		return(-1);
	}
	
	return(tamTrama);
}

int llread(int fd,  char * buffer)
{
	printf("\n\t\tLLREAD()\n");
	
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
		printf("LLREAD() :: NUMERO DE RETRANSMISSOES EXCEDIDAS\n");
		return(-1);
	}
	
	return(tamPacote);
}

