#include "hilolay_alumnos.h"


int suse_create(int tid){
	printf("Entro en nuestro SUSE Create");
	return 0;
}

static struct hilolay_operations hiloops = {
		.suse_create = &suse_create,
		.suse_schedule_next = &suse_create,
		.suse_join = &suse_create,
		.suse_close = &suse_create
};

void hilolay_init(void){
	init_internal(&hiloops);
}
