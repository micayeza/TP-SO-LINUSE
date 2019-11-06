#include "conexion.h"

//FUNCIONES A USAR:

 /* Creamos un Socket Servidor de escucha.
 * Retorna ERROR si el puerto ya estaba en uso, si el bind no funcionó
 *  o si el Listen no funcionó.
 */
int crearSocketEscucha (int puerto, t_log* logger) {

    int socketDeEscucha = crearSocketServidor(puerto, logger);

    //Escuchar conexiones
    escucharSocketsEn(socketDeEscucha, logger);

    return socketDeEscucha;
}

/**
 ***Crea socket para conexion con servidor y se conecta***
 *  pasados por parametro.
 * Retorna ERROR si no se pudo conectar con el servidor.
 **/
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger) {

    int cliente;
    struct sockaddr_in direccionServidor;

    direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
    direccionServidor.sin_addr.s_addr = inet_addr(ipServidor);
    direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
    memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura

    cliente = crearSocket(logger); //Creamos socket
    int valorConnect = connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

    if(valorConnect == ERROR)  {
        log_error(logger, "No se pudo establecer conexión entre el socket y el servidor.");
        return ERROR;
    }
    else {
//        log_info(logger, "Se estableció correctamente la conexión con el servidor a través del socket %i.", cliente);
        return cliente;
    }
}

/**
 ***Acepta la conexion con un cliente que se está queriendo conectar***
 * Retorna el fd del cliente.
 * Retorna ERROR si no se pudo conectar con el servidor.
 **/
int aceptarCliente(int fd_servidor, t_log* logger){

	struct sockaddr_in unCliente;
	memset(&unCliente, 0, sizeof(unCliente));
	unsigned int addres_size = sizeof(unCliente);

	int fd_cliente = accept(fd_servidor, (struct sockaddr*) &unCliente, &addres_size);
	if(fd_cliente == ERROR)  {
		log_error(logger, "El servidor no pudo aceptar la conexión entrante.\n");
		puts("El servidor no pudo aceptar la conexión entrante.\n");
	} else	{
		log_error(logger, "Servidor conectado con cliente %i.\n", fd_cliente);
	}

	return fd_cliente;

}

int enviarMensaje(int fdDestinatario, TipoDato tipoDato, void* mensaje,  t_log* logger){
	int resMensaje = 0;
	int resTipoDato = send(fdDestinatario, tipoDato, sizeof(int), MSG_WAITALL);
	if(resTipoDato == ERROR){
		log_error(logger, "Hubo un error al enviar TipoDato a %i", fdDestinatario);
		return ERROR;
	}
	switch(tipoDato)  {
	case ENTERO:
		resMensaje = send(fdDestinatario, mensaje, sizeof(int), MSG_WAITALL);
		break;
	case TEXTO:
		int tamanio = pesoString((char*) mensaje);
		int resTamanio = send(fdDestinatario, tamanio, sizeof(int), MSG_WAITALL);
		if(resTamanio == ERROR){
			log_error(logger, "Hubo un error al enviar Tamanio a %i", fdDestinatario);
			return ERROR;
		}
		resMensaje = send(fdDestinatario, mensaje, tamanio, MSG_WAITALL);
		break;
	}
	if(resMensaje == ERROR){
		log_error(logger, "Hubo un error al enviar el Mensaje a %i", fdDestinatario);
	}
	return resMensaje;
}

void* recibirMensaje(int fdOrigen, t_log* logger){
	int resMensaje = 0;
	void* mensaje;
	TipoDato tipoDato;
	int resTipoDato = recv(fdOrigen, tipoDato, sizeof(int), MSG_WAITALL);
	if(resTipoDato == ERROR){
		log_error(logger, "Hubo un error al recibir TipoDato de %i", fdOrigen);
		return ERROR;
	}
	switch(tipoDato)  {
	case ENTERO:
		mensaje = malloc(sizeof(int));
		resMensaje = recv(fdOrigen, mensaje, sizeof(int), MSG_WAITALL);
		break;
	case TEXTO:
		int tamanio;
		int resTamanio = recv(fdOrigen, tamanio, sizeof(int), MSG_WAITALL);
		if(resTamanio == ERROR){
			log_error(logger, "Hubo un error al recibir Tamanio de %i", fdOrigen);
			return ERROR;
		}
		mensaje = malloc(tamanio);
		resMensaje = recv(fdOrigen, mensaje, tamanio, MSG_WAITALL);
		break;
	}
	return mensaje;
}

/**
 * ENVIO PAQUETE
 * Envio paquete 'mensaje' al destinatario fdDestinatario.
 * El parametro mensaje tiene que venir sin serializar (se serializa dependiendo el tipoDato.
 */
int enviarPaquete(int fdDestinatario, TipoDato tipoDato, TipoMensaje tipoMensaje, void* mensaje, int pid)  {

	void* mensajeSerializado = malloc(1);
	int pesoMensaje = serializarMensaje(mensaje, tipoDato, mensajeSerializado);
	Header header = armarHeader(fdDestinatario, pesoMensaje, tipoDato, tipoMensaje, pid);
    void* headerSerializado = serializarHeader(header);
    void* paqueteSerializado = malloc(1);
    int pesoPaquete = empaquetar(headerSerializado, mensajeSerializado, pesoMensaje, tipoDato, paqueteSerializado);
    int res = send(fdDestinatario, paqueteSerializado, pesoPaquete, MSG_WAITALL);
    //free(headerSerializado); //Me da error al hacer free de un void*
    //free(paqueteSerializado);

    return res;
}

/**
 * RECIBIR PAQUETE
 * Espera conexiones y espera paquetes de los clientes ya conextados.
 * Retorna NULL si se recibió una conexión, retorna distinto de NULL
 * si se recibió un paquete.
 */
Mensaje* recibirPaquete(int fdCliente, t_log* logger){
	Mensaje* mensaje = inicializarMensaje();
	int bytesRecibidos;
	Header headerSerializado;
	bytesRecibidos = recv(fdCliente, &headerSerializado, sizeof(Header), MSG_DONTWAIT);
	switch(bytesRecibidos)  {
		// Hubo un error al recibir los datos:
		case -1:
			log_warning(logger, "Hubo un error al recibir el header proveniente del socket %i", fdCliente);
			break;
		// Se desconectó:
		case 0:
			break;

		//Obtengo Header:
		default: ;
			Header header = deserializarHeader(&headerSerializado);
			header.fdRemitente = fdCliente;
			int pesoMensaje = header.tamanioMensaje * sizeof(char);
			void* mensajeBruto = (void*) malloc(pesoMensaje);
			//Obtengo mensaje:
			bytesRecibidos = recv(fdCliente, mensajeBruto, pesoMensaje, MSG_DONTWAIT);
			if(bytesRecibidos == -1 || bytesRecibidos < pesoMensaje)
				log_warning(logger, "Hubo un error al recibir el mensaje proveniente del socket %i", fdCliente);
			else if(bytesRecibidos == 0){
				//Cliente desconectado
			}else{

				mensaje->fd_remitente = fdCliente;
				mensaje->size = pesoMensaje;
				mensaje->tipoDato = header.tipoDato;
				mensaje->tipoMensaje = header.tipoMensaje;
				mensaje->contenido = deserializarMensaje(mensajeBruto, pesoMensaje, header.tipoDato);
			}

	}
	return mensaje;
}


//-------------------------------------

//Crear socket servidor
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

    if (puertoYaAsociado == ERROR) {
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

//Crear socket
int crearSocket(t_log* logger) {
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
    if (fileDescriptor == ERROR) {
        log_error(logger, "No se pudo crear el file descriptor.");
    }

    return fileDescriptor;
}

int serializarMensaje(void* mensaje, TipoDato tipoDato, void* mensajeSerializado){
	if(tipoDato == ENTERO){
		int dato = (int) mensaje;
		int tamanio = sizeof(int);
		realloc(mensajeSerializado,tamanio);
		void* puntero = mensajeSerializado;
		memcpy(puntero, dato, tamanio);
		return tamanio;
	}
	if(tipoDato == TEXTO){
		int puntero = 0;
		int tamanio = pesoString((char*)mensaje);
		realloc(mensajeSerializado,tamanio); //Uso REALOC para que siga manteniendo la dir de memoria.
		memcpy(mensajeSerializado + puntero, mensaje, tamanio);

		return tamanio;
	}
	mensajeSerializado = NULL;
	return -1;
}

void* deserializarMensaje(void* mensajeSerializado, int tamanio, TipoDato tipoDato)	{
    if(tipoDato == ENTERO){
    	int* mensaje = malloc(tamanio);
    	mensaje = memcpy(mensajeSerializado, mensaje, tamanio);
    	return (void*) mensaje;
    }

    if(tipoDato == TEXTO){
    	char* mensaje = malloc(tamanio);
    	mensaje = memcpy(mensaje,mensajeSerializado,tamanio);
    	return (void*) mensaje;
    }
}

/**
 * Serializa una estructura Header.
 */
void* serializarHeader(Header header)    {
    void* headerSerializado = malloc(sizeof(Header));
    void* puntero = headerSerializado;
    int tamanioSize = sizeof(typeof(header.tamanioMensaje));
    int pesoTipoRemitente = sizeof(typeof(header.fdRemitente));
    int pesoTipoDato = sizeof(typeof(header.tipoDato));
    int pesoTipoMensaje = sizeof(typeof(header.tipoMensaje));

    memcpy(puntero, &(header.tamanioMensaje), tamanioSize);
    puntero += tamanioSize;
    memcpy(puntero, &(header.fdRemitente), pesoTipoRemitente);
    puntero += pesoTipoRemitente;
    memcpy(puntero, &(header.tipoDato), pesoTipoDato);
    puntero += pesoTipoDato;
    memcpy(puntero, &(header.tipoMensaje), pesoTipoMensaje);
    puntero += pesoTipoMensaje;
    memcpy(puntero, &(header.pid), sizeof(int));

    return headerSerializado;
}

Header deserializarHeader(void* headerSerializado)	{
    Header header;
    void* puntero = headerSerializado;
    int tamanioSize = sizeof(typeof(header.tamanioMensaje));
    int pesoTipoRemitente = sizeof(typeof(header.fdRemitente));
    int pesoTipoDato = sizeof(typeof(header.tipoDato));
    int pesoTipoMensaje = sizeof(typeof(header.tipoMensaje));

    memcpy(&(header.tamanioMensaje), puntero, tamanioSize);
    puntero += tamanioSize;

    memcpy(&(header.fdRemitente), puntero, pesoTipoRemitente);
    puntero += pesoTipoRemitente;

    memcpy(&(header.tipoDato), puntero, pesoTipoDato);
    puntero += pesoTipoDato;

    memcpy(&(header.tipoMensaje), puntero, pesoTipoMensaje);
    puntero += pesoTipoMensaje;

    memcpy(&(header.pid), puntero, sizeof(int));

    return header;
}

/**
 * Arma una estructura header a partir de la informacion pasada por parametro.
 */
Header armarHeader(int fdDestinatario, int tamanioDelMensaje, TipoDato tipoDato, TipoMensaje tipoMensaje, int pid)    {
    Header header = {.tamanioMensaje = tamanioDelMensaje, .fdRemitente = fdDestinatario, .tipoDato = tipoDato, .tipoMensaje = tipoMensaje, .pid = pid};
    return header;
}

/**
 * Empaquetar
 */
int empaquetar(void* headerSerializado, void* mensajeSerializado, int pesoMensaje, TipoDato tipoDato, void* paqueteSerializado)   {
    int pesoPaquete = sizeof(Header) + pesoMensaje;
	realloc(paqueteSerializado,pesoPaquete);
    void* puntero = paqueteSerializado;
    memcpy(puntero, headerSerializado, sizeof(Header));
    puntero += sizeof(Header);
    memcpy(puntero, mensajeSerializado, pesoMensaje);
    return pesoPaquete;
}

Mensaje* inicializarMensaje(){
	Mensaje* mensaje = malloc(sizeof(Mensaje));
	mensaje->fd_remitente = -1;
	mensaje->size = 0;
	mensaje->tipoDato = INDEFINIDO;
	mensaje->tipoMensaje = VACIO;
	mensaje->contenido = NULL;

	return mensaje;
}

void freeMensaje(Mensaje* mensaje){
	free(mensaje->contenido);
	free(mensaje);
}

void freeCharArray(char** charArray){

	int i = 0;
	while(charArray[i] != NULL){
		free(charArray[i]);
		i++;
	}
	free(charArray);
}

int pesoString(char *string) {
    return string == NULL ? 0 : sizeof(char) * (strlen(string) + 1);
}
