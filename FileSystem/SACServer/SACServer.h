/*
 * SACServer.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#ifndef SACSERVER_H_
#define SACSERVER_H_
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <commons/config.h>
#include <commons/log.h>
//#include "../CommonsFUSE.c"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
//Librerias propias
#include <conexion.h>

#define ERROR -1
t_log *SacServerLog;


//const unsigned long TamBloque = 4096;
//const unsigned long CantidadBloques = 0;
//const unsigned long CantidadMaxArchivos = 1024;
//const unsigned long CaracteresMaximosNombre = 71;

char* configPath;
t_log* log_resultados;
t_log* log_interno;

typedef struct {
	int listenPort; //Puerto TCP utilizado para recibir las conexiones de CPU y I/O.
} t_configSAC;

typedef struct {
	int socket;
	//agregar más cosas si es necesario
} t_cliente;

//FUNCIONES CONFIG
t_configSAC* getConfigSAC(char* configPath);
void freeConfig(t_configSAC* config);

//FUNCIONES SAC SERVER
t_cliente* create_cliente(int socket);

//FUNCIONES ATENCION CLIENTE
void atenderCliente(t_cliente* cliente);


#endif /* SACSERVER_H_ */
