#include "SACCLI.h"


#define DEFAULT_FILE_CONTENT "Hello World!\n"
#define DEFAULT_FILE_NAME "hello"
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME


#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }



 int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	 /*MensajeFUSE* mensaje;
	 mensaje->syscall = 3;
	 strcpy(mensaje->path,path);
	 free(path);
	 mensaje->buff = buf;
	 memcpy(mensaje->size,size);*/
}



 int fuse_open(const char *path, struct fuse_file_info *fi) {

}

static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){


}

int fuse_mkdir(const char *path, mode_t mode) {
	 /*MensajeFUSE* mensaje;
	 mensaje->syscall = 3;
	 strcpy(mensaje->path,path);
	 free(path);
	 mensaje->buff = buf;
	 memcpy(mensaje->size,size);*/
}


//BORRAR ARCHIVO
int fuse_unlink(const char *path) {
	int resEnvioOperacion = enviarEntero(socketServidor, SYS_UNLINK,  log_interno);
	int resEnvioTexto = enviarTexto(socketServidor, path, log_interno);
	return recibirEntero(socketServidor, log_interno);
}

static int fuse_rmdir(const char *path) {

}


int main(int argc, char*argv[]){

	struct fuse_args args = FUSE_ARGS_INIT(argc,argv);

	if(fuse_opt_parse(&args,&runtime_options,fuse_options,NULL)==-1){
		perror("Argumentos invalidos");
		return EXIT_FAILURE;
	}

	log_interno = log_create("log_interno.txt", "LOG-INT", false, LOG_LEVEL_INFO);
	socketServidor = crearSocketCliente("127.0.0.1", 5003, log_interno);
	//Enviar mensaje con un INIT para hacerme conocer al SACServer. No recibo respuesta.
	int resOperacion = enviarEntero(socketServidor, INIT,  log_interno);
}


