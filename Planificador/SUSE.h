
#ifndef SUSE_H_
#define SUSE_H_

//Librerias generales
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/socket.h>

//Librerias COMMONS
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

//Librerias propias
#include <libs.h>
//#include <sockets.h>
#include <conexion.h>

//------------------------- SECCION CONEXION -------------------------------

//------------------------- SECCION CONEXION -------------------------------
void atenderPedidos();

// -------------------------- FIN SECCIÓN PLANIFICACIÓN --------------------
pthread_t mainThread;
pthread_mutex_t mutexColaEjecucion;
int proximo_pcb_id;

typedef struct {
	int socket;
	t_dictionary* TCBs;
	int proximo_tcb_id;
} t_PCB;

typedef struct {
	int id;
	void* funcion;
} t_TCB;

t_dictionary* PCBs;
t_queue* colaNew;

t_TCB* create_TCB_en_PCB(t_PCB* PCB, void* funcion);
void atenderPrograma(t_PCB* pcb);
void encolarEnNew(t_TCB* TCB);
t_PCB* create_PCB(int socket);
void free_PCB(t_PCB* PCB);
t_TCB* create_TCB(int id, void* funcion);
void free_TCB(t_TCB* TCB);
// ------------------------ FIN SECCIÓN PLANIFICACIÓN ------------------------


// -------------------------- SECCIÓN CONFIG y LOG --------------------------

char* configPath;

typedef struct {
	int listenPort; //Puerto TCP utilizado para recibir las conexiones de CPU y I/O.
	int metricsTimer; //Intervalo de tiempo en segundos entre cada loggeo de métricas.
	int maxMultiprog; //Grado máximo de Multiprogramación del Sistema.
	char** semId; //Identificador alfanumérico de cada semáforo del sistema. Cada posición del array representa un semáforo.
	char** semInit; //Valor inicial de cada semáforo definido en SEM_IDS, según su posición.
	char** semMax; //Valor máximo de cada semáforo definido en SEM_IDS, según su posición.
	double alphaSJF; //Parámetro de peso relativo del algoritmo SJF
} t_configSUSE;

t_configSUSE* getConfigSUSE(char* configPath);
void freeConfig(t_configSUSE* config);

t_log* log_resultados;
t_log* log_interno;

// ------------------------ FIN SECCIÓN CONFIG y LOG ------------------------

#endif /* SUSE_H_ */
