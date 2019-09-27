#include "hilolay.h"

//La parte de Hilolay que interactúa con los programas ya se encuentra desarrollada por la cátedra.
//Acá deberíamos implementar la parte que se conecta con nuestro SUSE.

/*_suse_init: Esta función es la encargada de inicializar todos los recursos de la biblioteca.
_suse_create: Esta función es invocada cuando se necesita crear un nuevo hilo, donde la función que se pase por parámetro actuará como main del mismo, finalizando el hilo al terminar esa función.
_suse_schedule_next: Obtiene el próximo hilo a ejecutar.
_suse_wait: Genera una operación de wait sobre el semáforo dado.
_suse_signal: Genera una operación de signal sobre el semáforo dado.
_suse_join: Bloquea al thread esperando que el mismo termine. El thread actual pasará a estar BLOCKED y saldrá del mismo luego de que el PID indicado finalice su ejecución. También es posible realizar un join a un thread ya finalizado.*/
