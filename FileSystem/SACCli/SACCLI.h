
#ifndef FILESYSTEM_SACCLI_H_
#define FILESYSTEM_SACCLI_H_
#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <conexion.h>

t_log* log_interno;
int socketServidor;

#define DEFAULT_FILE_CONTENT "Hello World!\n"

#define DEFAULT_FILE_NAME "hello"

#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME

struct t_runtime_options {
	char* welcome_msg;
} runtime_options;

int fuse_mkdir(const char *path, mode_t mode);
static int fuse_getattr(const char *path, struct stat *stbuf);
static int fuse_open(const char *path, struct fuse_file_info *fi);
static int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int fuse_unlink(const char *path);
static int fuse_rmdir(const char *path);

#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }

static struct fuse_operations fuse_oper = {
		.getattr = fuse_getattr,
		.readdir = fuse_readdir,
		.open = fuse_open,
		.read = fuse_read,
		.mkdir = fuse_mkdir,
		.rmdir = fuse_rmdir,
		.unlink = fuse_unlink
};

enum {
	KEY_VERSION,
	KEY_HELP,
};

static struct fuse_opt fuse_options[] = {
		// Este es un parametro definido por nosotros
		CUSTOM_FUSE_OPT_KEY("--welcome-msg %s", welcome_msg, 0),

		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};


#endif /* FILESYSTEM_SACCLI_H_ */
