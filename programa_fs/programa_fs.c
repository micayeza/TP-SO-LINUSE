#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	//fseek(arch, 3, SEEK_SET);
	//FILE* arch = open("/home/utnso/ppp.txt","r+");
	FILE* arch = fopen("/home/utnso/pipo/AAA/CCC/aaa.txt","r+");
	//char* texto = malloc(6);
	//texto = "karma";
	char* datos = string_repeat('h', 5000);
	fseek(arch,0,SEEK_SET);
	int tam = fwrite(datos, sizeof(char), 5000, arch);
	printf("Bytes escritura: %d \n", tam);
	fclose(arch);

	/*char* datos = string_repeat('h', 5000);
	//int pos = ftell(arch);
	//fseek(arch,0,SEEK_SET);
	//pos = ftell(arch);
	int tam = fwrite(datos, sizeof(char), 5000, arch);*/


	/*FILE* arch = fopen("/home/utnso/pipo/AAA/CCC/aaa.txt","r+");
	char* buf = malloc(5000);
	fseek(arch,4100,SEEK_SET);
	int res = fread(buf,sizeof(char), 5000, arch);
	printf("Bytes lectura: %s \n", buf);
	fclose(arch);

	arch = fopen("/home/utnso/pipo/AAA/CCC/aaa.txt","r+");
	fseek(arch,4100,SEEK_SET);
	res = fread(buf,sizeof(char), 5000, arch);
	printf("Bytes lectura: %s \n", buf);
	fclose(arch);

	arch = fopen("/home/utnso/pipo/AAA/CCC/aaa.txt","r+");
	fseek(arch,4100,SEEK_SET);
	res = fread(buf,sizeof(char), 5000, arch);
	printf("Bytes lectura: %s \n", buf);
	fclose(arch);*/


	/*char* buf = malloc(1000);
	fread(buf,sizeof(char), 1000, arch);*/

	/*char* texto2 = malloc(13);
	texto2 = "BuenaPersona";
	fwrite(texto2, sizeof(char), 13, arch);
	pos = ftell(arch);*/



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
