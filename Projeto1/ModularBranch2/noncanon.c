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
    //return (fcntl(fd, F_GETFD) != -1 || errno != EBADF);
    close(fd);
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

int read_UA(int * fd) // lê o UA
{
    int res = 0;
    unsigned char buf[5];
    unsigned char aux1[5];
    
    strcpy(buf, "");
    strcpy(aux1, "");
    
    tcflush(*fd, TCIFLUSH);
    
    int i = 0;
    printf("A receber UA... \n");
    
    
    i = 0;
    while (STOP==FALSE && !flag) // leitura do UA vindo do Receptor
    {
        res = read(*fd, buf, 1);
        if (res != 0)
        {
            aux1[i] = buf[0];
            if (aux1[i] == F && i != 0)
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
        printf("UA recebido: ");
        for (i = 0; i < strlen(aux1); i++)
        {
            printf("0x%x ", aux1[i]);
        } printf("\n");
    }
    if(flag == 1)
    {
        //STOP = FALSE;
        return -1;
    }
    
    return 0;
}

int write_SET(int * fd) // escreve o SET
{
    print("A enviar SET...\n");
    tcflush(*fd, TCOFLUSH);
    
    write(*fd, set, 5);
    
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
        
        sprintf(buf, "\nTentativa %d...\n", tentativas);
        write(STDOUT_FILENO, buf, sizeof(buf));
    }
}



int send_nc(int fd)
{
    (void) signal(SIGALRM, alarm_handler);  // instala rotina que atende interrupcao
    
    init_SET_and_UA(); // define SET e UA
    
    system("clear");
    
    set_newsettings_custom(fd, &oldtermios, &newtermios, 0.1, 0); // atribui o descritor do ficheiro de serial port
    
    while ( tentativas < 3 )
    {
        if ( flag == 1 )
        {
            alarm(3);
            write_SET(&fd);
            flag = 0;
            
            if ( read_UA(&fd) == 0 )
            {
                break;
            }
        }
    }
    
    set_oldsettings(fd,&oldtermios);
    close_nc(fd);
    
    if (tentativas == 3)
    {
        print("\nExcedido o tempo limite\n");
        return -1;
    }
    
    print("\nConexao estabelecida e garantida\n");
    
    return fd;
    
}

int receive_nc(int fd)
{
    int res=0;
    unsigned int x;
    unsigned char buf[5];
    
    init_SET_and_UA(); // define SET e UA
    
    system("clear");
    
    set_newsettings_custom(fd, &oldtermios, &newtermios, 0, 5); // atribui o descritor do ficheiro de serial port
    
    // receber set
    printf("Receiving SET...\n");
    while ( STOP==FALSE ) /* loop for input */
    {
        res = read(fd,buf,5);   /* returns after 5 chars have been input */
        printf("HEXACODES: ");
        for (x = 0; x < 5; x++)
        {
            printf("0x%x ", buf[x] );
        } printf("\n");
        
        if ( strcmp(buf, set) == 0 )
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

    
    set_oldsettings(fd,&oldtermios);
    close_nc(fd);
    
    if (tentativas == 3)
    {
        print("\nExcedido o tempo limite\n");
        return -1;
    }
    
    print("\nConexao estabelecida e garantida\n");
    
    return 0;
    
}



