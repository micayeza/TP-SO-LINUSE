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
#include <commons/bitarray.h>
#include <commons/collections/list.h>
//Librerias propias
#include <conexion.h>

#define ERROR -1
#define TAM_MAX_NOMBRE_ARCHIVO 71
#define CANT_MAX_ARCHIVOS 1024
#define TAM_BLOQUE 4096
#define TAM_TABLA_NODOS 1024
#define TAM_P_BLOQUE 4
#define TAM_RELLENO_HEADER 4081

//const unsigned long TamBloque = 4096;
//const unsigned long CantidadBloques = 0;
//const unsigned long CantidadMaxArchivos = 1024;
//const unsigned long CaracteresMaximosNombre = 71;

char* configPath;
t_log* log_resultados;
t_log* log_interno;

typedef struct {
	int listenPort; //Puerto TCP utilizado para recibir las conexiones de CPU y I/O.
	char* pathFs;
} t_configSAC;

typedef struct {
	int socket;
	//agregar más cosas si es necesario
} t_cliente;

typedef struct {
	char estado;
	char nombre_archivo[TAM_MAX_NOMBRE_ARCHIVO];
	char bloque_padre[TAM_P_BLOQUE];
	char tam_archivo[4];
	char fecha_creacion[8];
	char fecha_modificacion[8];
	t_list* p_indirectos;
} t_nodo;

typedef struct {
	char* identificador;
	int version;
	void* inicio_bitmap; //Almaceno una direccion
	int tam_bitmap;
} t_header;

typedef struct {
	t_header header;
	t_bitarray bitmap;
	t_list* tabla_nodos;
} t_fs;

t_log *SacServerLog;
t_header* fs_header;

//FUNCIONES CONFIG
t_configSAC* getConfigSAC(char* configPath);
void freeConfig(t_configSAC* config);

//FUNCIONES SAC SERVER
t_cliente* create_cliente(int socket);

//FUNCIONES ATENCION CLIENTE
void atenderCliente(t_cliente* cliente);

//FUNCIONES FILESYSTEM



#endif /* SACSERVER_H_ */
