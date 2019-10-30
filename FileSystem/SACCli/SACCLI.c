/*
 * SACCLI.c
 *
 *  Created on: 16 oct. 2019
 *      Author: utnso
 */


/*
 * SACServer.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#include "SacCLI.h"


#define DEFAULT_FILE_CONTENT "Hello World!\n"
#define DEFAULT_FILE_NAME "hello"
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME

int g_socketSAC = -1;
t_log* g_logger;

struct t_runtime_options {
	char* welcome_msg;
} runtime_options;


#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }



 int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	 MensajeFUSE* mensaje;
	 mensaje->syscall = 3;
	 strcpy(mensaje->path,path);
	 free(path);
	 mensaje->buff = buf;
	 memcpy(mensaje->size,size);




}



 int fuse_open(const char *path, struct fuse_file_info *fi) {

}

static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){


}

int fuse_mkdir(const char *path, mode_t mode) {
	 MensajeFUSE* mensaje;
	 mensaje->syscall = 3;
	 strcpy(mensaje->path,path);
	 free(path);
	 mensaje->buff = buf;
	 memcpy(mensaje->size,size);
}


//BORRAR ARCHIVO
 int fuse_unlink(const char *path) {

}

static int fuse_rmdir(const char *path) {

}




static struct fuse_operations operations = {
		.open = fuse_open,
		.read = fuse_read,
		.write = fuse_write,
		.mkdir = fuse_mkdir,
		.unlink = fuse_unlink,
		.rmdir = fuse_rmdir,
		//.getattr = fuse_getattr

};

enum {
	KEY_VERSION,
	KEY_HELP,
};

struct fuse_opt fuse_options[]= {
		FUSE_OPT_KEY("-V",KEY_VERSION),
		FUSE_OPT_KEY("--version",KEY_VERSION),
		FUSE_OPT_KEY("-h","KEY_HELP"),
		FUSE_OPT_END,

};


int conexionInicial(){
	char* ip = "127.0.0.1";
	int puerto = 8085;
	//leer puerto e ip del config
	g_logger = log_create("log.h","SACCLI",true,LOG_LEVEL_INFO);
		int cliente;
		struct sockaddr_in direccionServidor;

		direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
		direccionServidor.sin_addr.s_addr = inet_addr(ip);
		direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
		memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura

		cliente = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP

		if (cliente == -1) {
			//perror("No se pudo crear el file descriptor.\n");
			return -1;
		}

		int valorConnect = connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

		if(valorConnect == -1)  {
				return -1;
			}
		else {
			g_socketSAC = cliente;
			return cliente;
		}
}





int main(int argc, char*argv[]){

	struct fuse_args args = FUSE_ARGS_INIT(argc,argv);

	if(fuse_opt_parse(&args,&runtime_options,fuse_options,NULL)==-1){
		perror("Argumentos invalidos");
		return EXIT_FAILURE;
	}

	int sock = conexionInicial();
	if(sock!=-1){
		return fuse_main(args.argc,args.argv,&operations,NULL);
	}

}


