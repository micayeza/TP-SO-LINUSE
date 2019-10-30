/*
 * CommonsFUSE.h
 *
 *  Created on: 18 oct. 2019
 *      Author: utnso
 */

#ifndef FILESYSTEM_COMMONSFUSE_H_
#define FILESYSTEM_COMMONSFUSE_H_

#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <commons/log.h>
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

typedef enum {
    writeSYS,
    openSYS,
	readSYS,
    rmdirSYS,
	unlinkSYS,
	mkdirSYS,
	readdirSYS,

}Syscall;

//AGREGAR LO QUE FALTA
typedef struct{
	char* path;
	char* buff;
	size_t size;
	off_t offset; //AGREGAR A LA SERIALIZACION
	Syscall syscall;


}MensajeFUSE;
int recibirTamanio(int fdDestinatario);
int enviarTamanio(int destinatario, int loQueEnvio);
int enviarPaqueteFUSE(int fdDestinatario,void* mensajeSerializado);
void* SerializarMensajeFUSE(MensajeFUSE* mensaje, int tamPath, int tamBuff);
MensajeFUSE DesserializarMensajeFUSE(void* serializado);
#endif /* FILESYSTEM_COMMONSFUSE_H_ */
