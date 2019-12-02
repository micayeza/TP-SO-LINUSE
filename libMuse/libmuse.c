/*
 * libMuse.c
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

#include "libmuse.h"

int muse_init(int id, char* ip, int puerto){
		logLib = log_create("LIB.txt", "LOG", true, LOG_LEVEL_INFO);
		//int cliente;
		struct sockaddr_in direccionServidor;

		direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
		direccionServidor.sin_addr.s_addr = inet_addr(ip);
		direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
		memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura

		muse = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
		if (muse == -1) {
			//perror("No se pudo crear el file descriptor.\n");
			return -1;
		}

		int valorConnect = connect(muse, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

		if(valorConnect == -1)  {
				return -1;
			}
		else {

			int res = enviarInt(muse, SALUDO);
			if(res < 0){
				return -1;
			}else{
				res = enviarInt(muse, id);
				if (res<0){
					return -1;
				}
				res = recibirInt(muse);
			}
			return 0;
		}


}

uint32_t muse_alloc(uint32_t tam){

	int res = enviarInt(muse, RESERVAR);
	if(res > 0){
		res = enviarUint32_t(muse, tam);
		if(res > 0){
			return recibirUint32_t(muse);
		}
	}

	return -1;
}
void muse_free(uint32_t posicion){

	int res = enviarInt(muse, LIBERAR);
	if (res >0){
		enviarUint32_t(muse, posicion);
	}


}


int muse_cpy(uint32_t dst, void* src, int n){
	char* frase = malloc(n);
	strncpy(frase, src, n);
	frase[n]='\0';
	enviarInt(muse, COPIAR);
	enviarUint32_t(muse, dst);
	enviarInt(muse, n );
	enviarTexto(muse, frase, logLib);

	return recibirInt(muse);

}

int muse_get(void* dst, uint32_t src, size_t n){

	enviarInt(muse, OBTENER);
	enviarUint32_t(muse, src );
	enviarSizet(muse, n);

	char* copiar = recibirTexto(muse, logLib);
	if(copiar == NULL){
		return -1;
	}else{
	char* result = strcpy(dst, copiar);
	printf("El dato obtenido fue: %s\n", dst);
		if(result == NULL){
			return -1;
		}
	return 0;
	}


}


uint32_t muse_map(char *path, size_t length, int flags){

	if(length == 0){
		return 0;
	}
	enviarInt(muse , MAPEAR);
	enviarInt(muse, flags);
	enviarSizet(muse,  length);
	int res = enviarTexto(muse, path, logLib);
	if(res>0){
		uint32_t resultado = recibirUint32_t(muse);
		return resultado;
	}
	else {
		return 0;
	}



}


int muse_sync(uint32_t addr, size_t len){

	enviarInt(muse, SINCRO);
	enviarSizet(muse, len);

	enviarUint32_t(muse, addr);
	int result = recibirInt(muse);

	return result;
}

int muse_unmap(uint32_t dir){

	enviarInt(muse, DESMAP);
	enviarUint32_t(muse,  dir);

	return recibirInt(muse);
}


void muse_enviar(){
	enviarInt(muse, VER);
	return;
}
