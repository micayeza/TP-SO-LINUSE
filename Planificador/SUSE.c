#include "SUSE.h"

void new_destroy(t_new *self) {
	free(self);
}

void block_destroy(t_block *self) {

	 free(self);
}
void hilos_destroy(t_hilo *self) {
	free(self);
}
void ready_destroy(t_new *self) {
	free(self);
}
void prog_destroy(t_programa *self) {
	list_destroy_and_destroy_elements(self->hijos, (void*) hilos_destroy);
	if(list_size(self->tablaReady)){
		list_destroy_and_destroy_elements(self->tablaReady, (void*)ready_destroy);}
	free(self);
}
void handler(){

//	pthread_mutex_destroy(&sem_new);
//	pthread_mutex_destroy(&sem_lock);
//	pthread_mutex_destroy(&sem_exit);
//	pthread_mutex_destroy(&wt);
//	pthread_mutex_destroy(&multi);
////
//	if(list_size(tabla_new)>0){
//		list_destroy_and_destroy_elements(tabla_new, (void*)new_destroy);
//	}else{
//		list_destroy(tabla_new);
//	}
//	if(list_size(tabla_exit)>0){
//		list_destroy_and_destroy_elements(tabla_exit, (void*)new_destroy);
//	} else{
//		list_destroy(tabla_exit);
//	}
//	if(list_size(tabla_lock)>0){
//		list_destroy_and_destroy_elements(tabla_lock, (void*)block_destroy);
//	}else{
//		list_destroy(tabla_lock);
//	}
//	if(list_size(tabla_programas)>0){
//		list_destroy_and_destroy_elements(tabla_programas, (void*) prog_destroy);
//	}else{
//		list_destroy(tabla_programas);
//	}
//******************esto dejar comentado
////	queue_destroy_and_destroy_elements(sem_blocked, (void*) block_destroy);
////	int* sem_values;
//	for(int i=0; i<config_suse->cantSem; i++){
//			queue_destroy_and_destroy_elements(sem_blocked[i],(void*)block_destroy);
//		}
//	free(sem_values);
	exit(0);

}


int crearSocket(t_log* logger) {
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
    if (fileDescriptor == -1) {
        log_error(logger, "No se pudo crear el file descriptor.");
    }

    return fileDescriptor;
}

void escucharSocketsEn(int fd_socket, t_log* logger){

    int valorListen;
    valorListen = listen(fd_socket, SOMAXCONN);/*Le podríamos poner al listen
				SOMAXCONN como segundo parámetro, y significaría el máximo tamaño de la cola*/
    if(valorListen == -1) {
        log_error(logger, "El servidor no pudo recibir escuchar conexiones de clientes.");
    } else	{
        log_info(logger, "El servidor está escuchando conexiones a través del socket %i.", fd_socket);
    }
}



int crearSocketServidor(int puerto, t_log* logger)	{
    struct sockaddr_in miDireccionServidor;
    int socketDeEscucha = crearSocket(logger);

    miDireccionServidor.sin_family = AF_INET;			//Protocolo de conexion
    miDireccionServidor.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0 y significa que usa la IP actual de la maquina
    miDireccionServidor.sin_port = htons(puerto);		//Puerto en el que escucha
    memset(&(miDireccionServidor.sin_zero), '\0', 8);	//Pone 0 al campo de la estructura "miDireccionServidor"

    //Veamos si el puerto está en uso
    int puertoEnUso = 1;
    int puertoYaAsociado = setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &puertoEnUso, sizeof(puertoEnUso));

    if (puertoYaAsociado == -1) {
        log_error(logger, "El puerto asignado ya está siendo utilizado.");
    }
    //Turno del bind
    int activado = 1;
    //Para evitar que falle el bind, al querer usar un mismo puerto
    setsockopt(socketDeEscucha,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

    int valorBind = bind(socketDeEscucha,(void*) &miDireccionServidor, sizeof(miDireccionServidor));

    if ( valorBind !=0) {
        log_error(logger, "El bind no funcionó, el socket no se pudo asociar al puerto");
        return 1;
    }

    log_info(logger, "Servidor levantado en el puerto %i", puerto);

    return socketDeEscucha;
}

int crearSocketEscucha (int puerto, t_log* logger) {

    int socketDeEscucha = crearSocketServidor(puerto, logger);

    //Escuchar conexiones
    escucharSocketsEn(socketDeEscucha, logger);

    return socketDeEscucha;
}

int aceptarCliente(int fd_servidor, t_log* logger){

	struct sockaddr_in unCliente;
	memset(&unCliente, 0, sizeof(unCliente));
	unsigned int addres_size = sizeof(unCliente);

	int fd_cliente = accept(fd_servidor, (struct sockaddr*) &unCliente, &addres_size);
	if(fd_cliente == -1)  {
		log_error(logger, "El servidor no pudo aceptar la conexión entrante.\n");
		puts("El servidor no pudo aceptar la conexión entrante.\n");
	} else	{
		log_info(logger, "Servidor conectado con cliente %i.\n", fd_cliente);
	}

	return fd_cliente;

}


int enviarInt(int destinatario, int loQueEnvio){

	 void* paquete = malloc(sizeof(int));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(int));
	 int res = send(destinatario, paquete, sizeof(int), MSG_WAITALL);
	 free(paquete);
	 return res;
}

char* recibirTexto(int fdOrigen, t_log* logger){
	int tamanio;
	int resTamanio = recv(fdOrigen, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == -1){
		log_error(logger, "Hubo un error al recibir Tamanio de Texto de %i", fdOrigen);
		return NULL;
	}
	if(tamanio == 0){
		return NULL;
	}
	char* textoRecibido = malloc(tamanio);
	int resTexto = recv(fdOrigen, textoRecibido, tamanio, MSG_WAITALL);
	if(resTexto == -1){
		log_error(logger, "Hubo un error al recibir Texto de %i", fdOrigen);
		return NULL;
	}
	textoRecibido[tamanio-1] = '\0';
	return textoRecibido;
}


int recibirInt(int destinatario){
	int algo;
	if(recv(destinatario, &algo, sizeof(int), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}


void inicializarSemaforos(){
	sem_values  = malloc(sizeof(int)*config_suse->cantSem);
	sem_blocked = malloc(sizeof(t_queue*)*config_suse->cantSem);
	int i;
	for(i=0; i<config_suse->cantSem; i++){
		sem_values[i] = config_suse->semInit[i];
		sem_blocked[i] = queue_create();
	}
}



int inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");

	log_interno = log_create("log_interno.txt", "LOG-INT", true, LOG_LEVEL_INFO);
	config_suse = malloc(sizeof(t_configSUSE));
	config_ruta = config_create(configPath);

	if (config_ruta != NULL){

		config_suse->puerto 	    = config_get_int_value(config_ruta, "LISTEN_PORT");
		config_suse->metricas	 	= config_get_int_value(config_ruta, "METRICS_TIMER");
		config_suse->multiprog		= config_get_int_value(config_ruta, "MAX_MULTIPROG");
		config_suse->semId		 	= config_get_array_value(config_ruta, "SEM_IDS");
		char** semInitAux		 	= config_get_array_value(config_ruta, "SEM_INIT");
		char** semMaxAux	     	= config_get_array_value(config_ruta, "SEM_MAX");
		config_suse->alpha 			= config_get_double_value(config_ruta, "ALPHA_SJF");

		int len=0;
		while(config_suse->semId[len]!=NULL){
			len++;
		}
		config_suse->cantSem = len;
		config_suse->semInit = malloc(sizeof(int)*len);
		config_suse->semMax  = malloc(sizeof(int)*len);
		for(int i=0; i<len; i++){
			config_suse->semInit[i] = atoi(semInitAux[i]);
			free(semInitAux[i]);
			config_suse->semMax[i] = atoi(semMaxAux[i]);
			free(semMaxAux[i]);
		}
		free(semInitAux);
		free(semMaxAux);
			config_destroy(config_ruta);
	}

	if(config_suse == NULL){
		log_error(log_interno, "ERROR No se pudo levantar el archivo de configuración de SUSE \n");
		return -1;
	}




	tabla_programas = list_create();
	tabla_new       = list_create();
	tabla_lock	    = list_create();
	tabla_exit      = list_create();
	lista_sem_programas = list_create();

	pthread_mutex_init(&sem_new ,NULL);
	pthread_mutex_init(&sem_lock ,NULL);
	pthread_mutex_init(&sem_exit,NULL);
	pthread_mutex_init(&multi ,NULL);
	pthread_mutex_init(&wt ,NULL);
	pthread_mutex_init(&sl ,NULL);

	lista_semaforos = list_create();


    return 0;
//	PCBs = dictionary_create();
//	colaNew = queue_create();
//	pthread_mutex_init(&mutexColaNew, NULL);

}




void aceptarClientes(){

	int cliente = 0;
	while((cliente = aceptarCliente(socket_escucha, log_interno)) > 0){
		int operacionRecibida = recibirInt(cliente);
				if(operacionRecibida == INIT) {

					t_programa* programa = malloc(sizeof(t_programa));
					programa->programa  = cliente;
					programa->hijos     = list_create();
					programa->tablaReady= list_create();
					recibirInt(cliente);
					programa->id  = recibirInt(cliente);
					programa->estado    = ACTIVO;
					programa->init      = clock();
					programa->fin = 0;
					programa->cant_hilos = 0;

					list_add(tabla_programas, programa);
// AGREGO EL CONTADOR DE HILOS DEL PROGRAMA
					t_sem_contador* contador = malloc(sizeof(t_sem_contador));
					sem_t sem ;
					sem_init(&sem, 0,0);
					contador->cont = sem;
					contador->idProg = cliente;

					list_add(lista_semaforos, contador);
// AGREGO EL PROGRAMA A LOS MUTEX PORQUE ASDDFFASDA
					t_pth_programas* reg_prog = malloc(sizeof(t_pth_programas));
					pthread_mutex_t sem_prog;
					pthread_mutex_init(&sem_prog, NULL);
					reg_prog->mtx    = sem_prog;
					reg_prog->idProg = cliente;
					list_add(lista_sem_programas, reg_prog);

					pthread_t hiloPrograma;
					pthread_create(&hiloPrograma, NULL, (void*)&atenderPrograma, (void*)programa);


				}
		}


}
void atenderMetricas(){

	while(1){
		sleep(config_suse->metricas);
		printefearMetricas();

	}


}




int main() {
	signal(SIGINT, &handler);
	inicializacion();

	inicializarSemaforos();

	socket_escucha = crearSocketEscucha(config_suse->puerto, log_interno);

	aceptarClientes(); //Planificador de largo plazo, fifo

	pthread_t hiloMetricas;
	pthread_create(&hiloMetricas, NULL, (void*)&atenderMetricas, NULL);


}
