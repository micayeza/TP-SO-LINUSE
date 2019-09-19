/*
 * TablaDeNodos.h
 *
 *  Created on: 19 sep. 2019
 *      Author: utnso
 */

#ifndef TABLADENODOS_H_
#define TABLADENODOS_H_

#include "Bloques.h"


typedef struct GFile{
	int Estado;
	char NombreArchivo[71];
	//BloquePadre
	unsigned long TamanioArchivo;
	unsigned long FechaCreacion;
	unsigned long FechaModificacion;
	//ARRAY PUNTEROS
}GFile;

typedef struct TablaNodos{
	GFile archivos[1024];

}TablaNodos;



#endif /* TABLADENODOS_H_ */
