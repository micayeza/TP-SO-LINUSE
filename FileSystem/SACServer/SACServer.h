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
#include <math.h>
#include <sys/time.h>
//Librerias propias
#include <conexion.h>

#define ERROR -1
#define TAM_MAX_NOMBRE_ARCHIVO 71
#define CANT_MAX_ARCHIVOS 1024
#define TAM_BLOQUE 4096
#define TAM_TABLA_NODOS 1024
#define TAM_P_BLOQUE 4
#define TAM_MAX_PUNT_IND 1000

typedef enum e_estado {BORRADO, OCUPADO, DIRECTORIO} e_estado;

char* configPath;
t_log* log_resultados;
t_log* log_interno;
FILE* archivo_fs;


typedef struct {
	int listenPort; //Puerto TCP utilizado para recibir las conexiones de CPU y I/O.
	char* pathFs;
} t_configSAC;

typedef struct {
	int socket;
	//agregar más cosas si es necesario
} t_cliente;

typedef struct {
	uint8_t estado;
	unsigned char nombre_archivo[TAM_MAX_NOMBRE_ARCHIVO];
	uint32_t bloque_padre;
	uint32_t tam_archivo;
	struct timeval fecha_creacion;
	struct timeval fecha_modificacion;
	int p_indirectos[TAM_MAX_PUNT_IND];
} t_nodo;

typedef struct {
	long T;
	char* identificador;
	int version;
	int inicio_bitmap; //Almaceno el numero de bloque
	int tam_bitmap;
	int inicio_tabla_nodos; //Numero de bloque
	int inicio_bloques_datos;
	int tam_bloques_datos;
} t_header;

typedef struct {
	t_header header;
	t_bitarray bitmap;
	t_list* tabla_nodos;
} t_fs;

t_log *SacServerLog;
t_header* fs_header;
t_configSAC* config;

//FUNCIONES CONFIG
t_configSAC* getConfigSAC(char* configPath);
void freeConfig(t_configSAC* config);

//FUNCIONES SAC SERVER
t_cliente* create_cliente(int socket);

//FUNCIONES ATENCION CLIENTE
void atenderCliente(t_cliente* cliente);

//FUNCIONES FILESYSTEM
void abrirHeaderFS();
t_nodo* crearNodoVacio();


#endif /* SACSERVER_H_ */
