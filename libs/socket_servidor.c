#include "socket_servidor.h"

/**
 ***INICIALIZA CONEXION
 */
GestorConexiones* inicializarConexion() {
    GestorConexiones* nuevaConexion = (GestorConexiones*) malloc(sizeof(GestorConexiones));

    nuevaConexion->descriptorMaximo = -1;
    nuevaConexion->servidor = -1;
    nuevaConexion->conexiones = list_create();

    return nuevaConexion;
}

/**
 ***CREA SOCKET SERVIDOR E INICIALIZA SU CONEXION***.
 */
void crearInicializarServidor(int puerto, GestorConexiones* conexion, t_log* logger) {
    int fdServidor = crearSocketEscucha(puerto, logger);
    conexion->servidor = fdServidor;
    conexion->descriptorMaximo = getFdMaximo(conexion);
}

/**
 ***CREA CONEXION CLIENTE, CONECTA CON SERVIDOR Y LE ENVIA UN HANDSHAKE***
 **/
int crearClienteconectarseAServidor(char* ip, int puerto, t_log* logger)  {
    int* fdNuevoServidor = (int*) malloc(sizeof(int));
    *fdNuevoServidor = crearSocketCliente(ip, puerto, logger);

    return *fdNuevoServidor;
}

/**
 * ENVIO PAQUETE
 * Envio paquete 'mensaje' al destinatario fdDestinatario.
 * El parametro mensaje tiene que venir sin serializar.
 */
int enviarPaquete(int fdDestinatario, TipoDato tipoDato, TipoMensaje tipoMensaje, void* mensaje, int pid)  {

	void* mensajeSerializado = malloc(1);
	int pesoMensaje = serializarMensaje(mensaje, tipoDato, mensajeSerializado);
	Header header = armarHeader(fdDestinatario, pesoMensaje, tipoDato, tipoMensaje, pid);
    void* headerSerializado = serializarHeader(header);
    void* paqueteSerializado = malloc(1);
    int pesoPaquete = empaquetar(headerSerializado, mensajeSerializado, pesoMensaje, tipoDato, paqueteSerializado);
    int res = send(fdDestinatario, paqueteSerializado, pesoPaquete, MSG_WAITALL);
    free(headerSerializado);
    free(paqueteSerializado);

    return res;
}


/**
 * RECIBIR CONEXION O PAQUETE
 * Espera conexiones y espera paquetes de los clientes ya conextados.
 * Retorna NULL si se recibió una conexión, retorna distinto de NULL
 * si se recibió un paquete.
 */
Mensaje* recibirConexionPaquete(GestorConexiones* conexion, t_log* logger){
	fd_set* descriptoresLectura = malloc(sizeof(fd_set));
	Mensaje* mensaje = NULL;
	FD_ZERO (descriptoresLectura);
	cargarListaClientes(conexion, descriptoresLectura);
	int resultSelect = select(conexion->descriptorMaximo + 1, descriptoresLectura, NULL, NULL, NULL);
	if (resultSelect < 0 && errno != EINTR) {
		log_error(logger, "Error en select(): %s\n", strerror(errno));
	}
	else if (resultSelect > 0) {
		if (FD_ISSET(conexion->servidor, descriptoresLectura)) {
			//Verificar fd del servidor: cliente nuevo
			struct sockaddr_in addr;
			 int len = sizeof(addr);
			 int fdNuevoCliente = accept(conexion->servidor, &addr, &len);
			 if (fdNuevoCliente < 0) {
				 log_error(logger, "Error en accept(): %s\n", strerror(errno));
			 }else {
				list_add(conexion->conexiones,fdNuevoCliente);
				conexion->descriptorMaximo = getFdMaximo(conexion);
			 }
		}else{
			//Verificar fd de clientes: mensaje nuevo
			for (int j=0; j<conexion->descriptorMaximo; j++) {
				int fdActual = list_get(conexion->conexiones,j);
				if (FD_ISSET(fdActual, descriptoresLectura)) {
					int bytesRecibidos;
					Header headerSerializado;
					do {
						bytesRecibidos = recv(fdActual, &headerSerializado, sizeof(Header), MSG_DONTWAIT);
					} while (bytesRecibidos == -1 && errno == EINTR); //Ya que este caso no hay que tomarlo como error y probar otra vez.
					switch(bytesRecibidos)  {
						// Hubo un error al recibir los datos:
						case -1:
							log_warning(logger, "Hubo un error al recibir el header proveniente del socket %i", fdActual);
							break;
						// Se desconectó:
						case 0:
							desconectarCliente(fdActual, conexion, logger);
							break;

						//Obtengo Header:
						default: ;
							Header header = deserializarHeader(&headerSerializado);
							header.fdRemitente = fdActual;
							int pesoMensaje = header.tamanioMensaje * sizeof(char);
							void* mensajeBruto = (void*) malloc(pesoMensaje);
							//Obtengo mensaje:
							bytesRecibidos = recv(fdActual, mensajeBruto, pesoMensaje, MSG_DONTWAIT);
							if(bytesRecibidos == -1 || bytesRecibidos < pesoMensaje)
								log_warning(logger, "Hubo un error al recibir el mensaje proveniente del socket %i", fdActual);
							else if(bytesRecibidos == 0){
								desconectarCliente(fdActual, conexion, logger);
							}else{
								Mensaje* mensaje = malloc(sizeof(Mensaje));
								mensaje->fd_remitente = fdActual;
								mensaje->size = pesoMensaje;
								mensaje->tipoDato = header.tipoDato;
								mensaje->contenido = mensajeBruto;
							}

					} // end switch
				} //End if fd_iset
			} //End for descriptores
		} //End else not conexion->servidor
	} // End if resultSelect > 0
	free(descriptoresLectura);
	return mensaje;
}


//-----------------------------------------------
//-----------------------------------------------
//Funciones que implementan las de arriba:

int pesoString(char *string) {
    return string == NULL ? 0 : sizeof(char) * (strlen(string) + 1);
}

void freeCharArray(char** charArray){

	int i = 0;
	while(charArray[i] != NULL){
		free(charArray[i]);
		i++;
	}
	free(charArray);
}

//Crear socket
int crearSocket(t_log* logger) {
    int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
    if (fileDescriptor == ERROR) {
        log_error(logger, "No se pudo crear el file descriptor.");
    }

    return fileDescriptor;
}

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

//Habilitar socket servidor de esucha
void escucharSocketsEn(int fd_socket, t_log* logger){

    int valorListen;
    valorListen = listen(fd_socket, SOMAXCONN);/*Le podríamos poner al listen
				SOMAXCONN como segundo parámetro, y significaría el máximo tamaño de la cola*/
    if(valorListen == ERROR) {
        log_error(logger, "El servidor no pudo recibir escuchar conexiones de clientes.");
    } else	{
        log_info(logger, "El servidor está escuchando conexiones a través del socket %i.", fd_socket);
    }
}

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


void desconectarCliente(int fdCliente, GestorConexiones* unaConexion, t_log* logger)	{
    cerrarSocket(fdCliente, logger);
    eliminarFdDeListaDeConexiones(fdCliente, unaConexion);
    log_info(logger, "El socket %i ha cerrado la conexión (posiblemente haya sido terminado).", fdCliente);
}

void eliminarFdDeListaDeConexiones(int fdCliente, GestorConexiones* unaConexion)  {
    bool esElClienteDesconectado(void* cliente)	{
        return *((int*) cliente) == fdCliente;
    }
    list_remove_and_destroy_by_condition(unaConexion->conexiones, esElClienteDesconectado, free);
    unaConexion->descriptorMaximo = getFdMaximo(unaConexion);
}

void cerrarSocket(int fd_socket, t_log* logger) {

    int cerrar = close(fd_socket);
    if (cerrar == ERROR)
        log_error(logger, "No se pudo cerrar el file descriptor del socket: %d", fd_socket);
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

void cargarListaClientes(GestorConexiones* unaConexion, fd_set* emisores)	{
    FD_ZERO(emisores);
    FD_SET(unaConexion->servidor, emisores);

    for(int i=0; i < list_size(unaConexion->conexiones); i++)   {
        int fdConexion = list_get(unaConexion->conexiones, i);
        FD_SET(fdConexion, emisores);
    }
}


int getFdMaximo(GestorConexiones* conexion) {
    int maximo = conexion->servidor;
    int tamanioLista = list_size(conexion->conexiones);

    for(int i=0; i < tamanioLista; i++) {
        int fdConexion = list_get(conexion->conexiones, i);
        if(fdConexion > maximo)
            maximo = fdConexion;
    }

    return maximo;
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

/**
 * Arma una estructura header a partir de la informacion pasada por parametro.
 */
Header armarHeader(int fdDestinatario, int tamanioDelMensaje, TipoDato tipoDato, TipoMensaje tipoMensaje, int pid)    {
    Header header = {.tamanioMensaje = tamanioDelMensaje, .fdRemitente = fdDestinatario, .tipoDato = tipoDato, .tipoMensaje = tipoMensaje, .pid = pid};
    return header;
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
