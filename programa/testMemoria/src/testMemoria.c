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

char*  palabras_a_copiar(char* contenido){

		    int letra_final  = 0;
			int letra_actual = 0;

				while(contenido[letra_actual] != '"'){

					letra_actual++;
				}
					letra_actual++;
					letra_final = letra_actual;
					while(contenido[letra_final] != '"'){

						letra_final++;
				}

				char* palabra =	string_substring(contenido, letra_actual, letra_final-1);
				int len = letra_final -letra_actual;
				palabra[len] = '\0';

				return palabra;



}

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
		//break;
	}
	if( strcmp(palabras[0] ,"VER")==0){
        muse_enviar();
        //break;
	}
	if( strcmp(palabras[0] ,"FREE")==0){
		uint32_t pos = atoi(palabras[1]);
		muse_free(pos);
		//break;
	}
	if( strcmp(palabras[0] ,"COPY")==0){
		uint32_t pos = atoi(palabras[1]);
		int      n   = atoi(palabras[2]);

		char* frase = palabras_a_copiar(entrada);

		void* puntero;
		puntero = frase;

		int resultado =  muse_cpy(pos, puntero, n);
		printf("%d \n", resultado);
		//break;
	}
	if( strcmp(palabras[0] ,"GET")==0){
		uint32_t pos = atoi(palabras[1]);
		int      n   = atoi(palabras[2]);
		char* frase = malloc(n);
		void * puntero = frase;
		int result = muse_get(puntero, pos, n);
		//break;
	}


}
	//return EXIT_SUCCESS;
}
