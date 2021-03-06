/*
 * mensajes.c
 *
 *  Created on: 21 oct. 2019
 *      Author: utnso
 */

#include "mensajes.h"

int enviarInt(int destinatario, int loQueEnvio){

	 void* paquete = malloc(sizeof(int));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(int));
	 int res = send(destinatario, paquete, sizeof(int), MSG_WAITALL);
	 free(paquete);
	 return res;
}

int recibirInt(int destinatario){
	int algo;
	if(recv(destinatario, &algo, sizeof(int), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

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
	uint32_t algo;
	if(recv(destinatario, &algo, sizeof(uint32_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}
int enviarSizet(int destinatario,size_t n){
	 int tamanio = sizeof(size_t);
	 void* paquete = malloc(sizeof(size_t));
	 void* puntero = paquete;

	 memcpy(puntero, &(n), sizeof(size_t));
	 int res = send(destinatario, paquete, tamanio, MSG_WAITALL);
	 free(paquete);
	 return res;
}

size_t recibirSizet(int destinatario){

	size_t algo;
	if(recv(destinatario, &algo, sizeof(size_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}
}

int enviarString(int destinatario, char* loQueEnvio){
	int tamanio;
	if(loQueEnvio == NULL){
		tamanio = 0;
	} else{
		tamanio = strlen(loQueEnvio) + 1;
	}

	int res = enviarInt(destinatario, tamanio);
    if(res > 0){
    	void* estring = malloc(tamanio);

    	memcpy(estring, &(loQueEnvio), tamanio);

    	res = send(destinatario, estring, tamanio, MSG_WAITALL);
    	free(estring);
    	return res;
    }


	return res;
}

char* recibirString(int destinatario){

		char* buffer;
		int size;// = recibirInt(destinatario);
		recv(destinatario, &size, sizeof(int), MSG_WAITALL);
		buffer = malloc(size);
		recv(destinatario, buffer, size, MSG_WAITALL);

		return buffer;


}

int enviarVoid(int destinatario, void* textoEnviar, int tam){


	int res = enviarInt(destinatario, tam);
	if(res != 0){
	  res = send(destinatario, textoEnviar, tam, MSG_WAITALL);
	if(res == -1){

		return -1;
	}
	return res;
	}
	return 0;
}



char* recibirVoid(int fdOrigen){
	int tamanio;

	tamanio = recibirInt(fdOrigen);
	if(tamanio == 0){
		return NULL;
	}
	void* textoRecibido = malloc(tamanio);
	int resTexto = recv(fdOrigen, textoRecibido, tamanio, MSG_WAITALL);
	if(resTexto == -1){
		return NULL;
	}

	return textoRecibido;
}




