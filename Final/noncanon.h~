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

void set_newsettings(int fd);
void set_newsettings_custom(int fd, float vtime, int vmin);
void set_oldsettings(int fd);

int  read_cc(int fd, unsigned char cc);
int  write_cc(int fd, unsigned char cc);

int  send(int fd);
int  receive(int fd);


/************************************************
 *								MAIN FUNCTION
 ************************************************/


#endif /* defined(__RCOM_PROJ1__noncanon__) */
