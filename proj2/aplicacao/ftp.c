/*
*	RCOM 2014/2015
*
*	Turma 3 Bancada 5
*/
#include "ftp.h"

int main(int argc, char** argv)
{
	/*********************************
	*	VARIABLES
	*********************************/
	SOCKADDR_IN server_addr;
	SOCKADDR_IN server_addr2;
	
	int	bytes;
	int	sockfd, sockfd2;

	char user[40];
	char pass[40];
	char host[40];
	char path[2048];

	HOSTENT * h;
	HOSTENT * h2;

	char * SERVER_ADDR;

	int codigo = 0;
	char response[256];
	
	char userSend[45];
	char passSend[45];
	char pasvSend[45];
	
	int pasv1, pasv2, pasv3, pasv4, pasv5, pasv6;
	int port;
	char ip[256];
	
	char retrSend[45];
	
	char buffer[256];
	char filename[2048];
	int i, j, nb, tam;
	
	/*********************************
	*	INICIO DO PROGRAMA
	*********************************/
	system("clear");
	msg("-----------------------------------------------------------------------------------");
	msg("			RCOM PROJECTO 2 :: APLICACAO DOWNLOAD");
	msg("-----------------------------------------------------------------------------------");
	msg("		Socket 1");
	msg("-----------------------------------");
	
	if (argc != 2)
	{
		msgErro("Assim se usa: ftp ftp://[<user>:<password>@]<host>/<url-path>");
	}

	if(sscanf(argv[1], "ftp://%[^:]:%[^@]@%[^/]/%s\n", user, pass, host, path) == 4)
	{
		printf("User: %s\n", user);
		printf("Pass: %s\n", pass);
		printf("Host: %s\n", host);
		printf("Path: %s\n", path);
	}
	else if(sscanf(argv[1], "ftp://%[^/]/%s\n", host, path) == 2)
	{
		printf("Host: %s\n", host);
		printf("Path: %s\n", path);
		strcpy(user, "Anonymous");
		strcpy(pass, "123");

	}
	else
	{
		fprintf(stderr,"Invalid URL! Usage: ftp ftp://[<user>:<password>@]<host>/<url-path>");
	}

	h=gethostbyname(host);
	if (h == NULL)
	{
		herror("gethostbyname");
		exit(1);
	}

	SERVER_ADDR = inet_ntoa(*((struct in_addr *)h->h_addr));

	printf("Host name  : %s\n", h->h_name);
	printf("IP Address : %s\n", SERVER_ADDR);

	//manipulacao de endereco do servidor
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	//32 bit Internet address network byte ordered
	server_addr.sin_port = htons(SERVER_PORT);		//server TCP port must be network byte ordered

	//abertura do socket TCP para a ligacao de controlo
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0)
	{
		msgErro("socket()");
	}
	
	msg("\n[Programa] ~ Pedido de conexao enviado");
	
	//ligacao ao servidor 
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		msgErro("connect()");
	}

    	//resposta do servidor
	memset(response, 0, 256);
	bytes = read(sockfd, response, 50);
	
	printf("[Servidor] < %s\n", response);
	
	/* login no servidor*/
	// enviar username
	strcpy(userSend, "user ");
	strcat(userSend, user);
	strcat(userSend, "\n");
	
	printf("[Comando]  > %s", userSend);

	if(write(sockfd, userSend, strlen(userSend))<0)
	{
		msgErro("ftp.c: line 124");
	}

    	//resposta do servidor
	memset(response, 0, 256);
	bytes = read(sockfd, response, 50);
	printf("[Servidor] < %s\n", response);
		
			
	//enviar password
	strcpy(passSend, "pass ");
	strcat(passSend, pass);
	strcat(passSend, "\n");
	printf("[Comando]  > %s", passSend);

	if(write(sockfd, passSend, strlen(passSend))<0)
	{
		perror("");
		exit(0);
	}
	
	// resposta do servidor
	memset(response, 0, 256);
	bytes = read(sockfd, response, 50);
	printf("[Servidor] < %s\n", response);

	if (getCodigo(response) == 530) // password errada
	{
		msgErro("Palavra-chave errada");
	}
	
	//enviar comando pasv
	strcpy(pasvSend, "pasv");
	strcat(pasvSend, "\n");
	printf("[Comando]  > %s", pasvSend);

	if(write(sockfd, pasvSend, strlen(pasvSend))<0)
	{
		perror("");
		exit(0);
	}


    	//resposta do servidor com o novo ip e a porta de ligacao
	memset(response, 0, 256);
	bytes = read(sockfd, response, 50);
	printf("[Servidor] < %s\n", response);

	
	if(sscanf(response, "%*[^(](%d,%d,%d,%d,%d,%d)\n", &pasv1, &pasv2, &pasv3, &pasv4, &pasv5, &pasv6)!=6)
	{
		perror("pasv");
	}

	//conversao do novo ip e da porta de ligacao ao servidor
	port = pasv5*256 + pasv6;
	memset(ip, 0, 256),
	sprintf(ip, "%d.%d.%d.%d", pasv1, pasv2, pasv3, pasv4);
	printf("New IP: %s\n", ip);
	printf("New Port: %d (%d*256 + %d)\n", port, pasv5, pasv6);

	//repetir a coneccao, mas agora ao novo IP
	h2=gethostbyname(ip);
	if (h2 == NULL)
	{
		herror("gethostbyname");
		exit(1);
	}

	SERVER_ADDR = inet_ntoa(*((struct in_addr *)h2->h_addr));

	printf("Host name  : %s\n", h2->h_name);
	printf("IP Address : %s\n\n", SERVER_ADDR);

	//manipulacao de endereco do servidor
	bzero((char*)&server_addr2,sizeof(server_addr2));
	server_addr2.sin_family = AF_INET;
	server_addr2.sin_addr.s_addr = inet_addr(SERVER_ADDR);	//32 bit Internet address network byte ordered
	server_addr2.sin_port = htons(port);		//server TCP port must be network byte ordered

	msg("-----------------------------------");
	msg("		Socket 2");
	msg("-----------------------------------");

	//abertura de um segundo socket TCP para ligacao de dados
	sockfd2 = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd2 < 0)
	{
		perror("socket()");
		exit(0);
	}
	
	//ligacao ao servidor 
	if(connect(sockfd2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0)
	{
		perror("connect()");
		exit(0);
	}

	//enviar o comando de retorno de um ficheiro para o socket de controlo
	strcpy(retrSend, "retr ");
	strcat(retrSend, path);
	strcat(retrSend, "\n");
	printf("[Comando]  > %s", retrSend);

	if(write(sockfd, retrSend, strlen(retrSend)) < 0)
	{
		msgErro("ftp.c : line 226");
	}

	// resposta do servidor
	memset(response, 0, 256);
	bytes = read(sockfd, response, 50);
	printf("[Servidor] < %s\n", response);

	if (getCodigo(response) == 550) // password errada
	{
		exit(1);
	}
	
	// recepcao ficheiro
	i = strlen(path)-1;
	j = 0;
	while(i > 0)
	{
		if(path[i] == '/')
		{
			i++;
			break;
		}
		
		i--;
	}

	while(i < strlen(path))
	{
		filename[j] = path[i];
		i++;
		j++;
	}
	filename[j] = '\0';

	printf("[Programa] ~ Nome do ficheiro: %s\n", filename);
	printf("[Programa] ~ A receber ficheiro...\n\n");
	FILE* fx = fopen(filename, "wb");
	while((nb=read(sockfd2, buffer, 255)) > 0)
	{
		buffer[nb] = '\0';
		fwrite(buffer, sizeof(char), nb, fx);
	}
	fclose(fx);
	
	close(sockfd);
	close(sockfd2);
	
	tam = tamanhoFicheiro(filename);
	
	msg("-----------------------------------");
	msg("		Resultado final");
	msg("-----------------------------------");
	
	if (tam < 0)
	{
		msgErro("[Programa] ~ Ficheiro recebido com erros ou nao foi recebido");
	}
	else
	{
		printf("[Programa] ~ Ficheiro recebido com sucesso\n");
		printf("[Programa] ~ Tamanho ficheiro: %d bytes\n", tam);
	}
	
	msg("-----------------------------------------------------------------------------------");
	msg("				FIM DO PROGRAMA");
	msg("-----------------------------------------------------------------------------------");

	exit(0);
}
