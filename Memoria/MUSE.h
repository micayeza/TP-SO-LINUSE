/*
 * MUSE.h
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */

#ifndef MUSE_H_
#define MUSE_H_

#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


#define rutaConfigMuse "../configs/muse.cfg"
#define rutaSwap "swap.txt"

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

t_inicial* tabla_inicial;


int  cantidad_paginas;
int  paginas_swap;
char* vaciar;

t_configuracion *config_muse;
t_config *config_ruta;
t_log *logMuse;

FILE * archivoSwap;

bool existeArchivoConfig(char*);
int crearConfigMemoria();


void inicializarMemoria();
void inicializarSwap();



// Para swap tengo al funcion rewind que devuelve el cursor al inicio del archivo
//Esta char *fgets(char *buffer, int tamaño, FILE *archivo); buffer donde lo guarda, tamaño es el maximio
// archivo DA
// int fputs(const char *buffer, FILE *archivo)
// char cadena[] = "Mostrando el uso de fputs en un fichero.\n"; TEngo que poner el \n



#endif /* MUSE_H_ */
