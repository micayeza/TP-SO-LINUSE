#include "SUSE.h"

void atenderPrograma(t_programa* parametros){

	pthread_mutex_t tabla_hijos;
	pthread_mutex_t sem_ready;
	pthread_mutex_t sem_exec;

	t_list* tabla_ready = list_create();
	t_new*  hilo_exec;

	//Me mantengo a la espera de Recibir Mensaje de este PCB->socket
	while(1){
		int operacionRecibida = recibirInt(parametros->programa);
		switch(operacionRecibida){
			case CREATE_HILO:{
				int tid = recibirEntero(parametros->programa, log_interno);
				create_hilo(tid, parametros, tabla_hijos); //Hacer los pasos para un Create Hilo.
				planificadorLargo(tabla_ready, sem_ready);
				break;
			}
			case SCHEDULE_NEXT:{
				int nextTid = next_hilo(tabla_ready, sem_ready,hilo_exec,  sem_exec); //Hacer los pasos para un Schedule Next.
				int resNext = enviarEntero(parametros->programa, nextTid,  log_interno);
				break;
			}
			case JOIN:{
				int tid = recibirEntero(parametros->programa, log_interno);
				join_hilo(tid, hilo_exec);//Hacer los pasos para un Join.
				break;
			}
			case CLOSE:{
				int tid = recibirEntero(parametros->programa, log_interno);
				close_hilo(tid);//Hacer los pasos para un Close.
				break;
			}
			case WAIT:{
				int tid = recibirEntero(parametros->programa, log_interno);
				char* semName = recibirTexto(parametros->programa, log_interno);
				int resultadoWait = wait_hilo(tid,semName); //Hacer los pasos para un Wait.
				int resWait = enviarEntero(parametros->programa, resultadoWait,  log_interno);
				break;
			}
			case SIGNAL:{
				int tid = recibirEntero(parametros->programa, log_interno);
				char* semName = recibirTexto(parametros->programa, log_interno);
				signal_hilo(tid,semName); //Hacer los pasos para un Signal.
				break;
			}
		}
	}
}

void recalcularEstimado(t_hilo* hilo){
	hilo->estimadoActual = ((1-config_suse->alpha)*hilo->estimadoAnterior) + config_suse->alpha * hilo->real;
	}

t_hilo* buscar_prog_hilo(int prog, int hilo){
	bool buscar_prog(void* progam){
					return ((t_programa*)progam)->programa == prog ;
				}

		 t_programa* programa = list_find(tabla_programas, &buscar_prog);

		 if(programa != NULL){
			 bool buscar_hilo(void* hl){
					return ((t_hilo*)hl)->hijo == hilo ;
				}

			 t_hilo* hilo = list_find(programa->hijos, &buscar_hilo);

			 return hilo;
		 }
		 return NULL;
}

void planificadorLargo(t_list* tabla_ready, pthread_mutex_t sem_ready){

	if(cant_programas < config_suse->multiprog){
		t_new* nuevo = list_remove(tabla_new, 0);
		if(nuevo != NULL){
			t_hilo* hilo = buscar_prog_hilo(nuevo->programa, nuevo->hijos);
			hilo->estado = READY;
			hilo->deNaR  = clock() - hilo->initNew;
			hilo->initReady = clock();

			log_info(log_interno , "Pasa el hilo %d del programa %da READY \n", nuevo->hijos, nuevo->programa );
			pthread_mutex_lock(&sem_ready);
			list_add(tabla_ready, nuevo);
			pthread_mutex_unlock(&sem_ready);
		 }
		}

	}

void create_hilo(int tid, t_programa* programa, pthread_mutex_t tabla_hijos){

	t_hilo* hijo   		 = malloc(sizeof(t_hilo));
	hijo->estado   		 = NEW;
	hijo->estimadoActual = 0;
	hijo->hijo     		 = tid;//list_size(hermanos);
	hijo->real	   		 = 0;
	hijo->initNew  		 = clock();

	t_new* nuevo = malloc(sizeof(t_new));
	nuevo->estimado = 0;
	nuevo->hijos    = hijo->hijo;
	nuevo->programa = programa->programa;

	pthread_mutex_lock(&tabla_hijos);
	list_add(programa->hijos, hijo);
	pthread_mutex_unlock(&tabla_hijos);
	pthread_mutex_lock(&sem_new);
	list_add(tabla_new, nuevo);
	pthread_mutex_unlock(&sem_new);

}

int next_hilo(t_list* tabla_ready, pthread_mutex_t sem_ready,t_new* hilo_exec,  pthread_mutex_t sem_exec){

	bool ordenar_estimado(void* pri, void* sec){
		return ((t_new*)pri)->estimado <= ((t_new*)sec)->estimado;
	}
	pthread_mutex_lock(&sem_ready);
	list_sort(tabla_ready , &ordenar_estimado);

	t_new* nuevo = list_remove(tabla_ready, 0);
	pthread_mutex_unlock(&sem_ready);

	log_info(log_interno, "Pasa el hilo %d del programa %d a ejecutar \n", nuevo->hijos, nuevo->programa);
	hilo_exec = nuevo;

	t_hilo* hilo = buscar_prog_hilo(nuevo->programa, nuevo->hijos);
	hilo->enReady  += clock() - hilo->initReady;
	hilo->initExec  = clock();

	return nuevo->hijos;
}

void join_hilo(int tid, t_new* hilo_exe){

	//Se bloquea el hilo exe a la espera de que termine tid


}

void close_hilo(int tid){

}

int wait_hilo(int tid,char* semName){

}

void signal_hilo(int tid, char* semName){

}

//void encolarEnNew(t_TCB* TCB){
//
//	pthread_mutex_lock(&mutexColaNew);
//	queue_push(colaNew, TCB);
//	pthread_mutex_unlock(&mutexColaNew);
//}

//void transicionNewAReady(int idHilo){

//	t_configSUSE* config = getConfigSUSE(configPath);

	//while(/*numeroActualDeProcesosEnMemoria < config->maxMultiprog*/){

	//}

//}

//t_PCB* create_PCB(int socket){
//	t_PCB* PCB = malloc(sizeof(t_PCB));
//	PCB->socket = socket;
//	PCB->TCBs = dictionary_create();
//	PCB->estado = NEW;
//	return PCB;
//}

//void free_PCB(t_PCB* PCB){
//	dictionary_destroy_and_destroy_elements(PCB->TCBs, (void*)free_TCB);
//	free(PCB);
//}

//t_TCB* create_TCB_en_PCB(t_PCB* PCB){
//	t_TCB* TCB = malloc(sizeof(t_TCB));
//	TCB->id = cantidadTCBsEnPCB; //Si la PCB ya tiene tres hilos, el ID del nuevo será 3.
//	TCB->procesoPadre = PCB;
//	dictionary_put(PCB->TCBs,string_itoa(TCB->id),TCB);
//	return TCB;
//}
//void free_TCB(t_TCB* TCB){
//	//No libero 'procesoPadre' porque me liberaría la PCB y no corresponde.
//	free(TCB);
//}
//
//int cantidadTCBsEnPCB(t_PCB* PCB){
//	return dictionary_size(PCB->TCBs);
//}
//
//int cantidadHilosEnEjecucion(){
//	//TODO: Acá devuelvo todos los TCBs que se encuentran en estado READY o EXEC.
//}

//Se me ocurrio hacer una cola "ejecución" que tenga a las colas "bloqueado", "ready" y "ejecutando" adentro. Contando todos los hilos que están corriendo adentro
//debería devolver la cantidad de procesos en memoria.

//Hay que armar un struct que sea TCB (Thread Control Block) con la info necesaria. Ver qué es lo que necesitamos para administrar la planificación.
