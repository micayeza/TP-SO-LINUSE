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

int main(void) {
	puts("!!!Hola Mica!!!\n"); /* prints !!!Hello World!!! */

	int result = muse_init(1, "127.0.0.1", 5003);
	printf("%d \n", result);
	return EXIT_SUCCESS;
}
