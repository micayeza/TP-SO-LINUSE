#include "SUSE.h"

void atenderPrograma(void* par){

	t_programa* parametros = par;

	t_new*  hilo_exec = NULL;

	t_list* joins = list_create();

	//Me mantengo a la espera de Recibir Mensaje de este PCB->socket
	while(1){

		int operacionRecibida = recibirInt(parametros->programa);
		switch(operacionRecibida){
			case CREATE_HILO:{

				int tid = recibirInt(parametros->programa);
				log_info(log_interno, "[CREATE] hilo %d Programa %d \n",tid, parametros->programa);
				create_hilo(tid, parametros); //Hacer los pasos para un Create Hilo.
				planificadorLargo();

			}break;
			case SCHEDULE_NEXT:{
				log_info(log_interno, "[SCHEDULE NEXT] Programa %d \n", parametros->programa);

				//Paso el hilo de exec a ready y le cambio el estimado
				if(hilo_exec != NULL){
					t_hilo* hilo = buscar_prog_hilo(parametros->programa, hilo_exec->id);
					 if(hilo!=NULL){//evito que rompa
						 hilo->real = clock() - hilo->initExec;
						 recalcularEstimado(hilo);
						 hilo->estado = READY;
						 hilo->initReady = clock();
						 hilo->enExec += clock() - hilo->initExec;

						 hilo_exec->estimado = hilo->estimadoActual;

						 t_pth_programas* mut = buscar_mutex(parametros->programa);
						 pthread_mutex_lock(&mut->mtx);
						 list_add(parametros->tablaReady, hilo_exec);
						 pthread_mutex_unlock(&mut->mtx);
						 hilo_exec= NULL;

							pthread_mutex_lock(&sem_lista_semaforos);
							t_sem_contador* contador = buscar_contador(parametros->programa);

							pthread_mutex_unlock(&sem_lista_semaforos);
							sem_post(&contador->cont);
					 }
				}
				//MMMM no me convence mandar esto asique salvo que venga el close se cagan. que hagan ctrl c
				if(parametros->cant_hilos == 0){
					enviarInt(parametros->programa, parametros->id);
					break;
				}


				pthread_mutex_lock(&sem_lista_semaforos);
				t_sem_contador* contador = buscar_contador(parametros->programa);
				pthread_mutex_unlock(&sem_lista_semaforos);
				sem_wait(&contador->cont);


				hilo_exec = next_hilo(parametros->tablaReady);
				enviarInt(parametros->programa, hilo_exec->id);

			}break;
			case JOIN:{
				int tid = recibirInt(parametros->programa);
				log_info(log_interno, "[JOIN] %d \n",tid);
				join_hilo(tid, parametros->programa, joins);//Hacer los pasos para un Join.
				if(list_size(joins)>0)parametros->estado = BLOQUEADO;

			}break;
			case CLOSE:{
				int tid = recibirInt(parametros->programa);
				log_info(log_interno, "[CLOSE] %d \n",tid);
				hilo_exec = close_hilo(tid, parametros, parametros->tablaReady, hilo_exec, joins);//Hacer los pasos para un Close.
				printefearMetricas();
				if(parametros->estado == FINALIZADO){
					for(int i=0; i<list_size(parametros->tablaReady);i++){
						t_new* read = list_remove(parametros->tablaReady, i);
						free(read);
					}
					for(int i=0; i<list_size(joins);i++){
						t_join* jo = list_remove(joins, i);
						free(jo);
					}
					free(parametros->tablaReady);
					free(joins);
					if(hilo_exec != NULL)free(hilo_exec);
					pthread_exit("Chau");

				}else{
					planificadorLargo();
				}

			}break;
			case WAIT:{
				int tid = recibirInt(parametros->programa);

				char* semName = recibirTexto(parametros->programa, log_interno);
				log_info(log_interno, "[WAIT] %s \n",semName);
				pthread_mutex_lock(&wt);
				int resultadoWait = wait_hilo(tid,semName); //Hacer los pasos para un Wait.

				 if(resultadoWait == 1){//Se bloqueo
					 log_info(log_interno, "Se bloquea el hilo %d a la espera del semaforo %s \n",tid, semName);
					 t_hilo* hilo = buscar_prog_hilo(parametros->programa, tid);

					 if(hilo!=NULL){//evito que rompa
					 t_pth_programas* mut = buscar_mutex(parametros->programa);
					 pthread_mutex_lock(&mut->mtx);

					 hilo->real = clock() - hilo->initExec;
					 recalcularEstimado(hilo);
					 hilo->initLock = clock();
					 hilo->enExec  += clock() - hilo->initExec;
					 hilo->estado   = BLOCKED;

					 pthread_mutex_unlock(&mut->mtx);

					 t_block* lock  = malloc(sizeof(t_block));
					 lock->estimado = hilo->estimadoActual;
					 lock->id       = hilo->id;
					 lock->programa = parametros->programa;
					 lock->tid      = -1;
					 lock->sem      = posicionSemaforo(semName);

					 pthread_mutex_lock(&sem_lock);
					 list_add(tabla_lock, lock);
					 pthread_mutex_unlock(&sem_lock);


					 bloquearEnSemaforo(lock,  semName);
					 hilo_exec=NULL;
					 }


				 }
				enviarInt(parametros->programa, resultadoWait);
				free(semName);
				pthread_mutex_unlock(&wt);

			}break;
			case SIGNAL:{
				int tid = recibirInt(parametros->programa);
				pthread_mutex_lock(&wt);
				pthread_mutex_lock(&sem_lock);//Habria que chequear
//				pthread_mutex_lock(&sl);

				char* semName  = recibirTexto(parametros->programa, log_interno);
				log_info(log_interno, "[SIGNAL] %s \n",semName);
				t_block* block = signal_hilo(tid,semName); //Hacer los pasos para un Signal.
				if(block != NULL){
					bool buscar_lock(void* blo){
						return ((t_block*)blo)->id == block->id &&
								((t_block*)blo)->programa == block->programa;
					}

				block = list_remove_by_condition(tabla_lock, &buscar_lock);
				free(block);

				}
				free(semName);
//				pthread_mutex_unlock(&sl);

				pthread_mutex_unlock(&sem_lock);
				pthread_mutex_unlock(&wt);
			}break;
		}
   }
}


void recalcularEstimado(t_hilo* hilo){
	hilo->estimadoAnterior = hilo->estimadoActual;
	hilo->estimadoActual   = ((1-config_suse->alpha)*hilo->estimadoAnterior) + (config_suse->alpha * hilo->real);

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


t_sem_contador* buscar_contador(int id){

	bool buscar_semaforo(void* sem_cont){
			return ((t_sem_contador*)sem_cont)->idProg == id ;
		}

	return list_find(lista_semaforos, &buscar_semaforo);
}

t_hilo* buscar_hilo_bool(int id, t_programa* programa){

	bool buscar_hilo_b(void* hl){
					return ((t_hilo*)hl)->id == id ;
	}

   return list_find(programa->hijos, &buscar_hilo_b);
}

t_programa* buscar_programa_bool(int id){

		bool buscar_prog_b(void* progam){
					return ((t_programa*)progam)->programa == id ;
				}

 return list_find(tabla_programas, &buscar_prog_b);
}

t_pth_programas*  buscar_mutex( int programa){

	bool buscar_mu(void* pth){
				return ((t_pth_programas*)pth)->idProg == programa ;
			}

 return list_find(lista_sem_programas, &buscar_mu);
}


void planificadorLargo(){

	pthread_mutex_lock(&multi);
	log_info(log_interno, "Cantidad de programas en memoria %d \n",cant_programas);
//	if(cant_programas < config_suse->multiprog){
	while(cant_programas < config_suse->multiprog && list_size(tabla_new)>0){

		t_new* nuevo = list_remove(tabla_new, 0);
		if(nuevo != NULL){

			t_programa* programa = buscar_programa_bool(nuevo->programa);

			if(programa == NULL)continue;

			t_hilo* hilo = buscar_hilo_bool(nuevo->id, programa);

			hilo->estado = READY;
			hilo->deNaR  = clock() - hilo->initNew;
			hilo->initReady = clock();

			log_info(log_interno , "Pasa el hilo %d del programa %d a READY \n", nuevo->id, nuevo->programa );

			t_pth_programas* mut = buscar_mutex(programa->programa);
			pthread_mutex_lock(&mut->mtx);
			list_add(programa->tablaReady, nuevo);
			pthread_mutex_unlock(&mut->mtx);

			pthread_mutex_lock(&sem_lista_semaforos);
			t_sem_contador* contador = buscar_contador(programa->programa);
			sem_post(&contador->cont);
			pthread_mutex_unlock(&sem_lista_semaforos);

			cant_programas ++;

		 }
		}
	pthread_mutex_unlock(&multi);

}

void create_hilo(int tid, t_programa* programa){

	log_info(log_interno, "Se crea el hilo %d del programa %d ", tid, programa->programa);
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


	list_add(programa->hijos, hijo);
	programa->cant_hilos ++;
	pthread_mutex_lock(&sem_new);
	list_add(tabla_new, nuevo);
	pthread_mutex_unlock(&sem_new);

}

t_new* next_hilo(t_list* tabla_ready){

	//Si el hilo que esta ejecutando puede seguir ejecutando que siga mierda

	if(list_size(tabla_ready)==0){
		log_error(log_interno, "No hay hilo para ejecutar \n");
		return NULL;
	}

	bool ordenar_estimado(void* pri, void* sec){
		return ((t_new*)pri)->estimado <= ((t_new*)sec)->estimado;
	}

	list_sort(tabla_ready , &ordenar_estimado);

	t_new* nuevo = list_remove(tabla_ready, 0);


	log_info(log_interno, "Pasa el hilo %d del programa %d a ejecutar \n", nuevo->id, nuevo->programa);

	pthread_mutex_lock(&sem_new);
	t_hilo* hilo = buscar_prog_hilo(nuevo->programa, nuevo->id);
	pthread_mutex_unlock(&sem_new);
	hilo->enReady  += clock() - hilo->initReady;
	hilo->initExec  = clock();
	hilo->estado    = EXEC;

	return nuevo;
}

void join_hilo(int tid, int programa, t_list* joins){
	//Si el tid ya esta en exit sigo ejecutando tranka
//	t_hilo* hilo = buscar_prog_hilo(hilo_exe->programa, hilo_exe->id);
	bool buscar_exit(void* ex){
		return (((t_new*)ex)->id == tid &&
			   ((t_new*)ex)->programa == programa);
	}

	pthread_mutex_lock(&sem_exit);
	t_new* exit = list_find(tabla_exit, &buscar_exit);
	pthread_mutex_unlock(&sem_exit);

	if(exit!=NULL){
		log_info(log_interno, "El hilo a joinear ya habia finalizado \n");
		return;
	}

	//Se cloquea el programa a la espera de que termine el hilo, si hacen close antes de este hilo no cierro
	log_info(log_interno, "El programa %d queda bloqueado hasta que finalice el hilo %d \n",programa, tid);
	t_join* join = malloc(sizeof(t_join));
	join->id = tid;
	list_add(joins, join);



}

t_new* close_hilo(int tid, t_programa* programa, t_list* tabla_ready, t_new* hilo_exec, t_list* joins){

	if(tid == programa->id){
		//NO puedo cerrar el hilo central porque hay hilos pendientes, qqueda bloqueado
		if(list_size(joins)>0){
			log_error(log_interno, "No se puede cerrar el hilo %d ya que aun hay hilos joineados", tid);
			return NULL;
			}
		log_info(log_interno, "Se finaliza el programa %d \n",programa->programa);
		programa->estado = FINALIZADO;
		programa->fin = clock();
		return NULL;

	}
	programa->cant_hilos --;
	t_hilo* hilo = buscar_prog_hilo(programa->programa,  tid);

	switch(hilo->estado){
	 case NEW:{
			log_info(log_interno, "El hilo ha cerrar estaba en estado nuevo \n");
		 hilo->deNaR   = -1;

		 bool buscar_new(void* h){
		 				 return ((t_new*)h)->id == hilo->id &&
		 						 ((t_new*)h)->programa == programa->programa;
		 			 }

			pthread_mutex_lock(&sem_new);
			t_new* hilo_exit = list_remove_by_condition(tabla_new, &buscar_new );
			pthread_mutex_unlock(&sem_lock);

			pthread_mutex_lock(&sem_exit);
			list_add(tabla_exit, hilo_exit);
			pthread_mutex_unlock(&sem_exit);

	 }break;
	 case READY:{
		 log_info(log_interno, "El hilo ha cerrar estaba en estado ready \n");
		 hilo->enReady += clock() - hilo->initReady;
			 bool buscar_ready(void* h){
				 return ((t_new*)h)->id == hilo->id;
			 }
		t_new* hilo_exit = list_remove_by_condition(tabla_ready, &buscar_ready );
		pthread_mutex_lock(&sem_lista_semaforos);
		t_sem_contador* contador = buscar_contador(programa->programa);
		sem_wait(&contador->cont);
		pthread_mutex_unlock(&sem_lista_semaforos);


		pthread_mutex_lock(&sem_exit);
		list_add(tabla_exit, hilo_exit);
		pthread_mutex_unlock(&sem_exit);

	 } break;
	 case EXEC: {
		 log_info(log_interno, "El hilo ha cerrar estaba en estado exec \n");
		 hilo->enExec += clock() - hilo->initExec;

			pthread_mutex_lock(&sem_exit);
			list_add(tabla_exit, hilo_exec);
			pthread_mutex_unlock(&sem_exit);

			hilo_exec = NULL;

	 }break;
	 case BLOCKED:{
		 log_info(log_interno, "El hilo ha cerrar estaba en estado bloqueado \n");
		 hilo->enLock  += clock() - hilo->initLock;
		 hilo->enReady += clock() - hilo->initReady;

		 bool buscar_lock(void* h){
				 return ((t_block*)h)->id == hilo->id &&
						 ((t_block*)h)->programa == programa->programa;
			 }

		pthread_mutex_lock(&sem_lock);
		t_block* hilo_block = list_remove_by_condition(tabla_lock, &buscar_lock );
		pthread_mutex_unlock(&sem_lock);

		t_new* hilo_exit = malloc(sizeof(t_new));
		hilo_exit->estimado = 0;
		hilo_exit->id       = hilo->id;
		hilo_exit->programa = programa->programa;

		free(hilo_block);

		pthread_mutex_lock(&sem_exit);
		list_add(tabla_exit, hilo_exit);
		pthread_mutex_unlock(&sem_exit);

	 }
	 break;
	 default: break;

	}
	log_info(log_interno, "Pasa el hilo %d a EXIT \n", tid);
	pthread_mutex_lock(&multi);
	log_info(log_interno, "Programas en memoria %d \n", cant_programas);
	cant_programas --;
	pthread_mutex_unlock(&multi);
	hilo->estado = EXIT;

	bool buscar_int(void* joi){
		return ((t_join*)joi)->id == tid;
	}
	t_join* ju = list_remove_by_condition(joins, &buscar_int);
	if(ju != NULL){
	free(ju);	}
	if(list_size(joins)==0)programa->estado = ACTIVO;
	return hilo_exec;

	//***************** esto deja de ser necesario, solo sacarlo de join
	//Algun otro hilo estaba trabado esperando qeu el termine?

//			int i = 0;
//			pthread_mutex_lock(&sem_lock);
//			while(i<list_size(tabla_lock)){
//
//				t_block* bloc = list_get(tabla_lock, i);
//
//				if(bloc->tid == hilo->id && bloc->programa == programa->programa){
//
//					bloc = list_remove(tabla_lock, i);
//
//					log_info(log_interno, "Se desbloquea el hilo %d, del programa %d por haber finalizado el hilo %d \n", bloc->id, bloc->programa, tid );
//					t_hilo* hiloAready    = buscar_prog_hilo(bloc->programa, bloc->id);
//					hiloAready->enLock   += clock() - hiloAready->initLock;
//					hiloAready->initReady = clock();
//					hiloAready->estado    = READY;
//
//					t_new* bloc_a_ready = malloc(sizeof(t_new));
//					bloc_a_ready->estimado = 0;
//					bloc_a_ready->id       = bloc->id;
//					bloc_a_ready->programa = programa->programa;
//
//					list_add(tabla_ready, bloc_a_ready);
//
//
//					free(bloc);
//
//				}
//			}
//			pthread_mutex_unlock(&sem_lock);

//	return hilo_exec;
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
	if(i==config_suse->cantSem){
		log_error(log_interno, "No se encontro el semaforo, revise el nombre en el SUSE.config \n");
		i=-1;
	}
	return i;
}


int wait_hilo(int tid,char* semName){

	    int bloquear = 0;
		int pos = posicionSemaforo(semName);if(pos==-1)return -1;
//		if(sem_values[pos]>0)
			sem_values[pos]--;

		log_info(log_interno, "Semaforo %s. Valor actual: %d",semName,sem_values[pos]);
		if(sem_values[pos]<0){
			bloquear = 1;
		}
		return bloquear;
}

t_block* getNextFromSemaforo(int sem_pos){
	t_block* block = queue_pop(sem_blocked[sem_pos]);
	return block;
}

t_block* signal_hilo(int tid, char* semName){

	int pos = posicionSemaforo(semName);if(pos==-1)return NULL;
		if(sem_values[pos] < config_suse->semMax[pos]){
		sem_values[pos]++;
		log_info(log_interno, "[SIGNAL] Semaforo %s. Valor actual: %d",semName,sem_values[pos]);
		if(sem_values[pos]<=0){
			t_block* block = getNextFromSemaforo(pos);

				log_info(log_interno, "Desbloqueado de Semaforo %s, PID: %d",semName,block->id);


					t_programa* programa = buscar_programa_bool(block->programa);
					t_pth_programas* mut = buscar_mutex(programa->programa);
					pthread_mutex_lock(&mut->mtx);

					if(programa == NULL) return NULL;
					t_hilo* hilo = buscar_hilo_bool(block->id, programa);

					hilo->estado = READY;
					hilo->enLock += clock()-hilo->initLock;
					hilo->initReady = clock();

					t_new* nuevo = malloc(sizeof(t_new));
					nuevo->id 		= block->id;
					nuevo->programa = block->programa;
					nuevo->estimado = block->estimado;


					list_add(programa->tablaReady, nuevo);
					pthread_mutex_unlock(&mut->mtx);

					pthread_mutex_lock(&sem_lista_semaforos);
					t_sem_contador* contador = buscar_contador(nuevo->programa);
					sem_post(&contador->cont);
					pthread_mutex_unlock(&sem_lista_semaforos);



			return block;
			}

		}
		return NULL;

}

void printefearMetricas(){

	int i= 0;
	pthread_mutex_lock(&multi);
		log_info(log_interno, "Metrica del sistema: Grado actual multiprogramación %d \n", cant_programas);
	pthread_mutex_unlock(&multi);
		log_info(log_interno,"Metrica del sistema: Valor de semaforos: \n" );

		pthread_mutex_lock(&wt);
		for(int x = 0; x< config_suse->cantSem; x++){

		log_info(log_interno,"Semaforo %s : Valor Actual %d \n", config_suse->semId[x], sem_values[x] );

		}
		pthread_mutex_unlock(&wt);

	pthread_mutex_lock(&multi);
	while(i<list_size(tabla_programas)){
		t_programa* prog = list_get(tabla_programas, i);
		int new = 0; int ready = 0; int exec = 0; int blocked = 0; int exit = 0;
		double algo =0; double div = 0;
		for(int x = 0; x<list_size(prog->hijos); x++){
			t_hilo* hilo = list_get(prog->hijos, x);
			if(hilo->estado == NEW) new++;
			if(hilo->estado == READY) ready++;
			if(hilo->estado == EXEC) exec++;
			if(hilo->estado == BLOCKED) blocked++;
			if(hilo->estado == EXIT) exit++;

//			tiempo += hilo->enExec;

		}

		double vida =clock() - prog->init;
		if(prog->fin != 0) vida = prog->fin - prog->init;
		if(prog->estado == ACTIVO)log_info(log_interno,"Programa %d. Estado Activo. Tiempo de vida %lf \n ", prog->programa, vida );
		if(prog->estado == BLOQUEADO)log_info(log_interno,"Programa %d. Estado Bloqueado. Tiempo de vida %lf \n ", prog->programa, vida );
		if(prog->estado == FINALIZADO)log_info(log_interno,"Programa %d. Estado Finalizado. Tiempo de vida %lf \n ", prog->programa, vida );

		log_info(log_interno," Cantidad de Hilos por estado \n " );
		log_info(log_interno," Estado New     : %d \n", new);
		log_info(log_interno," Estado Ready   : %d \n", ready );
		log_info(log_interno," Estado Exec    : %d \n", exec);
		log_info(log_interno," Estado Blocked : %d \n", blocked);
		log_info(log_interno," Estado Exit    : %d \n", exit);

		for(int x = 0; x<list_size(prog->hijos); x++){
				t_hilo* hilo = list_get(prog->hijos, x);
				algo += clock() - hilo->initNew;
				if(hilo->enExec >0) div   = (hilo->enExec/vida)*100;
				log_info(log_interno,"Metricas del hilo %d \n ", hilo->id/CLOCKS_PER_SEC );
				log_info(log_interno," Tiempo de ejecución  : %lf segundos \n", algo/CLOCKS_PER_SEC);
				log_info(log_interno," Tiempo de espera     : %lf segundos \n", hilo->enReady/CLOCKS_PER_SEC );
				log_info(log_interno," Tiempo de uso de CPU : %lf segundos \n", hilo->enExec/CLOCKS_PER_SEC);
				log_info(log_interno," Tiempo bloqueado     : %lf segundos \n", hilo->enLock/CLOCKS_PER_SEC);
				log_info(log_interno," Demora en entrar a Ready : %lf segundos \n", hilo->deNaR/CLOCKS_PER_SEC);
				if(hilo->enExec >0)
				log_info(log_interno," Porcentaje del Tiempo de Ejecución    : %lf segundos \n", div/CLOCKS_PER_SEC);


				}
		i++;
	}
	pthread_mutex_unlock(&multi);

		return;

}


