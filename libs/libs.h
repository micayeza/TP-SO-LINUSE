#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ERROR -1

void freeCharArray(char** charArray);
int crearSocket();
int crearSocketEscucha(int);
int aceptarCliente(int);
int crearSocketCliente(char*, int);
void cerrarSocket(int);
int crearSocketServidor(int);
