#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	colaNew = queue_create();
	pthread_mutex_init(&mutexColaEjecucion, NULL);

}

void finalizacion(){
	free(configPath);
	pthread_mutex_destroy(&mutexColaEjecucion);
}

void aceptarClientes(){

	configSUSE* config = getConfigSUSE(configPath);
	int socket_escucha = crearSocketEscucha(config->listenPort);

	int cliente = 0;
	while((cliente = aceptarCliente(socket_escucha)) > 0){

		pthread_t hilo;
		pthread_create(&hilo, NULL, (void*)encolarEnNew, (void*)cliente);
		//No se hace el join porque sino esperaría hasta que termine este para aceptar a otro

	}

}

int main(void) {
	inicializacion();

	pthread_create(&mainThread, NULL, (void*)aceptarClientes, NULL);
	pthread_join(mainThread, NULL);

	finalizacion();
}
