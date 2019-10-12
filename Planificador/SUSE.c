#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	log_resultados = log_create("log_resultados.txt", "LOG-RES", false, LOG_LEVEL_INFO);
	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);

	proximo_pcb_id = 0;
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
	//while((cliente = aceptarCliente(socket_escucha)) > 0){
		t_PCB* nuevoPCB = create_PCB(cliente);
		dictionary_put(PCBs,string_itoa(proximo_pcb_id),nuevoPCB);
		proximo_pcb_id++;

		pthread_t hiloPrograma;
		pthread_create(&hiloPrograma, NULL, (void*)&atenderPrograma, (void*) nuevoPCB);
		//No se hace el join porque sino esperaría hasta que termine este para aceptar a otro

	//}

}

void probando_select(){

}


int main(void) {
	inicializacion();
	probando_select();
	pthread_create(&mainThread, NULL, (void*)aceptarClientes, NULL);
	pthread_join(mainThread, NULL);

	finalizacion();
}
