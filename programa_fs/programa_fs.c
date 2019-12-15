#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
	//FILE* aaa = fopen("/home/utnso/pppp.txt","w+");

	truncate("/home/utnso/pipo/ggg.yt",11);
	/*FILE* arch = fopen("/home/utnso/ggg.yt","w+");
	ftruncate(arch,120);
	fclose(arch);
	char* texto = malloc(4);
	texto = "mal";
	fwrite(texto, sizeof(char), 4, arch);
	int pos = ftell(arch);
	printf("Posicion: %d", pos);

	char* texto2 = malloc(5);
	texto2 = "saco";
	fwrite(texto2, sizeof(char), 5, arch);
	pos = ftell(arch);
	printf("Posicion: %d", pos);*/
	//fclose(aaa);
}
