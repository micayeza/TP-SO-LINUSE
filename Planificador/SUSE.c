#include "SUSE.h"

void inicializacion(){
	configPath = string_new();
	string_append(&configPath, "../configs/SUSE.config");
}

void finalizacion(){
	free(configPath);
}

int main(void) {

	inicializacion();
	configSUSE* config = getConfigSUSE(configPath);

	//Hago cosas con la config...

	freeConfig(config);
	finalizacion();
}
