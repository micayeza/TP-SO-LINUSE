/*
 * CommonsFUSE.c
 *
 *  Created on: 18 oct. 2019
 *      Author: utnso
 */

#include "CommonsFUSE.h"

void* SerializarMensajeFUSE(MensajeFUSE* mensaje, int tamPath, int tamBuff){
	void* MensajeSerializado = malloc(sizeof(MensajeFUSE));
	//void* puntero = MensajeSerializado;
	int desplazamiento = 0;

	memcpy(MensajeSerializado + desplazamiento, &(mensaje->syscall), sizeof(Syscall));
	desplazamiento += sizeof(Syscall);
	memcpy(MensajeSerializado + desplazamiento,&(tamPath),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(MensajeSerializado + desplazamiento,&(mensaje->path),tamPath);
	desplazamiento += tamPath;
	memcpy(MensajeSerializado + desplazamiento,&(tamBuff),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(MensajeSerializado + desplazamiento,&(mensaje->buff),tamBuff);
	desplazamiento += tamBuff;
	memcpy(MensajeSerializado + desplazamiento,&(mensaje->size),sizeof(mensaje->size));
	desplazamiento += sizeof(mensaje->size);

	return MensajeSerializado;
}

MensajeFUSE DesserializarMensajeFUSE(void* serializado){
	MensajeFUSE mensaje;
	void* puntero = serializado;
	int tamPath;
	int tamBuff;

	memcpy(&(mensaje.syscall), puntero, sizeof(Syscall));
	puntero += sizeof(Syscall);
	memcpy(&tamPath,puntero,sizeof(int));
	puntero += sizeof(int);
	memcpy(&(mensaje.path),puntero,tamPath);
	puntero+= tamPath;
	memcpy(&tamBuff,puntero,sizeof(int));
	puntero += sizeof(int);
	memcpy(&(mensaje.buff),puntero,tamBuff);
	puntero += tamBuff;
	memcpy(&(mensaje.size),puntero,sizeof(mensaje.size));

	return mensaje;
}

int enviarPaqueteFUSE(int fdDestinatario,void* mensajeSerializado) {

    int res = send(fdDestinatario, mensajeSerializado,sizeof(mensajeSerializado), MSG_WAITALL);
    free(mensajeSerializado);
    return res;
}


