#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ERROR -1


typedef enum{
	HAND
}operacion;

typedef struct {
	operacion operacion;
	int size;
}header;

typedef struct {
	header header;
	void*  contenido;
}mensaje;


void freeCharArray(char** charArray);
int pesoString(char *string);


void freeMensaje(mensaje* mensaje);
mensaje* recibirMensaje(int socket);
int enviarMensaje(int socket, mensaje* msj);
mensaje* armarMensaje(operacion operacion, int size, void* contenido);


//int crearSocketServidor(int);

