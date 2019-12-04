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
	long pos;
	/*long pos = ftell(archivo);
	fseek(archivo,3,SEEK_CUR); //Se mueve desde donde quedó.
	pos = ftell(archivo);
	char* letra = malloc(sizeof(char));
	fread(letra,sizeof(char),1,archivo);*/

	t_configSAC* config = getConfigSAC(configPath);
	archivo_fs = fopen(config->pathFs,"r");

	//Tamanio Archivo
	fseek(archivo_fs, 0, SEEK_END); //Me paro al final del archivo
	fs_header->T = ftell(archivo_fs); //Veo en que byte estoy parado
	fseek(archivo_fs, 0, SEEK_SET); //Vuelvo el puntero al primer byte para seguir trabajando

	/*
	//ESTA INFO POR AHI NO SIRVA, POR LAS DUDAS LA DEJO CALCULADA
	//bitmap_byte_inicio
	fs_header->bitmap_byte_inicio = TAM_BLOQUE*1;
	//bitmap_byte_tam
	fs_header->bitmap_byte_tam = fs_header->T / TAM_BLOQUE; //2560 en ej de 10MB
	//tabla_nodos_byte_inicio
	int bloques_bm = ceil( (float)fs_header->bitmap_byte_tam / (float)TAM_BLOQUE); //Cantidad bloques que ocupa el Bitmap
	fs_header->tabla_nodos_byte_inicio = fs_header->bitmap_byte_inicio + bloques_bm * TAM_BLOQUE;
	//tabla_nodos_byte_tam
	fs_header->tabla_nodos_byte_tam = TAM_TABLA_NODOS * TAM_BLOQUE;
	//bloque_datos_byte_inicio
	fs_header->bloque_datos_byte_inicio = fs_header->tabla_nodos_byte_inicio + fs_header->tabla_nodos_byte_tam;
	//bloque_datos_byte_tam
	fs_header->bloque_datos_byte_tam = ((fs_header->T / TAM_BLOQUE) - 1 - bloques_bm - TAM_TABLA_NODOS) * TAM_BLOQUE;
	*/

	//Identificador
	int tam = 4;
	fs_header->identificador = malloc(tam);
	fread(fs_header->identificador,sizeof(char),tam,archivo_fs);

	//Version
	fread(&(fs_header->version),sizeof(int),1,archivo_fs);

	//Inicio bitmap
	fread(&(fs_header->inicio_bitmap),sizeof(int),1,archivo_fs);

	//Tamanio bitmap
	fread(&(fs_header->tam_bitmap),sizeof(int),1,archivo_fs);

	//inicio_tabla_nodos
	fs_header->inicio_tabla_nodos = 1 + fs_header->tam_bitmap;

	//Cant_bloques_datos
	fs_header->tam_bloques_datos = (fs_header->T / TAM_BLOQUE) - 1 - TAM_TABLA_NODOS - fs_header->tam_bitmap;


	freeConfig(config);
}

//Retorna uno de los nodos (metadata) de los 1024 archivos
t_nodo* obtenerNodo(int numeroNodo){
	t_nodo* nodo = malloc(sizeof(t_nodo));
	fseek(archivo_fs, fs_header->tam_bitmap * TAM_BLOQUE, SEEK_SET);
	fseek(archivo_fs, numeroNodo * TAM_BLOQUE, SEEK_CUR);
	nodo->estado = malloc(1);
	fread(nodo->estado,sizeof(char),1,archivo_fs);
	nodo->nombre_archivo = malloc(71);
	fread(nodo->nombre_archivo,sizeof(char),71,archivo_fs);
	fread(&(nodo->bloque_padre),sizeof(int),1,archivo_fs);
	fread(&(nodo->fecha_creacion),sizeof(struct timeval),1,archivo_fs);
	fread(&(nodo->fecha_modificacion),sizeof(struct timeval),1,archivo_fs);
	nodo->p_indirectos = malloc(sizeof(int)*1000);
	fread(nodo->p_indirectos,sizeof(int)*1000,1,archivo_fs);

	return nodo;
}

int obtenerBloqueDesocupado(){
	long pos;
	char* info = malloc(1);
	info = "h"; //01101000
	int val;
	t_bitarray* bitarrayPrueba = bitarray_create_with_mode(info,8,MSB_FIRST);


	int tamanio = fs_header->tam_bitmap * TAM_BLOQUE;
	char* infoBitmap = malloc(tamanio);
	pos = ftell(archivo_fs);
	fseek(archivo_fs, TAM_BLOQUE, SEEK_SET);
	pos = ftell(archivo_fs);
	fread(infoBitmap,320,1,archivo_fs);
	pos = ftell(archivo_fs);
	t_bitarray* bitarray = bitarray_create_with_mode(infoBitmap,320,LSB_FIRST);

}


int main(){
	inicializacion();
	abrirFS();
	t_nodo* nodo = obtenerNodo(3);
	obtenerBloqueDesocupado();
	aceptarClientes();
	fclose(archivo_fs);
};


