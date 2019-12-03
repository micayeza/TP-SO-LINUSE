#include "SUSE.h"

int inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	config_suse = malloc(sizeof(t_configSUSE));
	config_ruta = config_create(configPath);

	if (config_ruta != NULL){

		config_suse->puerto 	    = config_get_int_value(config_ruta, "LISTEN_PORT");
		config_suse->metricas	 	= config_get_int_value(config_ruta, "METRICS_TIMER");
		config_suse->multiprog		= config_get_int_value(config_ruta, "MAX_MULTIPROG");
		config_suse->semId		 	= config_get_array_value(config_ruta, "SEM_IDS");
		config_suse->semInit	 	= config_get_array_value(config_ruta, "SEM_INIT");
		config_suse->semMax		 	= config_get_array_value(config_ruta, "SEM_MAX");
		config_suse->alpha 			= config_get_double_value(config_ruta, "ALPHA_SJF");


	}

	if(config_suse == NULL){
		log_error(log_interno, "ERROR No se pudo levantar el archivo de configuración de SUSE \n");
		return -1;
	}

	config_destroy(config_ruta);


	tabla_programas = list_create();
	tabla_new       = list_create();
	tabla_lock	    = list_create();
	tabla_exit      = list_create();


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

					list_add(tabla_new, programa);

					pthread_t hiloPrograma;
					pthread_create(&hiloPrograma, NULL, (void*)&atenderPrograma, (void*) cliente);


				}
		}


}


int main() {
	inicializacion();

	socket_escucha = crearSocketEscucha(config_suse->puerto, log_interno);

	aceptarClientes(); //Planificador de largo plazo, fifo


	finalizacion();
}
