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
	SALUDO
} museOperacion;

typedef enum{
	PEDIDO
}museTipoMensaje;

typedef struct {
    int tamanioMensaje;
    museTipoMensaje tipoMensaje;
    museOperacion   operacion;
} __attribute__((packed)) HeaderMuse;


int   enviarPaqueteInt(int destinatario,   museTipoMensaje tipoMensaje, museOperacion operacion, int loQueEnvio);
void* armarHeaderMuse (int tamanioMensaje, museTipoMensaje tipoMensaje, museOperacion operacion);
HeaderMuse desempaquetarHeaderMuse(void* paquete);
int contenidoMensajeInt(void* paquete);
void* recibirPaqueteInt(int destinatario);
#endif /* MENSAJES_H_ */
