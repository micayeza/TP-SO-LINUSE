#include "SACServer.h"


//Hice esta función para que cada vez que tengamos que leer la config nos genere este struct y así actualizar los valores en el momento que los necesitamos.
void getConfigSAC(){

	//Hago esto ya que la ubicacion del ejecutable es distinto si se debuggea o se ejecuta por consola
	FILE* filePathConsola = fopen(pathConfigConsola, "r+");
	if(filePathConsola != NULL){
		fclose(filePathConsola);
		config = config_create(pathConfigConsola);
	}else{
		config = config_create(pathConfigDebug);
	}

	configSac = malloc(sizeof(t_configSAC));

	configSac->listenPort = config_get_int_value(config, "LISTEN_PORT"); //int

}

void freeConfig(t_configSAC* config){
	free(config->pathFs);
	free(config);
}
