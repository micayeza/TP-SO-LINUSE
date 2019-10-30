/*
 * SACServer.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#ifndef SACSERVER_H_
#define SACSERVER_H_
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <commons/log.h>
#include "../CommonsFUSE.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#define ERROR -1


//const unsigned long TamBloque = 4096;
//const unsigned long CantidadBloques = 0;
//const unsigned long CantidadMaxArchivos = 1024;
//const unsigned long CaracteresMaximosNombre = 71;
t_log * logger;



#endif /* SACSERVER_H_ */
