#include "SUSE.h"

void atenderPrograma(t_PCB* pcb){
	//Me mantengo a la espera de Recibir Mensaje de este PCB->socket
	while(1){
		TipoOperacion operacionRecibida = (TipoOperacion) recibirEntero(pcb->socket, log_interno); //Chequear que no se bloquea, debuggear.
		switch(operacionRecibida){
			case CREATE_HILO:{
				int tid = recibirEntero(pcb->socket, log_interno);
				create_hilo(tid);//Hacer los pasos para un Create Hilo.
				break;
			}
			case SCHEDULE_NEXT:{
				int nextTid = next_hilo();//Hacer los pasos para un Schedule Next.
				int resNext = enviarEntero(pcb->socket, nextTid,  log_interno);
				break;
			}
			case JOIN:{
				int tid = recibirEntero(pcb->socket, log_interno);
				join_hilo(tid);//Hacer los pasos para un Join.
				break;
			}
			case CLOSE:{
				int tid = recibirEntero(pcb->socket, log_interno);
				close_hilo(tid);//Hacer los pasos para un Close.
				break;
			}
			case WAIT:{
				int tid = recibirEntero(pcb->socket, log_interno);
				char* semName = recibirTexto(pcb->socket, log_interno);
				int resultadoWait = wait_hilo(tid,semName);//Hacer los pasos para un Wait.
				int resWait = enviarEntero(pcb->socket, resultadoWait,  log_interno);
				break;
			}
			case SIGNAL:{
				int tid = recibirEntero(pcb->socket, log_interno);
				char* semName = recibirTexto(pcb->socket, log_interno);
				signal_hilo(tid,semName);//Hacer los pasos para un Signal.
				break;
			}
		}
	}
}

void create_hilo(int tid){

}

int next_hilo(){

}

void join_hilo(int tid){

}

void close_hilo(int tid){

}

int wait_hilo(int tid,char* semName){

}

void signal_hilo(int tid, char* semName){

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
