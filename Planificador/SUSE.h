
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
#include <arpa/inet.h>
#include <signal.h>
//Librerias COMMONS
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

//Librerias propias
//#include <libs.h>
//#include <mensajes.h>
//#include <conexion.h>

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

typedef enum {
	VACIO,
	INIT,
	//Para SUSE
    CREATE_HILO,
	SCHEDULE_NEXT,
	WAIT,
	SIGNAL,
	JOIN,
	CLOSE,
} TipoOperacion;
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
typedef enum {
	ACTIVO,
	FINALIZADO,
	BLOQUEADO
} estProg;

typedef struct{
	int 	 programa;
	t_list*	 hijos;
	int      id;
	estProg  estado;
	double   init;
	double   fin;
	t_list*  tablaReady;
	int      cant_hilos;
}t_programa;

typedef struct{
	int    programa;
	int	   id;
	double estimado;
}t_new;

typedef struct{
	int	   id;
}t_join;

typedef struct{
	int    programa;
	int	   id;
	double estimado;
	int    tid;
	int    sem; //guardo la posicion y a la mierda
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

typedef struct{
	sem_t cont;
	int   idProg;
}t_sem_contador;

typedef struct{
	pthread_mutex_t mtx;
	int   idProg;
}t_pth_programas;

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
pthread_mutex_t sem_lista_semaforos;

t_list* lista_semaforos; //1 semaforo contador por programa
t_list* lista_sem_programas;

int socket_escucha;
int cant_programas;

void atenderPrograma(void* cliente);
void join_hilo(int tid, int programa, t_list* joins);
void planificadorLargo();
t_hilo* buscar_prog_hilo(int programa, int hilo);
void create_hilo(int tid, t_programa* programa);
t_new* next_hilo(t_list* tabla_ready);
void recalcularEstimado(t_hilo* hilo);

t_new* close_hilo(int tid, t_programa* programa, t_list* tabla_ready, t_new* hilo_exec, t_list* joins);
int wait_hilo(int tid,char* semName);
t_block* signal_hilo(int tid, char* semName);
void inicializarSemaforos();
int posicionSemaforo(char* sem_id);
void bloquearEnSemaforo(t_block* block, char* sem_id);
t_block* getNextFromSemaforo(int sem_pos);
void printefearMetricas();


int recibirInt(int destinatario);
char* recibirTexto(int fdOrigen, t_log* logger);
int enviarInt(int destinatario, int loQueEnvio);
int aceptarCliente(int fd_servidor, t_log* logger);
int crearSocketEscucha (int puerto, t_log* logger);
void escucharSocketsEn(int fd_socket, t_log* logger);
int crearSocketServidor(int puerto, t_log* logger);
int crearSocket(t_log* logger);

void prog_destroy(t_programa *self);
void hilos_destroy(t_hilo *self);
void block_destroy(t_block *self);
void new_destroy(t_new *self);

t_sem_contador* buscar_contador(int id);
t_hilo* buscar_hilo_bool(int id, t_programa* programa);
t_programa* buscar_programa_bool(int id);
t_pth_programas*  buscar_mutex( int programa);

// ------------------------ FIN SECCIÓN MICA ------------------------

#endif /* SUSE_H_ */
