
#ifndef CONEXION_H_
#define CONEXION_H_

#include <sys/socket.h>
#include <arpa/inet.h> //INADDR_ANY e INET_ADDR
#include <commons/log.h>

#define ERROR -1 // Las system-calls de sockets retornan -1 en caso de error

typedef enum  {
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
} TipoOperacion;

typedef struct {
	TipoOperacion tipoOperacion;
	int id;
	char* texto;
} Paquete;

int crearSocketEscucha (int puerto, t_log* logger);
int crearSocketServidor(int puerto, t_log* logger);
int crearSocket(t_log* logger);
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger);
int aceptarCliente(int fd_servidor, t_log* logger);
int enviarMensaje(int fdDestinatario, TipoDato tipoDato, void* mensaje,  t_log* logger);
void* recibirMensaje(int fdOrigen, t_log* logger);
void escucharSocketsEn(int fd_socket, t_log* logger);
int enviarPaquete(int fdDestinatario, Paquete* paquete, t_log* logger);
Paquete* recibirPaquete(int fdOrigen, t_log* logger);
Paquete* crearPaqueteEnvio(TipoOperacion tipoOperacion, int id, char* texto);
void freePaquete(Paquete* paquete);
void freeCharArray(char** charArray);
int pesoString(char *string);



#endif /* CONEXION_H_ */
