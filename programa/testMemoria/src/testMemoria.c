/*
 ============================================================================
 Name        : testMemoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <libmuse.h>
#include <readline/readline.h>
#include <readline/history.h>

char** descomponer_operacion(char* contenido){

	int cantidad_de_palabras = 0;
	for(int i = 0; i < strlen(contenido);i++){

		if(contenido[i]==' '){
			cantidad_de_palabras++;
		}

	}

	cantidad_de_palabras++;

	char** retorno = malloc(sizeof(char*) * (cantidad_de_palabras+1));

	int inicio_palabra = 0;
	int letra_actual = 0;

	for(int i = 0; i < cantidad_de_palabras; i++){

		int cantidad_letras = 0;


		while(contenido[letra_actual] != ' ' && strlen(contenido) > letra_actual){
			cantidad_letras++;
			letra_actual++;
		}
			letra_actual++;
		char* palabra = malloc(sizeof(char)*(cantidad_letras + 1));

		memcpy(palabra, contenido + inicio_palabra, cantidad_letras);
		palabra[cantidad_letras] = '\0';

		retorno[i] = palabra;

		//printf("Palabra actual: %s\n", palabra);

		inicio_palabra += cantidad_letras + 1;

	}

//	for(int i = 0; i < cantidad_de_palabras; i++){
//		printf("Palabra %d: %s\n", i, retorno[i]);

//	}
	retorno[cantidad_de_palabras] = NULL;
	return retorno;
}


int main(void) {
	puts("!!!Hola Mica!!!\n"); /* prints !!!Hello World!!! */

	int result = muse_init(1, "127.0.0.1", 5003);
	printf("%d \n", result);


	char* entrada;
	while(1){

	entrada = readline("Operación> ");
	add_history(entrada);
	entrada[strlen(entrada)] = '\0';

	char** palabras = descomponer_operacion(entrada);


	if( strcmp(palabras[0] ,"MALLOC")==0){
		uint32_t tam = atoi(palabras[1]);
		uint32_t resultado = muse_alloc(tam);
		printf("%d \n", resultado);

	}
	}

	return EXIT_SUCCESS;
}
