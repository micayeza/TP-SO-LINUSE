/*
 * SACServer.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#include "SACServer.h"


int SacServerRead(const char *path, char *buf, size_t size, off_t offset){
	return 0;
 }

int SacServerOpen(const char *path) {
	 return 0;

 }

 int SacServerWrite(const char *path, const char *buf, size_t size, off_t offset){
	 return 0;
 }
/*
 int SacServerMkdir(const char *path, mode_t mode){
	 return 0;
 }*/

 //BORRAR ARCHIVO
 int SacServerUnlink(const char *path) {
	 return 0;
 }

 int SacServerRmdir(const char *path) {
	 return 0;
 }

 //CAMBIAR EL VALOR DE RETORNO
 int SacServerReaddir(const char* path){
	 return 0;
 }



 int crearSocketSACServidor(int puerto)	{
 	struct sockaddr_in miDireccionServidor;
 	int socketDeEscucha = socket(AF_INET, SOCK_STREAM, 0);

 	miDireccionServidor.sin_family = AF_INET;			//Protocolo de conexion
 	miDireccionServidor.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0 y significa que usa la IP actual de la maquina
 	miDireccionServidor.sin_port = htons(puerto);		//Puerto en el que escucha
 	memset(&(miDireccionServidor.sin_zero), '\0', 8);		//Pone 0 a la estructura
 		//Veamos si el puerto está en uso
 	int puertoEnUso = 1;
 	int puertoYaAsociado = setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &puertoEnUso, sizeof(puertoEnUso));

 	if (puertoYaAsociado == ERROR) {
 		//log_error(logMuse, "El puerto asignado ya está siendo utilizado \n");
 	}
 		//Turno del bind
 	int activado = 1;
 	setsockopt(socketDeEscucha,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));//Para evitar que falle el bind, al querer usar un mismo puerto

 	int valorBind = bind(socketDeEscucha,(void*) &miDireccionServidor, sizeof(miDireccionServidor));

 	if ( valorBind !=0) {

 		//log_error(logMuse, "El bind no funcionó, el socket no se pudo asociar al puerto \n");
 		return 1;
 	}

 	return socketDeEscucha;
 }

 int crearSocketEscuchaFUSE(int puerto) {

 	int socketDeEscucha = crearSocketSACServidor(puerto);

 	//Escuchar conexiones
 		int valorListen;
 		valorListen = listen(socketDeEscucha, SOMAXCONN);//SOMAXCONN como segundo parámetro, y significaría el máximo tamaño de la cola
 		if(valorListen == ERROR) {

 			//log_error(logMuse, "El servidor no pudo recibir escuchar conexiones de clientes.\n");
 		} else	{

 			//log_info(logMuse, "¡Hola, estoy escuchando! \n");
 		}

 	// hasta que no salga del listen, nunca va a retornar el socket del servidor ya que el listen es bloqueante

 	return socketDeEscucha;
 }

 int aceptarSacCli(int fd_socket){

 	struct sockaddr_in unCliente;
 	memset(&unCliente, 0, sizeof(unCliente));
 	unsigned int addres_size = sizeof(unCliente);

 	int fd_Cliente = accept(fd_socket, (struct sockaddr*) &unCliente, &addres_size);
 	if(fd_Cliente == ERROR)  {
 		//log_error(logMuse, "El servidor no pudo aceptar la conexión entrante \n");
 	} else	{
 		//log_info(logMuse, "Se conectó un proceso \n");
 	}

 	return fd_Cliente;

 }





void atenderConexiones(int cliente){

	int tam = recibirTamanio(cliente);

	MensajeFUSE* msjl = malloc(tam);
	//msjl = recibirMensajeFUSE(cliente);
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
		//SacServerMkdir(msjl->path);

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

void crearHiloParalelos(){

	int socket_escucha = crearSocketEscuchaFUSE(8091);

	if(socket_escucha > 0){
		log_info(SacServerLog,"levanto");
	int cliente = 0;

		while(( cliente = aceptarSacCli(socket_escucha)) > 0 ){
			log_info(SacServerLog,"nueva conexion");
			pthread_t hilo;
			pthread_create(&hilo, NULL, (void*)atenderConexiones, (void*)cliente );
			pthread_join(hilo, NULL);

		}
	}
	else {
		//log_error(logMuse, "Error al crear el socket de escucha");
		exit(1);
	}
}


int main(){
	SacServerLog = log_create("SACServer.txt", "LOG", true, LOG_LEVEL_INFO);
	crearHiloParalelos();
};


