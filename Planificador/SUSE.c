#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);

	PCBs = dictionary_create();
	colaNew = queue_create();
	pthread_mutex_init(&mutexColaNew, NULL);

}

void finalizacion(){
	free(configPath);
	log_destroy(log_interno);
	pthread_mutex_destroy(&mutexColaNew);
}

void aceptarClientes(){

	t_configSUSE* config = getConfigSUSE(configPath);
	int socket_escucha = crearSocketEscucha(config->listenPort, log_interno);

	int cliente = 0;
	while((cliente = aceptarCliente(socket_escucha, log_interno)) > 0){

		TipoOperacion operacionRecibida = (TipoOperacion) recibirEntero(cliente, log_interno);
		if(operacionRecibida == INIT) {

			t_PCB* nuevoPCB = create_PCB(cliente);

			dictionary_put(PCBs, cliente, nuevoPCB); //Utilizo el número de cliente como ID del PCB.

			pthread_t hiloPrograma;
			pthread_create(&hiloPrograma, NULL, (void*)&atenderPrograma, (void*) nuevoPCB);
			//No se hace el join porque sino esperaría hasta que termine este para aceptar a otro
		} else {
			log_info(log_interno, "Operación incorrecta - Se debe invocar a la función 'hilolay_init' primero");
		}

	}

}



int main(void) {
	inicializacion();
	aceptarClientes();

	//finalizacion();
}
