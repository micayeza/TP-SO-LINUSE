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
		}else{
			if(estado == 2){
				stbuf->st_mode = S_IFDIR | 0755;
				stbuf->st_nlink = 2;
			}else{
				res = -ENOENT;
			}

		}
		printf("CLIENTE: UN GETATTR. \n");
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
		return -ENOENT;
	}
	char** archivos = string_split(nombresArchivos, ";");
	int tam = sizeArrayChar(archivos);
	for(int i = 0; i < tam; i++){
		filler(buf, archivos[i], NULL, 0);
	}

	return 0;
}

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
	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

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
	size_t len;
	(void) fi;
	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
		return -ENOENT;

	len = strlen(DEFAULT_FILE_CONTENT);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, DEFAULT_FILE_CONTENT + offset, size);
	} else
		size = 0;

	return size;
}

//Crear directorio
int fuse_mkdir(const char *path, mode_t mode) {
	int resEnvioOperacion = enviarEntero(socketServidor, SYS_MKDIR,  log_interno);
	int resEnvioTexto = enviarTexto(socketServidor, path, log_interno);
	printf("CLIENTE: UN MKDIR. \n");
	int resultado = recibirEntero(socketServidor, log_interno);
	return resultado;
}

//Borrar archivo
int fuse_unlink(const char *path) {
	int resEnvioOperacion = enviarEntero(socketServidor, SYS_UNLINK,  log_interno);
	int resEnvioTexto = enviarTexto(socketServidor, path, log_interno);
	return recibirEntero(socketServidor, log_interno);
}

//Borrrar directorio
static int fuse_rmdir(const char *path) {
	int res = enviarEntero(socketServidor, SYS_RMDIR, log_interno);
	    res = enviarTexto(socketServidor, path, log_interno);
	return recibirEntero(socketServidor, log_interno);
}

static int fuse_utimens(const char *path, const struct timespec tv[2]) {
	enviarEntero(socketServidor, SYS_UTIMES, log_interno);
	enviarTexto(socketServidor, path, log_interno);
	enviarTiempo(socketServidor, tv[0],  log_interno);
	enviarTiempo(socketServidor, tv[1],  log_interno);
	int res = recibirEntero(socketServidor,  log_info);

	return res;
}

static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){

	int res = enviarEntero(socketServidor, SYS_WRITE,  log_interno);

	    res = enviarTexto(socketServidor, path, log_interno);
	    res = enviarTexto(socketServidor,  buf,  log_interno);
	    res = send(socketServidor, &size, sizeof(size_t), MSG_WAITALL);
	    res = send(socketServidor, &offset, sizeof(off_t),MSG_WAITALL);

	    res = recibirEntero(socketServidor,  log_info);

	if(res == ENOSPC){
		return -ENOSPC;
	}

	 if(res==ENOENT){

    	 return -ENOENT;

	 }

	return size;
}

static int fuse_move(const char* path, const char *newPath) {

	//Path origen
	int res = enviarTexto(socketServidor, path, log_interno);
	    res = enviarTexto(socketServidor, newPath, log_interno);

	//Recibir respuesta
	    res = recibirEntero(socketServidor, log_interno);

	//Procesar respuesta
	    	if(res == EEXIST){
	    		return -EEXIST;

			}
			if (res == ENAMETOOLONG){

				return -ENAMETOOLONG;
			}
			if(res == EACCES){
				return -EACCES;

			}
	//Libero todas las estructuras


	return 0;

}

static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

	//Seteo estructuras de envio
	enviarEntero(socketServidor, SYS_CREATE, log_interno);
	enviarTexto(socketServidor, path,  log_interno);
	int res = recibirEntero(socketServidor, log_interno);
	//Procesar respuesta
	if(res == EEXIST){

		return -EEXIST;

	}
	if (res == ENAMETOOLONG){

		return -ENAMETOOLONG;
	}
	if(res == EDQUOT){

		return -EDQUOT;
	}
	if(res == EACCES){

		return -EACCES;
	}
	return 0;

}


static int fuse_chmod(const char *path, struct fuse_file_info *fi) {
	int a = 1;
	return 0;
}

static int fuse_truncate(const char *path, off_t offset) {
	int res = enviarEntero(socketServidor, SYS_TRUNCATE, log_interno);

	res = enviarTexto(socketServidor, path, log_interno);
	res = send(socketServidor, &offset, sizeof(off_t), MSG_WAITALL);
	if(res >0){
		res = recibirEntero(socketServidor, log_interno);
	//Respuesta del servidor

	if(res == -1){ //Archivo muy grande como para truncarlo

		return -EFBIG;
	}

}
	return 0;

}

int sizeArrayChar(char** array){
	int tam = 0;
	while(array[tam] != NULL){
		tam++;
	}
	return tam;
}

int main(int argc, char *argv[]) {
	argc = 4;
	argv[0] = "./SACCli";
	argv[1] = "/home/utnso/pipo/";
	argv[2] = "-d";
	argv[3] = "-s";
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
	socketServidor = crearSocketCliente("127.0.0.1", 5003, log_interno);
	//Enviar mensaje con un INIT para hacerme conocer al SACServer. No recibo respuesta.
	int resOperacion = enviarEntero(socketServidor, INIT,  log_interno);

	return fuse_main(args.argc, args.argv, &fuse_oper, NULL);
}
