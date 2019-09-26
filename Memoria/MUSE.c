/*
 * MUSE.c
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */
#include "MUSE.h"

bool existeArchivoConfig(char* ruta) {
	FILE * file = fopen(ruta, "r");
	if (file!=NULL){
		fclose(file);
		return true;//
	} else {
	return false;
	}
}

int crearConfigMemoria(){

	logMuse = log_create("MUSE.txt", "LOG", true, LOG_LEVEL_INFO);

    log_info(logMuse, "Se creó correctamente el Log. \n");

	//Valido que exista la ruta de configuracion

	if (!existeArchivoConfig(rutaConfigMuse)) {
		log_error(logMuse, "Verificar path del archivo \n");
	    return -1;
	}
	//Voy a levantar el archivo y guardar los valores en mi work area

	    	t_configuracion *config_muse = malloc(sizeof(t_configuracion));
	        config_ruta = config_create(rutaConfigMuse);

	        if (config_ruta != NULL){

	        	config_muse->puerto 	    = config_get_int_value(config_ruta, "LISTEN_PORT");
	        	config_muse->tamanio_total 	= config_get_int_value(config_ruta, "MEMORY_SIZE");
	        	config_muse->tamanio_pagina	= config_get_int_value(config_ruta, "PAGE_SIZE");
	        	config_muse->tamanio_swap 	= config_get_int_value(config_ruta, "SWAP_SIZE");


	        }

	        if(config_muse == NULL){
	        	log_error(logMuse, "ERROR No se pudo levantar el archivo de configuración de la MUSE \n");
	        	return -1;
	        }

	        return 0;


}

	int main(void) {

		remove("MUSE.txt");
		int v_error = crearConfigMemoria();
		if(v_error == 0){







		} //esta llave cierra luego de ver que se levanto ok el config
	}
