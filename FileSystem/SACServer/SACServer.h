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


typedef enum {
    write,
    open,
	read,
    rmdir,
	unlink,
	mkdir

} Syscall;

//AGREGAR LO QUE FALTA
typedef struct{
	char* path;
	char* buff;
	size_t size;
	Syscall syscall1;
	Syscall syscall2;
	Syscall syscall3;

}MensajeFUSE;



const unsigned long TamBloque = 4096;
const unsigned long CantidadBloques = 0;
const unsigned long CantidadMaxArchivos = 1024;
const unsigned long CaracteresMaximosNombre = 71;
t_log * logger;

MensajeFUSE* recibirMensajeFUSE(int socket);

#endif /* SACSERVER_H_ */
