#include "hilolay_alumnos.h"

int suse_create(int tid){
	//Envio tipo operacion
	TipoOperacion tipoOperacion = CREATE_HILO;
	int resEnvioTipoOperacion = enviarMensaje(socketServidor, ENTERO, &tipoOperacion, log_interno);
	//Envio id
	int resEnvioId = enviarMensaje(socketServidor, ENTERO, &tid, log_interno);

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
	socketServidor = crearSocketCliente("127.0.0.1", 8086, log_interno);

	init_internal(&hiloops);
}
