#include "SUSE.h"

void recibirHilo(int idHilo){

	int* valor = malloc(sizeof(int));
	*valor = idHilo;

	//wait semaforo cola new
	queue_push(colaNew, valor);
	//signal semaforo cola new

}

void transicionNewAReady(int idHilo){

	configSUSE* config = getConfigSUSE(configPath);

	while(/*numeroActualDeProcesosEnMemoria < config->maxMultiprog*/){

	}

}

//Se me ocurrio hacer una cola "ejecución" que tenga a las colas "bloqueado", "ready" y "ejecutando" adentro. Contando todos los hilos que están corriendo adentro
//debería devolver la cantidad de procesos en memoria.
