
#ifndef CONEXION_H_
#define CONEXION_H_

#include <sys/socket.h>
#include <arpa/inet.h> //INADDR_ANY e INET_ADDR
#include <commons/log.h>

#define ERROR -1 // Las system-calls de sockets retornan -1 en caso de error

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

int crearSocketEscucha (int puerto, t_log* logger);
int crearSocketServidor(int puerto, t_log* logger);
int crearSocket(t_log* logger);
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger);
int enviarPaquete(int fdDestinatario, TipoDato tipoDato, TipoMensaje tipoMensaje, void* mensaje, int pid);
Mensaje* recibirPaquete(int fdConexion, t_log* logger);
int aceptarCliente(int fd_servidor, t_log* logger)
int serializarMensaje(void* mensaje, TipoDato tipoDato, void* mensajeSerializado);
void* deserializarMensaje(void* mensajeSerializado, int tamanio, TipoDato tipoDato);
void* serializarHeader(Header header);
Header armarHeader(int fdDestinatario, int tamanioDelMensaje, TipoDato tipoDato, TipoMensaje tipoMensaje, int pid);
int empaquetar(void* headerSerializado, void* mensajeSerializado, int pesoMensaje, TipoDato tipoDato, void* paqueteSerializado);
Header deserializarHeader(void* headerSerializado);
Mensaje* inicializarMensaje();
void freeMensaje(Mensaje* mensaje);



#endif /* CONEXION_H_ */
