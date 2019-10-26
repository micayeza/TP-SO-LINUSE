/*
 * SACServer.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#include "SACServer.h"
#include "sockets.h"

 int SacServerRead(const char *path, char *buf, size_t size, off_t offset) {

 }

 int SacServerOpen(const char *path) {


 }

 int SacServerWrite(const char *path, const char *buf, size_t size, off_t offset){

 }

 int SacServerMkdir(const char *path, mode_t mode){

 }

 //BORRAR ARCHIVO
 int SacServerUnlink(const char *path) {

 }

 int SacServerRmdir(const char *path) {

 }

 //CAMBIAR EL VALOR DE RETORNO
 int SacServerReaddir(const char* path){

 }

MensajeFUSE* recibirMensajeFUSE(int socket){
	MensajeFUSE* mensaje = malloc(sizeof(MensajeFUSE));
	if (recv(socket, &(mensaje->header), sizeof(header),MSG_WAITALL) < 1){ return NULL;  };

	//mensaje->contenido = malloc(mensaje->header.size);
	mensaje->size = malloc(mensaje->size);
	//if( recv(socket, mensaje->contenido, mensaje->header.size, MSG_WAITALL) < 1){ return NULL;}

 return mensaje;
}

void atenderConexiones(){
	//CONFIGURAR PUERTO
int socket_escucha = crearSocketEscucha(8888);
int cliente = aceptarCliente(socket_escucha);

while((cliente=aceptarCliente(socket_escucha))>0){

	enviar_mensaje(cliente,HANDSHAKE,sizeof(int),0);


	//mensaje* msjl = malloc(sizeof(mensaje));
	//msjl = recibir_mensaje(cliente);

	MensajeFUSE* msjl = malloc(sizeof(MensajeFUSE));
	msjl = recibirMensajeFUSE(cliente);
	//char* contenido = contenido_toString(msj);

	switch(msjl->syscall){

	case 1:{

		SacServerWrite(msjl->path,msjl->buff,msjl->size,msjl->offset);
	}
	break;
	case 2:{
		SacServerOpen(msjl->path);
	}
	break;
	case 3:{

		SacServerRead(msjl->path,msjl->buff,msjl->size,msjl->offset);
		}
	break;
	case 4:{
		SacServerRmdir(msjl->path);
		}
	break;
	case 5:{
		SacServerUnlink(msjl->path);
		}
	break;
	case 6:{
		//FALTA AGREGARLE EL MODO
		SacServerMkdir(msjl->path);
		}
	break;
	case 7:{

		SacServerReaddir(msjl->path);
		}
	break;
	default:
		break;
	}
}
}


int main(int argc, char*argv[]){
	pthread_t hiloConexiones;
	pthread_create(&hiloConexiones,NULL,(void*) &atenderConexiones,NULL);
	pthread_join(hiloConexiones,NULL);
}


