/*
 * MUSE.h
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */

#ifndef MUSE_H_
#define MUSE_H_

#include <commons/log.h>
#include <commons/config.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


#define rutaConfigMuse "../configs/muse.cfg"

typedef struct {
	int puerto;
	int tamanio_total;
	int tamanio_pagina;
	int tamanio_swap;

} t_configuracion;

t_configuracion *config_muse;
t_config *config_ruta;
t_log *logMuse;



bool existeArchivoConfig(char* ruta);
int crearConfigMemoria();



#endif /* MUSE_H_ */
