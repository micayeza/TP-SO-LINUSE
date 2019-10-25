#include "hilolay_alumnos.h"

void suse_init(){
	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	int socketServidor = crearClienteconectarseAServidor("127.0.0.1", 8085, log_interno);

	char* mensajeTexto = "maldito";
	int resultadoEnvio = enviarPaquete(socketServidor, TEXTO, OTRO, (void*) mensajeTexto, -1);
}

int suse_create(int tid){
	printf("Entro en nuestro SUSE Create");
	return 0;
}


static struct hilolay_operations hiloops = {
		.suse_create = &suse_create,
		.suse_schedule_next = &suse_create,
		.suse_join = &suse_create,
		.suse_close = &suse_create
};

void hilolay_init(void){
	suse_init();
	init_internal(&hiloops);

}
