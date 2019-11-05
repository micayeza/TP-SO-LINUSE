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
Mensaje* recibirPaquete(int fdConexion, t_log* logger){
	Mensaje* mensaje = inicializarMensaje();
	int bytesRecibidos;
	Header headerSerializado;
	bytesRecibidos = recv(fdConexion, &headerSerializado, sizeof(Header), MSG_DONTWAIT);
	switch(bytesRecibidos)  {
		// Hubo un error al recibir los datos:
		case -1:
			log_warning(logger, "Hubo un error al recibir el header proveniente del socket %i", fdConexion);
			break;
		// Se desconectó:
		case 0:
			break;

		//Obtengo Header:
		default: ;
			Header header = deserializarHeader(&headerSerializado);
			header.fdRemitente = fdConexion;
			int pesoMensaje = header.tamanioMensaje * sizeof(char);
			void* mensajeBruto = (void*) malloc(pesoMensaje);
			//Obtengo mensaje:
			bytesRecibidos = recv(fdConexion, mensajeBruto, pesoMensaje, MSG_DONTWAIT);
			if(bytesRecibidos == -1 || bytesRecibidos < pesoMensaje)
				log_warning(logger, "Hubo un error al recibir el mensaje proveniente del socket %i", fdConexion);
			else if(bytesRecibidos == 0){
				//Cliente desconectado
			}else{

				mensaje->fd_remitente = fdConexion;
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
