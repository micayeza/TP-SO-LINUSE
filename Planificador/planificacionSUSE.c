#include "SUSE.h"

void atenderPrograma(void* par){

	t_programa* parametros = par;
	pthread_mutex_t tabla_hijos;
	pthread_mutex_t sem_ready;
	pthread_mutex_t sem_exec;

	t_list* tabla_ready = list_create();
	t_new*  hilo_exec = NULL;

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
				enviarEntero(parametros->programa, nextTid,  log_interno);
				break;
			}
			case JOIN:{
				int tid = recibirEntero(parametros->programa, log_interno);
				join_hilo(tid, hilo_exec);//Hacer los pasos para un Join.
				break;
			}
			case CLOSE:{
				int tid = recibirEntero(parametros->programa, log_interno);
				close_hilo(tid, parametros->programa, tabla_ready, hilo_exec);//Hacer los pasos para un Close.
				printefearMetricas();
				break;
			}
			case WAIT:{
				int tid = recibirEntero(parametros->programa, log_interno);
				char* semName = recibirTexto(parametros->programa, log_interno);
				pthread_mutex_lock(&wt);
				int resultadoWait = wait_hilo(tid,semName); //Hacer los pasos para un Wait.

				 if(resultadoWait == 1){//Se bloqueo
					 t_hilo* hilo = buscar_prog_hilo(parametros->programa, tid);
					 hilo->real = clock() - hilo->initExec;
					 recalcularEstimado(hilo);
					 hilo->initLock = clock();
					 hilo->enExec  += clock() - hilo->initExec;
					 hilo->estado   = BLOCKED;

					 t_block* lock  = malloc(sizeof(t_block));
					 lock->estimado = hilo->estimadoActual;
					 lock->id       = hilo->id;
					 lock->programa = parametros->programa;
					 lock->tid      = -1;
					 lock->sem      = semName;

					 pthread_mutex_lock(&sem_lock);
					 list_add(tabla_lock, lock);
					 pthread_mutex_unlock(&sem_lock);

					 bloquearEnSemaforo(lock,  semName);
					 hilo_exec=NULL;


				 }
				enviarEntero(parametros->programa, resultadoWait,  log_interno);
				pthread_mutex_unlock(&wt);

			}break;
			case SIGNAL:{
				int tid = recibirEntero(parametros->programa, log_interno);
				pthread_mutex_lock(&sem_lock);//Habria que chequear
				pthread_mutex_lock(&sl);
				char* semName  = recibirTexto(parametros->programa, log_interno);
				t_block* block = signal_hilo(tid,semName, tabla_ready); //Hacer los pasos para un Signal.
				if(block != NULL){
				bool buscar_lock(void* blo){
					return ((t_block*)blo)->id == block->id &&
							((t_block*)blo)->programa == block->programa;
				}

				block = list_remove_by_condition(tabla_lock, &buscar_lock);

				free(block->sem);
				free(block);
				}
				pthread_mutex_unlock(&sl);
				pthread_mutex_unlock(&sem_lock);
			}break;
		}
	}
}

void recalcularEstimado(t_hilo* hilo){
	hilo->estimadoAnterior = hilo->estimadoActual;
	hilo->estimadoActual   = ((1-config_suse->alpha)*hilo->estimadoAnterior) + config_suse->alpha * hilo->real;

}

t_hilo* buscar_prog_hilo(int prog, int hilo){
	bool buscar_prog(void* progam){
					return ((t_programa*)progam)->programa == prog ;
				}

		 t_programa* programa = list_find(tabla_programas, &buscar_prog);

		 if(programa != NULL){
			 bool buscar_hilo(void* hl){
					return ((t_hilo*)hl)->id == hilo ;
				}

			 t_hilo* hilo = list_find(programa->hijos, &buscar_hilo);

			 return hilo;
		 }
		 return NULL;
}

void planificadorLargo(t_list* tabla_ready, pthread_mutex_t sem_ready){

	pthread_mutex_lock(&multi);
	if(cant_programas < config_suse->multiprog){
		t_new* nuevo = list_remove(tabla_new, 0);
		if(nuevo != NULL){
			t_hilo* hilo = buscar_prog_hilo(nuevo->programa, nuevo->id);
			hilo->estado = READY;
			hilo->deNaR  = clock() - hilo->initNew;
			hilo->initReady = clock();

			log_info(log_interno , "Pasa el hilo %d del programa %da READY \n", nuevo->id, nuevo->programa );

			list_add(tabla_ready, nuevo);

			cant_programas ++;


		 }
		}
	pthread_mutex_unlock(&multi);

}

void create_hilo(int tid, t_programa* programa, pthread_mutex_t tabla_hijos){

	t_hilo* hijo   		 = malloc(sizeof(t_hilo));
	hijo->estado   		 = NEW;
	hijo->estimadoActual = 0;
	hijo->id     		 = tid;//list_size(hermanos);
	hijo->real	   		 = 0;
	hijo->initNew  		 = clock();

	hijo->dNaM = 0; hijo->deNaR = 0; hijo->enExec = 0; hijo->enLock =0; hijo->enReady = 0;


	t_new* nuevo = malloc(sizeof(t_new));
	nuevo->estimado = 0;
	nuevo->id       = hijo->id;
	nuevo->programa = programa->programa;

	pthread_mutex_lock(&tabla_hijos);
	list_add(programa->hijos, hijo);
	pthread_mutex_unlock(&tabla_hijos);
	pthread_mutex_lock(&sem_new);
	list_add(tabla_new, nuevo);
	pthread_mutex_unlock(&sem_new);

}

int next_hilo(t_list* tabla_ready, pthread_mutex_t sem_ready,t_new* hilo_exec,  pthread_mutex_t sem_exec){

	//Si el hilo que esta ejecutando puede seguir ejecutando que siga mierda
	if(hilo_exec != NULL){
		return hilo_exec->id;
	}

	if(list_size(tabla_ready)==0){
		return -1;
	}

	bool ordenar_estimado(void* pri, void* sec){
		return ((t_new*)pri)->estimado <= ((t_new*)sec)->estimado;
	}
	pthread_mutex_lock(&sem_ready);
	list_sort(tabla_ready , &ordenar_estimado);

	t_new* nuevo = list_remove(tabla_ready, 0);
	pthread_mutex_unlock(&sem_ready);

	log_info(log_interno, "Pasa el hilo %d del programa %d a ejecutar \n", nuevo->id, nuevo->programa);
	hilo_exec = nuevo;

	t_hilo* hilo = buscar_prog_hilo(nuevo->programa, nuevo->id);
	hilo->enReady  += clock() - hilo->initReady;
	hilo->initExec  = clock();
	hilo->estado    = EXEC;

	return nuevo->id;
}

void join_hilo(int tid, t_new* hilo_exe){
	//Si el tid ya esta en exit sigo ejecutando tranka
	t_hilo* hilo = buscar_prog_hilo(hilo_exe->programa, hilo_exe->id);
	bool buscar_exit(void* ex){
		return (((t_new*)ex)->id == hilo->id &&
			   ((t_new*)ex)->programa == hilo_exe->programa);
	}

	pthread_mutex_lock(&sem_exit);
	t_new* exit = list_find(tabla_exit, &buscar_exit);
	pthread_mutex_unlock(&sem_exit);

	if(exit!=NULL){
		log_info(log_interno, "El hilo a joinear ya habia finalizado, contuniar ejecución de hilo %d \n",hilo_exe->id );
		return;
	}

	//Se bloquea el hilo exe a la espera de que termine tid


	hilo->real     = clock() - hilo->initExec;
	hilo->enExec  += clock() - hilo->initExec;
	hilo->initLock = clock();
	hilo->estado   = BLOCKED;
	recalcularEstimado(hilo);


	t_block* bloqueado  = malloc(sizeof(t_block));
	bloqueado->estimado = hilo_exe->estimado;
	bloqueado->id       = hilo_exe->id;
	bloqueado->programa = hilo_exe->programa;
	bloqueado->tid      = tid;
	bloqueado->sem      = NULL;

	pthread_mutex_lock(&sem_lock);
	list_add(tabla_lock, bloqueado);
	pthread_mutex_unlock(&sem_lock);

	t_new* aux = hilo_exe;

	free(aux);
	hilo_exe=NULL;

}

void close_hilo(int tid, int programa, t_list* tabla_ready, t_new* hilo_exec){

	t_hilo* hilo = buscar_prog_hilo(programa,  tid);
	switch(hilo->estado){
	 case NEW:{

		 hilo->deNaR   = -1;

		 bool buscar_new(void* h){
		 				 return ((t_new*)h)->id == hilo->id &&
		 						 ((t_new*)h)->programa == programa;
		 			 }

			pthread_mutex_lock(&sem_new);
			t_new* hilo_exit = list_remove_by_condition(tabla_new, &buscar_new );
			pthread_mutex_unlock(&sem_lock);

			pthread_mutex_lock(&sem_exit);
			list_add(tabla_exit, hilo_exit);
			pthread_mutex_unlock(&sem_exit);

	 }break;
	 case READY:{
		 hilo->enReady += clock() - hilo->initReady;
			 bool buscar_ready(void* h){
				 return ((t_new*)h)->id == hilo->id;
			 }
		t_new* hilo_exit = list_remove_by_condition(tabla_ready, &buscar_ready );

		pthread_mutex_lock(&sem_exit);
		list_add(tabla_exit, hilo_exit);
		pthread_mutex_unlock(&sem_exit);

	 } break;
	 case EXEC: {
		 hilo->enExec += clock() - hilo->initExec;

			pthread_mutex_lock(&sem_exit);
			list_add(tabla_exit, hilo_exec);
			pthread_mutex_unlock(&sem_exit);

			hilo_exec = NULL;

	 }break;
	 case BLOCKED:{
		 hilo->enLock  += clock() - hilo->initLock;
		 hilo->enReady += clock() - hilo->initReady;

		 bool buscar_lock(void* h){
				 return ((t_block*)h)->id == hilo->id &&
						 ((t_block*)h)->programa == programa;
			 }

		pthread_mutex_lock(&sem_lock);
		t_block* hilo_block = list_remove_by_condition(tabla_lock, &buscar_lock );
		pthread_mutex_unlock(&sem_lock);

		t_new* hilo_exit = malloc(sizeof(t_new));
		hilo_exit->estimado = 0;
		hilo_exit->id       = hilo->id;
		hilo_exit->programa = programa;

		free(hilo_block);

		pthread_mutex_lock(&sem_exit);
		list_add(tabla_exit, hilo_exit);
		pthread_mutex_unlock(&sem_exit);

	 }
	 break;


	}
	pthread_mutex_lock(&multi);
	cant_programas --;
	pthread_mutex_unlock(&multi);
	hilo->estado = EXIT;

	//Algun otro hilo estaba trabado esperando qeu el termine?

			int i = 0;
			pthread_mutex_lock(&sem_lock);
			while(i<list_size(tabla_lock)){

				t_block* bloc = list_get(tabla_lock, i);

				if(bloc->tid == hilo->id && bloc->programa == programa){

					bloc = list_remove(tabla_lock, i);

					log_info(log_interno, "Se desbloquea el hilo %d, del programa %d por haber finalizado el hilo %d \n", bloc->id, bloc->programa, tid );
					t_hilo* hilo    = buscar_prog_hilo(bloc->programa, hilo->id);
					hilo->enLock   += clock() - hilo->initLock;
					hilo->initReady = clock();
					hilo->estado    = READY;

					t_new* bloc_a_ready = malloc(sizeof(t_new));
					bloc_a_ready->estimado = 0;
					bloc_a_ready->id       = bloc->id;
					bloc_a_ready->programa = programa;

					list_add(tabla_ready, bloc_a_ready);


					free(bloc);

				}
			}
			pthread_mutex_unlock(&sem_lock);
}

void bloquearEnSemaforo(t_block* block, char* sem_id){
	int sem_pos = posicionSemaforo(sem_id);
	log_info(log_interno, "Bloqueado en Semaforo %s, PID: %d",sem_id,block->id);
	queue_push(sem_blocked[sem_pos],block);
}


int posicionSemaforo(char* sem_id){
	int i=0;
	while(i<config_suse->cantSem && !string_equals_ignore_case(config_suse->semId[i],sem_id)){
		i++;
	}
	return i;
}


int wait_hilo(int tid,char* semName){

	    int bloquear = 0;
		int pos = posicionSemaforo(semName);
		sem_values[pos]--;
		log_info(log_interno, "[WAIT] Semaforo %s. Valor actual: %d",semName,sem_values[pos]);
		if(sem_values[pos]<0){
			bloquear = 1;
		}
		return bloquear;
}

t_block* getNextFromSemaforo(int sem_pos){
	t_block* block = queue_pop(sem_blocked[sem_pos]);
	return block;
}

t_block* signal_hilo(int tid, char* semName, t_list* tabla_ready){

	int pos = posicionSemaforo(semName);
		if(sem_values[pos] < config_suse->semMax[pos]){
		sem_values[pos]++;
		log_info(log_interno, "[SIGNAL] Semaforo %s. Valor actual: %d",semName,sem_values[pos]);
		if(sem_values[pos]>0){
			t_block* block = getNextFromSemaforo(pos);
			log_info(log_interno, "Desbloqueado de Semaforo %s, PID: %d",semName,block->id);
			if(block!=NULL){
				t_hilo* hilo = buscar_prog_hilo(block->programa, block->id);
				hilo->estado = READY;
				hilo->enLock += clock()-hilo->initLock;
				hilo->initReady = clock();

				t_new* nuevo = malloc(sizeof(t_new));
				nuevo->id 		= block->id;
				nuevo->programa = block->programa;
				nuevo->estimado = block->estimado;

				list_add(tabla_ready, nuevo);

				}
			return block;
			}

		}
		return NULL;

}

void printefearMetricas(){

	int i= 0;
		log_info(log_interno, "Metrica del sistema: Grado actual multiprogramación %d \n", cant_programas);
		log_info(log_interno,"Metrica del sistema: Valor de semaforos: \n" );
		for(int x = 0; x< config_suse->cantSem; x++){
		log_info(log_interno,"Semaforo %s : Valor Actual %d \n", config_suse->semId[i], sem_values[i] );
		}

	while(i<list_size(tabla_programas)){
		t_programa* prog = list_get(tabla_programas, i);
		int new = 0; int ready = 0; int exec = 0; int blocked = 0; int exit = 0;
		double tiempo = 0; double algo =0; double div;
		for(int x = 0; x<list_size(prog->hijos); x++){
			t_hilo* hilo = list_get(prog->hijos, x);
			if(hilo->estado == NEW) new++;
			if(hilo->estado == READY) ready++;
			if(hilo->estado == EXEC) exec++;
			if(hilo->estado == BLOCKED) blocked++;
			if(hilo->estado == EXIT) exit++;

			tiempo += hilo->enExec;

		}

		log_info(log_interno,"Metricas del Programa: Hilos por estado \n " );
		log_info(log_interno," Estado New     : %d \n", new);
		log_info(log_interno," Estado Ready   : %d \n", ready );
		log_info(log_interno," Estado Exec    : %d \n", exec);
		log_info(log_interno," Estado Blocked : %d \n", blocked);
		log_info(log_interno," Estado Exit    : %d \n", exit);

		for(int x = 0; x<list_size(prog->hijos); x++){
				t_hilo* hilo = list_get(prog->hijos, x);
				algo += clock() - hilo->initNew;
				div   = tiempo/hilo->enExec;
				log_info(log_interno,"Metricas del hilo &d \n ", hilo->id );
				log_info(log_interno," Tiempo de ejecución  : %lf \n", algo);
				log_info(log_interno," Tiempo de espera     : %lf \n", hilo->enReady );
				log_info(log_interno," Tiempo de uso de CPU : %lf \n", hilo->enExec);
				log_info(log_interno," Tiempo bloqueado     : %lf \n", hilo->enLock);
				log_info(log_interno," Demora en entrar a Ready : %lf \n", hilo->deNaR);
				log_info(log_interno," Porcentaje del Tiempo de Ejecución    : %lf \n", div);


				}
		i++;
	}


}


