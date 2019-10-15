#include "libs.h"

void freeCharArray(char** charArray){

	int i = 0;
	while(charArray[i] != NULL){
		free(charArray[i]);
		i++;
	}
	free(charArray);
}

int crearSocket() {
	int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
	if (fileDescriptor == ERROR) {
		perror("No se pudo crear el file descriptor.\n");
	}

	//Hay que mejorarlo para el caso en el que escucha muchas conexiones

	return fileDescriptor;
	}



int crearSocketEscucha(int puerto) {

	int socketDeEscucha = crearSocketServidor(puerto);

	//Escuchar conexiones
		int valorListen;
		valorListen = listen(socketDeEscucha, SOMAXCONN);/*
					SOMAXCONN como segundo parámetro, y significaría el máximo tamaño de la cola*/
		if(valorListen == ERROR) {
			printf("Ocurrió un error, no se pueden escuchar conexiones\n");
		} else	{
			printf("Esperando conexiones en el puerto %d\n", puerto);
		}

	// Hasta que no salga del listen, nunca va a retornar el socket del servidor ya que el listen es bloqueante

	return socketDeEscucha;
}


int aceptarCliente(int fd_socket){

	struct sockaddr_in unCliente;
	memset(&unCliente, 0, sizeof(unCliente));
	unsigned int address_size = sizeof(unCliente);

	int fdCliente = accept(fd_socket, (struct sockaddr*) &unCliente, &address_size);
	if(fdCliente == ERROR)  {
		printf("El servidor no pudo aceptar la conexión entrante\n");
	} else	{
		printf("Nueva conexión establecida - ID %d\n", fdCliente);
	}

	return fdCliente;

}

//Funciones para Cliente

int crearSocketCliente(char *ipServidor, int puerto){

	int cliente;
	struct sockaddr_in direccionServidor;

	direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
	direccionServidor.sin_addr.s_addr = inet_addr(ipServidor);
	direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
	memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura

	cliente = crearSocket();
	int valorConnect = connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

	if(valorConnect == ERROR)  {
//			perror("No se pudo establecer conexión entre el socket y el servidor.\n");
			return ERROR;
		} else {
//		puts("¡Socket conectado a Servidor!");
		return cliente;
	}
}

void cerrarSocket(int fd_socket) {

	int cerrar = close(fd_socket);
	if (cerrar == ERROR) {
		printf("No se pudo cerrar el file descriptor del socket: %d\n",fd_socket);
	}
}

int crearSocketServidor(int puerto)	{
	struct sockaddr_in miDireccionServidor;
	int socketDeEscucha = crearSocket();

	miDireccionServidor.sin_family = AF_INET;			//Protocolo de conexion
	miDireccionServidor.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0 y significa que usa la IP actual de la maquina
	miDireccionServidor.sin_port = htons(puerto);		//Puerto en el que escucha
	memset(&(miDireccionServidor.sin_zero), '\0', 8);		//Pone 0 a la estructura
		//Veamos si el puerto está en uso
	int puertoEnUso = 1;
	int puertoYaAsociado = setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &puertoEnUso, sizeof(puertoEnUso));

	if (puertoYaAsociado == ERROR) {
		perror("El puerto asignado ya está siendo utilizado.\n");
	}
		//Turno del bind
	int activado = 1;
	setsockopt(socketDeEscucha,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));//Para evitar que falle el bind, al querer usar un mismo puerto

	int valorBind = bind(socketDeEscucha,(void*)&miDireccionServidor, sizeof(miDireccionServidor));

	if (valorBind != 0) {
		perror("El bind no funcionó, el socket no se pudo asociar al puerto");
		return 1;
	}

	return socketDeEscucha;
}


//Mensaje para memoria

//int enviarMensaje(int socket, Mensaje msg) { // Devuelve la cantidad de bits leidos
//
//	//Verifico la existencia del socket y que el mensaje tenga contenido
//	if(socket == -1 || msg.contenido == NULL || msg.header == NULL) return -1;
//
//	//Calculo el tamaño total
//	int size = sizeof(ContentHeader) + msg.header->size;
//
//	//Creo y relleno un buffer
//	void * buffer = malloc(size);
//	memcpy(buffer, msg.header, sizeof(ContentHeader));
//	memcpy(buffer + sizeof(ContentHeader), msg.contenido, msg.header->size);
//
//	//Envio y registro el resultado que es el tamaño de lo que devolvio o -1 en caso de error
//	int resultado = send(socket, buffer, size, 0);
//
//	free(buffer);
//
//	return resultado;
//}

mensaje* armarMensaje(operacion operacion, int size, void* contenido){
	mensaje* mensaje   = malloc(sizeof(mensaje));
	mensaje->header.operacion = operacion;
	mensaje->header.size      = size;

//	memcpy(mensaje->header, &header, sizeof(header) );
	mensaje->contenido = malloc(size);
	memcpy(mensaje->contenido , contenido , mensaje->header.size);
	return mensaje;
}

int enviarMensaje(int socket, mensaje* msj){

	int resultado = send(socket, &(msj->header), sizeof(header), 0);
		if(resultado < 0){ return -1; };

	resultado = send(socket, msj->contenido, msj->header.size, 0);
		if(resultado < 0){return -1; };

	return 0;

}


mensaje* recibirMensaje(int socket){
	mensaje* mensaje = malloc(sizeof(mensaje));
	if (recv(socket, &(mensaje->header), sizeof(header),MSG_WAITALL) < 1){ return NULL;  };

	mensaje->contenido = malloc(mensaje->header.size);
	if( recv(socket, mensaje->contenido, mensaje->header.size, MSG_WAITALL) < 1){ return NULL;}

 return mensaje;
}

void freeMensaje(mensaje* mensaje){

	free(mensaje->contenido);
	free(mensaje);
}


