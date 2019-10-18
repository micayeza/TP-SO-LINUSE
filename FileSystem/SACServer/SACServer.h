/*
 * SACServer.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#ifndef SACSERVER_H_
#define SACSERVER_H_
#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <commons/log.h>
#include "CommonsFUSE.h"




const unsigned long TamBloque = 4096;
const unsigned long CantidadBloques = 0;
const unsigned long CantidadMaxArchivos = 1024;
const unsigned long CaracteresMaximosNombre = 71;
t_log * logger;

MensajeFUSE* recibirMensajeFUSE(int socket);

#endif /* SACSERVER_H_ */
