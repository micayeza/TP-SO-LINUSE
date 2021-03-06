#include "SACCLI.h"

//Describir directorios y archivos.
static int fuse_getattr(const char *path, struct stat *stbuf) {
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));

	//Si path es igual a "/" nos estan pidiendo los atributos del punto de montaje
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	}else{
		int resEnvioOperacion = enviarEntero(socketServidor, SYS_GETATTR,  log_interno);
		int resEnvioTexto = enviarTexto(socketServidor, path, log_interno);
		int estado = recibirEntero(socketServidor, log_interno);
		if(estado == 1){
			stbuf->st_mode = S_IFREG | 0777;
			stbuf->st_nlink = 1;
			int size = recibirEntero(socketServidor, log_interno);
			struct timespec timeCreacion = recibirTiempo(socketServidor, log_interno);
			struct timespec timeModificacion = recibirTiempo(socketServidor, log_interno);
			stbuf->st_size = size;
			stbuf->st_ctim = timeCreacion;
			stbuf->st_mtim = timeModificacion;
			log_info(log_resultados, "***GETATTR--> Path: %s", path);
			log_info(log_resultados, "      Tipo: archivo");
			log_info(log_resultados, "      Size: %d.", size);
			log_info(log_resultados, "      Time Creacion: %d", timeCreacion);
			log_info(log_resultados, "      Time Modificacion: %d", timeModificacion);
		}else{
			if(estado == 2){
				stbuf->st_mode = S_IFDIR | 0755;
				stbuf->st_nlink = 2;
				log_info(log_resultados, "***GETATTR--> Path: %s", path);
				log_info(log_resultados, "      Tipo: directorio");
			}else{
				log_info(log_resultados, "***GETATTR--> Path: %s NO ENCONTRADO", path);
				res = -ENOENT;
			}

		}
	}
	return res;
}

/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener la lista de archivos o directorios que se encuentra dentro de un directorio
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		buf - Este es un buffer donde se colocaran los nombres de los archivos y directorios
 * 		      que esten dentro del directorio indicado por el path
 * 		filler - Este es un puntero a una función, la cual sabe como guardar una cadena dentro
 * 		         del campo buf
 *
 * 	@RETURN
 * 		O directorio fue encontrado. -ENOENT directorio no encontrado
 */

//Listar directorios y sus archivos.
static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	(void) offset;
	(void) fi;

	int resEnvioOperacion = enviarEntero(socketServidor, SYS_READDIR,  log_interno);
	int resEnvioTexto = enviarTexto(socketServidor, path, log_interno);
	char* nombresArchivos = recibirTexto(socketServidor, log_interno);
	//El servidor retornará vacio cuando no existe el path
	if(strcmp(nombresArchivos, "") == 1){
		log_info(log_resultados, "***READDIR--> Path: %s PATH INEXISTENTE", path);
		free(nombresArchivos);
		return -ENOENT;
	}
	char* puntoComa = malloc(2);
	strcpy(puntoComa, ";");
	char** archivos = string_split(nombresArchivos, puntoComa);
	free(puntoComa);
	int tam = sizeArrayChar(archivos);
	for(int i = 0; i < tam; i++){
		filler(buf, archivos[i], NULL, 0);
	}
	log_info(log_resultados, "***READDIR--> Path: %s", path);
	log_info(log_resultados, "      Contenido: %d", nombresArchivos);
	free(nombresArchivos);
	free_char_as_as(archivos);

	return 0;
}

//Crear directorio
int fuse_mkdir(const char *path, mode_t mode) {
	int resEnvioOperacion = enviarEntero(socketServidor, SYS_MKDIR,  log_interno);
	int resEnvioTexto = enviarTexto(socketServidor, path, log_interno);
	int resultado = recibirEntero(socketServidor, log_interno);
	log_info(log_resultados, "***MKDIR--> Path: %s", path);
	return resultado;
}

static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

	//Seteo estructuras de envio
	enviarEntero(socketServidor, SYS_CREATE, log_interno);
	enviarTexto(socketServidor, path,  log_interno);
	int res = recibirEntero(socketServidor, log_interno);
	//Procesar respuesta
	if(res == EEXIST){
		log_info(log_resultados, "***CREATE--> Path: %s ERROR", path);
		return -EEXIST;

	}
	if (res == ENAMETOOLONG){
		log_info(log_resultados, "***CREATE--> Path: %s NOMBRE DE ARCHIVO MUY LARGO", path);
		return -ENAMETOOLONG;
	}
	if(res == EDQUOT){
		log_info(log_resultados, "***READDIR--> Path: %s ERROR", path);
		return -EDQUOT;
	}
	if(res == EACCES){
		log_info(log_resultados, "***CREATE--> Path: %s INACCESIBLE", path);
		return -EACCES;
	}
	log_info(log_resultados, "***CREATE--> Path: %s", path);
	return 0;

}

//Actualizacion de Fechas
static int fuse_utimens(const char *path, const struct timespec tv[2]) {
	enviarEntero(socketServidor, SYS_UTIMES, log_interno);
	enviarTexto(socketServidor, path, log_interno);
	enviarTiempo(socketServidor, tv[0],  log_interno);
	enviarTiempo(socketServidor, tv[1],  log_interno);
	int res = recibirEntero(socketServidor,  log_info);
	log_info(log_resultados, "***UTIMENS--> Path: %s", path);
	log_info(log_resultados, "      Time Creacion: %d", tv[0]);
	log_info(log_resultados, "      Time Modificacion: %d", tv[1]);

	return res;
}


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para tratar de abrir un archivo
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		fi - es una estructura que contiene la metadata del archivo indicado en el path
 *
 * 	@RETURN
 * 		O archivo fue encontrado. -EACCES archivo no es accesible
 */
static int fuse_open(const char *path, struct fuse_file_info *fi) {

	return 0;
}

/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener el contenido de un archivo
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		buf - Este es el buffer donde se va a guardar el contenido solicitado
 * 		size - Nos indica cuanto tenemos que leer
 * 		offset - A partir de que posicion del archivo tenemos que leer
 *
 * 	@RETURN
 * 		Si se usa el parametro direct_io los valores de retorno son 0 si  elarchivo fue encontrado
 * 		o -ENOENT si ocurrio un error. Si el parametro direct_io no esta presente se retorna
 * 		la cantidad de bytes leidos o -ENOENT si ocurrio un error. ( Este comportamiento es igual
 * 		para la funcion write )
 */

//Leer archivo
static int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	int res = enviarEntero(socketServidor, SYS_READ,  log_interno);

	res = enviarTexto(socketServidor, path, log_interno);
	res = enviarEntero(socketServidor, offset,  log_interno);
	res = enviarEntero(socketServidor, size,  log_interno);
	//res = enviarDatos(socketServidor, (void*) buf, size,  log_interno);

	res = recibirDatos(socketServidor, buf,log_interno);
	res = recibirEntero(socketServidor, log_interno);
	log_info(log_resultados, "***READ--> Path: %s", path);
	//log_info(log_resultados, "      Datos: %s.", (char*)buf);
	log_info(log_resultados, "      Size: %d", res);

	return res;
}

//Borrar archivo
int fuse_unlink(const char *path) {
	int res = enviarEntero(socketServidor, SYS_UNLINK,  log_interno);
	res = enviarTexto(socketServidor, path, log_interno);
	res = recibirEntero(socketServidor, log_interno);

	if(res == EACCES){
		log_info(log_resultados, "***UNLINK--> Path: %s INACCESIBLE", path);
		return -EACCES;
	}

	log_info(log_resultados, "***UNLINK--> Path: %s", path);
	return 0;
}

//Borrrar directorio
static int fuse_rmdir(const char *path) {
	int res = enviarEntero(socketServidor, SYS_RMDIR,  log_interno);
	res = enviarTexto(socketServidor, path, log_interno);
	res = recibirEntero(socketServidor, log_interno);

	if(res == ENOTEMPTY){
		log_info(log_resultados, "***RMDIR--> Path: %s NO VACIO", path);
		return -ENOTEMPTY;
	}
	if(res == EACCES){
		log_info(log_resultados, "***RMDIR--> Path: %s INACCESIBLE", path);
		return -EACCES;
	}

	log_info(log_resultados, "***RMDIR--> Path: %s", path);
	return 0;
}

static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){

	int res = enviarEntero(socketServidor, SYS_WRITE,  log_interno);

	res = enviarTexto(socketServidor, path, log_interno);
	res = enviarEntero(socketServidor, offset,  log_interno);
	res = enviarEntero(socketServidor, size,  log_interno);
	res = enviarDatos(socketServidor, (void*) buf, size,  log_interno);

	res = recibirEntero(socketServidor, log_interno);

	if(res == EDQUOT){
		log_info(log_resultados, "***WRITE--> Path: %s NO HAY ESPACIO EN EL ARCHIVO", path);
		return -EDQUOT;
	}

	log_info(log_resultados, "***WRITE--> Path: %s", path);
	//log_info(log_resultados, "      Datos: %s.", (char*)buf);
	log_info(log_resultados, "      Size: %d", res);

	return res;
}

static int fuse_move(const char* path, const char *newPath) {
	int res = enviarEntero(socketServidor, SYS_MOVE,  log_interno);
	res = enviarTexto(socketServidor, path, log_interno);
	res = enviarTexto(socketServidor, newPath, log_interno);

	res = recibirEntero(socketServidor, log_interno);

	if(res == EEXIST){
		log_info(log_resultados, "***MOVE--> ERROR");
		return -EEXIST;
	}
	if (res == ENAMETOOLONG){
		log_info(log_resultados, "***MOVE--> NOMBRE MUY LARGO");
		return -ENAMETOOLONG;
	}
	if(res == EACCES){
		log_info(log_resultados, "***MOVE--> PATH INACCESIBLE");
		return -EACCES;
	}

	log_info(log_resultados, "***MOVE--> Path: %s", path);
	log_info(log_resultados, "      New Path: %s", newPath);
	return 0;

}

static int fuse_chmod(const char *path, struct fuse_file_info *fi) {

	return 0;
}

//No hay que chequear la existencia, ya que antes del truncate se ejecuta Getattr
//y si no existe el archivo Fuse no ejecuta truncate.
static int fuse_truncate(const char *path, off_t tamanio) {
	int res = enviarEntero(socketServidor, SYS_TRUNCATE, log_interno);

	res = enviarTexto(socketServidor, path, log_interno);
	res = enviarEntero(socketServidor, tamanio,  log_interno);
	if(res >0){
		res = recibirEntero(socketServidor, log_interno);
		log_info(log_resultados, "***TRUNCATE--> Path: %s", path);
		log_info(log_resultados, "      Size: %d", tamanio);
	}
	return res;

}

int sizeArrayChar(char** array){
	int tam = 0;
	while(array[tam] != NULL){
		tam++;
	}
	return tam;
}

void free_char_as_as(char** array){
	int i=0;
	while(array[i] != NULL){
		free(array[i]);
		i++;
	}
	free(array);
}

int main(int argc, char *argv[]) {

	if(argv[1] == NULL){
		argc = 4;
		argv[0] = "./SACCli";
		argv[1] = "/home/utnso/pipo/";
		argv[2] = "-d";
		argv[3] = "-s";
	}
	printf("--------------------------------\n");
	printf("FS levantado en: %s \n",argv[1]);
	printf("--------------------------------\n");

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
		/** error parsing options */
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	if( runtime_options.welcome_msg != NULL ){
		printf("%s\n", runtime_options.welcome_msg);
	}

	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	log_resultados = log_create("log_resultados.txt", "LOG-RES", true, LOG_LEVEL_INFO);
	socketServidor = crearSocketCliente("127.0.0.1", 5003, log_interno);
	//Enviar mensaje con un INIT para hacerme conocer al SACServer. No recibo respuesta.
	int resOperacion = enviarEntero(socketServidor, INIT,  log_interno);

	return fuse_main(args.argc, args.argv, &fuse_oper, NULL);
}
