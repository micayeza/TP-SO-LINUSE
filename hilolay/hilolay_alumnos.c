#include "hilolay_alumnos.h"

int suse_create(int tid){
	char* mensajeTexto = "Create";
	int resultadoEnvio = enviarPaquete(socketServidor, TEXTO, CREATE_HILO, (void*) mensajeTexto, -1);
	return 0;
}

int suse_schedule_next(int tid){
	char* mensajeTexto = "Schedule next";
	int resultadoEnvio = enviarPaquete(socketServidor, TEXTO, SCHEDULE_NEXT, (void*) mensajeTexto, -1);
	return 0;
}

int suse_join(int tid){
	char* mensajeTexto = "Join";
	int resultadoEnvio = enviarPaquete(socketServidor, TEXTO, JOIN, (void*) mensajeTexto, -1);
	return 0;
}

int suse_close(int tid){
	char* mensajeTexto = "Close";
	int resultadoEnvio = enviarPaquete(socketServidor, TEXTO, CLOSE, (void*) mensajeTexto, -1);
	return 0;
}

int suse_wait(int tid){
	char* mensajeTexto = "Wait";
	int resultadoEnvio = enviarPaquete(socketServidor, TEXTO, WAIT, (void*) mensajeTexto, -1);
	return 0;
}

int suse_signal(int tid){
	char* mensajeTexto = "Signal";
	int resultadoEnvio = enviarPaquete(socketServidor, TEXTO, SIGNAL, (void*) mensajeTexto, -1);
	return 0;
}


static struct hilolay_operations hiloops = {
		.suse_create = &suse_create,
		.suse_schedule_next = &suse_schedule_next,
		.suse_join = &suse_join,
		.suse_close = &suse_close
};

void hilolay_init(void){
	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	socketServidor = crearClienteconectarseAServidor("127.0.0.1", 8086, log_interno);

	init_internal(&hiloops);
}
