/*
 * SACServer.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#include "SACServer.h"

#define DEFAULT_FILE_CONTENT "Hello World!\n"
#define DEFAULT_FILE_NAME "hello"
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME

struct t_runtime_options {
	char* welcome_msg;
} runtime_options;


#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }



 int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	printf("fuse_read");
/*	unsigned long IndexPrimerBloque = BuscarPath(path);
	if(IndexPrimerBloque<0)
		return -ENOENT;
*/
	// bloque = BloqueAddr(IndexPrimerBloque)  OBTENER BLOQUE
	//VALIDAR SI TIENE INFO EN OTRO BLOQUE
	// size = size - offset ????
	//memcpy(buf,bloque+offset,size);


	//EJEMPLO SO


}



 int fuse_open(const char *path, struct fuse_file_info *fi) {
	//if (strcmp(path, DEFAULT_FILE_PATH) != 0)
		//return -ENOENT;
	printf("fuse_open");
	logger = log_create("archivo.txt","FUSE",true,LOG_LEVEL_INFO);
	log_info(logger,"OPEN");
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	/*unsigned long IndexPrimerBloque = BuscarPath(path);
	if(IndexPrimerBloque<0)
		return -ENOENT;
*/
	//return 0;


	//EJEMPLO SO

	/*if(strcmp(path,DEFAULT_FILE_PATH)!=0)
		return -EACCES;

	if((fi->flags & 3) !=O_RDONLY)
		return -EACCES;

	return 0;*/
}

static int fuse_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){


	/*unsigned long IndexPrimerBloque = BuscarPath(path);
	    if (IndexPrimerBloque < 0) {
	    	return -EISDIR;
	    		} */
	 //DEBERIA IR TODO ADENTRO DE UN FOR PARA CUANDO
	    //la long sea mayor que 4096(un bloque)
	    //bloque = BlockAddr(IndexPrimerBloque);
	   // if (offset + size > 4096) {
	        //CREO QUE DEBERIA BUSCAR OTRO BLOQUE MAS
	    //}
	//	memcpy(cur_block + offset, buf, size);

	//inode *cur_inode = InodoAddr(IndexPrimerBloque); ??
//	cur_inode->size_of = size;
	  //  return size;

}

int fuse_mkdir(const char *path, mode_t mode) {
	printf("MKDIR");
	printf("MKDIR");
	printf("MKDIR");
	printf("MKDIR");
	printf("MKDIR");
	printf("MKDIR");



	/*
	unsigned long IndexPrimerBloque = BuscarPath(path);
    if (IndexPrimerBloque >= 0) {
        return -EEXIST;
    }*/
    //BUSCAR PROX INODO LIBRE
    //int aval_idx = inode_bitmap_find_next_empty(inode_bitmap_addr());
    //if (aval_idx < 0) {
      //  return aval_idx; // ENOSPC: operation failed due to lack of memory or disk space
    //}
    // create the new inode ptr
   // inode *cur_inode = InodoAddr(aval_idx);



    //LEER TODO ESTO
    /*inode_init(cur_inode, mode, 1, 0);
    // update inode_bitmap
    inode_bitmap_addr()[aval_idx] = 1;

    // create the new iblock ptr
    directory *cur_dir = single_iblock_addr(aval_idx);

    // create the new directory ptr
    directory_init(cur_dir, slist_last(path)->data);
    // flush the dir ptr to disk
    iblocks_addr()[aval_idx] = cur_dir;
    // update the iblock_bitmap
    iblock_bitmap_addr()[aval_idx] = 1;

    // check iblock, if entry does not exist, then create it and put it to the given dir
    int new_entry_idx = add_dir_entry(path, aval_idx);
    if (new_entry_idx < 0) {
        // ERROR out
        return new_entry_idx;
    }

    */
    return -1;
}


//BORRAR ARCHIVO
 int fuse_unlink(const char *path) {
	printf("UNLINK");
	/*unsigned long IndexPrimerBloque = BuscarPath(path);
    if (IndexPrimerBloque < 0) {
        return -ENOENT;
    }*/

    //inode *cur_inode = InodoAddr(IndexPrimerBloque);
  //  if (!cur_inode->is_file) {
    //    return -EISDIR;
    // }


   // int rv = BorrarArchivo(path);


    //WHILE PARA LIBERAR TODOS LOS BLOQUES EN EL BITMAP
  /*  // set the bitmaps to be avaliable
    inode_bitmap_addr()[index] = 0;
    iblock_bitmap_addr()[index] = 0;
   */  //SETEAR LOS BLOQUES EN EL BITMAP COMO LIBRES
   // return rv;
}

static int fuse_rmdir(const char *path) {

	/*unsigned long IndexPrimerBloque = BuscarPath(path);
    if (IndexPrimerBloque < 0) {
        return IndexPrimerBloque; // ENOENT: path doesn't exist
    }
*/
    //inode *cur_inode = InodoAddr(IndexPrimerBloque);
   // if (cur_inode->is_file) {
     //   return -ENOTDIR;
    //}

   // int rv = BorrarArchivo(path);


    /*
    inode_bitmap_addr()[index] = 0;
    iblock_bitmap_addr()[index] = 0;
    */
    //return rv;
}




static struct fuse_operations operations = {
		.open = fuse_open,
		.read = fuse_read,
		.write = fuse_write,
		.mkdir = fuse_mkdir,
		.unlink = fuse_unlink,
		.rmdir = fuse_rmdir,
		//FALTA EL DESCRIBE

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



int main(int argc, char*argv[]){
	printf("______","%s");
	struct fuse_args args = FUSE_ARGS_INIT(argc,argv);

	if(fuse_opt_parse(&args,&runtime_options,fuse_options,NULL)==-1){
		perror("Argumentos invalidos");
		return EXIT_FAILURE;
	}
return fuse_main(args.argc,args.argv,&operations,NULL);
}


