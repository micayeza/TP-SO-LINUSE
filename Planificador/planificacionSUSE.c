#include "SUSE.h"

void encolarEnNew(int idHilo){

	int* valor = malloc(sizeof(int));
	*valor = idHilo;

	pthread_mutex_lock(&mutexColaEjecucion);
	queue_push(colaNew, valor);
	pthread_mutex_unlock(&mutexColaEjecucion);
}

void transicionNewAReady(int idHilo){

	configSUSE* config = getConfigSUSE(configPath);

	//while(/*numeroActualDeProcesosEnMemoria < config->maxMultiprog*/){

	//}

}

//Se me ocurrio hacer una cola "ejecución" que tenga a las colas "bloqueado", "ready" y "ejecutando" adentro. Contando todos los hilos que están corriendo adentro
//debería devolver la cantidad de procesos en memoria.

//Hay que armar un struct que sea TCB (Thread Control Block) con la info necesaria. Ver qué es lo que necesitamos para administrar la planificación.
