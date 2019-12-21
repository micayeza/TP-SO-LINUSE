/*
 * SACServer.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#include "SACServer.h"

int eliminarDirectorioArchivo(char* path, int esDirectorio){
	int numNodo = existeArchivo(path);
	if(numNodo == ERROR)
		return EACCES;

	if(esDirectorio == 1){
		char* archivosDirectorio = obtenerArchivosDeDirectorio(path);
		char* separador = malloc(2);
		strcpy(separador,";");
		char** archivosSeparados = string_split(archivosDirectorio, separador);
		int tam = sizeArrayChar(archivosSeparados);
		free(separador);
		free(archivosDirectorio);
		free_char_as_as(archivosSeparados);
		if(tam > 2){
			return ENOTEMPTY;
		}
	}

	t_nodo* nodo = obtenerNodo(numNodo);
	if(esDirectorio == 0){
		for(int i = 0 ; i < TAM_MAX_PUNT_IND; i++){
			int numBloque = nodo->p_indirectos[i];
			if(numBloque != NULL){
				desocuparBloqueBitmap(numBloque);
			}
		}
	}
	t_nodo* nodoNuevo = crearNodoVacio();
	persistirNodo(numNodo, nodoNuevo);
	free_nodo(nodo);
	free_nodo(nodoNuevo);

	return 0;
}

int cambiarUbicacion(char* path, char* newPath){
	int numNodo = existeArchivo(path);
	char* pathPadreNuevo = cortarPathPadre(newPath);
	int numNodoPadreNuevo = existeArchivo(pathPadreNuevo);
	free(pathPadreNuevo);
	//Chequeo si no existe el archivo original o si ya existe el nuevo o si el nuevo padre no existe
	if(numNodo == ERROR || existeArchivo(newPath) != ERROR || numNodoPadreNuevo == ERROR){
		return EEXIST;
	}
	//Chequeo que el nuevo nombre no sea de tamanio mayor al permitido
	char* barra = malloc(2);
	strcpy(barra,"/");
	char** pathSeparado = string_split(newPath, barra);
	int tam = sizeArrayChar(pathSeparado);
	char* nombreNuevoArchivo = pathSeparado[tam-1];
	if(string_length(nombreNuevoArchivo)+1 > TAM_MAX_NOMBRE_ARCHIVO){
		free(barra);
		free_char_as_as(pathSeparado);
		return ENAMETOOLONG;
	}

	t_nodo* nodo = obtenerNodo(numNodo);
	strcpy(nodo->nombre_archivo,nombreNuevoArchivo);
	nodo->bloque_padre = numNodoPadreNuevo;
	persistirNodo(numNodo, nodo);
	free_nodo(nodo);
	free(barra);
	free_char_as_as(pathSeparado);
	return 0;

}

int leerArchivo(char* path, int offset, int tamanio, void* buf){
	if(tamanio == 0)
		return 0;
	int numNodo = existeArchivo(path);
	if(numNodo == ERROR)
		return ERROR;
	t_nodo* nodo = obtenerNodo(numNodo);

	//Si desde el offset leyendo ese tamanio me paso del archivo, paso a leer menos.
	//(guardo el sobrante ya que me va aservir para completar el buffer con nulo)
	int tamSobrante = 0;
	if(offset + tamanio > nodo->tam_archivo){
		tamSobrante = offset + tamanio - nodo->tam_archivo;
		tamanio = nodo->tam_archivo - offset;
	}

	//Bloque inicial
	int iBloqueInicial = offset / TAM_BLOQUE;
	int offsetBloqueInicial = offset % TAM_BLOQUE;//Offset dentro bloque inicial

	//Bloque final
	int iBloqueFinal = (offset + tamanio - 1) / TAM_BLOQUE; //Resto un byte ya que empieza desde cero el offset
	int finLecturaBloqueFinal = (offset + tamanio) % TAM_BLOQUE;//Offset dentro bloque final

	int cantidadBloques = iBloqueFinal - iBloqueInicial  + 1;

	int tamLeido = 0;
	//Leo bloque inicial
	int numBloqueInicial = nodo->p_indirectos[iBloqueInicial];
	int tamanioLeerInicial;
	if(cantidadBloques == 1)
		tamanioLeerInicial = tamanio;
	else
		tamanioLeerInicial = TAM_BLOQUE - offsetBloqueInicial;
	tamLeido += leerBloqueDatos(numBloqueInicial, offsetBloqueInicial, tamanioLeerInicial, buf);
	buf += tamanioLeerInicial;

	//Leo bloques intermedios
	for(int i = iBloqueInicial + 1; i < iBloqueFinal; i++){
		int numBloque = nodo->p_indirectos[i];
		tamLeido += leerBloqueDatos(numBloque, 0, TAM_BLOQUE, buf);
		buf += TAM_BLOQUE;
	}

	//Leer bloque final
	if(cantidadBloques > 1){
		int numBloque = nodo->p_indirectos[iBloqueFinal];
		tamLeido += leerBloqueDatos(numBloque, 0, finLecturaBloqueFinal, buf);
		buf += finLecturaBloqueFinal;
	}

	//Completar sobrante
	if(tamSobrante > 0){
		char* bytesCopiar = malloc(tamSobrante);
		bytesCopiar = string_repeat('\0', tamSobrante);
		memcpy(buf, bytesCopiar, tamSobrante);
		free(bytesCopiar);
	}
	free_nodo(nodo);
	return tamLeido;

}

int escribirArchivo(char* path, int offset, int tamanio, void* datos){

	int numNodo = existeArchivo(path);
	if(numNodo == ERROR)
		return ERROR;
	t_nodo* nodo = obtenerNodo(numNodo);
	int nuevoTamanio = offset + tamanio;

	//Ya que si el tamanio de lo q voy a escribir es menor no tengo que truncar el archivo
	if(nuevoTamanio > nodo->tam_archivo){
		free_nodo(nodo);
		int resCambiarTam = cambiarTamanioArchivo(path, nuevoTamanio);
		if(resCambiarTam == ERROR){
			return EDQUOT;
		}
		nodo = obtenerNodo(numNodo);
	}

	//Bloque inicial
	int iBloqueInicial = offset / TAM_BLOQUE;
	int offsetBloqueInicial = offset % TAM_BLOQUE;//Offset dentro bloque inicial

	//Bloque final
	int iBloqueFinal = (offset + tamanio - 1) / TAM_BLOQUE; //Resto un byte ya que empieza desde cero el offset
	int finEscrituraBloqueFinal = (offset + tamanio) % TAM_BLOQUE;//Offset dentro bloque final

	int cantidadBloques = iBloqueFinal - iBloqueInicial  + 1;
	//int desplazamiento = 0;

	int tamEscrito = 0;
	//Escribo bloque inicial
	int numBloqueInicial = nodo->p_indirectos[iBloqueInicial];
	int tamanioEscribirInicial;
	if(cantidadBloques == 1)
		tamanioEscribirInicial = tamanio;
	else
		tamanioEscribirInicial = TAM_BLOQUE - offsetBloqueInicial;
	tamEscrito += escribirBloqueDatos(numBloqueInicial, offsetBloqueInicial, tamanioEscribirInicial, datos);
	datos += tamanioEscribirInicial;

	//Escribo bloques intermedios
	for(int i = iBloqueInicial + 1; i < iBloqueFinal; i++){
		int numBloque = nodo->p_indirectos[i];
		tamEscrito += escribirBloqueDatos(numBloque, 0, TAM_BLOQUE, datos);
		datos += TAM_BLOQUE;
	}

	//Escribo bloque final
	if(cantidadBloques > 1){
		int numBloque = nodo->p_indirectos[iBloqueFinal];
		tamEscrito += escribirBloqueDatos(numBloque, 0, finEscrituraBloqueFinal, datos);
		datos += finEscrituraBloqueFinal;
	}

	free_nodo(nodo);
	return tamEscrito;
}

int escribirBloqueDatos(int numBloque, int offset, int size, void* dato){
	openFS();
	fseek(archivo_fs,numBloque * TAM_BLOQUE,SEEK_SET); //Hasta el primer byte del bloque
	fseek(archivo_fs,offset,SEEK_CUR); //Hasta el byte del offset

	int tamEscrito = fwrite(dato,sizeof(char),size,archivo_fs);
	closeFS();
	log_info(log_resultados, "<<< BLOQUES DE DATOS --> SE MODIFICA EL BLOQUE NUM: %d.", numBloque);
	return tamEscrito;
}

int leerBloqueDatos(int numBloque, int offset, int size, void* buf){
	openFS();
	fseek(archivo_fs,numBloque * TAM_BLOQUE,SEEK_SET); //Hasta el primer byte del bloque
	fseek(archivo_fs,offset,SEEK_CUR); //Hasta el byte del offset
	int tamLeido = fread(buf,sizeof(char),size,archivo_fs);
	closeFS();
	return tamLeido;
}

int cambiarTamanioArchivo(char* path, int tamanio){
	int numNodo = existeArchivo(path);
	t_nodo* nodo = obtenerNodo(numNodo);

	//Verificar si hay que agregar bloque, quitar o dejar como está
	int cantBloquesActual = ceil( (float)nodo->tam_archivo / TAM_BLOQUE);
	int cantBloquesNecesarios = ceil( (float)tamanio / TAM_BLOQUE);
	int cantBloquesAdicionales = cantBloquesNecesarios - cantBloquesActual;
	int resultado = 0;
	if(cantBloquesAdicionales > 0){
		//Agregar bloques
		if(hayBloquesLibres(cantBloquesAdicionales) && quedaEspacioEnArchivo(nodo, cantBloquesAdicionales)){
			int posPunteroOcupar = cantBloquesActual;
			for(int i = 0; i < cantBloquesAdicionales; i++){
				int numBloque = ocuparBloqueLibreBitmap();
				nodo->p_indirectos[posPunteroOcupar + i] = numBloque;
			}
			nodo->tam_archivo = tamanio;
			persistirNodo(numNodo, nodo);
		}else resultado = ERROR;
	}else{
		if(cantBloquesAdicionales < 0){
			//Borrar bloques
			cantBloquesAdicionales = cantBloquesAdicionales * -1;
			int posPunteroBorrar = cantBloquesActual - 1;
			for(int i = 0; i < cantBloquesAdicionales; i++){
				desocuparBloqueBitmap(nodo->p_indirectos[posPunteroBorrar - i]);
				nodo->p_indirectos[posPunteroBorrar - i] = NULL; //Indica desocupado
			}
			nodo->tam_archivo = tamanio;
			persistirNodo(numNodo, nodo);
		}else{
			//No hacer nada
			if(tamanio > nodo->tam_archivo){
				nodo->tam_archivo = tamanio;
			}else{
				nodo->tam_archivo = tamanio;
			}
			persistirNodo(numNodo, nodo);
		}
	}
	free_nodo(nodo);
	return resultado;
}

int modificarFechas(char* path, struct timespec tiempoCreacion, struct timespec tiempoModificacion){
	int numNodo = existeArchivo(path);
	if(numNodo == ERROR)
		return -ENOENT;
	t_nodo* nodo = obtenerNodo(numNodo);
	nodo->fecha_creacion = tiempoCreacion;
	nodo->fecha_modificacion = tiempoModificacion;
	persistirNodo(numNodo, nodo);
	free_nodo(nodo);

	return 0;
}

//No es necesario chequear si ya existía el directorio (lo hace fuse usando getattr)
int crearNodoDirectorioArchivo(char* path, int esDirectorio){
	int numeroNodoLibre = buscarNodoLibre();
	//No hay espacio para guardar nodo
	if(numeroNodoLibre == ERROR){
		return -EDQUOT;
	}
	char* barra = malloc(2);
	strcpy(barra,"/");
	char** pathSeparado = string_split(path, barra);
	free(barra);
	int tam = sizeArrayChar(pathSeparado);
	char* nombreDir = pathSeparado[tam-1];
	if(string_length(nombreDir) > TAM_MAX_NOMBRE_ARCHIVO){
		free_char_as_as(pathSeparado);
		return ENAMETOOLONG;
	}
	char* pathPadre = cortarPathPadre(path);
	int numPadre = existeArchivo(pathPadre);

	//Se crea y persiste el nodo
	t_nodo* nodo = crearNodoVacio();
	if(esDirectorio == 1)
		nodo->estado = 2;
	else nodo->estado = 1;
	strcpy(nodo->nombre_archivo, nombreDir);
	nodo->bloque_padre = numPadre;
	gettimeofday(&(nodo->fecha_creacion), NULL);
	gettimeofday(&(nodo->fecha_modificacion), NULL);
	persistirNodo(numeroNodoLibre, nodo);
	free_nodo(nodo);
	free_char_as_as(pathSeparado);
	free(pathPadre);
	return 0;
}

char* cortarPathPadre(char* path){
	char* barra = malloc(2);
	strcpy(barra,"/");
	char** pathSeparado = string_split(path, barra);
	char* pathPadre = string_new();
	int tam = sizeArrayChar(pathSeparado);
	if(tam == 1){
		strcpy(pathPadre,"");
		free(barra);
		free_char_as_as(pathSeparado);
		return pathPadre; //Si se está intentando crear en el raiz.
	}

	for(int i = 0; i < tam - 1; i++){
		string_append(&pathPadre,barra);
		string_append(&pathPadre,pathSeparado[i]);
	}
	free(barra);
	free_char_as_as(pathSeparado);
	return pathPadre;
}

t_nodo* obtenerNodoDePath(char* path){
	int numNodo = existeArchivo(path);
	if(numNodo == ERROR) return NULL;
	return obtenerNodo(numNodo);
}

char* obtenerArchivosDeDirectorio(char* path){
	char* separador = malloc(2);
	strcpy(separador,";");
	char* punto = malloc(2);
	strcpy(punto,".");
	char* dosPuntos = malloc(3);
	strcpy(dosPuntos,"..");
	int numNodo = existeArchivo(path);
	char* nombresArchivos = string_new();
	if(numNodo == ERROR){
		free(separador);
		free(punto);
		free(dosPuntos);
		strcpy(nombresArchivos, "");
		return nombresArchivos;
	}
	string_append(&nombresArchivos,punto);
	string_append(&nombresArchivos,separador);
	string_append(&nombresArchivos,dosPuntos);
	for(int i = 0; i < TAM_TABLA_NODOS; i++){
		t_nodo* nodo = obtenerNodo(i);
		if(nodo->bloque_padre == numNodo){
			string_append(&nombresArchivos,separador);
			string_append(&nombresArchivos,nodo->nombre_archivo);
		}
		free_nodo(nodo);
	}
	free(separador);
	free(punto);
	free(dosPuntos);
	return nombresArchivos;

 }

//CONEXION CLIENTES----------------
void aceptarClientes(){

	int socket_escucha = crearSocketEscucha(configSac->listenPort, log_interno);

	int socket = 0;
	while((socket = aceptarCliente(socket_escucha, log_interno)) > 0){
		log_info(log_resultados, "<<< CONEXION CLIENTES --> SE CONECTA CLIENTE");
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

//---------------------------


//INICIALIZACION ------------------------

void formatearSAC(){
	openFS();
	fseek(archivo_fs, 0, SEEK_END); //Me paro al final del archivo
	long tamFS = ftell(archivo_fs); //Veo en que byte estoy parado
	fseek(archivo_fs, 0, SEEK_SET);

	//HEADER
	char* identif = malloc(4);
	strcpy(identif,"SAC");
	fwrite(identif,sizeof(char),4,archivo_fs); //Identificador
	free(identif);
	int version = 1;
	fwrite(&version,sizeof(int),1,archivo_fs); //Version
	int ini_bit = 1;
	fwrite(&ini_bit,sizeof(int),1,archivo_fs); //Bloque inicio bitmap
	long tam_byte_bitmap = tamFS / TAM_BLOQUE;
	int bloques_bm = ceil( (float)tam_byte_bitmap / (float)TAM_BLOQUE); //Cantidad bloques que ocupa el Bitmap
	fwrite(&bloques_bm,sizeof(int),1,archivo_fs); //Tamaño bitmap en bloque
	char* relleno = string_repeat('\0', 4080);
	fwrite(relleno,4080,1,archivo_fs); //Relleno*/
	free(relleno);
	//BITMAP
	char* bytesBitmap = string_repeat('\0', bloques_bm * TAM_BLOQUE);
	t_bitarray* bitmap = bitarray_create_with_mode(bytesBitmap,bloques_bm * TAM_BLOQUE,MSB_FIRST);
	bitarray_set_bit(bitmap, 0); //Seteo ocupado el bit del header
	for(int i = 1; i <= bloques_bm; i++){ //Seteo ocupado los bits del bitmap
		bitarray_set_bit(bitmap, i);
	}
	for(int i = 1 + bloques_bm; i <= 1 + bloques_bm + TAM_TABLA_NODOS - 1; i++){ //Seteo ocupado los bits de la tabla de nodos
		bitarray_set_bit(bitmap, i);
	}
	fwrite(bitmap->bitarray,1,bloques_bm * TAM_BLOQUE,archivo_fs);
	free_bitarray(bitmap);

	//TABLA DE NODOS
	//Agrego el nodo correspondiente a la raiz.
	t_nodo* nodoRaiz = crearNodoVacio();
	nodoRaiz->estado = 2;
	fwrite(&(nodoRaiz->estado),1,1,archivo_fs);
	strcpy(nodoRaiz->nombre_archivo, "");
	fwrite(nodoRaiz->nombre_archivo,sizeof(char),71,archivo_fs);
	nodoRaiz->bloque_padre = 0;
	fwrite(&(nodoRaiz->bloque_padre),sizeof(int),1,archivo_fs);
	fwrite(&(nodoRaiz->tam_archivo),sizeof(int),1,archivo_fs);
	fwrite(&(nodoRaiz->fecha_creacion),sizeof(struct timespec),1,archivo_fs);
	fwrite(&(nodoRaiz->fecha_modificacion),sizeof(struct timespec),1,archivo_fs);
	fwrite(nodoRaiz->p_indirectos,sizeof(int)*TAM_MAX_PUNT_IND,1,archivo_fs);
	free_nodo(nodoRaiz);
	//Agrego el resto de los nodos
	t_nodo* nodo= crearNodoVacio();
	for(int i = 1; i < TAM_TABLA_NODOS; i++){
		fwrite(&(nodo->estado),1,1,archivo_fs);
		fwrite(nodo->nombre_archivo,sizeof(char),71,archivo_fs);
		fwrite(&(nodo->bloque_padre),sizeof(int),1,archivo_fs);
		fwrite(&(nodo->tam_archivo),sizeof(int),1,archivo_fs);
		fwrite(&(nodo->fecha_creacion),sizeof(struct timespec),1,archivo_fs);
		fwrite(&(nodo->fecha_modificacion),sizeof(struct timespec),1,archivo_fs);
		fwrite(nodo->p_indirectos,sizeof(int)*TAM_MAX_PUNT_IND,1,archivo_fs);
	}
	free_nodo(nodo);

	//BLOQUES DE DATOS
	int tam_bloques_datos = (tamFS / TAM_BLOQUE) - 1 - TAM_TABLA_NODOS - bloques_bm;
	char* caracterVacio = string_repeat('\0', TAM_BLOQUE);
	for(int i = 0; i < tam_bloques_datos; i++){
		fwrite(caracterVacio,TAM_BLOQUE,1,archivo_fs);
	}
	free(caracterVacio);
	closeFS();
}

int noFormateado(){
	openFS();
	char* identifArchivo = malloc(TAM_IDENTIFICADOR);
	fread(identifArchivo,sizeof(char),TAM_IDENTIFICADOR,archivo_fs);
	int resultado = 1;
	if(strcmp(identifArchivo,"SAC") == 0)
		resultado = 0;
	free(identifArchivo);
	closeFS();
	return resultado;
}

void abrirHeaderFS(){
	/*long pos = ftell(archivo_fs);
	fseek(archivo_fs,3,SEEK_CUR); //Se mueve desde donde quedó.
	pos = ftell(archivo_fs);
	char* letra = malloc(sizeof(char));
	fread(letra,sizeof(char),1,archivo_fs);*/

	/*if(noFormateado() == 0){
		//formatearFS();
	}*/

	openFS();

	//Tamanio Archivo
	fseek(archivo_fs, 0, SEEK_END); //Me paro al final del archivo
	fs_header->T = ftell(archivo_fs); //Veo en que byte estoy parado
	fseek(archivo_fs, 0, SEEK_SET); //Vuelvo el puntero al primer byte para seguir trabajando

	//Identificador
		//identificador ya está allocado
	fread(fs_header->identificador,sizeof(char),TAM_IDENTIFICADOR,archivo_fs);
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
//---------------------------------


//TABLA DE NODOS -------------------

//Retorna uno de los nodos (metadata) de los 1024 archivos
t_nodo* obtenerNodo(int numeroNodo){
	openFS();
	t_nodo* nodo = malloc(sizeof(t_nodo));
	nodo->nombre_archivo = malloc(TAM_MAX_NOMBRE_ARCHIVO);
	fseek(archivo_fs, (fs_header->tam_bitmap + 1) * TAM_BLOQUE, SEEK_SET);
	fseek(archivo_fs, numeroNodo * TAM_BLOQUE, SEEK_CUR);
	fread(&(nodo->estado),sizeof(uint8_t),1,archivo_fs);
	fread(nodo->nombre_archivo,sizeof(char),TAM_MAX_NOMBRE_ARCHIVO,archivo_fs);
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

	log_info(log_resultados, "<<< TABLA DE NODOS --> SE MODIFICA NODO NUM: %d.", numeroNodo);

	closeFS();
}

int buscarNodoLibre(){
	for(int i = 0; i < TAM_TABLA_NODOS - 1; i++){
		//Me muevo hasta el inicio de la Tabla de Nodos. Luego cada 1, bloque, cada 2, cada 3, etc.
		t_nodo* nodo = obtenerNodo(i);
		if(nodo->estado == 0){
			free_nodo(nodo);
			return i;
		}
		free_nodo(nodo);
	}
	log_info(log_interno , "No quedan nodos libre.");
	return ERROR;
}

//Retorna el ID del nodo si existe o ERROR en caso de no existir.
int existeArchivo(char* path){
	char* barra = malloc(2);
	strcpy(barra, "/");
	char** pathSeparado = string_split(path, barra);
	free(barra);
	int tam = sizeArrayChar(pathSeparado);
	if(tam == 0){ //Si el contenido es barra sola devuelve 0 que es el id del bloque raiz
		free_char_as_as(pathSeparado);
		return 0;
	}
	for(int i = 0; i < TAM_TABLA_NODOS; i++){
		int i_path = tam - 1;
		t_nodo* nodo = obtenerNodo(i);

		if(nodo->estado != 0 && strcmp(pathSeparado[i_path], nodo->nombre_archivo) == 0){
			if(chequearNombrePadre(pathSeparado, i_path, nodo->bloque_padre) == 0){
				free_nodo(nodo);
				free_char_as_as(pathSeparado);
				return i;
			}
		}
		free_nodo(nodo);
	}
	free_char_as_as(pathSeparado);
	return ERROR;
}

int chequearNombrePadre(char** pathSeparado, int i_path, int padre){
	if(padre==0)
		return 0;
	t_nodo* nodo = obtenerNodo(padre);
	i_path--;
	if(nodo->estado != 0 && strcmp(pathSeparado[i_path], nodo->nombre_archivo) == 0){
		int resultado = chequearNombrePadre(pathSeparado, i_path, nodo->bloque_padre);
		free_nodo(nodo);
		return resultado;
	}
	free_nodo(nodo);
	return ERROR;
}

t_nodo* crearNodoVacio(){
	t_nodo* nodo = malloc(sizeof(t_nodo));
	nodo->estado = 0;
	//nodo->nombre_archivo = malloc(TAM_MAX_NOMBRE_ARCHIVO);
	nodo->nombre_archivo = string_repeat('\0', TAM_MAX_NOMBRE_ARCHIVO);
	nodo->bloque_padre = 0;
	nodo->tam_archivo = 0;
	gettimeofday(&(nodo->fecha_creacion), NULL);
	gettimeofday(&(nodo->fecha_modificacion), NULL);

	for(int i = 0 ; i < TAM_MAX_PUNT_IND; i++){
		nodo->p_indirectos[i] = NULL;
	}

	return nodo;
}

int quedaEspacioEnArchivo(t_nodo* nodo, int cantidad){
	int cantBloques = ceil( (float)nodo->tam_archivo / TAM_BLOQUE);
	if(cantBloques + cantidad <= TAM_MAX_PUNT_IND)
		return 1;
	else
		return 0;
}
//--------------------------------

//BITMAP -------------------
t_bitarray* obtenerBitmap(){
	openFS();
	int tamanio = fs_header->tam_bitmap * TAM_BLOQUE; //Tamanio de los bloques del bitmap
	char* bytesArch = malloc(tamanio);
	fseek(archivo_fs, TAM_BLOQUE, SEEK_SET); //Me desplazo hasta terminar el header

	fread(bytesArch,tamanio,1,archivo_fs);
	closeFS();
	t_bitarray* bitarray = bitarray_create_with_mode(bytesArch,tamanio,MSB_FIRST);

	return bitarray;
}

void persistirBitmap(t_bitarray* bitarray){
	openFS();
	fseek(archivo_fs, TAM_BLOQUE, SEEK_SET);
	fwrite(bitarray->bitarray,1,fs_header->tam_bitmap * TAM_BLOQUE,archivo_fs);
	closeFS();
}

int ocuparBloqueLibreBitmap(){
	t_bitarray* bitarray = obtenerBitmap();
	int id_primer_bloque_datos = fs_header->inicio_bloques_datos;
	int id_ultimo_bloque = 1 + fs_header->tam_bitmap + TAM_TABLA_NODOS + fs_header->tam_bloques_datos - 1;
	for(int i = id_primer_bloque_datos; i <= id_ultimo_bloque; i++){
		if(bitarray_test_bit(bitarray,i) == 0){
			bitarray_set_bit(bitarray, i);
			persistirBitmap(bitarray);
			log_info(log_resultados, "<<< BITMAP --> SE OCUPA BLOQUE NUM: %d.", i);
			free_bitarray(bitarray);
			return i;
		}
	}
	free_bitarray(bitarray);
	return ERROR;
}

int desocuparBloqueBitmap(int numBloque){
	int id_ultimo_bloque = 1 + fs_header->tam_bitmap + TAM_TABLA_NODOS + fs_header->tam_bloques_datos - 1;
	t_bitarray* bitarray = obtenerBitmap();
	if(numBloque > id_ultimo_bloque){
		//Numero de bloque no existente.
		free_bitarray(bitarray);
		return ERROR;
	}
	bitarray_clean_bit(bitarray, numBloque);
	persistirBitmap(bitarray);
	log_info(log_resultados, "<<< BITMAP --> SE DESOCUPA BLOQUE NUM: %d.", numBloque);
	free_bitarray(bitarray);
	return 0;
}

int hayBloquesLibres(int cantidad){
	t_bitarray* bitarray = obtenerBitmap();
	int id_primer_bloque_datos = fs_header->inicio_bloques_datos;
	int id_ultimo_bloque = 1 + fs_header->tam_bitmap + TAM_TABLA_NODOS + fs_header->tam_bloques_datos - 1;
	for(int i = id_primer_bloque_datos; i <= id_ultimo_bloque; i++){
		if(bitarray_test_bit(bitarray,i) == 0){
			cantidad--;
			if(cantidad == 0){
				free_bitarray(bitarray);
				return 1;
			}
		}
	}
	free_bitarray(bitarray);
	return 0;
}

//----------------------------

void inicializacion(){
	pthread_mutex_init(&m_acceso_fs, NULL);
	log_resultados = log_create("log_resultados.txt", "LOG-RES", true, LOG_LEVEL_INFO);
	log_info(log_resultados, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	log_info(log_interno, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	getConfigSAC();
	fs_header = malloc(sizeof(t_header));
	fs_header->identificador = malloc(TAM_IDENTIFICADOR);
}

void finalizar(){
	freeConfig(configSac);
	config_destroy(config);
	free_header(fs_header);
	log_destroy(log_resultados);
	log_destroy(log_interno);
}

void openFS(){
	archivo_fs = fopen(configSac->pathFs,"r+");
	//printf("--OPEN\n");
}

void closeFS(){
	fclose(archivo_fs);
	//printf("--CLOSE\n");
}

int sizeArrayChar(char** array){
	int tam = 0;
	while(array[tam] != NULL){
		tam++;
	}
	return tam;
}

void free_cliente(t_cliente* cliente){
	free(cliente);
}

void free_nodo(t_nodo* nodo){
	if(nodo == NULL) return;
	free(nodo->nombre_archivo);
	free(nodo);
}

void free_header(t_header* header){
	free(header->identificador);
	free(header);
}

void free_char_as_as(char** array){
	int i=0;
	while(array[i] != NULL){
		free(array[i]);
		i++;
	}
	free(array);
}

void free_bitarray(t_bitarray* bitarray){
	free(bitarray->bitarray);
	bitarray_destroy(bitarray);
}

int setPathFS(int argc, char* argv[]){
	//Validar ruta del archivo del FS
	if(argc >= 2){
		if(argv[1] == NULL){
			printf("***ERROR: Debe ingresar por parámetro una ruta de archivo de FS.\n");
			return ERROR;
		}

		FILE* arch = fopen(argv[1],"r+");
		if(arch == NULL){
			printf("***ERROR: La ruta ingresada no es de un archivo válido.\n");
			return ERROR;
		}
		fclose(arch);
		configSac->pathFs = malloc(string_length(argv[1])+1);
		strcpy(configSac->pathFs, argv[1]);
		printf("--------------------------------\n");
		printf("***PATH FS: %s ***\n", configSac->pathFs);
		printf("--------------------------------\n");
		return 1;
	}else{
		configSac->pathFs = malloc(string_length("/home/utnso/workspace/tp-2019-2c-capitulo-2/FileSystem/disco.bin")+1);
		strcpy(configSac->pathFs,"/home/utnso/workspace/tp-2019-2c-capitulo-2/FileSystem/disco.bin");
		printf("--------------------------------\n");
		printf("***PATH FS DEFAULT: /home/utnso/workspace/tp-2019-2c-capitulo-2/FileSystem/disco.bin ***\n");
		printf("--------------------------------\n");
		return 1;
	}

}

int main(int argc, char *argv[]){
	inicializacion();

	if(setPathFS(argc,argv) == ERROR){
		finalizar();
		return ERROR;
	}


	if(argv[2] != NULL && strcmp(argv[2], "-f") == 0){
		formatearSAC();
		printf("***Se formatea el FS***\n");
	}

	abrirHeaderFS();

	//int res = cambiarUbicacion("/BBB/abb.txt","/AAA/CCC/aaa.rar");
	//int res = cambiarUbicacion("/AAA/CCC/aaa.rar","/BBB/abb.txt");

	//int res = eliminarDirectorioArchivo("/BBB", 1);

	/*t_nodo* nodo = crearNodoVacio();
	nodo->estado = 2;
	nodo->bloque_padre = 3;
	strcpy(nodo->nombre_archivo,"KKK");
	nodo->tam_archivo = 0;
	persistirNodo(5,nodo);
	free_nodo(nodo);*/

	/*char* buf = malloc(6);
	int res = leerArchivo("/AAA/CCC/aaa.txt", 4100, 6, buf);
	printf("INFO: %s \n", buf);
	free(buf);*/


	/*char* datos = malloc(20);
	strcpy(datos,"acbdefghijklmnopqrs");*/

	//cambiarTamanioArchivo("/AAA/CCC/aaa.txt", 4000);
	/*char* datos = string_repeat('h', 4000);
	escribirArchivo("/AAA/CCC/aaa.txt", 0, 4000, datos);
	free(datos);*/


	//leerBloqueDatos(1027, 300, 20, datos);

	//escribirBloqueDatos(1027, 300, 20, datos);


	/*struct timespec fecha_cre;
	struct timespec fecha_mod;
	gettimeofday(&(fecha_cre), NULL);
	gettimeofday(&(fecha_mod), NULL);
	modificarFechas("AAA/CCC/mmm.txt", fecha_cre, fecha_mod);*/

	/*int numNodoLibre = buscarNodoLibre();
	t_nodo* nodo = obtenerNodo(numNodoLibre);*/

	//int existe = existeArchivo("/CCC/BBB/aaa.txt");


	//crearNodoDirectorioArchivo("/AAA", 1);

	//cambiarTamanioArchivo("/home/utnso/pipo/BBB/aaa.txt", 5000);
	//cambiarTamanioArchivo("/AAA/CCC/ppp.txt", 4296);

	/*char* datos;//
	datos = string_repeat('h', 4096);
	int tamEscrito = escribirArchivo("/AAA/CCC/ppp.txt", 0, 4096, datos);
	//int tamEscrito = escribirArchivo("/AAA/CCC/ppp.txt", 5095, 7242, datos);*/

	/*char* buf = malloc(16384);
	int tamLeido = leerArchivo("/AAA/CCC/ppp.txt", 4096, 12288, buf);*/

	/*void* infoBloqueA = malloc(TAM_BLOQUE);
	leerBloqueDatos(1026, 0, TAM_BLOQUE, infoBloqueA);*/

	//int res = existeArchivo("/AAA");
	//char* archivos = obtenerArchivosDeDirectorio("/AAA/CCC");

	//crearNodoDirectorioArchivo("/jjj.txt", 0);
	//cambiarTamanioArchivo("/jjj.txt", 4296);
	/*fseek(archivo_fs, (fs_header->inicio_bloques_datos) * TAM_BLOQUE + TAM_BLOQUE * 2, SEEK_SET);
	long pos = ftell(archivo_fs);
	char* unBloque = malloc(TAM_BLOQUE);
	fread(unBloque,TAM_BLOQUE,1,archivo_fs);*/
	//fseek(archivo_fs, numeroNodo * TAM_BLOQUE, SEEK_CUR);
	//long pos = ftell(archivo_fs);

	//char* archivos = obtenerArchivosDeDirectorio("/AAA");

	//t_nodo* nodo = crearNodoVacio();


	/*nodo->estado = 1;
	strcpy(&(nodo->nombre_archivo), "mmm.txt");
	nodo->bloque_padre = res;
	nodo->tam_archivo = 0;
	gettimeofday(&(nodo->fecha_modificacion), NULL);*/


	//persistirNodo(8, nodo);
	//int numeroNodoLibre = buscarNodoLibre();
	//persistirNodo(numeroNodoLibre, nodo);


	/*t_bitarray* bitarray = obtenerBitmap();
	ocuparBloqueLibreBitmap(bitarray);
	persistirBitmap(bitarray);*/

	aceptarClientes();

	finalizar();
}


