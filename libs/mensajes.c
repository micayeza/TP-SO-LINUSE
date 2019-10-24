/*
 * mensajes.c
 *
 *  Created on: 21 oct. 2019
 *      Author: utnso
 */

#include "mensajes.h"

void* armarHeaderMuse(tipoRemitente remitente, museOperacion operacion) {
    HeaderMuse header = {.remitente = remitente, .operacion = operacion};
    	void* headerSerializado = malloc(sizeof(HeaderMuse));
        void* puntero = headerSerializado;

        int pesoTipoRemitente = sizeof(typeof(header.remitente));
        int pesoTipoRequest = sizeof(typeof(header.operacion));

        memcpy(puntero, &(header.remitente), pesoTipoRemitente);
        puntero += pesoTipoRemitente;
        memcpy(puntero, &(header.operacion), pesoTipoRequest);

        return headerSerializado;

}

int enviarHeaderMuse(int destinatario, tipoRemitente remitente, museOperacion operacion){
    void* headerSerializado = armarHeaderMuse(remitente, operacion);
    int pesoPaquete = sizeof(HeaderMuse);

    void* paquete = malloc(sizeof(HeaderMuse));
    void* puntero = paquete;
    memcpy(puntero, headerSerializado, sizeof(HeaderMuse));

    int res = send(destinatario, paquete, pesoPaquete, MSG_WAITALL);
    free(headerSerializado);
    free(paquete);

    return res;
}

HeaderMuse recibirHeaderMuse(int destinatario){
	int pesoPaquete = sizeof(HeaderMuse);
	void* paquete = malloc(pesoPaquete);

	int res = recv(destinatario, paquete, pesoPaquete, MSG_WAITALL);
	//if(res < 0) { return NULL;}

	HeaderMuse header;

	int pesoTipoRemitente = sizeof(typeof(header.remitente));
	int pesoTipoRequest = sizeof(typeof(header.operacion));

	memcpy(&(header.remitente), paquete, pesoTipoRemitente);
	paquete += pesoTipoRemitente;

	memcpy(&(header.operacion), paquete, pesoTipoRequest);
	paquete += pesoTipoRequest;
	free(paquete);
		return header;


}




int enviarInt(int destinatario, int loQueEnvio){

	 void* paquete = malloc(sizeof(int));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(int));
	 int res = send(destinatario, paquete, sizeof(int), MSG_WAITALL);
	 free(paquete);
	 return res;
}

int recibirInt(int destinatario){
	void* paquete = malloc(sizeof(int));
	int res = recv(destinatario, paquete, sizeof(int),MSG_WAITALL );
	if(res<0) {return -1;}
		int contenido = sizeof(int);

		memcpy(&(contenido), paquete, sizeof(int));
		free(paquete);
		return contenido;

}

int enviarUint32_t(int destinatario, uint32_t loQueEnvio){
	 void* paquete = malloc(sizeof(uint32_t));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(uint32_t));
	 int res = send(destinatario, paquete, sizeof(uint32_t), MSG_WAITALL);
	 free(paquete);
	 return res;
}

uint32_t recibirUint32_t(int destinatario){
	void* paquete = malloc(sizeof(uint32_t));
	recv(destinatario, paquete, sizeof(uint32_t),MSG_WAITALL );

		int contenido = sizeof(uint32_t);

		memcpy(&(contenido), paquete, sizeof(uint32_t));
		free(paquete);
		return contenido;

}
int enviarUintSizet(int destinatario, uint32_t loQueEnvio, size_t n){
	 int tamanio = sizeof(uint32_t) + sizeof(size_t);
	 void* paquete = malloc(sizeof(uint32_t) + sizeof(size_t));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(uint32_t));
	 puntero+= sizeof(uint32_t);
	 memcpy(puntero, &(n), sizeof(size_t));
	 int res = send(destinatario, paquete, tamanio, MSG_WAITALL);
	 free(paquete);
	 return res;
}

void recibirUintSizet(int destinatario, uint32_t contenido,size_t n ){
	int tamanio = sizeof(uint32_t)+sizeof(size_t);
	void* paquete = malloc(sizeof(uint32_t)+sizeof(size_t));
	recv(destinatario, paquete, tamanio,MSG_WAITALL );


		memcpy(&(contenido), paquete, sizeof(uint32_t));
		paquete += sizeof(uint32_t);
		memcpy(&(n), paquete, sizeof(size_t));
		free(paquete);

}

int enviarString(int destinatario, char* loQueEnvio){
	int tamanio;
	if(loQueEnvio == NULL){
		tamanio = 0;
	} else{
		tamanio = sizeof(char) * (strlen(loQueEnvio) + 1);
	}
	void* paquete = malloc(sizeof(int));
	void* puntero = paquete;
    memcpy(puntero, &(tamanio), sizeof(int));

    int res = send(destinatario, paquete, tamanio, MSG_WAITALL);
    free(paquete);
    if(res > 0){
    	void* estring = malloc(tamanio);
    	void* algo = estring;
    	memcpy(algo, &(loQueEnvio), tamanio);

    	res = send(destinatario, estring, tamanio, MSG_WAITALL);
    	free(estring);
    	return res;
    }


	return res;
}

char* recibirString(int destinatario){
	void* paquete = malloc(sizeof(int));
	int res = recv(destinatario, paquete, sizeof(int), MSG_WAITALL);
		if(res < 0) { return NULL;}

	int peso;
	memcpy(&(peso), paquete, sizeof(int));
	free(paquete);
	void* estring = malloc(peso);
	res = recv(destinatario,estring, peso, MSG_WAITALL );
	if(res < 0) {		return NULL;	}

	char* retorno = malloc(peso);
	memcpy(&(retorno), estring, peso);
	free (estring);
	return retorno;
}




