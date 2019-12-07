#include "SUSE.h"

void inicializarSemaforos(){
	sem_values  = malloc(sizeof(int)*config_suse->cantSem);
	sem_blocked = malloc(sizeof(t_queue*)*config_suse->cantSem);
	int i;
	for(i=0; i<config_suse->cantSem; i++){
		sem_values[i] = config_suse->semInit[i];
		sem_blocked[i] = queue_create();
	}
}



int inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	log_interno = log_create("log_interno.txt", "LOG-INT", true, LOG_LEVEL_INFO);
	config_suse = malloc(sizeof(t_configSUSE));
	config_ruta = config_create(configPath);

	if (config_ruta != NULL){

		config_suse->puerto 	    = config_get_int_value(config_ruta, "LISTEN_PORT");
		config_suse->metricas	 	= config_get_int_value(config_ruta, "METRICS_TIMER");
		config_suse->multiprog		= config_get_int_value(config_ruta, "MAX_MULTIPROG");
		config_suse->semId		 	= config_get_array_value(config_ruta, "SEM_IDS");
		char** semInitAux		 	= config_get_array_value(config_ruta, "SEM_INIT");
		char** semMaxAux	     	= config_get_array_value(config_ruta, "SEM_MAX");
		config_suse->alpha 			= config_get_double_value(config_ruta, "ALPHA_SJF");

		int len=0;
		while(config_suse->semId[len]!=NULL){
			len++;
		}
		config_suse->cantSem = len;
		config_suse->semInit = malloc(sizeof(int)*len);
		config_suse->semMax  = malloc(sizeof(int)*len);
		for(int i=0; i<len; i++){
			config_suse->semInit[i] = atoi(semInitAux[i]);
			free(semInitAux[i]);
			config_suse->semMax[i] = atoi(semMaxAux[i]);
			free(semMaxAux[i]);
		}
		free(semInitAux);
		free(semMaxAux);
			config_destroy(config_ruta);
	}

	if(config_suse == NULL){
		log_error(log_interno, "ERROR No se pudo levantar el archivo de configuración de SUSE \n");
		return -1;
	}




	tabla_programas = list_create();
	tabla_new       = list_create();
	tabla_lock	    = list_create();
	tabla_exit      = list_create();

	pthread_mutex_init(&sem_new ,NULL);
	pthread_mutex_init(&sem_lock ,NULL);
	pthread_mutex_init(&sem_exit,NULL);
	pthread_mutex_init(&multi ,NULL);
	pthread_mutex_init(&wt ,NULL);
	pthread_mutex_init(&sl ,NULL);


    return 0;
//	PCBs = dictionary_create();
//	colaNew = queue_create();
//	pthread_mutex_init(&mutexColaNew, NULL);

}

void finalizacion(){
	free(configPath);
	log_destroy(log_interno);
	pthread_mutex_destroy(&mutexColaNew);
}

void aceptarClientes(){

	int cliente = 0;
	while((cliente = aceptarCliente(socket_escucha, log_interno)) > 0){
		int operacionRecibida = recibirInt(cliente);
				if(operacionRecibida == INIT) {

					t_programa* programa = malloc(sizeof(t_programa));
					programa->programa  = cliente;
					programa->hijos     = list_create();
					int operacion = recibirInt(cliente);
					programa->id  = recibirInt(cliente);
					programa->estado    = ACTIVO;
					programa->init      = clock();
					programa->fin = 0;

					list_add(tabla_programas, programa);

					pthread_t hiloPrograma;
					pthread_create(&hiloPrograma, NULL, (void*)&atenderPrograma, (void*)programa);


				}
		}


}
void atenderMetricas(){

	while(1){
		sleep(config_suse->metricas);
		printefearMetricas();

	}


}


int main() {
	inicializacion();

	inicializarSemaforos();

	socket_escucha = crearSocketEscucha(config_suse->puerto, log_interno);

	aceptarClientes(); //Planificador de largo plazo, fifo

	pthread_t hiloMetricas;
	pthread_create(&hiloMetricas, NULL, (void*)&atenderMetricas, NULL);




	finalizacion();
}
