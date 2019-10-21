/*
 * mensajes.c
 *
 *  Created on: 21 oct. 2019
 *      Author: utnso
 */

#include "mensajes.h"

void* armarHeaderMuse(int tamanioMensaje, museTipoMensaje tipoMensaje, museOperacion operacion) {
    HeaderMuse header = {.tamanioMensaje = tamanioMensaje , .tipoMensaje = tipoMensaje, .operacion = operacion};
    	void* headerSerializado = malloc(sizeof(HeaderMuse));
        void* puntero = headerSerializado;
        int tamanioSize = sizeof(typeof(header.tamanioMensaje));
        int pesoTipoMensaje = sizeof(typeof(header.tipoMensaje));
        int pesoTipoRequest = sizeof(typeof(header.operacion));

        memcpy(puntero, &(header.tamanioMensaje), tamanioSize);
        puntero += tamanioSize;
        memcpy(puntero, &(header.tipoMensaje), pesoTipoMensaje);
        puntero += pesoTipoMensaje;
        memcpy(puntero, &(header.operacion), pesoTipoRequest);

        return headerSerializado;

}


int enviarPaqueteInt(int destinatario, museTipoMensaje tipoMensaje, museOperacion operacion, int loQueEnvio)  {
    int tamanioMensaje = sizeof(int);
    void* headerSerializado = armarHeaderMuse(tamanioMensaje, tipoMensaje, operacion);
    int pesoPaquete = sizeof(HeaderMuse) + tamanioMensaje;

    void* paquete = malloc(sizeof(HeaderMuse) + sizeof(int));
    void* puntero = paquete;
    memcpy(puntero, headerSerializado, sizeof(HeaderMuse));
    puntero += sizeof(HeaderMuse);
    memcpy(puntero, &(loQueEnvio), sizeof(int));
    int res = send(destinatario, paquete, pesoPaquete, MSG_WAITALL);
    free(headerSerializado);
    free(paquete);

    return res;
}

int recibirPaqueteInt(int destinatario){
	int pesoPaquete = sizeof(HeaderMuse) + sizeof(int);
	void* paquete = malloc(pesoPaquete);

	int res = recv(destinatario, paquete, pesoPaquete, MSG_WAITALL);
	if(res < 0) { return -1;}

	return 0;


}

HeaderMuse desempaquetarHeader(void* paquete){

	HeaderMuse header;
	int tamanioSize = sizeof(typeof(header.tamanioMensaje));
	int pesoTipoMensaje = sizeof(typeof(header.tipoMensaje));
	int pesoTipoRequest = sizeof(typeof(header.operacion));

	memcpy(&(header.tamanioMensaje), paquete, tamanioSize);
	paquete += tamanioSize;


	memcpy(&(header.tipoMensaje), paquete, pesoTipoMensaje);
	paquete += pesoTipoMensaje;

    memcpy(&(header.operacion), paquete, pesoTipoRequest);
    paquete += pesoTipoRequest;

	    return header;

}

int contenidoMensaje(void* paquete){
	int contenido = sizeof(int);
	paquete += sizeof(HeaderMuse);
	memcpy(&(contenido), paquete, sizeof(int));

	return contenido;
}




