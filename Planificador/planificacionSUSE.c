#include "SUSE.h"

void atenderPrograma(t_PCB* pcb){
	//Me mantengo a la espera de Recibir Mensaje de este PCB->socket
}

void encolarEnNew(t_TCB* TCB){

	pthread_mutex_lock(&mutexColaEjecucion);
	queue_push(colaNew, TCB);
	pthread_mutex_unlock(&mutexColaEjecucion);
}

void transicionNewAReady(int idHilo){

	t_configSUSE* config = getConfigSUSE(configPath);

	//while(/*numeroActualDeProcesosEnMemoria < config->maxMultiprog*/){

	//}

}

t_PCB* create_PCB(int socket){
	t_PCB* PCB = malloc(sizeof(t_PCB));
	PCB->socket = socket;
	PCB->TCBs = dictionary_create();
	PCB->proximo_tcb_id = 0;
	return PCB;
}

void free_PCB(t_PCB* PCB){
	dictionary_destroy_and_destroy_elements(PCB->TCBs, (void*)free_TCB);
	free(PCB);
}

t_TCB* create_TCB_en_PCB(t_PCB* PCB, void* funcion){
	t_TCB* TCB = malloc(sizeof(t_TCB));
	TCB->id = PCB->proximo_tcb_id;
	TCB->funcion = funcion;
	dictionary_put(PCB->TCBs,string_itoa(TCB->id),TCB);
	(PCB->proximo_tcb_id)++;
	return TCB;
}
void free_TCB(t_TCB* TCB){
	free(TCB->funcion);
	free(TCB);
}

//Se me ocurrio hacer una cola "ejecución" que tenga a las colas "bloqueado", "ready" y "ejecutando" adentro. Contando todos los hilos que están corriendo adentro
//debería devolver la cantidad de procesos en memoria.

//Hay que armar un struct que sea TCB (Thread Control Block) con la info necesaria. Ver qué es lo que necesitamos para administrar la planificación.
