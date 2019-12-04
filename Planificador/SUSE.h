
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
#include <sys/sem.h>
#include <semaphore.h>

//Librerias COMMONS
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

//Librerias propias
#include <libs.h>
#include <mensajes.h>
#include <conexion.h>

//------------------------- SECCION CONEXION -------------------------------
void atenderPedidos();

// -------------------------- SECCIÓN PLANIFICACIÓN --------------------
pthread_t mainThread;
pthread_mutex_t mutexColaNew;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCKED,
	EXIT
} estado;


//typedef struct {
//	int socket;
//	t_dictionary* TCBs;
//	estadoDeEjecucion estado;
//} t_PCB;
//
//typedef struct {
//	int id;
//	estadoDeEjecucion estado;
//	t_PCB* procesoPadre;
//} t_TCB;
//
//t_dictionary* PCBs;
//t_queue* colaNew;

//t_TCB* create_TCB_en_PCB(t_PCB* PCB, void* funcion);

//void encolarEnNew(t_TCB* TCB);
//t_PCB* create_PCB(int socket);
//void free_PCB(t_PCB* PCB);
//t_TCB* create_TCB(int id, void* funcion);
//void free_TCB(t_TCB* TCB);




// ------------------------ FIN SECCIÓN PLANIFICACIÓN ------------------------


// -------------------------- SECCIÓN CONFIG y LOG --------------------------

char* configPath;

typedef struct {
	int puerto; //Puerto TCP utilizado para recibir las conexiones de CPU y I/O.
	int metricas; //Intervalo de tiempo en segundos entre cada loggeo de métricas.
	int multiprog; //Grado máximo de Multiprogramación del Sistema.
	char** semId; //Identificador alfanumérico de cada semáforo del sistema. Cada posición del array representa un semáforo.
	int* semInit; //Valor inicial de cada semáforo definido en SEM_IDS, según su posición.
	int* semMax; //Valor máximo de cada semáforo definido en SEM_IDS, según su posición.
	double alpha; //Parámetro de peso relativo del algoritmo SJF
	int    cantSem;
} t_configSUSE;

//t_configSUSE* getConfigSUSE(char* configPath);
void freeConfig(t_configSUSE* config);

t_log* log_resultados;
t_log* log_interno;
t_configSUSE *config_suse;
t_config *config_ruta;

// ------------------------ FIN SECCIÓN CONFIG y LOG ------------------------

// -------------------------- SECCIÓN MICA --------------------------
typedef struct{
	int 	     programa;
	t_list*		 hijos;
}t_programa;

typedef struct{
	int    programa;
	int	   id;
	double estimado;
}t_new;

typedef struct{
	int    programa;
	int	   id;
	double estimado;
	int    tid;
	char*  sem;
}t_block;

t_list* tabla_new;
t_list* tabla_lock;
t_list* tabla_exit;
t_list* tabla_programas;

typedef struct{
	int 	id;
	estado  estado;
	double  real;
	double  estimadoActual;
	double	estimadoAnterior;
	double	deNaR;
	clock_t initNew;
	double	dNaM;
	double	enReady;
	clock_t	initReady;
	double	enExec;
	clock_t	initExec;
	double	enLock;
	clock_t	initLock;
}t_hilo;

typedef struct{
	char* name;
	int   init;
	int   max;
//	int   asigandos;
}t_semaforos;

//array de valores de semaforos para las primitivas signal y wait de Ansisop
int* sem_values;
//array de colas de procesos bloqueados en semaforos
t_queue** sem_blocked;


//int mult;

pthread_mutex_t sem_new;
pthread_mutex_t sem_lock;
pthread_mutex_t sem_exit;
pthread_mutex_t multi;
pthread_mutex_t wt;
pthread_mutex_t sl;

int socket_escucha;
int cant_programas;

void atenderPrograma(void* cliente);
void join_hilo(int tid, t_new* hilo_exec);
void planificadorLargo(t_list* tabla_ready, pthread_mutex_t sem_ready);
t_hilo* buscar_prog_hilo(int programa, int hilo);
void create_hilo(int tid, t_programa* programa, pthread_mutex_t tabla_hijos);
int next_hilo(t_list* tabla_ready, pthread_mutex_t sem_ready,t_new* hilo_exec,  pthread_mutex_t sem_exec);
void recalcularEstimado(t_hilo* hilo);
//FALTAN
void close_hilo(int tid, int programa, t_list* tabla_ready, t_new* hilo_exec);
int wait_hilo(int tid,char* semName);
t_block* signal_hilo(int tid, char* semName, t_list* tabla_ready);
void inicializarSemaforos();
int posicionSemaforo(char* sem_id);
void bloquearEnSemaforo(t_block* block, char* sem_id);
t_block* getNextFromSemaforo(int sem_pos);
void printefearMetricas();
// ------------------------ FIN SECCIÓN MICA ------------------------

#endif /* SUSE_H_ */
