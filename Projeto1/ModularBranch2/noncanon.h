//
//  noncanon.h
//  RCOM_PROJ1
//
//  Created by GROUP_.
//

#ifndef __RCOM_PROJ1__noncanon__
#define __RCOM_PROJ1__noncanon__

/*Non-Canonical Input Processing*/
/*********************************************************
 *	COMPILAR: 	gcc s.c -o s
 * COMPILAR AMBOS: 	gcc r.c -o r | gcc s.c -o s
 *	EXECUTAR: 	./s /dev/ttySx , x = {0, 4}
 *	x = 4: servidor
 *	x = 0: cliente
 * ------------------------------------------------------
 *										set | tentativa1
 *		emissor(programa) --------------> receptor
 *
 *										set | tentativa2
 *		emissor(programa) --------------> receptor
 *
 *							(caso sucesso)
 *
 *														ua
 *		emissor(programa) <-------------- receptor
 *********************************************************/

#include "include.h"

#include "auxiliar.h"
#include "protocol.h"

/************************************************
 *							FUNCTION DECLARATIONS
 ************************************************/

void alarm_handler();

int  open_nc(int * fd, const char * port);
void close_nc(int fd);

void set_newsettings(int fd, struct termios * oldtermios, struct termios * newtermios);
void set_newsettings_custom(int fd, struct termios * oldtermios, struct termios * newtermios, float vtime, int vmin);
void set_oldsettings(int fd, struct termios * oldtermios);

int  read_UA(int * fd);
int  write_SET(int * fd);

int  send_nc(int fd);
int  receive_nc(int fd);


/************************************************
 *								MAIN FUNCTION
 ************************************************/


#endif /* defined(__RCOM_PROJ1__noncanon__) */
