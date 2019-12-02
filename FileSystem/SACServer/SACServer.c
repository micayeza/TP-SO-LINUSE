/*
 * SACServer.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#include "SACServer.h"


int SacServerRead(const char *path, char *buf, size_t size, off_t offset){
	return 0;
 }

int SacServerOpen(const char *path) {
	 return 0;

 }

 int SacServerWrite(const char *path, const char *buf, size_t size, off_t offset){
	 return 0;
 }
/*
 int SacServerMkdir(const char *path, mode_t mode){
	 return 0;
 }*/

 //BORRAR ARCHIVO
 int SacServerUnlink(const char *path) {
	 return 0;
 }

 int SacServerRmdir(const char *path) {
	 return 0;
 }

 //CAMBIAR EL VALOR DE RETORNO
 int SacServerReaddir(const char* path){
	 return 0;
 }


void inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../../configs/SAC.config");

	log_resultados = log_create("log_resultados.txt", "LOG-RES", false, LOG_LEVEL_INFO);
	log_info(log_resultados, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	log_info(log_interno, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

	fs_header = malloc(sizeof(t_header));
}

void aceptarClientes(){

	t_configSAC* config = getConfigSAC(configPath);
	int socket_escucha = crearSocketEscucha(config->listenPort, log_interno);
	freeConfig(config);

	int socket = 0;
	while((socket = aceptarCliente(socket_escucha, log_interno)) > 0){

		TipoOperacion operacionRecibida = (TipoOperacion) recibirEntero(socket, log_interno);
		if(operacionRecibida == INIT){
			t_cliente* nuevoCliente = create_cliente(socket);
			pthread_t hiloCliente;
			pthread_create(&hiloCliente, NULL, (void*)&atenderCliente, nuevoCliente); //El ultimo parametro es para pasar algun dato a la funcion
		}else{
			//Tirar mensaje de operacion erronea por log.
		}

	}

}

t_cliente* create_cliente(int socket){
	t_cliente* cliente = malloc(sizeof(t_cliente));
	cliente->socket = socket;
	return cliente;
}

void free_cliente(t_cliente* cliente){
	free(cliente);
}

void abrirFS(){
	/*long pos = ftell(archivo);
	fseek(archivo,3,SEEK_CUR);
	pos = ftell(archivo);
	char* letra = malloc(sizeof(char));
	fread(letra,sizeof(char),1,archivo);*/

	t_configSAC* config = getConfigSAC(configPath);
	FILE* archivo = fopen(config->pathFs,"r");


	//Identificador
	int tam = 3;
	fs_header->identificador = malloc(tam+1);
	fread(fs_header->identificador,sizeof(char),tam,archivo);
	fs_header->identificador[tam] = '\0';

	//Version
	fread(&(fs_header->version),sizeof(int),1,archivo);

	//Inicio bitmap
	fread(&(fs_header->inicio_bitmap),sizeof(void*),1,archivo);

	//Tamanio bitmap
	fread(&(fs_header->tam_bitmap),sizeof(int),1,archivo);

	fclose(archivo);
	freeConfig(config);
}

int main(){
	inicializacion();
	abrirFS();
	aceptarClientes();
	//crearHiloParalelos();
};


