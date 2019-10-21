/*
 * mensajes.h
 *
 *  Created on: 21 oct. 2019
 *      Author: utnso
 */

#ifndef MENSAJES_H_
#define MENSAJES_H_


#include <string.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>


typedef enum {
	HANDSHAKE
} operacion;

typedef enum{
	PEDIDO
}tipoMensaje;

typedef struct {
    int tamanioMensaje;
    tipoMensaje tipoMensaje;
    operacion   operacion;
} __attribute__((packed)) HeaderMuse;


int   enviarPaqueteInt(int destinatario,   tipoMensaje tipoMensaje, operacion operacion, int loQueEnvio);
void* armarHeaderMuse (int tamanioMensaje, tipoMensaje tipoMensaje, operacion operacion);

#endif /* MENSAJES_H_ */
