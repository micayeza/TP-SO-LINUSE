#include <stdio.h>
#include <string.h>
#include <hilolay.h>

void *func1(void *arg) {
	int a = 1;
	a++;
}

int main(void) {
	hilolay_init();
	struct hilolay_t th1;

	hilolay_create(&th1, NULL, &func1, NULL);
	hilolay_join(&th1);
}
