#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	colaNew = queue_create();

}

void finalizacion(){
	free(configPath);
}

void aceptarClientes(){

	configSUSE* config = getConfigSUSE(configPath);
	int socket_escucha = crearSocketEscucha(config->listenPort);

		int cliente = 0;

			while((cliente = aceptarCliente(socket_escucha)) > 0){

				pthread_t hilo;
				pthread_create(&hilo, NULL, (void*)encolarEnNew, (void*)cliente );
			}

}

int main(void) {

	inicializacion();
	configSUSE* config = getConfigSUSE(configPath);

	//Hago cosas con la config...

	freeConfig(config);
	finalizacion();
}
