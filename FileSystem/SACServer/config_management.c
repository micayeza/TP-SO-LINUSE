#include "SACServer.h"


//Hice esta función para que cada vez que tengamos que leer la config nos genere este struct y así actualizar los valores en el momento que los necesitamos.
void getConfigSAC(char* path){

	config = config_create(path);
	configSac = malloc(sizeof(t_configSAC));

	configSac->listenPort = config_get_int_value(config, "LISTEN_PORT"); //int
	configSac->pathFs = config_get_string_value(config, "PATH_FS");

}

void freeConfig(t_configSAC* config){
	//free(config->pathFs);
	free(config);
}
