#include "libs.h"

//void freeCharArray(char** charArray){
//
//	int i = 0;
//	while(charArray[i] != NULL){
//		free(charArray[i]);
//		i++;
//	}
//	free(charArray);
//}
//
//
//
////Mensaje para memoria
//
//
//
//mensaje* armarMensaje(operacion operacion, int size, void* contenido){
//	mensaje* mensaje   = malloc(sizeof(mensaje));
//	mensaje->header.operacion = operacion;
//	mensaje->header.size      = size;
//
//	mensaje->contenido = malloc(size);
//	memcpy(mensaje->contenido , contenido , mensaje->header.size);
//	return mensaje;
//}
//
//int enviarMensaje(int socket, mensaje* msj){
//
//	int resultado = send(socket, &(msj->header), sizeof(header), 0);
//		if(resultado < 0){ return -1; };
//
//	resultado = send(socket, msj->contenido, msj->header.size, 0);
//		if(resultado < 0){return -1; };
//
//	return 0;
//
//}
//
//
//mensaje* recibirMensaje(int socket){
//	mensaje* mensaje = malloc(sizeof(mensaje));
//	if (recv(socket, &(mensaje->header), sizeof(header),MSG_WAITALL) < 1){ return NULL;  };
//
//	mensaje->contenido = malloc(mensaje->header.size);
//	if( recv(socket, mensaje->contenido, mensaje->header.size, MSG_WAITALL) < 1){ return NULL;}
//
// return mensaje;
//}
//
//void freeMensaje(mensaje* mensaje){
//
//	free(mensaje->contenido);
//	free(mensaje);
//}



