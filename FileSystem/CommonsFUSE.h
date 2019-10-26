/*
 * CommonsFUSE.h
 *
 *  Created on: 18 oct. 2019
 *      Author: utnso
 */

#ifndef FILESYSTEM_COMMONSFUSE_H_
#define FILESYSTEM_COMMONSFUSE_H_


typedef enum {
    write,
    open,
	read,
    rmdir,
	unlink,
	mkdir,
	readdir

}Syscall;

//AGREGAR LO QUE FALTA
typedef struct{
	char* path;
	char* buff;
	size_t size;
	off_t offset; //AGREGAR A LA SERIALIZACION
	Syscall syscall;


}MensajeFUSE;

int enviarPaqueteFUSE(int fdDestinatario,void* mensajeSerializado);
void* SerializarMensajeFUSE(MensajeFUSE* mensaje, int tamPath, int tamBuff);
MensajeFUSE DesserializarMensajeFUSE(void* serializado);
#endif /* FILESYSTEM_COMMONSFUSE_H_ */
