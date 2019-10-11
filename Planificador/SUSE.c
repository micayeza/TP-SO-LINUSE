#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	proximo_pcb_id = 0;
	PCBs = dictionary_create();
	colaNew = queue_create();
	pthread_mutex_init(&mutexColaEjecucion, NULL);

}

void finalizacion(){
	free(configPath);
	pthread_mutex_destroy(&mutexColaEjecucion);
}

void aceptarClientes(){

	//t_configSUSE* config = getConfigSUSE(configPath);
	//int socket_escucha = crearSocketEscucha(config->listenPort);

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


int main(void) {
	inicializacion();

	pthread_create(&mainThread, NULL, (void*)aceptarClientes, NULL);
	pthread_join(mainThread, NULL);

	finalizacion();
}
