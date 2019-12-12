
#ifndef CONEXION_H_
#define CONEXION_H_

#include <sys/socket.h>
#include <arpa/inet.h> //INADDR_ANY e INET_ADDR
#include <commons/log.h>
#include <string.h>
#include <stdlib.h>

#define ERROR -1 // Las system-calls de sockets retornan -1 en caso de error

typedef enum  {
    ENTERO,
	TEXTO
} TipoDato;

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
	//Para FUSE
	SYS_WRITE,
	SYS_OPEN,
	SYS_READ,
	SYS_RMDIR,
	SYS_UNLINK,
	SYS_MKDIR,
	SYS_READDIR,
	SYS_UTIMES,
	SYS_MOVE,
	SYS_CREATE,
	SYS_TRUNCATE,
	SYS_GETATTR
} TipoOperacion;

int crearSocketEscucha (int puerto, t_log* logger);
int crearSocketServidor(int puerto, t_log* logger);
int crearSocket(t_log* logger);
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger);
int aceptarCliente(int fd_servidor, t_log* logger);
int enviarEntero(int fdDestinatario, int enteroEnviar,  t_log* logger);
int enviarTexto(int fdDestinatario, char* textoEnviar,  t_log* logger);
int enviarTiempo(int fdDestinatario, struct timespec tiempoEnviar,  t_log* logger);
int recibirEntero(int fdOrigen, t_log* logger);
char* recibirTexto(int fdOrigen, t_log* logger);
struct timespec* recibirTiempo(int fdOrigen, t_log* logger);

void escucharSocketsEn(int fd_socket, t_log* logger);
void freeCharArray(char** charArray);
int pesoString(char *string);



#endif /* CONEXION_H_ */
