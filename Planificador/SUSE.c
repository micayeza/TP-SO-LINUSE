#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
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

/*void aceptarClientes(){

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

}*/

void atenderPedidos(){

	t_configSUSE* config = getConfigSUSE(configPath);

	GestorConexiones* conexionServidor = inicializarConexion();

	crearInicializarServidor(config->listenPort, conexionServidor, log_interno);


	while(1){
		Mensaje* mensaje = recibirConexionPaquete(conexionServidor, log_interno);
		switch((int) mensaje->tipoMensaje)  {
			case VACIO:
				log_error(log_interno, "Error en recepción de mensaje o conexion");
				break;
			case INIT:
				printf("Resultado: Se conectó el cliente %s \n", string_itoa((char*) mensaje->fd_remitente));
				break;
			case CREATE_HILO:
				printf("Resultado: %s \n", (char*) mensaje->contenido);
				break;
			case JOIN:
				printf("Resultado: %s \n", (char*) mensaje->contenido);
				break;
		}
	}

}


int main(void) {
	inicializacion();

	atenderPedidos();

	//finalizacion();
}
