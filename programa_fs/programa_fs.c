#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
	//FILE* arch = fopen("/home/utnso/pipo/AAA/CCC/ppp.txt","r+");
	FILE* arch = fopen("/home/utnso/ppp.txt","r+");
	fseek(arch, 3, SEEK_SET);
	char* texto = malloc(22);
	texto = "karmapalmatranzapanza";
	int pos = ftell(arch);
	fwrite(texto, sizeof(char), 21, arch);
	pos = ftell(arch);

	/*char* texto2 = malloc(13);
	texto2 = "BuenaPersona";
	fwrite(texto2, sizeof(char), 13, arch);
	pos = ftell(arch);*/

	fclose(arch);

	/*FILE* arch2 = fopen("/home/utnso/ppp.txt","r+");
	char* mensaje = malloc(16);
	pos = ftell(arch2);
	fread(mensaje,sizeof(char),16,arch2);
	pos = ftell(arch2);
	fclose(arch2);*/

	//truncate("/home/utnso/pipo/jjj.txt",4296);
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
}
