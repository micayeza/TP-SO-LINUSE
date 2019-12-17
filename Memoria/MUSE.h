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
#include <commons/string.h>
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
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>





//#define rutaConfigMuse "../../configs/muse.cfg"
#define rutaConfigMuse "/home/utnso/workspace/tp-2019-2c-capitulo-2/configs/muse.cfg"
//#define rutaSwap "swap.txt"
#define ERROR -1
#define CHAR (sizeof(char))
typedef struct {
	int puerto;
	int tamanio_total;
	int tamanio_pagina;
	int tamanio_swap;
	int paginas_totales; //sumo las paginas de mi memoria + swap, mas de eso no puedo tener. ;)
	char* rutaSwap;
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
	int       segmento;
	uint32_t  base;
	uint32_t  tamanio;
	bool      empty;   // 0 = vacio
	bool      ultimo;
	bool      dinamico;//Si el segmneto esdinamico 0, si es map 1
	bool      shared;  // Si map = 1, 1 es compartido, 0 privado, sino ignorar
	char*     path;
	t_list*   paginas;
} t_segmento;

typedef struct{
	int   id;
	int   cliente;
	char* ip;
	t_list* segmentos;
	t_list*  bloquesLibres;
	int memReservada;
	int memLiberada;

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
	int marco;
	int id;
	char* ip;
	int seg;
	int pag;
	int u;
	int m;
	int p; //--> p de puntero
} t_clock;

typedef struct {
	char* ip;
	int   id;
}t_ip_id;

typedef struct {
	char* nombre;
	int   seg;//Segmento
	t_list* proceso;
	t_list* puntero_a_pag;
}t_archivo;

typedef enum {
	NADA,
	SALUDO,
	CERRAR,
	RESERVAR,
	LIBERAR,
	OBTENER,
	COPIAR,
	MAPEAR,
	SINCRO,
	DESMAP,
	VER
} museOperacion;

typedef enum{
	PRIVATE,
	SHARED
}comparticion;

typedef struct {
	int segmento;
	int pagina;
	uint32_t tamanio;
	uint32_t posicion; //EN LA PAGINA
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

//FILE * archivoSwap;
void* punteroSwap;

bool stop;
int  paginas_usadas;
char* bitmap_marcos;
char* bitmap_swap;
//char* aux_swap;
pthread_mutex_t sem_mapeo;
pthread_mutex_t sem_bitmap_swap;
pthread_mutex_t sem_bitmap_marco;
pthread_mutex_t sem_paginas;
pthread_mutex_t sem_clock;
pthread_mutex_t global;
pthread_mutex_t sem_cant_pag;
pthread_mutex_t sem_procesos;


//typedef struct {
// pthread_mutex_t marco;
//} t_semaforo;

typedef struct{
	pthread_mutex_t marco;

}t_semaforo;

t_list* lista_marcos_memoria;
t_list* lista_marcos_swap;



int enviarInt(int destinatario, int loQueEnvio);
int recibirInt(int destinatario);

int enviarUint32_t(int destinatario, uint32_t loQueEnvio);
uint32_t recibirUint32_t(int destinatario);

int enviarSizet(int destinatario, size_t n);
size_t recibirSizet(int destinatario);

int enviarTexto(int fdDestinatario, char* textoEnviar);
char* recibirTexto(int fdOrigen);
int pesoString(char *string);

char* recibirVoid(int destinatario);
int enviarVoid(int destinatario, void* loQueEnvio, int tam);

bool existeArchivoConfig(char*);
int crearConfigMemoria();


void inicializarMemoria();
void inicializarTablas();
void* inicializarSwap();
void crearHiloParalelos();

int aceptarClienteMemoria(int);
int crearSocketMemoria();
int crearSocketEscuchaMemoria (int) ;
int crearSocketServidorMemoria(int);
void atenderConexiones(int);
void ip_de_programa(int,char*);


uint32_t mallocMuse(uint32_t tam, t_proceso* proceso);
void remover_bloque_libre(t_list* bloque_libre , int pag, int seg, uint32_t pos);
void nuevo_segmento(t_segmento* nuevo, uint32_t base,bool dinamico,bool empty,int numero,bool compartido,uint32_t tamanio,bool ultimo );
uint32_t crearSegmentoDinamico(uint32_t tamanio, t_list* tabla_segmentos, t_list* bloques_libres, t_proceso* proceso);
uint32_t crearPaginas(int tam, uint32_t tamanio, t_segmento* segmento, t_list* bloques_libres, t_proceso* proceso);
int calcular_paginas_malloc(uint32_t tamanio);
int buscar_marco_libre(char* bitmap);

int swap(t_pagina* pag_swap, bool nueva, t_proceso* proceso, t_segmento* segmento);
void agregar_pag_clock(int id, char* ip, int m ,int u, t_segmento* seg, int pag, int marco);
t_clock* buscar_clock(int marco);
void remover_clocky(int marco, t_proceso* proceso);
void modificar_clock_bitmap(t_pagina* pagina, int segmento, t_proceso* proceso);

void actualizar_header(int seg, int pag,uint32_t posicion, uint32_t tamAnterior, uint32_t tamanio, t_list* tabla_segmentos, t_list* bloquesLibres, int fin, t_proceso* proceso);
void actualizar_bloque_libre(int pag, t_segmento* seg, uint32_t desplazamiento, uint32_t tamanio, t_list* bloquesLibres);
t_pagina* buscar_segmento_pagina(t_list* segmentos , int seg, int pag, t_proceso* proceso);
void desperdicio(uint32_t sobrante, void* posicion, t_pagina* pag, uint32_t posHeader);
bool hayHashtag(int marco, int i);
void agregar_bloque_libre(t_list* bloquesLibres, int pagina,int segmento,uint32_t posicionEnPagina, uint32_t tamanioLibre);
uint32_t sobrante_pagina(uint32_t base_segmento, int numero_pagina, uint32_t desplazamiento);

void* convertir(uint32_t posicion,int marco);

int freeMuse(uint32_t posicionAliberar,t_list* tabla_segmentos,t_list* bloquesLibres,t_proceso* proceso, bool sg);
void compactar(t_list*  tabla_segmentos, t_list* bloquesLibres, t_proceso* proceso);
void vaciarSegmento(t_segmento* segmento, t_list* bloquesLibres, t_list* tabla_segmentos, t_proceso* proceso );

int copiarMuse(uint32_t posicionACopiar,int  bytes,char* copia,t_list* tabla_segmentos, t_proceso* proceso, bool sg);
//int copiarMuse(uint32_t posicionACopiar,int  bytes, void* copia,t_list* tabla_segmentos, t_proceso* proceso, bool sg);

char* getMuse(uint32_t posicion, size_t bytes,t_list* tabla_segmentos, t_proceso* proceso, bool sg);

uint32_t  mappearMuse(char* path, size_t len,int flag,t_proceso* proceso);
int tamanioArchivo(char* rutaArchivo);
int crearPaginasmapeadas(int tam,size_t len,t_segmento* segmentoVacio,t_proceso* proceso, int flag, char* path);
uint32_t  crearSegmentoMapeado(int len,t_proceso* proceso, int flag, char* path);

int syncMuse(uint32_t fd,size_t len,t_proceso* proceso, bool sg);
bool mapeoElArchivo(t_segmento* seg, t_proceso* proceso);

void liberarTodo(t_proceso* proceso);
int unmapMuse(uint32_t  fd,t_proceso* proceso, bool sg);

size_t highestOneBitPosition(uint32_t a);
bool addition_is_safe(uint32_t a, uint32_t b) ;

static void libres_destroy(t_libres *self);
static void seg_destroy(t_segmento *self);
static void proc_destroy(t_proceso *self);
static void clock_destroy(t_clock *self);
static void sem_destroy(t_semaforo *self);
static void ip_destroy(t_ip_id *self);
static void pag_destroy(t_pagina *self);
static void archivos_destroy(t_archivo *self);
static void sem_destroy(t_semaforo *self);

char** descomponer_auxiliar(char* auxiliar, int len);
void printefearMetricas();

#endif /* MUSE_H_ */
