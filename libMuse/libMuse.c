/*
 * libMuse.c
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

#include "libmuse.h"

int muse_init(int id, char* ip, int puerto){

	int cliente;
		struct sockaddr_in direccionServidor;

		direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
		direccionServidor.sin_addr.s_addr = inet_addr(ip);
		direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
		memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura


		cliente = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
		if (cliente == -1) {
			//perror("No se pudo crear el file descriptor.\n");
			return -1;
		}


		int valorConnect = connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

		if(valorConnect == -1)  {
	//			perror("No se pudo establecer conexión entre el socket y el servidor.\n");
				return -1;
			}
		else {
	//		//Enviar a MUSE el id para que lo guarde en algun lado
			return 0;
		}


}


