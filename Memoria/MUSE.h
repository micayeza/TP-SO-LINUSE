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
#include <commons/bitarray.h>
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
#include <mensajes.h>
#include <math.h>




//#define rutaConfigMuse "../../configs/muse.cfg"
#define rutaConfigMuse "/home/utnso/workspace/tp-2019-2c-capitulo-2/configs/muse.cfg"
#define rutaSwap "swap.txt"
#define ERROR -1

typedef struct {
	int puerto;
	int tamanio_total;
	int tamanio_pagina;
	int tamanio_swap;
	int paginas_totales; //sumo las paginas de mi memoria + swap, mas de eso no puedo tener. ;)
} t_configuracion;


typedef struct{
	uint32_t size;
	bool isFree;
} t_header;


typedef struct {
	void* marco;
} t_inicial;
void* punteroMemoria;
//void* posicionMemoria;

typedef struct{
	int      segmento;
	uint32_t base;
	uint32_t limite;
	int      empty;   // 0 = vacio
	int      dinamico;//Si el segmneto esdinamico 0, si es map 1
	int      shared;  // Si map = 1, 1 es compartido, 0 privado, sino ignorar
	t_list*  paginas;
} t_segmento;

typedef struct{
	int   id;
	int   cliente;
	char* ip;
	t_list* segmentos;
} t_proceso;


typedef struct {
	int numero;
	int p; //Presencia
	int marco; //Presencia = 0, ESTÁ EN TXT; Presencia = 1, ESTÁ EN MEMORIA
	uint32_t tamanio_header;
	uint32_t ultimo_header;
	int esFinPag;
}t_pagina;

typedef struct{
	int id;
	char* ip;
	int segmento;
	int u;
	int m;
} t_clock;


typedef struct {
	char* nombre;
	int   flag;
	void* puntero_a_pag;
}t_archivos;

typedef struct {
	int id;
	int segmento;
	int pagina;
	uint32_t tamanio;
	uint32_t posicion;
} t_libres;


t_inicial* tabla_inicial;
t_list*    tabla_archivos;
t_list*    tabla_procesos;
t_list*    tabla_clock;

int  socket_escucha;
int  cantidad_paginas;
int  paginas_swap;
int  contador;
char* vaciar;

t_configuracion *config_muse;
t_config *config_ruta;
t_log *logMuse;

FILE * archivoSwap;
bool activo;
int  paginas_usadas;
char* bitmap_marcos;
char* bitmap_swap;



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


uint32_t mallocMuse(uint32_t tam, t_list* bloquesLibres, t_list* segmentos);
void remover_bloque_libre(t_list* bloque_libre , int id);
int crearSegmento(int tamanio, t_list*  segmentos, t_list* bloques_libres);
uint32_t crearPaginas(int tam, uint32_t tamanio, t_segmento* segmento, t_list* bloques_libres);
int calcular_paginas_malloc(uint32_t tamanio);
int buscar_marco_libre(char* bitmap);

int swap(int pag_swap);

void actualizar_header(int seg, int pag,uint32_t posicion, uint32_t tamAnterior, uint32_t tamanio, t_list* tabla_segmentos, t_list* bloquesLibres);
t_pagina* buscar_segmento_pagina(t_list* segmentos , int seg, int pag);
void desperdicio(uint32_t sobrante, void* posicion, t_pagina* pag);

void* convertir(uint32_t posicion, int segmento, int size_tabla,int marco);
// Para swap tengo al funcion rewind que devuelve el cursor al inicio del archivo
//Esta char *fgets(char *buffer, int tamaño, FILE *archivo); buffer donde lo guarda, tamaño es el maximio
// archivo DA
// int fputs(const char *buffer, FILE *archivo)
// char cadena[] = "Mostrando el uso de fputs en un fichero.\n"; TEngo que poner el \n



#endif /* MUSE_H_ */
