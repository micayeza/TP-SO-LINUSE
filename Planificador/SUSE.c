#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
	proximo_pcb_id = 0;
	string_append(&configPath, "/home/utnso/workspace/tp-2019-2c-capitulo-2/configs/SUSE.config");

	log_resultados = log_create("log_resultados.txt", "LOG-RES", false, LOG_LEVEL_INFO);
	log_info(log_resultados, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	log_info(log_interno, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

	PCBs = dictionary_create();
	colaNew = queue_create();
	pthread_mutex_init(&mutexColaEjecucion, NULL);

}

void finalizacion(){
	free(configPath);
	log_destroy(log_resultados);
	log_destroy(log_interno);
	pthread_mutex_destroy(&mutexColaEjecucion);
}

void aceptarClientes(){

	t_configSUSE* config = getConfigSUSE(configPath);
	int socket_escucha = crearSocketEscucha(config->listenPort, log_interno);

	int cliente = 0;
	while((cliente = aceptarCliente(socket_escucha, log_interno)) > 0){

		TipoOperacion operacionRecibida = (TipoOperacion) recibirEntero(cliente, log_interno);
		if(operacionRecibida == INIT){
			t_PCB* nuevoPCB = create_PCB(cliente);
			dictionary_put(PCBs,string_itoa(proximo_pcb_id),nuevoPCB);
			proximo_pcb_id++;

			pthread_t hiloPrograma;
			pthread_create(&hiloPrograma, NULL, (void*)&atenderPrograma, (void*) nuevoPCB);
			//No se hace el join porque sino esperaría hasta que termine este para aceptar a otro
		}else{
			//Tirar mensaje de operacion erronea por log.
		}

	}

}



int main(void) {
	inicializacion();
	aceptarClientes();

	//finalizacion();
}
