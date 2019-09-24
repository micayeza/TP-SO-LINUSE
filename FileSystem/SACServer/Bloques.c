/*
 * Bloques.c
 *
 *  Created on: 19 sep. 2019
 *      Author: utnso
 */


#include "Bloques.h"



int BuscarPath(char* path){
	 unsigned long inodo;
	 //inodo= BUSCAR EN TABLA DE NODOS
	 //void* = ObtenerBloque(inodo);
	 char** listaDirectorio = string_split(path,"/");

}

//DEVUELVE POSICION DONDE TERMINA LA TABLA DE NODOS
void** BloqueAddr(){
	//DEVUELVE POSICION DONDE TERMINA LA TABLA DE NODOS
	//return (void**) pos bloque fijo + pos bitmap + pos tabla nodos
}


//DEVUELVE BLOQUE SEGUN INDEX
void* ObtenerBloque(unsigned long index){
	return (void*) (BloqueAddr() + 4096*index);
}

