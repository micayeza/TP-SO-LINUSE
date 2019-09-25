#include "SUSE.h"


//Hice esta función para que cada vez que tengamos que leer la config nos genere este struct y así actualizar los valores en el momento que los necesitamos.
configSUSE* getConfigSUSE(char* path){

	t_config* config = config_create(path);
	configSUSE* structConfig = malloc(sizeof(configSUSE));

	structConfig->listenPort = config_get_int_value(config, "LISTEN_PORT"); //int
	structConfig->metricsTimer = config_get_int_value(config, "METRICS_TIMER"); //int
	structConfig->maxMultiprog = config_get_int_value(config, "MAX_MULTIPROG"); //int
	structConfig->semId = config_get_array_value(config, "SEM_IDS"); //array
	structConfig->semInit = config_get_array_value(config, "SEM_INIT"); //array
	structConfig->semMax = config_get_array_value(config, "SEM_MAX"); //array
	structConfig->alphaSJF = config_get_double_value(config, "ALPHA_SJF"); //double

	config_destroy(config);

	return structConfig;
}

void freeConfig(configSUSE* config){

	freeCharArray(config->semId);
	freeCharArray(config->semInit);
	freeCharArray(config->semMax);

	free(config);
}

/*typedef struct {
	int listenPort; //Puerto TCP utilizado para recibir las conexiones de CPU y I/O.
	int metricsTimer; //Intervalo de tiempo en segundos entre cada loggeo de métricas.
	int maxMultiprog; //Grado máximo de Multiprogramación del Sistema.
	char** semId; //Identificador alfanumérico de cada semáforo del sistema. Cada posición del array representa un semáforo.
	char** semInit; //Valor inicial de cada semáforo definido en SEM_IDS, según su posición.
	char** semMax; //Valor máximo de cada semáforo definido en SEM_IDS, según su posición.
	int alphaSJF; //Parámetro de peso relativo del algoritmo SJF
} configSUSE;

LISTEN_PORT
METRICS_TIMER
MAX_MULTIPROG
SEM_IDS
SEM_INIT
SEM_MAX
ALPHA_SJF
*/
