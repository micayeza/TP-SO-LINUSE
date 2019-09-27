
#ifndef SUSE_H_
#define SUSE_H_

//Librerias generales
#include <stdio.h>
#include <stdlib.h>

//Librerias COMMONS
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>

//Librerias propias
#include "../libs/libs.h"


// -------------------------- SECCIÓN PLANIFICACIÓN --------------------------
t_queue* colaNew;

void encolarEnNew(int idSocket);
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
} configSUSE;

configSUSE* getConfigSUSE(char* configPath);
void freeConfig(configSUSE* config);

// ------------------------ FIN SECCIÓN CONFIG ------------------------

#endif /* SUSE_H_ */
