
#ifndef SUSE_H_
#define SUSE_H_

//Librerias generales
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

//Librerias COMMONS
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

//Librerias propias
#include "../libs/libs.h"


// -------------------------- SECCIÓN PLANIFICACIÓN --------------------------
pthread_t mainThread;
pthread_mutex_t mutexColaEjecucion;

typedef struct {
	int socket;
	t_dictionary* TCBs;
	int ultimo_tcb_id;
} t_PCB;

typedef struct {
	int id;
	char* funcion;
} t_TCB;

t_queue* colaNew;

void encolarEnNew(t_TCB* TCB);
t_PCB* create_PCB(int socket);
void free_PCB(t_PCB* PCB);
t_TCB* create_TCB(int id, char* funcion);
void free_TCB(t_TCB* TCB);
// ------------------------ FIN SECCIÓN PLANIFICACIÓN ------------------------


// -------------------------- SECCIÓN CONFIG --------------------------

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

// ------------------------ FIN SECCIÓN CONFIG ------------------------

#endif /* SUSE_H_ */
