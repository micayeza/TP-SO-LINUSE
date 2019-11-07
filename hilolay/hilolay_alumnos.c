#include "hilolay_alumnos.h"

int suse_create(int tid){
	//Prueba recepcion y envio ---------------
	int enteroEnviar = 12;
	int resEntero = enviarEntero(socketServidor, enteroEnviar,  log_interno);
	printf("Enviar entero --> resultado: %i \n", resEntero);
	char* textoRecibido = recibirTexto(socketServidor, log_interno);
	printf("Recibir texto--> contenido: %s \n", textoRecibido);
	//----------------------------

	return 0;
}

int suse_schedule_next(){

}

int suse_join(int tid){

}

int suse_close(int tid){

}

int suse_wait(int tid, char *sem_name){

}

int suse_signal(int tid, char *sem_name){

}


static struct hilolay_operations hiloops = {
		.suse_create = &suse_create,
		.suse_schedule_next = &suse_schedule_next,
		.suse_join = &suse_join,
		.suse_close = &suse_close
};

void hilolay_init(void){
	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	socketServidor = crearSocketCliente("127.0.0.1", 5003, log_interno);

	init_internal(&hiloops);
}
