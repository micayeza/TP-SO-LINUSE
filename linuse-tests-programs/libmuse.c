/*
 * libMuse.c
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

#include "libmuse.h"
#include <unistd.h>
#include <signal.h>

//MENSAJEEEEEEEEEEEEEEEEEEES
char* recibirTexto(int fdOrigen){
	int tamanio;
	int resTamanio = recv(fdOrigen, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == -1){

		return NULL;
	}
	if(tamanio == 0){
		return NULL;
	}
	char* textoRecibido = malloc(tamanio);
	int resTexto = recv(fdOrigen, textoRecibido, tamanio, MSG_WAITALL);
	if(resTexto == -1){

		return NULL;
	}
	textoRecibido[tamanio-1] = '\0';
	return textoRecibido;
}

int pesoString(char *string) {
    return string == NULL ? 0 : sizeof(char) * (strlen(string) + 1);
}

int enviarTexto(int fdDestinatario, char* textoEnviar){
	int tamanio = pesoString(textoEnviar);
	int resTamanio = send(fdDestinatario, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == -1){

		return -1;
	}
	if(tamanio != 0){
	int resTexto = send(fdDestinatario, (void*)textoEnviar, tamanio, MSG_WAITALL);
	if(resTexto == -1){

		return -1;
	}
	return resTexto;
	}
	return 0;
}

int enviarInt(int destinatario, int loQueEnvio){

	 void* paquete = malloc(sizeof(int));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(int));
	 int res = send(destinatario, paquete, sizeof(int), MSG_WAITALL);
	 free(paquete);
	 return res;
}

int recibirInt(int destinatario){
	int algo;
	if(recv(destinatario, &algo, sizeof(int), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}

int enviarUint32_t(int destinatario, uint32_t loQueEnvio){
	 void* paquete = malloc(sizeof(uint32_t));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(uint32_t));
	 int res = send(destinatario, paquete, sizeof(uint32_t), MSG_WAITALL);
	 free(paquete);
	 return res;
}

uint32_t recibirUint32_t(int destinatario){
	uint32_t algo;
	if(recv(destinatario, &algo, sizeof(uint32_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}
int enviarSizet(int destinatario,size_t n){
	 int tamanio = sizeof(size_t);
	 void* paquete = malloc(sizeof(size_t));
	 void* puntero = paquete;

	 memcpy(puntero, &(n), sizeof(size_t));
	 int res = send(destinatario, paquete, tamanio, MSG_WAITALL);
	 free(paquete);
	 return res;
}

size_t recibirSizet(int destinatario){

	size_t algo;
	if(recv(destinatario, &algo, sizeof(size_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}
}


int enviarVoid(int destinatario, void* textoEnviar, int tam){


	int res = enviarInt(destinatario, tam);
	if(tam!=0){
	if(res != 0){
	  res = send(destinatario, textoEnviar, tam, MSG_WAITALL);
	if(res == -1){

		return -1;
	}
	return res;
	}
	}	return 0;
}



char* recibirVoid(int fdOrigen){
	int tamanio;

	tamanio = recibirInt(fdOrigen);
	if(tamanio == 0){
		return NULL;
	}
	void* textoRecibido = malloc(tamanio);
	int resTexto = recv(fdOrigen, textoRecibido, tamanio, MSG_WAITALL);
	if(resTexto == -1){
		return NULL;
	}

	return textoRecibido;
}


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
	res=recibirInt(muse);
	if(res == -5){

			printf("[SEGMENTATION FAULT] Abortando programa.... \n");

			close(muse);
			raise(SIGSEGV);

	}

}


int muse_cpy(uint32_t dst, void* src, int n){

	enviarInt(muse, COPIAR);
	enviarUint32_t(muse, dst);


	void* frase;
	frase = malloc(n);
	memcpy(frase, src, n);
	if(n==4){
		char* extra = string_itoa(*(int*)src);
		printf("Copiar: %s \n" , extra);
		free(extra);


	}else{

		printf("Copiar: %s \n" , (char*)src);
	}


	enviarInt(muse, n );



	enviarVoid(muse, frase, n);

	int res = recibirInt(muse);
	if(res == -5){

		printf("[SEGMENTATION FAULT] Abortando programa.... \n");

		close(muse);
		raise(SIGSEGV);

 }
	return res;
}

int muse_get(void* dst, uint32_t src, size_t n){

	enviarInt(muse, OBTENER);
	enviarUint32_t(muse, src );
	enviarSizet(muse, n);

	void* copiar = recibirVoid(muse);
	if(copiar != NULL){
		if(strcmp(copiar, "SG_MICA")==0){

				printf("[SEGMENTATION FAULT] Abortando programa.... \n");
				free(copiar);
				close(muse);
				raise(SIGSEGV);

		}
		if(n==4){
			printf("El dato obtenido fue: %d \n", *(int*)copiar);
		}
		else{

			printf("El dato obtenido fue: %s \n", (char*)copiar);
		}
		memcpy(dst, copiar, n);
		free(copiar);
		return 0;
	}else{
			return -1;

	}

	return 0;




}


uint32_t muse_map(char *path, size_t length, int flags){

	if(length == 0){
		return 0;
	}
	enviarInt(muse , MAPEAR);
	enviarInt(muse, flags);
	enviarSizet(muse,  length);
	int res = enviarTexto(muse, path);
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
	if(result == -5){

		printf("[SEGMENTATION FAULT] Abortando programa.... \n");

		close(muse);
		raise(SIGSEGV);

 }
	return result;
}

int muse_unmap(uint32_t dir){

	enviarInt(muse, DESMAP);
	enviarUint32_t(muse,  dir);

	int res =recibirInt(muse);
	if(res == -5){
		printf("[SEGMENTATION FAULT] Abortando programa.... \n");

		close(muse);
		raise(SIGSEGV);

	}
	return res;
}

void muse_close(){
	enviarInt(muse,  CERRAR);
}


void muse_enviar(){
	enviarInt(muse, VER);
	return;
}
