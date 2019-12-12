/*
 * SACServer.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#include "SACServer.h"

int crearNodoDirectorio(char* path){
	int numeroNodoLibre = buscarNodoLibre();
	//No hay espacio para guardar nodo
	if(numeroNodoLibre == ERROR){
		return -EDQUOT;
	}
	//Se crea y persiste el nodo
	t_nodo* nodo = crearNodoVacio();
	nodo->estado = 2;
	strcpy(&(nodo->nombre_archivo), path);
	nodo->bloque_padre = 44;
	nodo->tam_archivo = 8;
	gettimeofday(&(nodo->fecha_modificacion), NULL);
	persistirNodo(numeroNodoLibre, nodo);

	return 0;
}

t_nodo* obtenerNodoDePath(char* path){
	int numNodo = existeArchivo(path);
	if(numNodo == ERROR) return NULL;
	return obtenerNodo(numNodo);
}

char* obtenerArchivosDeDirectorio(char* path){
	char* separador = ";";
	char* punto = ".";
	char* dosPuntos = "..";
	int numNodo = existeArchivo(path);
	if(numNodo == ERROR)
		return "";
	char* nombresArchivos = string_new();
	string_append(&nombresArchivos,punto);
	string_append(&nombresArchivos,separador);
	string_append(&nombresArchivos,dosPuntos);
	for(int i = 0; i < TAM_TABLA_NODOS; i++){
		t_nodo* nodo = obtenerNodo(i);
		if(obtenerNodo(i)->bloque_padre == numNodo){
			string_append(&nombresArchivos,separador);
			string_append(&nombresArchivos,&(nodo->nombre_archivo));
		}
	}
	return nombresArchivos;

 }

int SacServerOpen(const char *path) {
	 return 0;

 }

 int SacServerWrite(const char *path, const char *buf, size_t size, off_t offset){
	 return 0;
 }

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

	configPath = string_new();
	string_append(&configPath, "/home/utnso/workspace/tp-2019-2c-capitulo-2/configs/SAC.config");
	config = getConfigSAC(configPath);

	fs_header = malloc(sizeof(t_header));
	abrirHeaderFS();
}

void aceptarClientes(){

	int socket_escucha = crearSocketEscucha(config->listenPort, log_interno);

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

void free_nodo(t_nodo* nodo){
	free(nodo->nombre_archivo);
	free(nodo->p_indirectos);
	free(nodo);
}

void openFS(){
	archivo_fs = fopen(config->pathFs,"r+");
}

void closeFS(){
	fclose(archivo_fs);
}

int sizeArrayChar(char** array){
	int tam = 0;
	while(array[tam] != NULL){
		tam++;
	}
	return tam;
}

void formatearSAC(){
	openFS();
	fseek(archivo_fs, 0, SEEK_END); //Me paro al final del archivo
	long tamFS = ftell(archivo_fs); //Veo en que byte estoy parado
	fseek(archivo_fs, 0, SEEK_SET);

	//HEADER
	fwrite("SAC",sizeof(char),4,archivo_fs); //Identificador
	int version = 1;
	fwrite(&version,sizeof(int),1,archivo_fs); //Version
	int ini_bit = 1;
	fwrite(&ini_bit,sizeof(int),1,archivo_fs); //Bloque inicio bitmap
	long tam_byte_bitmap = tamFS / TAM_BLOQUE;
	int bloques_bm = ceil( (float)tam_byte_bitmap / (float)TAM_BLOQUE); //Cantidad bloques que ocupa el Bitmap
	fwrite(&bloques_bm,sizeof(int),1,archivo_fs); //Tamaño bitmap en bloque
	char barraCero = '\0';
	fwrite(&barraCero,sizeof(char),4080,archivo_fs); //Relleno
	//BITMAP
	char* bytesBitmap = malloc(bloques_bm * TAM_BLOQUE);
	bytesBitmap = string_repeat('\0', tam_byte_bitmap);
	t_bitarray* bitmap = bitarray_create_with_mode(bytesBitmap,tam_byte_bitmap,MSB_FIRST);
	bitarray_set_bit(bitmap, 0); //Seteo ocupado el bit del header
	for(int i = 1; i <= bloques_bm; i++){ //Seteo ocupado los bits del bitmap
		bitarray_set_bit(bitmap, i);
	}
	for(int i = 1 + bloques_bm; i <= 1 + bloques_bm + TAM_TABLA_NODOS - 1; i++){ //Seteo ocupado los bits de la tabla de nodos
		bitarray_set_bit(bitmap, i);
	}
	fwrite(bitmap->bitarray,1,bloques_bm * TAM_BLOQUE,archivo_fs);

	//TABLA DE NODOS
	t_nodo* nodo= crearNodoVacio();
	for(int i = 0; i < TAM_TABLA_NODOS; i++){
		fwrite(&(nodo->estado),1,1,archivo_fs);
		fwrite(nodo->nombre_archivo,sizeof(char),71,archivo_fs);
		fwrite(&(nodo->bloque_padre),sizeof(int),1,archivo_fs);
		fwrite(&(nodo->tam_archivo),sizeof(int),1,archivo_fs);
		fwrite(&(nodo->fecha_creacion),sizeof(struct timespec),1,archivo_fs);
		fwrite(&(nodo->fecha_modificacion),sizeof(struct timespec),1,archivo_fs);
		fwrite(nodo->p_indirectos,sizeof(int)*TAM_MAX_PUNT_IND,1,archivo_fs);
	}

	//BLOQUES DE DATOS
	int tam_bloques_datos = (tamFS / TAM_BLOQUE) - 1 - TAM_TABLA_NODOS - bloques_bm;
	char caracterVacio = '\0';
	for(int i = 0; i < tam_bloques_datos; i++){
		fwrite(&caracterVacio,1,TAM_BLOQUE,archivo_fs);
	}
	closeFS();
}

void abrirHeaderFS(){
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
	fread(&(nodo->estado),sizeof(uint8_t),1,archivo_fs);
	fread(&(nodo->nombre_archivo),sizeof(char),TAM_MAX_NOMBRE_ARCHIVO,archivo_fs);
	fread(&(nodo->bloque_padre),sizeof(uint32_t),1,archivo_fs);
	fread(&(nodo->tam_archivo),sizeof(uint32_t),1,archivo_fs);
	fread(&(nodo->fecha_creacion),sizeof(struct timespec),1,archivo_fs);
	fread(&(nodo->fecha_modificacion),sizeof(struct timespec),1,archivo_fs);
	fread(nodo->p_indirectos,sizeof(uint32_t)*TAM_MAX_PUNT_IND,1,archivo_fs);
	closeFS();

	return nodo;
}

void persistirNodo(int numeroNodo, t_nodo* nodo){
	openFS();
	fseek(archivo_fs, (fs_header->tam_bitmap + 1) * TAM_BLOQUE, SEEK_SET);
	fseek(archivo_fs, numeroNodo * TAM_BLOQUE, SEEK_CUR);

	fwrite(&(nodo->estado),1,1,archivo_fs);
	fwrite(nodo->nombre_archivo,sizeof(char),71,archivo_fs);
	fwrite(&(nodo->bloque_padre),sizeof(int),1,archivo_fs);
	fwrite(&(nodo->tam_archivo),sizeof(int),1,archivo_fs);
	fwrite(&(nodo->fecha_creacion),sizeof(struct timespec),1,archivo_fs);
	fwrite(&(nodo->fecha_modificacion),sizeof(struct timespec),1,archivo_fs);
	fwrite(nodo->p_indirectos,sizeof(int)*TAM_MAX_PUNT_IND,1,archivo_fs);

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

//Retorna el ID del nodo si existe o ERROR en caso de no existir.
int existeArchivo(char* path){
	char barra = "/";
	char** pathSeparado = string_split(path, "/");
	int tam = sizeArrayChar(pathSeparado);
	if(tam == 0) //Si el contenido es barra sola devuelve 0 que es el id del bloque raiz
		return 0;
	for(int i = 0; i < TAM_TABLA_NODOS; i++){
		int i_path = tam - 1;
		t_nodo* nodo = obtenerNodo(i);

		if(nodo->estado != 0 && strcmp(pathSeparado[i_path], nodo->nombre_archivo) == 0){
			if(chequearNombrePadre(pathSeparado, i_path, nodo->bloque_padre) == 0)
				return i;
		}
	}
	return ERROR;
}

int chequearNombrePadre(char** pathSeparado, int i_path, int padre){
	if(padre==0)
		return 0;
	t_nodo* nodo = obtenerNodo(padre);
	i_path--;
	if(nodo->estado != 0 && strcmp(pathSeparado[i_path], nodo->nombre_archivo) == 0){
		int resultado = chequearNombrePadre(pathSeparado, i_path, nodo->bloque_padre);
		return resultado;
	}
	return ERROR;
}

t_nodo* crearNodoVacio(){
	t_nodo* nodo = malloc(sizeof(t_nodo));
	nodo->estado = 0;
	strcpy(&(nodo->nombre_archivo), "");
	nodo->bloque_padre = 0;
	nodo->tam_archivo = 0;
	gettimeofday(&(nodo->fecha_creacion), NULL);
	gettimeofday(&(nodo->fecha_modificacion), NULL);

	for(int i = 0 ; i < TAM_MAX_PUNT_IND; i++){
		nodo->p_indirectos[i] = -1;
	}

	return nodo;
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

void finalizar(){
	freeConfig(config);
}

int main(){
	inicializacion();

	//int res = existeArchivo("/AAA/CCC/mmm.txt");
	//char* archivos = obtenerArchivosDeDirectorio("/AAA/CCC");
	//t_nodo* nodoNuevo = obtenerNodo(res);

	//t_nodo* nodo = crearNodoVacio();

	//formatearSAC();


	/*nodo->estado = 1;
	strcpy(&(nodo->nombre_archivo), "mmm.txt");
	nodo->bloque_padre = 5;
	nodo->tam_archivo = 22;
	gettimeofday(&(nodo->fecha_modificacion), NULL);
	nodo->p_indirectos[0] = 122;
	nodo->p_indirectos[1] = 123;
	nodo->p_indirectos[2] = 128;*/

	//persistirNodo(8, nodo);
	//int numeroNodoLibre = buscarNodoLibre();
	//persistirNodo(numeroNodoLibre, nodo);


	/*t_bitarray* bitarray = obtenerBitmap();
	ocuparBloqueLibreBitmap(bitarray);
	persistirBitmap(bitarray);*/

	aceptarClientes();

	finalizar();
}


