#include "SACServer.h"


//Hice esta función para que cada vez que tengamos que leer la config nos genere este struct y así actualizar los valores en el momento que los necesitamos.
t_configSAC* getConfigSAC(char* path){

	t_config* config = config_create(path);
	t_configSAC* structConfig = malloc(sizeof(t_configSAC));

	structConfig->listenPort = config_get_int_value(config, "LISTEN_PORT"); //int

	config_destroy(config);

	return structConfig;
}

void freeConfig(t_configSAC* config){
	free(config);
}
