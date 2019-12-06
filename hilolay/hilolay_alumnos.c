#include "hilolay_alumnos.h"

int suse_create(int tid){

	//Enviar mensaje con CREATE. Luego enviar mensaje con tid. No esperar respuesta
	int resOperacion = enviarEntero(socketServidor, CREATE_HILO,  log_interno);
	int resTid = enviarEntero(socketServidor, tid,  log_interno);

	return 0;
}

int suse_schedule_next(){
	//Enviar mensaje con SCHEDULE_NEXT. Esperar respuesta de un int.
	int resOperacion = enviarEntero(socketServidor, SCHEDULE_NEXT,  log_interno);
	return recibirEntero(socketServidor, log_interno);
}

int suse_join(int tid){
	int resOperacion = enviarEntero(socketServidor, JOIN,  log_interno);
	int resTid = enviarEntero(socketServidor, tid,  log_interno);

	return 0;
}

int suse_close(int tid){
	int resOperacion = enviarEntero(socketServidor, CLOSE,  log_interno);
	int resTid = enviarEntero(socketServidor, tid,  log_interno);

	return 0;
}

int suse_wait(int tid, char *sem_name){
	int resOperacion = enviarEntero(socketServidor, WAIT,  log_interno);
	int resTid = enviarEntero(socketServidor, tid,  log_interno);
	int resTexto = enviarTexto(socketServidor, sem_name, log_interno);

	return recibirEntero(socketServidor, log_interno);
}

int suse_signal(int tid, char *sem_name){
	int resOperacion = enviarEntero(socketServidor, SIGNAL,  log_interno);
	int resTid = enviarEntero(socketServidor, tid,  log_interno);
	int resTexto = enviarTexto(socketServidor, sem_name, log_interno);

	return 0;
}


static struct hilolay_operations hiloops = {
		.suse_create = &suse_create,
		.suse_schedule_next = &suse_schedule_next,
		.suse_join = &suse_join,
		.suse_close = &suse_close,
		.suse_wait = &suse_wait,
		.suse_signal = &suse_signal
};

void hilolay_init(void){

	char* configPath = string_new();
	string_append(&configPath, "/home/utnso/workspace/tp-2019-2c-capitulo-2/configs/SUSE.config");
	t_config* ruta = config_create(configPath);

	char* ip 	 = config_get_string_value(ruta, "IP");
	int   puerto = config_get_int_value(ruta, "LISTEN_PORT");


	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	socketServidor = crearSocketCliente(ip, puerto, log_interno); //Está hardcodeado - Habría que obtener IP y puerto del archivo de configuración?

	//Enviar mensaje con un INIT. No recibo respuesta.
	enviarEntero(socketServidor, INIT,  log_interno);

	init_internal(&hiloops);

	config_destroy(ruta);
}
