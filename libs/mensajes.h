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
#include <stdint.h>


typedef enum {
	SALUDO,
	CERRAR,
	RESERVAR,
	LIBERAR,
	OBTEBER,
	COPIAR,
	MAPEAR,
	SINCRO,
	DESMAP

} museOperacion;

typedef enum{
	MUSE,
	LIBMUSE
}tipoRemitente;

typedef struct {
    //int tamanioMensaje;
    tipoRemitente remitente;
    museOperacion   operacion;
} __attribute__((packed)) HeaderMuse;


void* armarHeaderMuse(tipoRemitente remitente, museOperacion operacion);
//HeaderMuse desempaquetarHeaderMuse(void* paquete);
int enviarHeaderMuse(int destinatario, tipoRemitente remitente, museOperacion operacion);
HeaderMuse recibirHeaderMuse(int destinatario);
int enviarInt(int destinatario, int loQueEnvio);
int recibirInt(int destinatario);
int enviarUint32_t(int destinatario, uint32_t loQueEnvio);
uint32_t recibirUint32_t(int destinatario);
int enviarUintSizet(int destinatario, uint32_t loQueEnvio, size_t n);
void recibirUintSizet(int destinatario, uint32_t contenido,size_t n );


char* recibirString(int destinatario);
int enviarString(int destinatario, char* loQueEnvio);


#endif /* MENSAJES_H_ */
