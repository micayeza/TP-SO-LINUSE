#include <hilolay/alumnos.h>
#include <stdio.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
//#include <conexion.h>
#include <commons/string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef enum {
	VACIO,
	INIT,
	//Para SUSE
    CREATE_HILO,
	SCHEDULE_NEXT,
	WAIT,
	SIGNAL,
	JOIN,
	CLOSE,
} TipoOperacion;

t_log* log_interno;
int socketServidor;

///**********+REEMPLAZO CONEXION
int crearSocket(t_log* logger) {
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
    if (fileDescriptor == -1) {
        log_error(logger, "No se pudo crear el file descriptor.");
    }

    return fileDescriptor;
}

int crearSocketCliente(char *ipServidor, int puerto, t_log* logger) {

    int cliente;
    struct sockaddr_in direccionServidor;

    direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
    direccionServidor.sin_addr.s_addr = inet_addr(ipServidor);
    direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
    memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura

    cliente = crearSocket(logger); //Creamos socket
    int valorConnect = connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

    if(valorConnect == -1)  {
        log_error(logger, "No se pudo establecer conexión entre el socket y el servidor.");
        return -1;
    }
    else {
//        log_info(logger, "Se estableció correctamente la conexión con el servidor a través del socket %i.", cliente);
        return cliente;
    }
}

int recibirEntero(int fdOrigen, t_log* logger){
	int enteroRecibido;
	int resEntero = recv(fdOrigen, &enteroRecibido, sizeof(int), MSG_WAITALL);
	if(resEntero == -1){
		log_error(logger, "Hubo un error al recibir TipoDato de %i", fdOrigen);
		return -1;
	}
	return enteroRecibido;
}

int pesoString(char *string) {
    return string == NULL ? 0 : sizeof(char) * (strlen(string) + 1);
}

int enviarTexto(int fdDestinatario, char* textoEnviar,  t_log* logger){
	int tamanio = pesoString(textoEnviar);
	int resTamanio = send(fdDestinatario, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == -1){
		log_error(logger, "Hubo un error al enviar Tamanio a %i", fdDestinatario);
		return -1;
	}
	if(tamanio != 0){
	int resTexto = send(fdDestinatario, (void*)textoEnviar, tamanio, MSG_WAITALL);
	if(resTexto == -1){
		log_error(logger, "Hubo un error al enviar el Texto a %i", fdDestinatario);
		return -1;
	}
	return resTexto;
	}
	return 0;
}

int enviarEntero(int fdDestinatario, int enteroEnviar,  t_log* logger){
	int resEntero =  send(fdDestinatario, &enteroEnviar, sizeof(int), MSG_WAITALL);
	if(resEntero == -1){
		log_error(logger, "Hubo un error al enviar el Entero a %i", fdDestinatario);
		return -1;
	}
	return resEntero;
}

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

//	char* configPath = string_new();
//	string_append(&configPath, "../../configs/SUSE.config");
//	t_config* ruta = config_create(configPath);

//	char* ip 	 = config_get_string_value(ruta, "IP");
//	int   puerto = config_get_int_value(ruta, "LISTEN_PORT");


	log_interno = log_create("log_interno.txt", "LOG-INT", true, LOG_LEVEL_INFO);
	socketServidor = crearSocketCliente("192.168.3.12", 5003, log_interno); //Está hardcodeado - Habría que obtener IP y puerto del archivo de configuración?

	//Enviar mensaje con un INIT. No recibo respuesta.
	enviarEntero(socketServidor, INIT,  log_interno);

	init_internal(&hiloops);

//	config_destroy(ruta);
}
