/*
 * MUSE.h
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */

#ifndef MUSE_H_
#define MUSE_H_

#include <commons/collections/list.h>
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
#include <libs.h>
#include <sockets.h>
#include <mensajes.h>


//#define rutaConfigMuse "../../configs/muse.cfg"
#define rutaConfigMuse "/home/utnso/workspace/tp-2019-2c-capitulo-2/configs/muse.cfg"
#define rutaSwap "swap.txt"
#define ERROR -1

typedef struct {
	int puerto;
	int tamanio_total;
	int tamanio_pagina;
	int tamanio_swap;
} t_configuracion;


typedef struct{
	uint32_t size;
	bool isFree;
} HeapMetadata;


typedef struct {
	void* marco;
} t_inicial;
void* punteroMemoria;
void* posicionMemoria;

typedef struct{
	int   id;
	int   cliente;
	char* ip;
	t_list* segmentos;
} t_proceso;

typedef struct{
	uint32_t base;
	int      tamnio;
	t_list*  paginas;
} t_segmento;

typedef struct {
	int numero;
	int u; //uso
	int p; //Presencia
	int m; //Modificado
}t_pagina;

typedef struct {
	char* nombre;
	int   flag;
	void* puntero_a_pag;
}t_archivos;

t_inicial* tabla_inicial;
t_list*    tabla_archivos;
t_list*    tabla_procesos;

int  socket_escucha;
int  cantidad_paginas;
int  paginas_swap;
char* vaciar;

t_configuracion *config_muse;
t_config *config_ruta;
t_log *logMuse;

FILE * archivoSwap;
bool activo;

bool existeArchivoConfig(char*);
int crearConfigMemoria();


void inicializarMemoria();
void inicializarTablas();
void inicializarSwap();
void crearHiloParalelos();

int aceptarClienteMemoria(int);
int crearSocketMemoria();
int crearSocketEscuchaMemoria (int) ;
int crearSocketServidorMemoria(int);
void atenderConexiones(int);
void ip_de_programa(int,char*);


void* recorrerSegmentos(t_list*);

// Para swap tengo al funcion rewind que devuelve el cursor al inicio del archivo
//Esta char *fgets(char *buffer, int tamaño, FILE *archivo); buffer donde lo guarda, tamaño es el maximio
// archivo DA
// int fputs(const char *buffer, FILE *archivo)
// char cadena[] = "Mostrando el uso de fputs en un fichero.\n"; TEngo que poner el \n



#endif /* MUSE_H_ */
