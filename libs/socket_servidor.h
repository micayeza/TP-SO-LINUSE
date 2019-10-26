
#ifndef SOCKET_SERVIDOR_H_
#define SOCKET_SERVIDOR_H_

#include <sys/socket.h>
#include <arpa/inet.h> //INADDR_ANY e INET_ADDR
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define ERROR -1 // Las system-calls de sockets retornan -1 en caso de error

typedef struct  {
    int descriptorMaximo;
    int servidor;
    t_list* conexiones;
} GestorConexiones;

typedef enum  {
	INDEFINIDO,
    ENTERO,
	TEXTO
} TipoDato;

typedef enum {
	VACIO,
	INIT,
    CREATE_HILO,
	SCHEDULE_NEXT,
	WAIT,
	SIGNAL,
	JOIN,
	CLOSE
} TipoMensaje;

typedef struct {
	TipoDato tipoDato;
	TipoMensaje tipoMensaje;
	int size;
	void* contenido;
	int fd_remitente;
} Mensaje;

typedef struct {
    int tamanioMensaje;
    int fdRemitente;
    TipoDato tipoDato;
    TipoMensaje tipoMensaje;
    int pid;
} __attribute__((packed)) Header;

//Funciones a usar:
GestorConexiones* inicializarConexion();
void crearInicializarServidor(int puerto, GestorConexiones* conexion, t_log* logger);
int crearClienteconectarseAServidor(char* ip, int puerto, t_log* logger);
int enviarPaquete(int fdDestinatario, TipoDato tipoDato, TipoMensaje tipoMensaje, void* mensaje, int pid);
Mensaje* recibirConexionPaquete(GestorConexiones* conexion, t_log* logger);

//Funciones para implementar las de arriba:
int pesoString(char *string);
void freeCharArray(char** charArray);
int crearSocket(t_log* logger);
int crearSocketServidor(int puerto, t_log* logger);
void escucharSocketsEn(int fd_socket, t_log* logger);
int crearSocketEscucha (int puerto, t_log* logger);
int empaquetar(void* headerSerializado, void* mensajeSerializado, int pesoMensaje, TipoDato tipoDato, void* paqueteSerializado);
void* serializarHeader(Header header);
Header armarHeader(int fdDestinatario, int tamanioDelMensaje, TipoDato tipoDato, TipoMensaje tipoMensaje, int pid);
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger);
int getFdMaximo(GestorConexiones* conexion);
void cargarListaClientes(GestorConexiones* unaConexion, fd_set* emisores);
Header deserializarHeader(void* headerSerializado);
void desconectarCliente(int fdCliente, GestorConexiones* unaConexion, t_log* logger);
int serializarMensaje(void* mensaje, TipoDato tipoDato, void* mensajeSerializado);
void cerrarSocket(int fd_socket, t_log* logger);
void eliminarFdDeListaDeConexiones(int fdCliente, GestorConexiones* unaConexion);
Mensaje* inicializarMensaje();
void* deserializarMensaje(void* mensajeSerializado, int tamanio, TipoDato tipoDato);

#endif /* SOCKET_SERVIDOR_H_ */
