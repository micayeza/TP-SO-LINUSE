
#ifndef FILESYSTEM_SACCLI_H_
#define FILESYSTEM_SACCLI_H_


#include <stdint.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <commons/log.h>
#include <fuse/fuse_opt.h>
#include <fuse/fuse.h>
#include <stdlib.h>
#include <conexion.h>

int g_socketSAC = -1;

t_log* log_interno;
int socketServidor;

//FUNCIONES FUSE
int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int fuse_open(const char *path, struct fuse_file_info *fi);
static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int fuse_mkdir(const char *path, mode_t mode);
int fuse_unlink(const char *path);
static int fuse_rmdir(const char *path);

struct t_runtime_options {
	char* welcome_msg;
} runtime_options;

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



#endif /* FILESYSTEM_SACCLI_H_ */
