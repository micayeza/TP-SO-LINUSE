/*
 * TablaDeNodos.c
 *
 *  Created on: 19 sep. 2019
 *      Author: utnso
 */
#include "Bloques.h";
#include <stdio.h>



int BuscarArchivo(char* nombreArchivo){

}


//DEVUELVE LA TABLA DE NODOS
TablaNodos* TablaNodosAddr(){
	return TablaDeNodos;
}

GFile* TablaNodos_BuscarArchivo(char* path){
	int i = 0;
	while(i<1024){
		if(string_equals_ignore_case(path,TablaDeNodos->archivos[i])){
			return TablaDeNodos->archivos[i];
		}
	}
	return NULL;
}


