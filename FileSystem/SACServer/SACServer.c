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

void openFS(){
	configPath = string_new();
	string_append(&configPath, "../../configs/SAC.config");
	t_configSAC* config = getConfigSAC(configPath);
	archivo_fs = fopen(config->pathFs,"r+");
	free(configPath);
	freeConfig(config);
}

void closeFS(){
	fclose(archivo_fs);
}

void abrirHeaderFS(){
	long pos;
	/*long pos = ftell(archivo);
	fseek(archivo,3,SEEK_CUR); //Se mueve desde donde quedó.
	pos = ftell(archivo);
	char* letra = malloc(sizeof(char));
	fread(letra,sizeof(char),1,archivo);*/


	openFS();

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

	//Inicio_tabla_nodos
	fs_header->inicio_tabla_nodos = 1 + fs_header->tam_bitmap;

	////Inicio_bloques_datos
	fs_header->inicio_bloques_datos = 1 + fs_header->tam_bitmap + TAM_TABLA_NODOS;

	//Tam_bloques_datos
	fs_header->tam_bloques_datos = (fs_header->T / TAM_BLOQUE) - 1 - TAM_TABLA_NODOS - fs_header->tam_bitmap;

	closeFS();
}


//TABLA DE NODOS -------------------

//Retorna uno de los nodos (metadata) de los 1024 archivos
t_nodo* obtenerNodo(int numeroNodo){
	openFS();
	t_nodo* nodo = malloc(sizeof(t_nodo));
	fseek(archivo_fs, (fs_header->tam_bitmap + 1) * TAM_BLOQUE, SEEK_SET);
	fseek(archivo_fs, numeroNodo * TAM_BLOQUE, SEEK_CUR);
	nodo->estado = malloc(1);
	fread(nodo->estado,sizeof(char),1,archivo_fs);
	nodo->nombre_archivo = malloc(71);
	fread(nodo->nombre_archivo,sizeof(char),71,archivo_fs);
	fread(&(nodo->bloque_padre),sizeof(int),1,archivo_fs);
	fread(&(nodo->tam_archivo),sizeof(int),1,archivo_fs);
	fread(&(nodo->fecha_creacion),sizeof(struct timeval),1,archivo_fs);
	fread(&(nodo->fecha_modificacion),sizeof(struct timeval),1,archivo_fs);
	fread(nodo->p_indirectos,sizeof(int)*1000,1,archivo_fs);
	closeFS();

	return nodo;
}

void persistirNodo(int numeroNodo, t_nodo* nodo){
	openFS();
	fseek(archivo_fs, (fs_header->tam_bitmap + 1) * TAM_BLOQUE, SEEK_SET);
	fseek(archivo_fs, numeroNodo * TAM_BLOQUE, SEEK_CUR);

	fwrite(nodo->estado,1,1,archivo_fs);
	fwrite(nodo->nombre_archivo,sizeof(char),71,archivo_fs);
	fwrite(&(nodo->bloque_padre),sizeof(int),1,archivo_fs);
	fwrite(&(nodo->tam_archivo),sizeof(int),1,archivo_fs);
	fwrite(&(nodo->fecha_creacion),sizeof(struct timeval),1,archivo_fs);
	fwrite(&(nodo->fecha_modificacion),sizeof(struct timeval),1,archivo_fs);
	fwrite(nodo->p_indirectos,sizeof(int)*1000,1,archivo_fs);

	closeFS();
}

int buscarNodoLibre(){
	openFS();
	char estado;

	for(int i = 0; i < TAM_TABLA_NODOS - 1; i++){
		//Me muevo hasta el inicio de la Tabla de Nodos. Luego cada 1, bloque, cada 2, cada 3, etc.
		fseek(archivo_fs, (fs_header->tam_bitmap + 1) * TAM_BLOQUE, SEEK_SET); //El +1 es por el header
		fseek(archivo_fs, TAM_BLOQUE * i, SEEK_CUR);
		fread(&estado,sizeof(char),1,archivo_fs);
		if(estado == '\0'){
			closeFS();
			return i;
		}
	}
	log_info(log_interno , "No quedan nodos libre.");
	closeFS();
	return -1;
}
//--------------------------------

//BITMAP -------------------
t_bitarray* obtenerBitmap(){
	openFS();
	int tamanio = fs_header->tam_bitmap * TAM_BLOQUE;
	char* bytesArch = malloc(tamanio);
	fseek(archivo_fs, TAM_BLOQUE, SEEK_SET); //Me desplazo hasta terminar el header

	fread(bytesArch,tamanio,1,archivo_fs);
	return bitarray_create_with_mode(bytesArch,TAM_BLOQUE,MSB_FIRST);
	//Byte 127)  11111111
	//Byte 128)  11000000 -->Hasta bloque 1025 ocupado. Bloque 1026 libre.
	closeFS();
}

void persistirBitmap(t_bitarray* bitarray){
	openFS();
	fseek(archivo_fs, TAM_BLOQUE, SEEK_SET);
	fwrite(bitarray->bitarray,1,fs_header->tam_bitmap * TAM_BLOQUE,archivo_fs);
	closeFS();
}

int ocuparBloqueLibreBitmap(t_bitarray* bitarray){
	int id_primer_bloque_datos = fs_header->inicio_bloques_datos;
	int id_ultimo_bloque = 1 + fs_header->tam_bitmap + TAM_TABLA_NODOS + fs_header->tam_bloques_datos - 1;
	for(int i = id_primer_bloque_datos; i <= id_ultimo_bloque; i++){
		if(bitarray_test_bit(bitarray,i) == 0){
			bitarray_set_bit(bitarray, i);
			return i;
		}
		i++;
	}
	return -1;
}

//----------------------------

t_nodo* crearNodoVacio(){
	t_nodo* nodo = malloc(sizeof(t_nodo));
	nodo->estado = malloc(1);
	nodo->estado = "";
	nodo->nombre_archivo = malloc(71);
	strcpy(nodo->nombre_archivo, "");
	nodo->bloque_padre = 0;
	nodo->tam_archivo = 0;
	nodo->fecha_creacion = malloc(sizeof(struct timeval));
	gettimeofday(nodo->fecha_creacion, NULL);
	nodo->fecha_modificacion = malloc(sizeof(struct timeval));
	gettimeofday(nodo->fecha_modificacion, NULL);

	for(int i = 0 ; i < 1000; i++){
		nodo->p_indirectos[i] = -1;
	}

	return nodo;
}

int main(){
	inicializacion();


	t_nodo* nodo = crearNodoVacio();
	strcpy(nodo->nombre_archivo, "gente.txt");
	nodo->bloque_padre = 4;
	nodo->tam_archivo = 3;
	gettimeofday(nodo->fecha_modificacion, NULL);
	nodo->p_indirectos[0] = 34;
	nodo->p_indirectos[1] = 12;

	//int numeroNodoLibre = buscarNodoLibre();
	//persistirNodo(numeroNodoLibre, nodo);
	t_nodo* nodoNuevo = obtenerNodo(0);


	/*t_bitarray* bitarray = obtenerBitmap();
	ocuparBloqueLibreBitmap(bitarray);
	persistirBitmap(bitarray);*/

	//aceptarClientes();
}


