/*
 * MUSE.c
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */
#include "MUSE.h"

//MENSAJEEEEEEEEEEEEEEEEEEES

char** descomponer_auxiliar(char* auxiliar, int len){


	 int a = 0;
	 int cantidad_de_palabras = 0;
	 while(a<=len){
		 if(auxiliar[a] == '\0'){
			 if(auxiliar[a+1] == '\0'){
				 break;
			 }
			 cantidad_de_palabras ++;

		 }
		 a++;
	 }

		cantidad_de_palabras++;

		char** retorno = malloc(sizeof(char*) * (cantidad_de_palabras +1));

		int inicio_palabra = 0;
		int letra_actual = 0;

		for(int i = 0; i < cantidad_de_palabras; i++){

			int cantidad_letras = 0;


			while(auxiliar[letra_actual] != '\0' && len > letra_actual){
				cantidad_letras++;
				letra_actual++;
			}
				letra_actual++;
			char* palabra = malloc(cantidad_letras + 1);

			memcpy(palabra, auxiliar + inicio_palabra, cantidad_letras);
			palabra[cantidad_letras] = '\0';

			retorno[i] = palabra;

	//		printf("Palabra actual: %s\n", palabra);

			inicio_palabra += cantidad_letras + 1;


		}

		retorno[cantidad_de_palabras] = NULL;
		return retorno;


}



char* recibirTexto(int fdOrigen){
	int tamanio;
	int resTamanio = recv(fdOrigen, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == ERROR){

		return NULL;
	}
	if(tamanio == 0){
		return NULL;
	}
	char* textoRecibido = malloc(tamanio);
	int resTexto = recv(fdOrigen, textoRecibido, tamanio, MSG_WAITALL);
	if(resTexto == ERROR){

		return NULL;
	}
	textoRecibido[tamanio-1] = '\0';
	return textoRecibido;
}

int pesoString(char *string) {
    return string == NULL ? 0 : sizeof(char) * (strlen(string) + 1);
}

int enviarTexto(int fdDestinatario, char* textoEnviar){
	int tamanio = pesoString(textoEnviar);
	int resTamanio = send(fdDestinatario, &tamanio, sizeof(int), MSG_WAITALL);
	if(resTamanio == ERROR){

		return ERROR;
	}
	if(tamanio != 0){
	int resTexto = send(fdDestinatario, (void*)textoEnviar, tamanio, MSG_WAITALL);
	if(resTexto == ERROR){

		return ERROR;
	}
	return resTexto;
	}
	return 0;
}

int enviarInt(int destinatario, int loQueEnvio){

	 void* paquete = malloc(sizeof(int));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(int));
	 int res = send(destinatario, paquete, sizeof(int), MSG_WAITALL);
	 free(paquete);
	 return res;
}

int recibirInt(int destinatario){
	int algo;
	if(recv(destinatario, &algo, sizeof(int), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}

int enviarUint32_t(int destinatario, uint32_t loQueEnvio){
	 void* paquete = malloc(sizeof(uint32_t));
	 void* puntero = paquete;
	 memcpy(puntero, &(loQueEnvio), sizeof(uint32_t));
	 int res = send(destinatario, paquete, sizeof(uint32_t), MSG_WAITALL);
	 free(paquete);
	 return res;
}

uint32_t recibirUint32_t(int destinatario){
	uint32_t algo;
	if(recv(destinatario, &algo, sizeof(uint32_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}

}
int enviarSizet(int destinatario,size_t n){
	 int tamanio = sizeof(size_t);
	 void* paquete = malloc(sizeof(size_t));
	 void* puntero = paquete;

	 memcpy(puntero, &(n), sizeof(size_t));
	 int res = send(destinatario, paquete, tamanio, MSG_WAITALL);
	 free(paquete);
	 return res;
}

size_t recibirSizet(int destinatario){

	size_t algo;
	if(recv(destinatario, &algo, sizeof(size_t), MSG_WAITALL) != 0)
			return algo;
		else
		{
			//close(destinatario);
			return -1;
		}
}


int enviarVoid(int destinatario, void* textoEnviar, int tam){


	int res = enviarInt(destinatario, tam);
	if(tam != 0){
	if(res != 0){
	  res = send(destinatario, textoEnviar, tam, MSG_WAITALL);
	if(res == -1){

		return -1;
	}
	return res;
	}
	}
	return 0;

}



char* recibirVoid(int fdOrigen){
	int tamanio;

	tamanio = recibirInt(fdOrigen);
	if(tamanio == 0){
		return NULL;
	}
	void* textoRecibido = malloc(tamanio);
	int resTexto = recv(fdOrigen, textoRecibido, tamanio, MSG_WAITALL);
	if(resTexto == -1){
		return NULL;
	}

	return textoRecibido;
}



size_t highestOneBitPosition(uint32_t a) {
    size_t bits=0;
    while (a!=0) {
        ++bits;
        a>>=1;
    };
    return bits;
}

bool addition_is_safe(uint32_t a, uint32_t b) {
    size_t a_bits=highestOneBitPosition(a), b_bits=highestOneBitPosition(b);
    return (a_bits<32 && b_bits<32);
}

void modificar_clock_bitmap(t_pagina* pagina, int segmento, t_proceso* proceso){
		if(pagina->p == 1){
			pthread_mutex_lock(&sem_bitmap_marco);
			bitmap_marcos[pagina->marco] = '0';
			pthread_mutex_unlock(&sem_bitmap_marco);
			pthread_mutex_lock(&sem_clock);
			remover_clocky(pagina->marco, proceso);
			pthread_mutex_unlock(&sem_clock);
		} else{
			pthread_mutex_lock(&sem_bitmap_swap);
			bitmap_swap[pagina->marco] = '0';
			pthread_mutex_unlock(&sem_bitmap_swap);

		}

}


void agregar_pag_clock(int id, char* ip, int m ,int u, t_segmento* seg, int pag, int marco){
//Busco el clock que tenga el puntero
	pthread_mutex_lock(&sem_clock);
	t_clock* clock = list_get(tabla_clock, marco);

	if(!seg->dinamico && seg->shared == SHARED){
//		pthread_mutex_lock(&sem_procesos);
		t_proceso* proceso = list_get(tabla_procesos, 0);
//		pthread_mutex_unlock(&sem_procesos);

		bool buscar_path(void* arch){
					return (strcmp(((t_segmento*)arch)->path,seg->path) ==0) ;
				}
		t_segmento* segmento = list_find(proceso->segmentos, &buscar_path);


		clock->ip      = malloc(strlen("0.0.0")+1);
		memcpy(clock->ip, "0.0.0", strlen("0.0.0"));
		clock->ip[strlen("0.0.0")]='\0';
		clock->seg 	 = segmento->segmento;
		clock->pag 	 = pag;//La pagina si es igual para todos

	}else{
		clock->ip      = malloc(strlen(ip)+1);
		memcpy(clock->ip, ip, strlen(ip));
		clock->ip[strlen(ip)]='\0';
		clock->seg 	 = seg->segmento;
		clock->pag 	 = pag;
		clock->id  	 = id;
	}
	clock->m   	 = m;
	clock->u   	 = u;
	pthread_mutex_unlock(&sem_clock);

}

t_clock* buscar_clock(int  marco){

	t_clock* clock = list_get(tabla_clock, marco);
	return clock;

}

void remover_clocky(int marco, t_proceso* proceso){
	t_clock* clock = list_get(tabla_clock, marco);//buscar_clock(proceso, seg, pag);
	clock->id = -1;
	free(clock->ip);
	clock->m = 0;
	clock->u = 0;
}

void actualizar_bloque_libre(int pag, t_segmento* seg, uint32_t desplazamiento, uint32_t tamanio, t_list* bloquesLibres){
	if(!seg->ultimo){
		bool buscar_seg_pag_pos(void* lib){
		return 	  (((t_libres*)lib)->pagina   == pag
				&& ((t_libres*)lib)->segmento == seg->segmento
				&& ((t_libres*)lib)->posicion == desplazamiento);

		}
	t_libres* libre = list_find(bloquesLibres, &buscar_seg_pag_pos);

	if(libre != NULL){
		libre->tamanio = tamanio;
	} else {
		libre = malloc(sizeof(t_libres));
		agregar_bloque_libre(bloquesLibres, pag, seg->segmento, desplazamiento, tamanio);
	}
	}
	return;
}

void nuevo_segmento(t_segmento* nuevo, uint32_t base,bool dinamico,bool empty,int numero,bool compartido,uint32_t tamanio,bool ultimo){
	 nuevo->base     = base;
	 nuevo->dinamico = dinamico;
	 nuevo->empty    = empty;
	 nuevo->paginas  = list_create();
	 nuevo->segmento = numero;
	 nuevo->shared   = compartido;
	 nuevo->tamanio  = tamanio;
	 nuevo->ultimo   = ultimo;

	 return;

}
void agregar_bloque_libre(t_list* bloquesLibres, int pagina,int segmento,uint32_t posicionEnPagina, uint32_t tamanioLibre){

	t_libres* newLibre = malloc(sizeof(t_libres));
	  		  newLibre->pagina   = pagina;
	  		  newLibre->segmento = segmento;
	  		  newLibre->posicion = posicionEnPagina + 5 ;
	  		  newLibre->tamanio  = tamanioLibre;

	  		  list_add(bloquesLibres, newLibre);
}

t_segmento* buscar_segmento(uint32_t logica, t_list* tabla_segmentos){
	bool buscar_segmento(void* seg){

			return (((t_segmento*)seg)->base <= logica && ((t_segmento*)seg)->base +((t_segmento*)seg)->tamanio  > logica );

		}

	t_segmento* segmento = list_find(tabla_segmentos, &buscar_segmento);
	return segmento;
}


void*  convertir(uint32_t logica, int marco){ //Ya se marco y posicion en la pagina.

	return punteroMemoria + (marco*config_muse->tamanio_pagina + logica);// + (logica%config_muse->tamanio_pagina);

}

uint32_t sobrante_pagina(uint32_t base_segmento, int numero_pagina, uint32_t desplazamiento){

	return((config_muse->tamanio_pagina)- (base_segmento + (numero_pagina*config_muse->tamanio_pagina + desplazamiento)));

}

void desperdicio(uint32_t sobrante, void* posicion, t_pagina* pag, uint32_t header){
	  char* desperdicio = string_repeat('#', sobrante);
	  memmove(posicion, desperdicio, sobrante);
	  pag->ultimo_header  = header;
	  pag->tamanio_header = 0;
	  free(desperdicio);
}

bool hayHashtag(int marco, int i){
	char* letra = malloc(sizeof(char)*2);
	void* puntero = punteroMemoria + (config_muse->tamanio_pagina * marco + (i));
	memmove(letra, puntero, 1);
	letra[1]='\0';
		if(string_starts_with(letra, "#")){
			free(letra);
			return true;
		}else{
			free(letra);
			return false;
		}
}

t_pagina* buscar_segmento_pagina(t_list* segmentos , int seg, int pag, t_proceso* proceso){


	bool buscar_seg(void* elSeg){

			return (((t_segmento*)elSeg)->segmento == seg);

		}
	bool buscar_pag(void* laPag){

			return (((t_pagina*)laPag)->numero == pag);

		}

	t_segmento* segmento  = list_find(segmentos, &(buscar_seg));
	t_pagina*   pagina    = list_find(segmento->paginas, &(buscar_pag));
	if(pagina == NULL) return NULL;
	if(pagina->p != 1){

		pagina->marco = swap(pagina, false, proceso, segmento);

	}else{
		t_clock* clock = buscar_clock(pagina->marco);
		clock->u = 1;
	}



	return pagina;

}

int swap(t_pagina* pag_swap, bool nueva, t_proceso* proceso , t_segmento* segmento){

	bool actualizar_viejo = false;

	//Busco a la vistima, primero veo si tengo un marco libre
	pthread_mutex_lock(&sem_bitmap_marco);

	int marco = buscar_marco_libre(bitmap_marcos);
	pthread_mutex_unlock(&sem_bitmap_marco);

	pthread_mutex_lock(&sem_clock);
	t_clock* clock;
	if(marco >= 0) clock = list_get(tabla_clock, marco);
	if(marco == -1){
		actualizar_viejo = true;
		//Aplico algoritmo, primero veo donde arajo esta el puntero


		int i = 0;

			while(i<list_size(tabla_clock)){
				clock = list_get(tabla_clock, i );
				if(clock->p ==1){
				   clock->p =0;
				   break; // entonces en i tengo el puntero
				}
				i++;
			}

	int c; //buscar 0 0 lo voy a hacer tantas veces como paginas de memoria existan, arrancando del puntero
	for(c = 0; c< cantidad_paginas;c++){
		clock = list_get(tabla_clock, i);
		if(clock->u == 0 && clock->m == 0 ){
			marco = clock->marco;
			if(i+1 == cantidad_paginas){
				i = 0;
			}else{
				i++;
			}
			t_clock* sig = list_get(tabla_clock, i);
			sig->p = 1;
			break;
		}
		i++;
		if(i == cantidad_paginas){
			i=0;
		}
	}

	if(marco==-1){
	//Busco u=0 m=0 ni empedo hago funciones aparte :P
		for(c=0; c<cantidad_paginas ; c++){
			clock= list_get(tabla_clock, i);
			if(clock->u == 0 && clock->m == 1){
				marco = clock->marco;
				if(i+1 == cantidad_paginas){
					i = 0;
				}else{
					i++;
				}
				t_clock* sig = list_get(tabla_clock, i);
				sig->p = 1;
				break;
			}
			clock->u = 0;
			i++;
			if(i== cantidad_paginas){
				i=0;
			}
		}
	}

	//Busco 0 0 --> tercer vuelta
	if(marco == -1){
		for(c = 0; c< cantidad_paginas;c++){
			clock = list_get(tabla_clock, i);
			if(clock->u == 0 && clock->m == 0 ){
				marco = clock->marco;
				if(i+1 == cantidad_paginas){
					i = 0;
				}else{
					i++;
				}
				t_clock* sig = list_get(tabla_clock, i);
				sig->p = 1;
				break;
			}
			i++;
			if(i == cantidad_paginas){
				i=0;
			}
		}
	}

	//Busco 1 0 --> cuarta vuelta, no hay ma chances
	if(marco==-1){
	//Busco u=0 m=0 ni empedo hago funciones aparte :P
		for(c=0; c<cantidad_paginas ; c++){
			clock= list_get(tabla_clock, i);
			if(clock->u == 0 && clock->m == 1){
				marco = clock->marco;
				if(i+1 == cantidad_paginas){
					i = 0;
				}else{
					i++;
				}
				t_clock* sig = list_get(tabla_clock, i);
				sig->p = 1;

				break;
			}
			clock->u = 0;
			i++;
			if(i== cantidad_paginas){
				i=0;
			}
		}
	 }


	}
	pag_swap->p = 1;

	pthread_mutex_unlock(&sem_clock);

	t_pagina* pagina_vieja;
	if(actualizar_viejo){
		//El ultimo clock es el de la pagina que se fue, asique vamos a buscarla para ponerle marco y ṕ
		bool buscar_proceso(void* proceso){

					return (((t_proceso*)proceso)->id == clock->id) &&
							(strcmp(((t_proceso*)proceso)->ip , clock->ip)==0);
					}
		pthread_mutex_lock(&sem_procesos);
		t_proceso* proceso = list_find(tabla_procesos, &buscar_proceso);
		pthread_mutex_unlock(&sem_procesos);

		bool buscar_seg(void* seg){

					return (((t_segmento*)seg)->segmento == clock->seg);
					}
		t_segmento* segmento = list_find(proceso->segmentos, &buscar_seg);
		bool buscar_pag(void* pag){

					return (((t_pagina*)pag)->numero == clock->pag);
					}
		pagina_vieja = list_find(segmento->paginas, &buscar_pag);

		log_info(logMuse, "Victima : Pagina %d Segmento %d ", clock->pag, clock->seg);
		pagina_vieja->p 	  = 0;
//		pagina_vieja->marco = marco_swap;
		free(clock->ip);
		clock->id  = -1;
		clock->m   =  0;
		clock->pag = -1;
		clock->seg = -1;
		clock->u   =  0;

	}

	agregar_pag_clock(proceso->id, proceso->ip, 0, 1, segmento, pag_swap->numero, marco);

	int marco_swap = pag_swap->marco;
	if(actualizar_viejo){//Si hay que "actualizar" lo viejo es que antes habia algo en memoria que pasa a swap
		//Primero bloqueo el marco, paso lo que tiene a un auxiliar y LUEGO lo marco como libre y bsuco uno nuevo
		char* aux_swap = malloc(config_muse->tamanio_pagina);
		t_semaforo* sem_swap = list_get(lista_marcos_swap, marco_swap);
		pthread_mutex_lock(&sem_swap->marco);

		//En que marco de swap esta lo que busco? en el que viene por parametro
				void* punteroAux = punteroSwap + (config_muse->tamanio_pagina * pag_swap->marco);
				void* salida;
				//Lo que estaba en swap lo meti en un char auxiliar
			if(!nueva){

				salida = memcpy(aux_swap, punteroAux, config_muse->tamanio_pagina);
				if(salida == aux_swap){
					//Salio ok
				}

			}
		pthread_mutex_unlock(&sem_swap->marco);

		pthread_mutex_lock(&sem_bitmap_swap); //Lo marco como libre y busco uno libre

		bitmap_swap[pag_swap->marco] = '0';
		marco_swap = buscar_marco_libre(bitmap_swap);
		pagina_vieja->marco = marco_swap;
		pthread_mutex_unlock(&sem_bitmap_swap);

	t_semaforo* sem = list_get(lista_marcos_memoria, marco);
	pthread_mutex_lock(&sem->marco);
		   sem_swap = list_get(lista_marcos_swap, marco_swap);
	pthread_mutex_lock(&sem_swap->marco);
		//Lo que estaba en memoria lo pase a swap, que marco? lo busco

		punteroAux= punteroSwap + (config_muse->tamanio_pagina * marco_swap);

		void* punteroMem = punteroMemoria + (config_muse->tamanio_pagina * marco);
		salida = memcpy(punteroAux, punteroMem ,config_muse->tamanio_pagina);
		if(salida == punteroAux){
			//Salio ok
		}
		pthread_mutex_unlock(&sem_swap->marco);
		//Lo del aux tengo que pasarlo a memoria
	if(!nueva){
		memcpy(punteroMem, aux_swap, config_muse->tamanio_pagina);
	}
	if(nueva){
		int i = 0;
		char*  punteroExtra = punteroMemoria + (config_muse->tamanio_pagina * marco);;
		while( i<config_muse->tamanio_pagina){
			punteroExtra[i]='\0';
			i++;
			}
	}


	pthread_mutex_unlock(&sem->marco);

	free(aux_swap);


	}	else{ //Si no hay que actualizar lo viejo es que tengo el marco libre en memoria y nada que pasar a swap
		//Lo que estaba en swap lo paso a memoria porqe no habia nada y libero el wap
			t_semaforo* sem = list_get(lista_marcos_memoria, marco);
			pthread_mutex_lock(&sem->marco);
			t_semaforo* sem_swap = list_get(lista_marcos_swap, marco_swap);
			pthread_mutex_lock(&sem_swap->marco);

				//En que marco de swap esta lo que busco? en el que viene por parametro
						void* punteroAux = punteroSwap + (config_muse->tamanio_pagina * pag_swap->marco);
						void* punteroMem = punteroMemoria + (config_muse->tamanio_pagina * marco);
						//Lo que estaba en swap lo meti en un char auxiliar

						memcpy(punteroMem, punteroAux,config_muse->tamanio_pagina );
				pthread_mutex_unlock(&sem_swap->marco);
				pthread_mutex_unlock(&sem->marco);

						pthread_mutex_lock(&sem_bitmap_swap); //Marco el swap como libre
						bitmap_swap[pag_swap->marco] = '0';
						pthread_mutex_unlock(&sem_bitmap_swap);
	}


	return marco;

}

void vaciarSegmento(t_segmento* segmento, t_list* bloquesLibres, t_list* tabla_segmentos , t_proceso* proceso){

		if(!segmento->dinamico){
			if(segmento->path != NULL){
			free(segmento->path);
		 }
		}
		while(list_size(segmento->paginas)>0){
			pthread_mutex_lock(&sem_cant_pag);
			paginas_usadas --;
			pthread_mutex_unlock(&sem_cant_pag);
			t_pagina* pagina = list_remove(segmento->paginas, 0);
			modificar_clock_bitmap(pagina, segmento->segmento, proceso);
			free(pagina);
		}
		bool buscar_por_segmento(void* bloque_libre){

			return (((t_libres*)bloque_libre)->segmento == segmento->segmento);
			}
		t_list* nuevos = list_filter(bloquesLibres, &buscar_por_segmento);

		for(int i=0; i<list_size(nuevos);i++){
			t_libres* nuevoLibre = list_get(nuevos, i);
			remover_bloque_libre(bloquesLibres, nuevoLibre->pagina,nuevoLibre->segmento, nuevoLibre->posicion);
		}
		void liberar(t_libres* libre){
			free(libre);
		}
		list_clean_and_destroy_elements(nuevos, (void*)liberar);
		free(nuevos);

		if(!segmento->ultimo){
			segmento->dinamico = true;
			segmento->empty    = true;
			//VER SI ESTO ESTA OK +******************************************************
			bool emp = true;
			uint32_t posSig = segmento->base + segmento->tamanio;
			while(emp){
				t_segmento* sigiente = buscar_segmento(posSig, tabla_segmentos);
				 emp = sigiente->empty;
				 if(emp){
					bool numerito_seg(void* segmento){
						return   ((t_segmento*)segmento)->segmento == sigiente->segmento;
					}

					segmento->tamanio += sigiente->tamanio;
					posSig = sigiente->base + sigiente->tamanio;
					sigiente = list_remove_by_condition(tabla_segmentos, &numerito_seg);
					free(sigiente);

				 }
			}



		} else{
			//Si es el ultimo segmento tengo que poner el anterior como ultimo
			bool buscar_por_segmento_seg(void* seg){

						return (((t_segmento*)seg)->segmento == segmento->segmento);
						}
			t_segmento* seg = list_remove_by_condition(tabla_segmentos, &buscar_por_segmento_seg);
			if(seg->base !=0){
				t_segmento* anterior = buscar_segmento(seg->base - config_muse->tamanio_pagina, proceso->segmentos);
				anterior->ultimo = true;
			}
			free(seg->paginas);
			free(seg);


		}

}

int copiarMuse(uint32_t posicionACopiar,int  bytes,char* src,t_list* tabla_segmentos, t_proceso* proceso){
//	int copiarMuse(uint32_t posicionACopiar,int  bytes,void* src,t_list* tabla_segmentos, t_proceso* proceso, bool sg){
	posicionACopiar -= 5;
	t_segmento* segmento = buscar_segmento(posicionACopiar, tabla_segmentos);
		if(segmento == NULL){
			log_error(logMuse, "[SEGFAULT] No existe el segmento  \n");
//			sg = true;
			return -5;
		}
	 if((posicionACopiar +5) + bytes > segmento->base + segmento->tamanio){
		 log_error(logMuse, "Segmentation Fault \n");

		 return -5;

	 }


	int      pag         	= posicionACopiar/config_muse->tamanio_pagina;
	uint32_t desplazamiento = posicionACopiar%config_muse->tamanio_pagina;

	t_pagina* pagina = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag, proceso);

	t_semaforo* sem = list_get(lista_marcos_memoria, pagina->marco);
	pthread_mutex_lock(&sem->marco);

	if(segmento->dinamico){
		t_header* head = punteroMemoria + ((config_muse->tamanio_pagina * pagina->marco) + desplazamiento );
		if(head->isFree){
			log_error(logMuse, "La posición en la que solicitan copiar esta libre \n" );
			pthread_mutex_unlock(&sem->marco);
			log_error(logMuse, "Segmentation Fault, acceso invalido \n");

			return -5;
		}
		if(head->size < bytes){//Si  que quieren copiar es mayor a lo que hay
			pthread_mutex_unlock(&sem->marco);

			log_error(logMuse, "No hay suficiente espacio en  %d \n", posicionACopiar);
			bytes = head->size;
			src[bytes]='\0';
		}
		  if(desplazamiento + 5 < config_muse->tamanio_pagina){
			  desplazamiento += 5;
		  }else{
			  desplazamiento = 5 -(config_muse->tamanio_pagina - desplazamiento);
			  pag ++;
			  pagina = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag, proceso);
		  }
	}
	char* dest = punteroMemoria + ((config_muse->tamanio_pagina * pagina->marco) + desplazamiento);

	t_clock* clock = buscar_clock(pagina->marco);
	clock->m = 1;

	  if(bytes <=config_muse->tamanio_pagina- desplazamiento ){

		  memcpy(dest, src, bytes);
		  pthread_mutex_unlock(&sem->marco);
	  }
	  else{

		  memcpy(dest, src,config_muse->tamanio_pagina- desplazamiento );
		  bytes -= config_muse->tamanio_pagina- desplazamiento;
		  src += config_muse->tamanio_pagina- desplazamiento;

		  pthread_mutex_unlock(&sem->marco);
		  while(bytes>=config_muse->tamanio_pagina){

			  pag ++;
			  pagina = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag, proceso);
			  sem = list_get(lista_marcos_memoria, pagina->marco);

			  pthread_mutex_lock(&sem->marco);
			  dest = punteroMemoria + ((config_muse->tamanio_pagina * pagina->marco));
			  memcpy(dest, src, config_muse->tamanio_pagina);

			  pthread_mutex_unlock(&sem->marco);

			  bytes -= config_muse->tamanio_pagina;
			  src+= config_muse->tamanio_pagina;
		  }
		  if(bytes >0){
			  pag ++;
			  pagina = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag, proceso);
			  sem = list_get(lista_marcos_memoria, pagina->marco);

			  pthread_mutex_lock(&sem->marco);
			  dest = punteroMemoria + ((config_muse->tamanio_pagina * pagina->marco));
			  memcpy(dest, src, bytes);

			  pthread_mutex_unlock(&sem->marco);
		  }
	  }

        log_info(logMuse, "Se copio correctamente \n" );
        return 0;

}

char* getMuse(uint32_t posicion, size_t bytes,t_list* tabla_segmentos, t_proceso* proceso){


 posicion -=5;

 t_segmento* seg = buscar_segmento(posicion, tabla_segmentos);
 if(seg == NULL){
	 log_error(logMuse, "No existe el segmento \n");

	 return "SG_MICA";
 }

 if((posicion +5) + bytes > seg->base + seg->tamanio){
	 log_error(logMuse, "Segmentation Fault \n");

	 return "SG_MICA";

 }

 int pagina = posicion/config_muse->tamanio_pagina;
 uint32_t desplazamiento = posicion%config_muse->tamanio_pagina;

 t_pagina* pag = buscar_segmento_pagina(tabla_segmentos, seg->segmento, pagina, proceso);
	 t_semaforo* sem = list_get(lista_marcos_memoria, pag->marco);

	 pthread_mutex_lock(&sem->marco);
// pthread_mutex_lock(&global);



 if(seg->dinamico){
	 t_header* head = punteroMemoria + ((config_muse->tamanio_pagina * pag->marco) + desplazamiento);

	  if(head->isFree){
//		  pthread_mutex_unlock(&global);
		  pthread_mutex_unlock(&sem->marco);
		  log_error(logMuse, "Lo que solicitan esta libre" );
		  return NULL;
	  }
	  if(head->size < bytes){
		  pthread_mutex_unlock(&sem->marco);

		  log_error(logMuse, "Lo que solicitan pisa otra asignacion");
			 log_error(logMuse, "Segmentation Fault \n");
			 return "SG_MICA";
	  }
	  if(desplazamiento + 5 < config_muse->tamanio_pagina){
		  desplazamiento += 5;
	  }else{
		  desplazamiento = 5 -(config_muse->tamanio_pagina - desplazamiento);
		  pagina ++;
		  pag = buscar_segmento_pagina(tabla_segmentos, seg->segmento, pagina, proceso);

	  }
 }
    pthread_mutex_unlock(&sem->marco);
	sem = list_get(lista_marcos_memoria, pag->marco);
	pthread_mutex_lock(&sem->marco);
//     int aux = bytes;
  char* dest = malloc(bytes);


  char* src = punteroMemoria + ((config_muse->tamanio_pagina * pag->marco) + desplazamiento);


//  Cuanto leo de esta pagina
  int leidos = 0;
  if(bytes <=config_muse->tamanio_pagina- desplazamiento ){
	  memcpy(dest, src, bytes);
//	  dest[bytes-1]='\0';
	  pthread_mutex_unlock(&sem->marco);
  }
  else{
	  memcpy(dest, src,config_muse->tamanio_pagina- desplazamiento );
	  pthread_mutex_unlock(&sem->marco);
	  bytes -= config_muse->tamanio_pagina- desplazamiento;

	  leidos = config_muse->tamanio_pagina- desplazamiento;

	  while(bytes>=config_muse->tamanio_pagina){
		  sem = list_get(lista_marcos_memoria, pag->marco);
		  pthread_mutex_lock(&sem->marco);

		  pagina ++;
		  pag = buscar_segmento_pagina(tabla_segmentos, seg->segmento, pagina, proceso);
		  src = punteroMemoria + ((config_muse->tamanio_pagina * pag->marco));

		  memcpy(dest+leidos, src, config_muse->tamanio_pagina);
		  pthread_mutex_unlock(&sem->marco);

		  bytes  -= config_muse->tamanio_pagina;
		  leidos += config_muse->tamanio_pagina;
	  }
	  if(bytes >0){
		  pagina ++;
		  pag = buscar_segmento_pagina(tabla_segmentos, seg->segmento, pagina, proceso);
		  sem = list_get(lista_marcos_memoria, pag->marco);
		  pthread_mutex_lock(&sem->marco);
		  src = punteroMemoria + ((config_muse->tamanio_pagina * pag->marco));

		  memcpy(dest+leidos, src, bytes);
		  pthread_mutex_unlock(&sem->marco);
	  }
//	  dest[aux-1]='\0';
  }


     return dest;
}


int freeMuse(uint32_t posicionAliberar,t_list* tabla_segmentos,t_list* bloquesLibres, t_proceso* proceso){
	//La posicion que me mandan es despues del header
	posicionAliberar -= 5;
	//Primero libero lo que pidieron
	t_segmento* segmento = buscar_segmento(posicionAliberar, tabla_segmentos);
	if(segmento == NULL){
		log_error(logMuse, "Posicion inexistente \n");

		return -5;
	}
	if(!segmento->dinamico){
		log_error(logMuse, "No se liberan los archivos mapeados mediante free. Use munmmap \n");
		return -1;
	}
	uint32_t desplazamiento1,desplazamiento2 , posicion2;
	t_header* head1;
	t_header* head2;
	t_pagina* pagina1;
	t_pagina* pagina2;
	t_semaforo* sem2;
	uint32_t size2;
	bool used;
	int pag1, pag2, marco2;

	pag1 = posicionAliberar/config_muse->tamanio_pagina;

	pagina1         = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag1, proceso);
	t_semaforo* sem = list_get(lista_marcos_memoria, pagina1->marco);
	pthread_mutex_lock(&sem->marco);

    desplazamiento1 = posicionAliberar%config_muse->tamanio_pagina;
	head1 		    = punteroMemoria + (config_muse->tamanio_pagina * pagina1->marco + desplazamiento1);
	if(head1->isFree){
		pthread_mutex_unlock(&sem->marco);

		return -1;
	}
	if(!head1->isFree){
	t_clock* clock = list_get(tabla_clock , pagina1->marco);//buscar_clock(proceso, segmento->segmento, pag1);
	clock->m       = 1;
	head1->isFree  = true;
	proceso->memLiberada += head1->size;
		//Veo si hay desperdicio
		if(config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size) < 5 ){
			head1->size += (config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size));
			pagina1->ultimo_header = desplazamiento1 + 5;
		}
			//Si era el ultimo header de la ultima pagina aca muere mi operación
			if(pagina1->esFinPag == 1 && pagina1->ultimo_header == (desplazamiento1+5)){
				pagina1->tamanio_header = head1->size;
				pthread_mutex_unlock(&sem->marco);
				//si no es el ultimo segmento hay que actualizar el bloques libres
				if(segmento->ultimo && posicionAliberar == segmento->base){
					vaciarSegmento(segmento, bloquesLibres, tabla_segmentos, proceso);

					log_info(logMuse, "Se libero el semgento %d \n", segmento);
				}else{
				actualizar_bloque_libre(pag1, segmento, desplazamiento1 + 5, head1->size, bloquesLibres);
				}

				return 0;
			}

			posicion2 = posicionAliberar + 5 + head1->size;
			pag2      = posicion2/config_muse->tamanio_pagina;
			//Si no era la ultima pagina hay otra y esa puede tener desperdicio
			 if(desplazamiento1 + 5 + head1->size == config_muse->tamanio_pagina){
				 pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2, proceso);
					 if(pagina2 == NULL) {
						 pthread_mutex_unlock(&sem->marco);
						 return 0;
					 }
				 for(int i= 0; i<5; i++){
					 if(!hayHashtag(pagina2->marco, i)){
							posicion2 += i;
							head1->size += i;
							break;
					 	}
				 }
			 }

					if(pag2 == pag1){
						marco2 = pagina1->marco;
					} else {
						pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2, proceso);

						if(pagina2 == NULL){
							pthread_mutex_unlock(&sem->marco);
							return 0;
						}
						sem2 = list_get(lista_marcos_memoria, pagina2->marco);
						pthread_mutex_lock(&sem2->marco);
						marco2 = pagina2->marco;
					}

	desplazamiento2 = posicion2%config_muse->tamanio_pagina;
	head2 = punteroMemoria + (config_muse->tamanio_pagina * marco2 + desplazamiento2);

	used = head2->isFree;
	size2= head2->size;
	if(pag2 != pag1){
		pthread_mutex_unlock(&sem2->marco);
	}

	if(used == true){
		//Si son bloques de la misma pagina veo si el segundo header es el ultimo del segmento
				if(pag1 == pag2){
					if(pagina1->esFinPag == 1){
						//Si el segundo header es el ultimo , pasa a ser el primero, sumo tamanio
						if(desplazamiento2 +5 == pagina1->ultimo_header){
							if(posicionAliberar == segmento->base){

								pthread_mutex_unlock(&sem->marco);
								vaciarSegmento(segmento, bloquesLibres, tabla_segmentos, proceso);

//								pthread_mutex_unlock(&global);
								return 0;
							}

							head1->size += (size2 + 5);
							pthread_mutex_unlock(&sem->marco);

							pagina1->ultimo_header = desplazamiento1 +5;
							pagina1->tamanio_header = head1->size;
							actualizar_bloque_libre(pag1, segmento, desplazamiento2 + 5, head1->size, bloquesLibres);
							remover_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento2 +5 );

//							pthread_mutex_unlock(&global);
							return 0;
						}
					}

				} else{ // Si son paginas distintas pero la segunda es la ultima
					if(pagina2->esFinPag == 1){
						if(pagina2->ultimo_header == (desplazamiento2 +5)){

							if(posicionAliberar == segmento->base){
								//Si entra acá significa que en la siguiente pagina solo estaba un header free, asique LA ELIMINO sin importar desperdicio
								log_info(logMuse, "Se libero el segmento %d \n", segmento->segmento);
								pthread_mutex_unlock(&sem->marco);
								vaciarSegmento(segmento, bloquesLibres, tabla_segmentos, proceso);

								return 0;
							}
	//El nuevo ultimo header pasa a ser el 1 y lo restante lo armo en otro segmento
							head1->size -=(config_muse->tamanio_pagina - (desplazamiento1 + 5));
							pthread_mutex_unlock(&sem->marco);
//							pthread_mutex_unlock(&global);
							pagina1->esFinPag = 1;
							pagina1->ultimo_header = desplazamiento1 + 5;
							pagina1->tamanio_header = head1->size;
//Bien, el nuevo size es solo de la pagina pero, cuantas paginas ocupaba antes el size? debo eliminarlas
							for(int i= pag1+1; i<=pag2;i++){
								t_pagina* eliminar = list_remove(segmento->paginas, i);
								modificar_clock_bitmap(eliminar, segmento->segmento, proceso);

								free(eliminar);
								segmento->tamanio -= (config_muse->tamanio_pagina);
								pthread_mutex_lock(&sem_cant_pag);
								paginas_usadas --;
								pthread_mutex_unlock(&sem_cant_pag);
							}
							if(!segmento->ultimo){
								actualizar_bloque_libre(pag1, segmento, desplazamiento1 + 5, head1->size, bloquesLibres);
								remover_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento2+5);
								t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
								nuevo_segmento(nuevoSegmento, segmento->base + segmento->tamanio, true, true, list_size(tabla_segmentos), true, config_muse->tamanio_pagina * (pag2-pag1), false);
								list_add(tabla_segmentos, nuevoSegmento);
							} else{
								remover_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento1+5);
							}

							return 0 ;

						}
					}
				}

				if(size2 + desplazamiento2 == config_muse->tamanio_pagina){
					pagina2->ultimo_header = 0;
				}
				remover_bloque_libre(bloquesLibres, pag2, segmento->segmento, desplazamiento2+5);

				head1->size += (5 + size2);
//				pthread_mutex_unlock(&sem->marco);

	}
	pthread_mutex_unlock(&sem->marco);
	agregar_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento1, head1->size);
	return 0;
	}
	return 0;
}


void compactar(t_list*  tabla_segmentos, t_list* bloquesLibres, t_proceso* proceso){
	//Solo puede haber desperdicio en la primer parte de las paginas, si hay un free delante.
	uint32_t desplazamiento1, desplazamiento2;
	t_header* head1;
	t_header* head2;
	t_pagina* pagina1;
	t_pagina* pagina2;
	t_segmento* segmento;
	int pag1, pag2, marco2;

	uint32_t posicion1 ;
	uint32_t posicion2;
	bool fin = false;

	int i = 0;
	while(i<list_size(tabla_segmentos)){
	segmento = list_get(tabla_segmentos, i);
		if(!segmento->dinamico || segmento->empty){
			continue;
		}


		posicion1 = segmento->base;
		fin 	  = false;

		while(fin){
			pag1 = posicion1/config_muse->tamanio_pagina;

			pagina1 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag1, proceso);
				if(pagina1->esFinPag == 1 && pagina1->ultimo_header == (posicion1 +5) ){
					fin = true;
					continue;
				}

			desplazamiento1 = posicion1%config_muse->tamanio_pagina;
			head1 = punteroMemoria + (pagina1->marco * config_muse->tamanio_pagina + desplazamiento1 );

			posicion2 = posicion1 + 5 + head1->size;
			if(!head1->isFree){ //Si esta en uso avanzo a la siguiente posicion, con o sin desperdicio?
						if(desplazamiento1 + 5 + head1->size < config_muse->tamanio_pagina){
							if(config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size) < 5){
							   posicion2 += (config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size));

							}
						}
						//Se desperdicio lo primero?
						if(posicion2 == config_muse->tamanio_pagina){
							pag2 = posicion2/config_muse->tamanio_pagina;
							pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2, proceso);
							for(int i = 0; i<5;i++){
								if(!hayHashtag(pagina2->marco, i)){
									posicion2 += i;
									break;
								}
							}
							}
						posicion1 = posicion2;
						continue;
						}
			//Si el head1 esta libre
			pag2 = posicion2/config_muse->tamanio_pagina;
					if(pag2 == pag1){
						marco2 = pagina1->marco;
					} else {
						pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2, proceso);
						marco2 = pagina2->marco;
					}

			desplazamiento2 = posicion2%config_muse->tamanio_pagina;
			head2 = punteroMemoria + (marco2 * config_muse->tamanio_pagina + desplazamiento2);
			//Si el head 2 esta en uso avanzo a la siguiente posicion
			if(!head2->isFree){

				posicion1 = posicion2;
				continue;

			}

			//Si viene por aca es que ambos headers estan  libres
				head1->size += (5 + head2->size);

				if(pag1 == pag2){
					if(pagina1->esFinPag == 1){
						//Si el segundo header es el ultimo , pasa a ser el primero, sumo tamanio
						if((desplazamiento2 +5) == pagina1->ultimo_header){

							pagina1->ultimo_header = desplazamiento1 + 5;
							pagina1->tamanio_header = head1->size;

							if(posicion1 ==  segmento->base){
								vaciarSegmento(segmento, bloquesLibres, tabla_segmentos, proceso);

								fin = true;
								continue;
							}
							//Pero el primero estaba en libres entonces lo elimino
							if(segmento->ultimo){
								remover_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento1+5);
							} else{
								actualizar_bloque_libre(pag1, segmento, desplazamiento1 + 5, head1->size, bloquesLibres);
								remover_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento2+5);
							}


						}
					}

				} else{ // Si son paginas distintas pero la segunda es la ultima
					if(pagina2->esFinPag == 1){
						if(pagina2->ultimo_header == (desplazamiento2 +5) ){
//Si entra acá significa que en la siguiente pagina solo estaba un header free, asique LA ELIMINO sin importar desperdicio
							if(posicion1 == segmento->base){
								vaciarSegmento(segmento, bloquesLibres, tabla_segmentos, proceso);
								fin = true;
								continue;
							}
							head1->size -=(config_muse->tamanio_pagina - (desplazamiento1 + 5));
							pagina1->esFinPag = 1;
							pagina1->ultimo_header = desplazamiento1 +5;
							pagina1->tamanio_header = head1->size;

//Bien, el nuevo size es solo de la pagina pero, cuantas paginas ocupaba antes el size? debo eliminarlas
							for(int i= pag1+1; i<=pag2;i++){
								t_pagina* eliminar = list_remove(segmento->paginas, i);
								modificar_clock_bitmap(eliminar, segmento->segmento, proceso);
								free(eliminar);
								segmento->tamanio -= (config_muse->tamanio_pagina);
								pthread_mutex_lock(&sem_cant_pag);
								paginas_usadas --;
								pthread_mutex_unlock(&sem_cant_pag);
							}
							if(!segmento->ultimo){
								//Si no es el ultimo segmento --> va todo en libres, ambos exitian, el 2 lo elimino y el uno lo actualizo
							    actualizar_bloque_libre(pag1, segmento, desplazamiento1 + 5, head1->size, bloquesLibres);
							    remover_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento2+5);

								t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
								nuevo_segmento(nuevoSegmento, segmento->base + segmento->tamanio, true, true, list_size(tabla_segmentos), true, config_muse->tamanio_pagina * (pag2-pag1), false);
								list_add(tabla_segmentos, nuevoSegmento);
							} else{
								//Si es el ultimo el head 1 que estaba en libres no tiene que estar.
								remover_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento1+5);
							}
							//ya estoy en el ultimo header de la ultima pagina
							fin = true;
							continue;

						}
					}
				}
			//Si ni la pag1 ni la 2 son ultimas , pero estan libres, elimino la 2 de los libres y edito la 1

				actualizar_bloque_libre(pag1, segmento, desplazamiento1 + 5, head1->size, bloquesLibres);
				remover_bloque_libre(bloquesLibres, pag2, segmento->segmento, desplazamiento2+5);
				//LA posicion1 queda igual
		}

	}

	//Voy a unir segmentos vacios
	int x = 0;

	segmento = buscar_segmento(0, tabla_segmentos);
	bool seg_seg(void* asd){
						return ((t_segmento*)asd)->segmento == segmento->segmento;
					}


	while(x<list_size(tabla_segmentos)){

		if(!segmento->empty){
		  segmento = buscar_segmento(segmento->base + segmento->tamanio, tabla_segmentos);
		  x++;
		}
		if(segmento->ultimo){
			segmento = list_remove_by_condition(tabla_segmentos, &seg_seg);
			free(segmento);
		}
		t_segmento* sig = buscar_segmento(segmento->base + segmento->tamanio, tabla_segmentos);
		bool seg_sig(void* asd){
							return ((t_segmento*)asd)->segmento == sig->segmento;
						}
		if(sig->empty){
			segmento->tamanio += sig->tamanio;
			sig = list_remove_by_condition(tabla_segmentos, &seg_sig);
			free(sig);
			x++;
		}else{
			segmento = buscar_segmento(sig->base + sig->tamanio, tabla_segmentos);
			x++;
		}


	}


}





void actualizar_header(int segmento, int pagina,uint32_t posicion, uint32_t tamAnterior, uint32_t tamanio,  t_list* tabla_segmentos, t_list* bloquesLibres, int fin, t_proceso* proceso){

	posicion -= 5;
	t_semaforo* sem2;
	//Busco la pagina a actualizar y la traigo a memoria
	t_pagina* pag = buscar_segmento_pagina(tabla_segmentos,segmento, pagina , proceso);
	t_semaforo* sem = list_get(lista_marcos_memoria, pag->marco);
	pthread_mutex_lock(&sem->marco);
//	pthread_mutex_lock(&global);

	//Actualizo el header que ya existia, empieza en posicion
	t_header* head = convertir(posicion, pag->marco);
	head->isFree   = false;
	head->size     = tamanio;

	//Tengo que actualizar la nueva "Cola" primero veo si necesito una cola.
	if(tamAnterior == tamanio){
		if( (posicion+5) == pag->ultimo_header){
			pag->tamanio_header = 0;
			pag->ultimo_header  = posicion +5;
		}
		pthread_mutex_unlock(&sem->marco);
//		pthread_mutex_unlock(&global);
		return;
	}

	//Necesito cola --> entra en la pagina actual?

	uint32_t ubicacionCola = posicion + 5 + tamanio;
	uint32_t sobrantePag   = config_muse->tamanio_pagina - (posicion + 5);
	if(config_muse->tamanio_pagina > ubicacionCola){
		uint32_t sobrante = config_muse->tamanio_pagina - ubicacionCola;
		//si lo que sobra de la pagina actual es menor que 5 lo desperdicio
		if(sobrante < 5){

			void* puntero = punteroMemoria + (config_muse->tamanio_pagina * pag->marco + ubicacionCola);
			desperdicio(sobrante, puntero, pag, posicion);
			pthread_mutex_unlock(&sem->marco);

//			pthread_mutex_unlock(&global);
			//sumando al tamanio lo desperdiciado, todavia sobra tamAnterior? esta en la otra pag
			if(tamAnterior >sobrantePag){
				tamAnterior -= sobrantePag;
				t_pagina* sigPagina = buscar_segmento_pagina(tabla_segmentos, segmento, pag->numero + 1, proceso);
				sem2 = list_get(lista_marcos_memoria, sigPagina->marco);
				pthread_mutex_lock(&sem2->marco);
				//Entra en el sobrante de esta pagina un header?
				if(tamAnterior < 5){
					void* puntero = punteroMemoria + (config_muse->tamanio_pagina*sigPagina->marco);
					desperdicio(tamAnterior,puntero, sigPagina, sigPagina->ultimo_header);
					pthread_mutex_unlock(&sem2->marco);

//					pthread_mutex_unlock(&global);
				}else{
					t_header* cola = convertir(0, sigPagina->marco);
					cola->isFree   = true;
					cola->size     = tamAnterior -5;
					pthread_mutex_unlock(&sem2->marco);

//					pthread_mutex_unlock(&global);
					if(fin == 1){
						pag->esFinPag       = 0;
						sigPagina->esFinPag = 1;
						sigPagina->tamanio_header = tamAnterior -5;
						sigPagina->ultimo_header  = 5;
					} else{

						agregar_bloque_libre(bloquesLibres, sigPagina->numero, segmento, 0, tamAnterior -5);
					}
				}

			}

		}
		if(sobrante >= 5){
			t_header* cola = convertir(ubicacionCola,pag->marco);
			cola->isFree   = true;
			cola->size     = tamAnterior - tamanio -5;

			pthread_mutex_unlock(&sem->marco);
//			pthread_mutex_unlock(&global);
			if(fin ==1 || pag->ultimo_header == (posicion +5)){
				pag->tamanio_header = tamAnterior - tamanio -5;
				pag->ultimo_header  = ubicacionCola + 5;
					if(fin != 1){
					agregar_bloque_libre(bloquesLibres, pag->numero, segmento, ubicacionCola,tamAnterior - tamanio -5 );
					}
			}else{
				agregar_bloque_libre(bloquesLibres, pag->numero, segmento, ubicacionCola,tamAnterior - tamanio -5 );
			}
		}


	}
	//Si la cola no entra en la pag inicial, busco la pagina final de este header
	if(config_muse->tamanio_pagina <= ubicacionCola){
		pthread_mutex_unlock(&sem->marco);
//		pthread_mutex_unlock(&global);
		//Necesito saber cuanto me falta ubicar despues de la pagina actual
		uint32_t reservado = tamanio;
		tamanio     -= sobrantePag;
		int cociente = tamanio/config_muse->tamanio_pagina;

		tamanio -= (config_muse->tamanio_pagina * cociente);
		cociente ++;
		t_pagina* ultPagina = buscar_segmento_pagina(tabla_segmentos, segmento, cociente, proceso);
		sem2 = list_get(lista_marcos_memoria, ultPagina->marco);
		pthread_mutex_lock(&sem2->marco);
//		pthread_mutex_lock(&global);
			uint32_t sobrante     = config_muse->tamanio_pagina - tamanio;
			uint32_t libreOtraPag = tamAnterior-reservado-sobrante;
			if(sobrante <5 ){

				void* puntero = punteroMemoria + (config_muse->tamanio_pagina*ultPagina->marco + tamanio);

				desperdicio(sobrante, puntero,ultPagina, ultPagina->ultimo_header);
				pthread_mutex_unlock(&sem2->marco);
//				pthread_mutex_unlock(&global);
				//Me quedo tamanio libre del espacio original?

				if(libreOtraPag > 0){
					ultPagina = buscar_segmento_pagina(tabla_segmentos, segmento, cociente+1, proceso);
					sem2 = list_get(lista_marcos_memoria, ultPagina->marco);
					pthread_mutex_lock(&sem2->marco);
//					pthread_mutex_lock(&marcos_memoria[ultPagina->marco]);
//					pthread_mutex_unlock(&global);
						puntero = punteroMemoria + config_muse->tamanio_pagina*ultPagina->marco;
				}
				if(libreOtraPag > 0 && libreOtraPag<5){
					desperdicio(libreOtraPag, puntero, ultPagina, ultPagina->ultimo_header);
					pthread_mutex_unlock(&sem2->marco);
//					pthread_mutex_unlock(&marcos_memoria[ultPagina->marco]);
//					pthread_mutex_unlock(&global);

				}
				if(libreOtraPag >=5){
					t_header* cola = puntero;
					cola->isFree   = true;
					cola->size     = libreOtraPag-5;

//					pthread_mutex_unlock(&global);
					pthread_mutex_unlock(&sem2->marco);
					agregar_bloque_libre(bloquesLibres, ultPagina->numero, segmento, 0, libreOtraPag-5);
				}
			}
			if(sobrante >= 5){
				t_header* cola = punteroMemoria + (config_muse->tamanio_pagina*ultPagina->marco + tamanio);
				cola->isFree   = true;
				cola->size     = libreOtraPag-5;
				pthread_mutex_unlock(&sem2->marco);
//				pthread_mutex_unlock(&marcos_memoria[ultPagina->marco]);
//				pthread_mutex_unlock(&global);
				if(ultPagina->ultimo_header< (tamanio +5)){
					ultPagina->ultimo_header  = tamanio + 5;
					ultPagina->tamanio_header = libreOtraPag-5;
				}
				agregar_bloque_libre(bloquesLibres, ultPagina->numero, segmento, tamanio, libreOtraPag-5);
			}

	}

}


bool mapeoElArchivo(t_segmento* seg, t_proceso* proceso){
	bool busco_archivo(void* arch){
				return (strcmp(((t_archivo*)arch)->nombre,seg->path) ==0) ;
			}

	t_archivo* archivo = list_find(tabla_archivos, &busco_archivo);

	if(archivo != NULL){

		bool busco_id_ip(void* pro){
					return (strcmp(((t_ip_id*)pro)->ip,proceso->ip) ==0) &&
						   ((t_ip_id*)pro)->id == proceso->id;
				}
		t_ip_id* existe = list_find(archivo->proceso, &busco_id_ip);
		if(existe!= NULL){
			return true;
		}else{
			return false;
		}

	}else{
		return false;
	}


}

int unmapMuse(uint32_t  fd,t_proceso* proceso){
	 fd -=5;
	 t_segmento* seg = buscar_segmento(fd, proceso->segmentos);


	 if(seg == NULL){
		 log_error(logMuse, "No es una posicion valida \n");

		 return -5;
	 }

	 if(seg->dinamico){
		 log_error(logMuse, "No es una posicion mappeada \n");
		 return -1;
	 }

	 if(seg->shared == SHARED ){
		 if(seg->path == NULL){
		 log_error(logMuse, "No esta completo el path en el segmento \n");
		 return -1;}
	 }
	 proceso->memLiberada += list_size(seg->paginas)*config_muse->tamanio_pagina;
	 //Tengo que desligar la memoria
	 if(seg->shared == SHARED){
	 bool busco_archivoo(void* arch){
	 				return (strcmp(((t_archivo*)arch)->nombre,seg->path) ==0) ;
	 			}

	 	t_archivo* archivo = list_find(tabla_archivos, &busco_archivoo);
	 	if(archivo == NULL){
			 log_error(logMuse, "El archivo no se encuentra en la tabla de archivos abiertos \n");
			 return -1;
	 	}

	 	bool busco_id_ips(void* pro){
	 						return (strcmp(((t_ip_id*)pro)->ip,proceso->ip) ==0) &&
	 							   ((t_ip_id*)pro)->id == proceso->id;
	 					}

	 	//Elimino de la tabla de procesos del archivo al proceso

	 	t_ip_id* existe = list_remove_by_condition(archivo->proceso, &busco_id_ips);
	 	if(existe == NULL){
			 log_error(logMuse, "El archivo no habia sido mapeado por el proceso o ya realizo muse_unmap \n");
			 return -1;
	 	}
	    //Vaciar el segmento mappeado en  el proceso SIN eliminar la tabla de paginas

	 	if(seg->ultimo){


		 	bool numero_seg(void* segmento){
				return   ((t_segmento*)segmento)->segmento == seg->segmento;
			}
	 		seg = list_remove_by_condition(proceso->segmentos, &numero_seg);
	 		t_segmento* anterior = buscar_segmento(seg->base - config_muse->tamanio_pagina, proceso->segmentos);
	 		if(anterior != NULL)
	 		anterior->ultimo = true;

	 		if(seg->path != NULL) free(seg->path);
	 		free(seg);


	 	}else{
	 		seg->dinamico = true;
	 		seg->empty    = true;
	 		seg->paginas  = NULL;
	 		//VER QUE NO ROMPAAAAAAAAAAAAAAAAAAA
	 		free(seg->path);

	 		bool emp = true;
			uint32_t posSig = seg->base + seg->tamanio;
	 		while(emp){
	 			t_segmento* sigiente = buscar_segmento(posSig, proceso->segmentos);
	 				bool numero_sigiente(void* segmento){
						return   ((t_segmento*)segmento)->segmento == sigiente->segmento;
					}
					seg->tamanio += sigiente->tamanio;
					posSig = sigiente->base + sigiente->tamanio;
					sigiente = list_remove_by_condition(proceso->segmentos, &numero_sigiente);

					free(sigiente);

	 		}
	 	}

	 	//Elimino al proceso del registro del archivo en la tabla de archivos
	 	free(existe->ip);
	 	free(existe);

	 	if(list_size(archivo->proceso)>0){
	 		log_info(logMuse, "Hay mas procesos mapeando aun al archivo \n");
	 		return 0;
	 	}

	 	//Si la tabla de procesos del archivo quedo vacio tengo que eliminar el segmento del proceso fantasma y
	 	// eliminar cualquier pag que este en el clock

	 	log_info(logMuse, "NO Hay mas procesos mapeando al archivo, elimino las paginas \n");
	 	pthread_mutex_lock(&sem_procesos);
	 	t_proceso* fantasma = list_get(tabla_procesos, 0);
	 	pthread_mutex_unlock(&sem_procesos);
	 	bool es_seg(void* segmento){
				return   ((t_segmento*)segmento)->segmento == archivo->seg;
			}
	 	t_segmento* segFantasma = list_remove_by_condition(fantasma->segmentos, &es_seg);

	 	for(int i = 0; i<list_size(segFantasma->paginas);i++){
	 		pthread_mutex_lock(&sem_cant_pag);
	 		paginas_usadas --;
	 		pthread_mutex_unlock(&sem_cant_pag);
			t_pagina* pagina = list_remove(segFantasma->paginas, i);
			modificar_clock_bitmap(pagina, segFantasma->segmento, fantasma);
			free(pagina);
		}

		free(segFantasma->path);
		free(segFantasma);
		return 0;
	 }
	 else{//SEGMENTO PRIVADO


		 vaciarSegmento(seg, proceso->bloquesLibres, proceso->segmentos, proceso);

	 }

	 return 0;
}


int syncMuse(uint32_t fd,size_t len,t_proceso* proceso){

	 fd -=5;
	 t_segmento* seg = buscar_segmento(fd, proceso->segmentos);

	 if(seg == NULL){
		 log_error(logMuse, "No es una posicion valida \n");

		 return -5;
	 }
	 if(seg->dinamico){
		 log_error(logMuse, "No es una posicion mappeada \n");
		 return -5;
	 }

	 if(seg->shared == SHARED ){
		 if(seg->path == NULL){
		 log_error(logMuse, "No esta completo el path en el segmento \n");
		 return -1;}

		 if(!mapeoElArchivo(seg, proceso)){
			 log_error(logMuse, "El proceso no mmapeo el archivo o ya realizo munmap \n");
			 return -1;
		 }
	 }
	 if(seg->tamanio < len){
		 log_error(logMuse, "El proceso se mapeo con menos tamanio que el que se quiere sincrinizar \n");
		 return -1;
	 }

	 int tam = calcular_paginas_malloc(len);//necesito saber cuantas paginas escribir
	 if(tam > list_size(seg->paginas)) tam = list_size(seg->paginas);

		FILE* buffer_archivo = fopen(seg->path, "wb+"); // Creo un nuevo archivo para persistir el bloque (píso el anterior si existe)
//	 int fd_archivo = open(seg->path, O_RDWR, S_IRUSR | S_IRGRP | S_IROTH);
	 if ( buffer_archivo == NULL){
//	 if(fd_archivo <0){
			log_error(logMuse, "No se pudo abrir el archivo, verificar que aun existe. \n");
			return -1;
		}

		char mode[] = "0777"; // Permisos totales
		int permisos = strtol(mode, 0, 8); // Administración para los permisos
		chmod(seg->path, permisos); // Aplico permisos al archivo

//En la primer pagina tomo los 4 primeros porque rompe

//		char* auxiliar = string_repeat('\0', len);

	 for(int i = 0; i<tam;i++){
		 t_pagina* pag = buscar_segmento_pagina(proceso->segmentos, seg->segmento, i, proceso);
			t_semaforo* sem = list_get(lista_marcos_memoria, pag->marco);
			pthread_mutex_lock(&sem->marco);

		 void* puntero = punteroMemoria + (config_muse->tamanio_pagina * pag->marco);


			fwrite(puntero, CHAR, config_muse->tamanio_pagina,buffer_archivo );

		pthread_mutex_unlock(&sem->marco);

		 t_clock* clock = buscar_clock(pag->marco);
		 clock->m = 0;


	 }

//	 csree(auxiliar);
	int close = fclose(buffer_archivo);
	if(close != 0){
		log_error(logMuse, "No se cerro bien el archivo u.u \n");
	}
	 return 0;

}




//Voy a asumir que el len es siempre igual para todos los que comparten sino me vuelvo loca
int crearPaginasmapeadas(int tam,size_t len,t_segmento* segmento,t_proceso* proceso, int flag, char* path){
	t_archivo* archivo;
	t_segmento * seg;
	t_proceso* fantasma;
	t_semaforo* sem;
	char mode[] = "0777"; // Permisos totales
	int permisos = strtol(mode, 0, 8); // Administración para los permisos
	chmod(path, permisos); // Aplico permisos al archivo

//	char*  path1 = string_new();
//	string_append(&path1, "/home/utnso/workspace/tp-2019-2c-capitulo-2/Memoria/Debug/una/ruta.txt" );
//	string_append(&path, ".txt" );
	int fd_archivo = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
//	FILE* fd_archivo = fopen(path, "w+");
	if(fd_archivo == -1){
		log_error(logMuse, "no se hacer open");
//	if(fd_archivo == NULL){
		//vacio el segmento y deberia devolver error
		segmento->dinamico =  true;
		segmento->empty    =  true;
		free(segmento->path);
		return -1;
	}

	//Busco el proceso fantasma

	if(flag == SHARED){
		pthread_mutex_lock(&sem_procesos);
		 fantasma = list_get(tabla_procesos, 0);
		 pthread_mutex_unlock(&sem_procesos);
		 seg = malloc(sizeof(t_segmento));
		t_segmento * anterior = list_get(proceso->segmentos, list_size(proceso->segmentos)-1);
	if(anterior != NULL){
		seg->base =  anterior->base * anterior->tamanio;
		seg->segmento = anterior->segmento +1;
	}else{
		seg->base = 0;
		seg->segmento = 0;
	}
		seg->empty   = false;
		seg->shared  = true;
		seg->tamanio = config_muse->tamanio_pagina * tam;
		seg->paginas =list_create();
		seg->path = malloc(strlen(path)+1);
		memcpy(seg->path, path, strlen(path));
		seg->path[strlen(path)]= '\0';
		list_add(fantasma->segmentos, seg);
	}
	void* punteroAux;
	t_pagina* pagina;
	for(int i=0; i<tam; i++){

		pagina = malloc(sizeof(t_pagina));

		pagina->numero = i;
		pagina->p      = 0;
		pthread_mutex_lock(&sem_bitmap_swap);
		pagina->marco  = buscar_marco_libre(bitmap_swap);
		pthread_mutex_unlock(&sem_bitmap_swap);
			if(pagina->marco >=0){
				sem = list_get(lista_marcos_swap, pagina->marco);
				pthread_mutex_lock(&sem->marco);

			}

		punteroAux = punteroSwap + (config_muse->tamanio_pagina * pagina->marco);

		if(pagina->marco == -1){
			pthread_mutex_lock(&sem_bitmap_marco);
			pagina->marco  = buscar_marco_libre(bitmap_marcos);
			pthread_mutex_unlock(&sem_bitmap_marco);
			sem = list_get(lista_marcos_memoria, pagina->marco);
			pthread_mutex_lock(&sem->marco);

			pagina->p = 1;

			punteroAux = punteroMemoria + (config_muse->tamanio_pagina * pagina->marco);
		}

//		pthread_mutex_lock(&global);
		if(flag == SHARED){
			list_add(seg->paginas , pagina); // Creo la lista de paginas en el segmento fantasma
			if(pagina->p == 1){
				agregar_pag_clock(-1, "0.0.0", 0, 1, seg, pagina->numero, pagina->marco);
			}
		}else{
			list_add(segmento->paginas, pagina);
			if(pagina->p ==1){
				agregar_pag_clock(proceso->id, proceso->ip, 0, 1, segmento, pagina->numero, pagina->marco);
			}
		}
//		const char* contenido = malloc(config_muse->tamanio_pagina);
		  if(i == 0){
			  int c ;
			  read(fd_archivo, &c, sizeof(int));
			  memcpy(punteroAux, &c, 4);
			  char* contenido = malloc(config_muse->tamanio_pagina-4);
			  		int res = read(fd_archivo, contenido, config_muse->tamanio_pagina-4);
			  		if(res != 0){

			  			memcpy(punteroAux+4, contenido, config_muse->tamanio_pagina-4);

			  			free(contenido);
			  		}
			  		pthread_mutex_unlock(&sem->marco);
			  		continue;
		  }
		char* contenido = malloc(config_muse->tamanio_pagina);
		int res = read(fd_archivo, contenido, config_muse->tamanio_pagina);
		if(res != 0){

			memcpy(punteroAux, contenido, config_muse->tamanio_pagina);
			pthread_mutex_unlock(&sem->marco);
		}
		else{
			char* dest;
			if(pagina->p == 0) dest = punteroSwap + (config_muse->tamanio_pagina * pagina->marco);

			if(pagina->p == 1) dest = punteroMemoria + (config_muse->tamanio_pagina * pagina->marco);

	        int i= 0;
			while(i<config_muse->tamanio_pagina){
	            dest[i] = '\0';
	            i++;
			}
			pthread_mutex_unlock(&sem->marco);
		}
		free(contenido);


	}
	if(flag == SHARED){

		 segmento->paginas = seg->paginas;
		 archivo = malloc(sizeof(t_archivo));
		 archivo->nombre = malloc(strlen(path)+1);
		 memcpy(archivo->nombre, path, strlen(path));
		 archivo->nombre[strlen(path)]= '\0';
		 archivo->proceso = list_create();
		 archivo->puntero_a_pag = seg->paginas;
		 archivo->seg = seg->segmento;
		 list_add(tabla_archivos, archivo);
	}

	close(fd_archivo);
	return 0;

}

uint32_t  crearSegmentoMapeado(int len,t_proceso* proceso, int flag, char* path){

	int res = 0;
	int size = list_size(proceso->segmentos) -1;
	int tam = calcular_paginas_malloc(len);
	pthread_mutex_lock(&sem_cant_pag);
	if(flag == 0 && paginas_usadas + tam > config_muse->paginas_totales){
		pthread_mutex_unlock(&sem_cant_pag);
		return 0;
	}

	bool buscar_archivo(void* arch){
						return (strcmp(((t_archivo*)arch)->nombre,path) ==0) ;
					}
			t_archivo* archivo = list_find(tabla_archivos, &buscar_archivo);
	if(archivo == NULL && paginas_usadas + tam > config_muse->paginas_totales){
		pthread_mutex_unlock(&sem_cant_pag);
				return 0;
	}

	paginas_usadas +=tam;
	pthread_mutex_unlock(&sem_cant_pag);
	t_segmento* segmento =  malloc(sizeof(t_segmento));
	if(list_size(proceso->segmentos) > 0){
		bool es_ultimo(void* seg){
			return ((t_segmento*)seg)->ultimo;
		}
		t_segmento* seg_anterior = list_find(proceso->segmentos, &es_ultimo);
		seg_anterior->ultimo = false;
		segmento->base = seg_anterior->base + seg_anterior->tamanio;
		t_segmento* seg_ultimo_agregado = list_get(proceso->segmentos, size);
		segmento->segmento      = seg_ultimo_agregado->segmento;

		uint32_t empieza = seg_anterior->base + seg_anterior->tamanio;
		uint32_t fin     = tam * config_muse->tamanio_pagina;
		if(!addition_is_safe(empieza, fin)){
				free(segmento);
				return 0;
			}

	}else{
		segmento->base = 0;
		segmento->segmento = 0;
	}
	segmento->dinamico = false;
	segmento->empty    = false;
	segmento->shared   = flag;
	segmento->ultimo   = true;
	segmento->tamanio  = config_muse->tamanio_pagina * tam;

	if(flag == PRIVATE){
		segmento->paginas = list_create();
		segmento->path = malloc(strlen(path)+1);
		memcpy(segmento->path, path, strlen(path));
		segmento->path[strlen(path)]='\0';
	 	res = crearPaginasmapeadas(tam, len, segmento, proceso, flag, path);
	 	if(res >=0) list_add(proceso->segmentos, segmento );
	}
	else{

//		bool buscar_archivo(void* arch){
//					return (strcmp(((t_archivo*)arch)->nombre,path) ==0) ;
//				}
		archivo = list_find(tabla_archivos, &buscar_archivo);

		if(archivo == NULL){

		res = crearPaginasmapeadas(tam, len, segmento, proceso, flag, path);

		}else{
			pthread_mutex_lock(&sem_cant_pag);
			paginas_usadas -=tam;
			pthread_mutex_unlock(&sem_cant_pag);
			segmento->paginas = archivo->puntero_a_pag;
		}
		//puede que se haya creado despues del if
		archivo = list_find(tabla_archivos, &buscar_archivo);
		if(res >=0 && archivo != NULL){

			segmento->path = malloc(strlen(path)+1);
			memcpy(segmento->path, path, strlen(path));
			segmento->path[strlen(path)]='\0';

			list_add(proceso->segmentos, segmento );
			t_ip_id* nuevo = malloc(sizeof(t_ip_id));
			nuevo->id = proceso->id;
			nuevo->ip = malloc(strlen(proceso->ip)+1);
			memcpy(nuevo->ip, proceso->ip, strlen(proceso->ip));
			nuevo->ip[strlen(proceso->ip)]='\0';
			list_add(archivo->proceso, nuevo);
		}
	}
	if(res>=0) {
		return segmento->base + 5;
	}else{
		return 0;
	}



}


uint32_t  mappearMuse(char* path, size_t len,int flag,t_proceso* proceso){

	//busco un segmento vacio en donde entre, sino creo uno
	int result = 0;
	int seg    = -1;
		for(int i = 0; i<list_size(proceso->segmentos); i++){
			t_segmento* segmentoVacio = list_get(proceso->segmentos, i);
			if(segmentoVacio->empty && segmentoVacio->tamanio >= len){
				seg = i;
				break;
			}

		}

		if(seg == -1){
			log_info(logMuse, "Se crea un nuevo segmento mapeado\n");
			uint32_t res = crearSegmentoMapeado(len, proceso, flag, path);
			return res;
		}else{
				log_info(logMuse, "Ocupa el segmento vacio: %d \n", seg);
				t_segmento* segmentoVacio = list_get(proceso->segmentos, seg);
				int tam = calcular_paginas_malloc(len);

				if(flag == PRIVATE){
					segmentoVacio->paginas = list_create();
				 	crearPaginasmapeadas(tam, len, segmentoVacio, proceso, flag, path);
				}
				else{
					bool buscar_archivo(void* arch){
								return (strcmp(((t_archivo*)arch)->nombre,path) ==0) ;
							}
					t_archivo* archivo = list_find(tabla_archivos, &buscar_archivo);
					if(archivo == NULL){
					 result = crearPaginasmapeadas(tam, len, segmentoVacio, proceso, flag, path);
					}else{
						segmentoVacio->paginas = archivo->puntero_a_pag;
					}
					if(result >0){
						segmentoVacio->path = malloc(strlen(path)+1);
						memcpy(segmentoVacio->path, path, strlen(path));
						segmentoVacio->path[strlen(path)]='\0';


						t_ip_id* nuevo = malloc(sizeof(t_ip_id));
						nuevo->id = proceso->id;
						nuevo->ip = malloc(strlen(proceso->ip)+1);
						memcpy(nuevo->ip, proceso->ip, strlen(proceso->ip));
						nuevo->ip[strlen(proceso->ip)]='\0';
						list_add(archivo->proceso, nuevo);
					}
				}
				if(result >=0){
					 uint32_t tam_anterior   = segmentoVacio->tamanio;
					 segmentoVacio->empty    = false;
					 segmentoVacio->dinamico = false;
					 segmentoVacio->ultimo   = false;
					 segmentoVacio->shared   = flag;
					 segmentoVacio->tamanio  = config_muse->tamanio_pagina * tam;
						 if(tam_anterior - segmentoVacio->tamanio > 0){
						 t_segmento* nuevo = malloc(sizeof(t_segmento));
						 nuevo_segmento(nuevo, segmentoVacio->base + segmentoVacio->tamanio, true, true,list_size(proceso->segmentos), true, tam_anterior - segmentoVacio->tamanio, false );
						 list_add(proceso->segmentos, nuevo);
						 }

					return segmentoVacio->base +5;
				} else{
					return 0;
				}
			}



}


int calcular_paginas_malloc(uint32_t tamanio){

	int cociente   = (tamanio)/config_muse->tamanio_pagina;
	int resto      = (tamanio)%config_muse->tamanio_pagina;
	int resto_pag  = config_muse->tamanio_pagina - (tamanio - (config_muse->tamanio_pagina * cociente));

	if(resto == 0){
		return cociente;
	}
	else{
		if(resto_pag >0 && resto_pag < 5){
			return cociente +1;// decia mas 2 pero creo que es solo 1
		}else{
		return cociente +1;

		}
	}
}

int buscar_marco_libre(char* bitmap){
	//reviso si hay un marco libre
	int i;
	for( i = 0; i<strlen(bitmap); i++){
		if(bitmap[i] == '0'){
		   bitmap[i] = '1';
			return i;
			}
	}
		return -1;
}

uint32_t crearPaginas(int cant_pag, uint32_t tamanio, t_segmento* segmento, t_list* bloques_libres, t_proceso* proceso){


	int size_tabla = list_size(segmento->paginas) -1;
	uint32_t retorno;
	uint32_t faltante;
	uint32_t sobrante;
	t_semaforo* sem;

	tamanio +=5;
	for(int i = 1; i<=cant_pag; i++){

		size_tabla++;
		t_pagina* pagina = malloc(sizeof(t_pagina));

		if(size_tabla>0 && i ==1){

			t_pagina* ultima_pagina = list_get(segmento->paginas, size_tabla-1);
			if (ultima_pagina->tamanio_header>0){
				tamanio-=5;
				free(pagina);
				size_tabla--;
					if(ultima_pagina->p ==1){
						sem = list_get(lista_marcos_memoria, ultima_pagina->marco);
						pthread_mutex_lock(&sem->marco);
					}
					if(ultima_pagina->p==0){
						ultima_pagina->marco = swap(ultima_pagina, true, proceso, segmento);
						sem = list_get(lista_marcos_memoria, ultima_pagina->marco);
						pthread_mutex_lock(&sem->marco);
//						agregar_pag_clock(proceso->id, proceso->ip, 0, 1, segmento, ultima_pagina->numero, ultima_pagina->marco);

						//Tengo que editar el clock
					}
				ultima_pagina->p=1;
//				pthread_mutex_lock(&marcos_memoria[ultima_pagina->marco]);


				t_header* head = convertir((ultima_pagina->ultimo_header-5), ultima_pagina->marco);

retorno = segmento->base + (config_muse->tamanio_pagina*ultima_pagina->numero + (ultima_pagina->ultimo_header));
				head->isFree = false;
				head->size   = tamanio;

				uint32_t sob_pag = config_muse->tamanio_pagina - (ultima_pagina->ultimo_header);
				if(sob_pag<tamanio){
					pthread_mutex_unlock(&sem->marco);
					tamanio = tamanio  - sob_pag;
					ultima_pagina->esFinPag = 0;
					ultima_pagina->tamanio_header = 0;

				}else{
					sob_pag -= tamanio;
					if(sob_pag>=5){
						t_header* cola = convertir(ultima_pagina->ultimo_header+tamanio,ultima_pagina->marco );
						cola->isFree   = true;
						cola->size     = sob_pag-5;
						pthread_mutex_unlock(&sem->marco);
						ultima_pagina->esFinPag = 1;
						ultima_pagina->tamanio_header = sob_pag-5;
						ultima_pagina->ultimo_header  = ultima_pagina->ultimo_header + 5+tamanio;
						if(!segmento->ultimo){
							agregar_bloque_libre(bloques_libres, ultima_pagina->numero, segmento->segmento, ultima_pagina->ultimo_header, ultima_pagina->tamanio_header);
						}
					}else{
						void* puntero = punteroMemoria + ultima_pagina->marco*config_muse->tamanio_pagina + (ultima_pagina->ultimo_header + tamanio);
						desperdicio(sob_pag, puntero, ultima_pagina, ultima_pagina->ultimo_header);
						pthread_mutex_unlock(&sem->marco);
						ultima_pagina->esFinPag = 1;

					}

				}
//				pthread_mutex_unlock(&global);
//				pthread_mutex_unlock(&marcos_memoria[ultima_pagina->marco]);

				continue;
			}
		}

		pthread_mutex_lock(&sem_bitmap_marco);
		pagina->marco = buscar_marco_libre(bitmap_marcos);
		pthread_mutex_unlock(&sem_bitmap_marco);
		pagina->p=1;

		pagina->numero   = size_tabla;
		pagina->esFinPag = 0;
		pagina->tamanio_header = 0;
		pagina->ultimo_header  = 0;


		if(pagina->marco != -1){ //Lo agrego al clock
			if(i==1 || i==cant_pag){
				sem = list_get(lista_marcos_memoria, pagina->marco);
				pthread_mutex_lock(&sem->marco);
			}
			agregar_pag_clock(proceso->id, proceso->ip, 0, 1, segmento, pagina->numero, pagina->marco);

		}
		if(pagina->marco == -1){
			pthread_mutex_lock(&sem_bitmap_swap);
		   pagina->marco  = buscar_marco_libre(bitmap_swap);
		   pthread_mutex_unlock(&sem_bitmap_swap);
		   pagina->p = 0;

		   	   if(i==cant_pag || i==1){
		   		    pagina->marco = swap(pagina, true, proceso, segmento);
		   		    sem = list_get(lista_marcos_memoria, pagina->marco);
		   		 	pthread_mutex_lock(&sem->marco);
//		   		 	pagina->p=1; //esto ahora lo hace el swap
//					agregar_pag_clock(proceso->id, proceso->ip, 0, 1, segmento, pagina->numero, pagina->marco);

		   	   }

				}
		if(i==1){

			t_header* head = convertir(0, pagina->marco);
			retorno = segmento->base + (config_muse->tamanio_pagina* size_tabla + 5);
			head->isFree = false;
			head->size   = tamanio - 5;


	   }


		if(tamanio>config_muse->tamanio_pagina){
					faltante = tamanio - config_muse->tamanio_pagina;
					tamanio  = faltante;
						if(i==1){
							pagina->ultimo_header  = 5;
							pagina->tamanio_header = 0;
						}
					}
				else{
					sobrante = config_muse->tamanio_pagina -tamanio;
					if(sobrante >= 5){
						t_header* cola = punteroMemoria + (pagina->marco*config_muse->tamanio_pagina + tamanio);
						cola->isFree = true;
						cola->size   = sobrante-5;

						pagina->ultimo_header = tamanio +5;
						pagina->tamanio_header = sobrante-5;
						pagina->esFinPag =1;
						if(!segmento->ultimo){
							agregar_bloque_libre(bloques_libres,pagina->numero, segmento->segmento, (pagina->ultimo_header-5), pagina->tamanio_header);
						}

					}else{
						//Voy a desperdiciar si sobra menos de 5 #$%$#
						desperdicio(sobrante,  punteroMemoria + (pagina->marco*config_muse->tamanio_pagina + tamanio), pagina, pagina->ultimo_header);
						pagina->esFinPag       = 1;

					}

				}
//		paginas_usadas ++;
		if(i==cant_pag || i==1){
			pthread_mutex_unlock(&sem->marco);
		}
		list_add(segmento->paginas, pagina);


	}


	return retorno;
}



uint32_t crearSegmentoDinamico(uint32_t tamanio, t_list* tabla_segmentos, t_list* bloques_libres, t_proceso* proceso){

	uint32_t extra = tamanio +5;
	int tam = calcular_paginas_malloc(extra);
	pthread_mutex_lock(&sem_cant_pag);
	if(paginas_usadas + tam > config_muse->paginas_totales){
		pthread_mutex_unlock(&sem_cant_pag);
		return 0;
	}
	paginas_usadas += tam;
	pthread_mutex_unlock(&sem_cant_pag);
	int ind = list_size(tabla_segmentos) -1;

	t_segmento* seg_nuevo = malloc(sizeof(t_segmento));

	if(ind>0){
		bool es_ultimo(void* seg){
			return ((t_segmento*)seg)->ultimo;
		}
		t_segmento* seg_anterior = list_find(tabla_segmentos, &es_ultimo);
		seg_anterior->ultimo     = false;
		seg_nuevo->base          = seg_anterior->base + seg_anterior->tamanio;
		t_segmento* seg_ultimo_agregado = list_get(tabla_segmentos, ind);
		seg_nuevo->segmento      = seg_ultimo_agregado->segmento +1;
	}else{
		seg_nuevo->base          = 0;
		seg_nuevo->segmento      = 0;
	}
	if(!addition_is_safe(seg_nuevo->base, tamanio+5)){
		pthread_mutex_lock(&sem_cant_pag);
		paginas_usadas -= tam;
		pthread_mutex_unlock(&sem_cant_pag);
		free(seg_nuevo);
		return 0;
	}
	seg_nuevo->tamanio  = tam*config_muse->tamanio_pagina;
	seg_nuevo->dinamico = true;
	seg_nuevo->shared   = true;
	seg_nuevo->ultimo   = true;
	seg_nuevo->paginas  = list_create();

	uint32_t res = crearPaginas(tam, tamanio, seg_nuevo, bloques_libres, proceso);
	if (res >=0){
	seg_nuevo->empty = false;
	 list_add(tabla_segmentos, seg_nuevo);
		}
	log_info(logMuse, "Nuevo segmento: %d, base: %d \n", seg_nuevo->segmento, seg_nuevo->base);
	return res;
}


void remover_bloque_libre(t_list* bloquesLibres , int pag, int seg, uint32_t pos){ //int pag, int seg){

	bool buscar_seg_pag_pos(void* lib){
			return 	(((t_libres*)lib)->pagina == pag
					&& ((t_libres*)lib)->segmento == seg
					&& ((t_libres*)lib)->posicion == pos);

			}
		t_libres* libre = list_remove_by_condition(bloquesLibres, &buscar_seg_pag_pos);
		if(libre!=NULL){
		free(libre);
		}
}


uint32_t mallocMuse(uint32_t tamanio, t_proceso* proceso){
//No olvidar, yo NO agrego los ultimos espacios libres de las paginas en bloques libres
	t_list* bloquesLibres   = proceso->bloquesLibres;
	t_list* tabla_segmentos = proceso->segmentos;

	bool mayor_tamanio(t_libres* bloque_libre){
		int tamanio_cinco = tamanio + 5;
		return bloque_libre->tamanio >= tamanio_cinco || bloque_libre->tamanio == tamanio;
	}

	//Busco en los bloques libres
	    t_libres*  valor_libre = list_find(bloquesLibres, (void* )mayor_tamanio );
		if(valor_libre!=NULL){
			bool buscar_seg(void* elSeg){

					return (((t_segmento*)elSeg)->segmento == valor_libre->segmento);

				}

			log_info(logMuse, "Habia hueco libre \n");
			actualizar_header(valor_libre->segmento, valor_libre->pagina,valor_libre->posicion, valor_libre->tamanio, tamanio, tabla_segmentos, bloquesLibres, 0 , proceso);

			t_segmento* segmento  = list_find(tabla_segmentos, &(buscar_seg));
			uint32_t retorno = segmento->base + (config_muse->tamanio_pagina * valor_libre->pagina + (valor_libre->posicion));
			remover_bloque_libre(bloquesLibres,valor_libre->pagina, valor_libre->segmento, valor_libre->posicion);
			return  retorno;

		}

		//reviso si en el utimo header hay lugar
		int i = list_size(tabla_segmentos) - 1;
		if(i != -1){
			//Tengo que buscar el ultimo segmento
			bool es_ultimo(void* seg){
				return ((t_segmento*)seg)->ultimo;
			}
			t_segmento* ult_segmento = list_find(tabla_segmentos, &es_ultimo);
			//SI el segmento es dinámico, voy a extenderlo (IGual a CERO)

			if(ult_segmento->dinamico && !ult_segmento->empty){
				i = list_size(ult_segmento->paginas) - 1;

				t_pagina* ult_pag = list_get(ult_segmento->paginas, i);


						if(ult_pag->tamanio_header >= (tamanio)){

							log_info(logMuse, "Habia hueco libre \n");
							uint32_t retorno = ult_segmento->base + (ult_pag->numero*config_muse->tamanio_pagina + (ult_pag->ultimo_header));
						    actualizar_header(ult_segmento->segmento, ult_pag->numero,ult_pag->ultimo_header, ult_pag->tamanio_header, tamanio, tabla_segmentos, bloquesLibres, 1,   proceso);

							return retorno;

						} else{
							log_info(logMuse, "Se extiende el segmento %d \n", ult_segmento->segmento);
							ult_pag->esFinPag = 0;

							uint32_t extra ;
							if(ult_pag->tamanio_header!=0){
								extra = tamanio - ult_pag->tamanio_header;
							}else{
								extra = tamanio +5;
							}

							int tam = calcular_paginas_malloc(extra);
							pthread_mutex_lock(&sem_cant_pag);
								if(paginas_usadas + tam > config_muse->paginas_totales){
									pthread_mutex_unlock(&sem_cant_pag);
									log_error(logMuse, "No hay más paginas disponibles (tampoco en swap) \n");
									return 0;
								}else{
									paginas_usadas += tam;
									pthread_mutex_unlock(&sem_cant_pag);
								}

								ult_segmento->tamanio += tam*config_muse->tamanio_pagina;

								//sobrepasa los 32 bits?
								uint32_t actual = ult_segmento->base + (config_muse->tamanio_pagina * list_size(ult_segmento->paginas));
								if(addition_is_safe(actual, config_muse->tamanio_pagina * tam )){
									if(ult_pag->tamanio_header!=0){
										tam++;
									}
									uint32_t res = crearPaginas(tam, tamanio, ult_segmento,bloquesLibres, proceso);

									return res;
								}else{//sobrepasa los 32 bits
									log_error(logMuse, "No se puede extender por sobrepasar 32 bits, busco espacio libre \n");
									int seg = -1;
									for(int i = 0; i<list_size(tabla_segmentos); i++){
										t_segmento* segmentoVacio = list_get(tabla_segmentos, i);
										if(segmentoVacio->empty && segmentoVacio->tamanio >= tamanio +5){
											seg = i;
//											break;
										}
									}
									if(seg != -1){
										log_info(logMuse, "Segmento vacio : %d \n", seg);
										t_segmento* segmentoVacio = list_get(tabla_segmentos, seg);
										int tam = calcular_paginas_malloc(tamanio+5);
										uint32_t res = crearPaginas(tam, tamanio, segmentoVacio, bloquesLibres, proceso);
											if (res >=0){
											 uint32_t tam_anterior = segmentoVacio->tamanio;
											 segmentoVacio->empty    = false;
											 segmentoVacio->dinamico = true;
											 segmentoVacio->ultimo   = false;
											 segmentoVacio->tamanio  = config_muse->tamanio_pagina * tam;
											 t_segmento* nuevo = malloc(sizeof(t_segmento));
											 nuevo_segmento(nuevo, segmentoVacio->base + segmentoVacio->tamanio, true, true,list_size(tabla_segmentos), true, tam_anterior - segmentoVacio->tamanio, false );
											 list_add(tabla_segmentos, nuevo);
											}
											return res;
									}else{
										pthread_mutex_lock(&sem_cant_pag);
										paginas_usadas -= tam;
										pthread_mutex_unlock(&sem_cant_pag);
										return 0;
									}


								}
						}



			}else{

		//---------------------------------------------------------------------------------
				//CREAR SEGMENTO
				//antes fijarme si no tengo uno vacio
				int seg = -1;
				for(int i = 0; i<list_size(tabla_segmentos); i++){
					t_segmento* segmentoVacio = list_get(tabla_segmentos, i);
					if(segmentoVacio->empty && segmentoVacio->tamanio >= tamanio +5){
						seg = i;
						break;
					}

				}

				if(seg == -1){
				log_info(logMuse, "Se crea un nuevo segmento dinamico \n");
				uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres, proceso);
				if(res == 0){
					return 0;
				}
				return res;
				}
				else{
					log_info(logMuse, "Ocupa el segmento vacio: %d \n", seg);
					t_segmento* segmentoVacio = list_get(tabla_segmentos, seg);
					int tam = calcular_paginas_malloc(tamanio+5);
					uint32_t res = crearPaginas(tam, tamanio, segmentoVacio, bloquesLibres, proceso);
						if (res >=0 && ((segmentoVacio->tamanio/config_muse->tamanio_pagina) > tam)){
						 uint32_t tam_anterior = segmentoVacio->tamanio;
						 segmentoVacio->empty    = false;
						 segmentoVacio->dinamico = true;
						 segmentoVacio->ultimo   = false;
						 segmentoVacio->tamanio  = config_muse->tamanio_pagina * tam;
						 t_segmento* nuevo = malloc(sizeof(t_segmento));
						 nuevo_segmento(nuevo, segmentoVacio->base + segmentoVacio->tamanio, true, true,list_size(tabla_segmentos), true, tam_anterior - segmentoVacio->tamanio, false );
						 list_add(tabla_segmentos, nuevo);
						}
						return res;
				}

		//---------------------------------------------------------------------------------

			}
	} else { // no existia ningun segmento --> creo uno nuevo
		//antes fijarme si no tengo uno vacio, si no habia segmento poque habria uno vacio? ajajjajaajajaj
						log_info(logMuse, "Se crea un nuevo segmento dinamico \n");
						uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres, proceso);
						if(res == 0){
							return 0;
						}
						return res;


	}
	return 0;
}

void liberarTodo(t_proceso* proceso){

	while(list_size(proceso->segmentos) > 0){


		t_segmento* seg = list_get(proceso->segmentos , 0);

		if(seg->dinamico){
			seg = list_remove(proceso->segmentos , 0);
			while(list_size(seg->paginas) > 0){
				    pthread_mutex_lock(&sem_cant_pag);
					paginas_usadas --;
					pthread_mutex_unlock(&sem_cant_pag);
					t_pagina* pagina = list_remove(seg->paginas, 0);
					modificar_clock_bitmap(pagina, seg->segmento, proceso);
					free(pagina);
				}
			free(seg->paginas)	;
			free(seg);

		} else{ //Es un segmento mapeado
			int res = unmapMuse(seg->base+5, proceso);
			if(res < 0){
				seg = list_remove(proceso->segmentos , 0);
			}
		}

     }//Cierra el while

	while(list_size(proceso->bloquesLibres) > 0){
		t_libres* libre = list_remove(proceso->bloquesLibres, 0);
		free(libre);
	}


	bool buscar_pro(void* pro){
				return (strcmp(((t_proceso*)pro)->ip,proceso->ip) ==0) &&
						((t_proceso*)pro)->id == proceso->id;
			}
	pthread_mutex_lock(&sem_procesos);
	t_proceso* elProc = list_remove_by_condition(tabla_procesos, &buscar_pro);
	pthread_mutex_unlock(&sem_procesos);
	free(elProc->ip);
	free(elProc->segmentos);
	free(elProc->bloquesLibres);
	free(elProc);

}



void atenderConexiones(int parametros){

//	bool sg = false;

	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->cliente = parametros;
	proceso->ip 	 = malloc(INET6_ADDRSTRLEN);
	proceso->segmentos = list_create();
	proceso->bloquesLibres = list_create();
	proceso->memLiberada  = 0;
	proceso->memReservada = 0;


	ip_de_programa(proceso->cliente, proceso->ip);


	while(1){
		int operacion = recibirInt(proceso->cliente);
		if(operacion == -1) operacion= CERRAR;
		switch (operacion) {
		case SALUDO:{

			proceso->id = recibirInt(proceso->cliente);
			if(proceso->id<0){
				enviarInt(proceso->cliente, -1);
				pthread_exit("chau");
			}
			log_info(logMuse, "Se conecto el proceso: %s - %d \n", proceso->ip, proceso->id);
			pthread_mutex_lock(&sem_procesos);
			list_add(tabla_procesos, proceso);
			pthread_mutex_unlock(&sem_procesos);

			enviarInt(proceso->cliente, 0);

		}break;
		case CERRAR:{
			//TEngo que liberar absolutamente todo
			log_info(logMuse,"Se desconecto el proceso %d : \n", parametros);
			printefearMetricas();
			liberarTodo(proceso);

			pthread_exit("CHAU");
		}break;
		case RESERVAR: {

			uint32_t tamanio = recibirUint32_t(proceso->cliente);
				if(tamanio == 0){
					enviarUint32_t(proceso->cliente,  0);
					break;
				}
				log_info(logMuse, "MALLOC  %d \n", tamanio);
				proceso->memReservada += tamanio;
			uint32_t posicion = mallocMuse(tamanio, proceso);
			printefearMetricas();
//			for(int i=0; i<cantidad_paginas;i++){
//				t_semaforo* sem = list_get(lista_marcos_memoria, i);
//				pthread_mutex_lock(&sem->marco);
//				printf("valor del sem %d = 1 \n", i);
//				pthread_mutex_unlock(&sem->marco);
//			}

//			pthread_mutex_unlock(&global);
//			if(posicion == 0){
//				log_error(logMuse,"Comienza compactacion por falta de espacio: \n");
//
//				compactar(proceso->segmentos, proceso->bloquesLibres, proceso);
//
//			    posicion = 	mallocMuse(tamanio, proceso);
			    if(posicion == 0){
			    	log_error(logMuse, "Memoria llena por favor finalice algun proceso \n");
			    	enviarUint32_t(proceso->cliente,  -1);
			    	break;
			    }
//			}
			enviarUint32_t(proceso->cliente,  posicion);

		} break;
		case LIBERAR:{
			uint32_t posicionAliberar = recibirUint32_t(proceso->cliente);
			log_info(logMuse, "FREE %d \n", posicionAliberar);

			int res = freeMuse(posicionAliberar, proceso->segmentos, proceso->bloquesLibres, proceso);
			printefearMetricas();
			enviarInt(proceso->cliente, res);

			if(res == -5){

				 liberarTodo(proceso);
				 pthread_exit("ABORTAR");
			}

		} break;
		case OBTENER:{
			uint32_t posicion = recibirUint32_t(proceso->cliente);
			size_t   bytes    = recibirSizet(proceso->cliente );
			log_info(logMuse, "GET %d ", posicion);

			char* frase = getMuse(posicion, bytes, proceso->segmentos, proceso);
			printefearMetricas();

			if(frase == NULL){
			  enviarVoid(proceso->cliente, frase, 0);
			}else if (strcmp(frase, "SG_MICA")==0){

				enviarVoid(proceso->cliente, frase, strlen("SG_MICA")+1);

				liberarTodo(proceso);
				pthread_exit("CHAU");
				}
			else{
				enviarVoid(proceso->cliente, frase, bytes);
			}
			if(frase == NULL){


				log_error(logMuse, "No se pudo obtener el dato solicitado");

				break;


			}
			else{
				if(bytes == 4){
					log_info(logMuse, "Dato solicitado %d \n", *(int*)frase );
				}else{
					log_info(logMuse, "Dato solicitado %s \n", frase );
				}
				free(frase);
			}




		}break;
		case COPIAR:{
			uint32_t posicionACopiar = recibirUint32_t(proceso->cliente);
			int      bytes = recibirInt(proceso->cliente);

//			char* copia = recibirTexto(proceso->cliente, logMuse);
			void* copia = recibirVoid(proceso->cliente);
			if(bytes==4){
				log_info(logMuse, "COPY %d en %d \n", *(int*)copia, posicionACopiar );
			}else{
				log_info(logMuse, "COPY %s en %d \n", copia, posicionACopiar );
			}
			int resultado = copiarMuse(posicionACopiar, bytes, copia,proceso->segmentos, proceso);
			printefearMetricas();
//			for(int i=0; i<cantidad_paginas;i++){
//				t_semaforo* sem = list_get(lista_marcos_memoria, i);
//				pthread_mutex_lock(&sem->marco);
//				printf("valor del sem %d = 1 \n", i);
//				pthread_mutex_unlock(&sem->marco);
//			}

			free(copia);
			enviarInt(proceso->cliente,  resultado);
			if(resultado == -5){
				liberarTodo(proceso);
				pthread_exit("CHAU");
			}


		}break;
		case MAPEAR: {
			int flag   = recibirInt(proceso->cliente);
			size_t len = recibirSizet(proceso->cliente);
			char* path = recibirTexto(proceso->cliente);
			proceso->memReservada += len;
			pthread_mutex_lock(&sem_mapeo);
			uint32_t result = mappearMuse(path, len, flag, proceso);
			pthread_mutex_unlock(&sem_mapeo);
			printefearMetricas();
//			if(result ==  0){
//				log_error(logMuse,"Comienza compactacion por falta de espacio: \n");
//				compactar(proceso->segmentos, proceso->bloquesLibres, proceso);
//				result = mappearMuse(path, len, flag, proceso);
				free(path);
				if(result == 0) log_error(logMuse, "Memoria llena por favor finalice algun proceso \n");

//			}
			enviarUint32_t(proceso->cliente, result);


		}break;
		case SINCRO:{
			size_t   len = recibirSizet(proceso->cliente);
			uint32_t fd  = recibirUint32_t(proceso->cliente);

			pthread_mutex_lock(&sem_mapeo);
			int result = syncMuse(fd, len, proceso);
			pthread_mutex_unlock(&sem_mapeo);
			printefearMetricas();
			enviarInt(proceso->cliente, result);
			if(result == -5){
				 liberarTodo(proceso);
				 pthread_exit("ABORTAR");
			}
		} break;
		case DESMAP:{
			uint32_t fd = recibirUint32_t(proceso->cliente);

			pthread_mutex_lock(&sem_mapeo);
			int result = unmapMuse( fd, proceso);
			pthread_mutex_unlock(&sem_mapeo);
			printefearMetricas();
			enviarInt(proceso->cliente, result);
			if(result == -5){

				liberarTodo(proceso);
				pthread_exit("CHAU");
			}
		}break;
		case VER:{

			int i= 0;
			while(i<list_size(proceso->segmentos)){
				t_segmento* seg = list_get(proceso->segmentos, i);
				printf("Segmento %d \n", seg->segmento);
				printf("Pagina | ultima | presencia | marco | header | tamanio \n");
				for(int x=0; x<list_size(seg->paginas); x++){
					t_pagina* pag = list_get(seg->paginas, x);
					printf("%d      | %d      | %d         | %d     | %d      | %d   \n",
					pag->numero, pag->esFinPag, pag->p, pag->marco, (pag->ultimo_header-5), pag->tamanio_header );

				}

				i++;
			}
			 i=0;

			 printf("IP      | ID    |  SEG  | PAG | U | M | \n");
			while(i<list_size(tabla_clock)){
				t_clock* clock = list_get(tabla_clock , i);

				printf("%s | %d    |  %d   | %d  | %d | %d | \n", clock->ip, clock->id,
						clock->seg, clock->pag, clock->u, clock->m);
				i++;
			}

		}break;

		default:{

		}


		}


	}



}


void printefearMetricas(){

	if(list_size(tabla_procesos)-1 ==0) return;


	int super_total = 0;
		for(int i = 1; i< list_size(tabla_procesos); i++){
			t_proceso* proceso = list_get(tabla_procesos, i);
			int x = 0;
			int total = 0;
			int frag  = 0;
			int compartida = 0;
			while(x<list_size(proceso->segmentos)){
				t_segmento* seg = list_get(proceso->segmentos, x);

				total += seg->tamanio;
				if(!seg->dinamico){
					compartida += seg->tamanio;
				}
				if(seg->ultimo){
					if(seg->dinamico){

					t_pagina* pag = list_get(seg->paginas, list_size(seg->paginas)-1);
					if(pag->ultimo_header != 0){
						frag = pag->tamanio_header;
					}
				}else{
					t_segmento* anterior = buscar_segmento(seg->base -1, proceso->segmentos);
				   if(anterior!=NULL){
					if(anterior->dinamico){

						t_pagina* pag = list_get(seg->paginas, list_size(seg->paginas)-1);
						if(pag->ultimo_header != 0){
							frag = pag->tamanio_header;
						}else{
							frag = 0;
					}
				    }
				}else{
					frag=0;
				}
				}
				}

			log_info(logMuse, "Metrica del Programa: %d - %s \n", proceso->id, proceso->ip);
			log_info(logMuse, "Memoria Total Pedida: %d bytes \n", proceso->memReservada);
			log_info(logMuse, "Memoria Total Liberada: %d bytes \n", proceso->memLiberada);
			log_info(logMuse, "Porcentaje asignacion Memoria: %d % \n", 100*total/(config_muse->tamanio_total+config_muse->tamanio_swap));
			log_info(logMuse, "Porcentaje Memoria Compartida: %d % \n", 100*compartida/total); //Calculo el porcentaje
			//de memoria compartida sobre el total asignada al proceso nomas
			log_info(logMuse, "Fragmentación interna: %d bytes \n", frag);
			super_total += total;

			x++;
		}


		}
		log_info(logMuse, "Metrica del sistema: Cantidad Memoria Dispoible %d bytes \n",(config_muse->tamanio_total+config_muse->tamanio_swap)-super_total );
}



void ip_de_programa(int s, char* ip){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];


	len = sizeof addr;
	getpeername(s, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in *)&addr;

		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;

		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	memcpy(ip, &(ipstr), INET6_ADDRSTRLEN);
}

int crearSocketMEmoria() {
	int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
	if (fileDescriptor == ERROR) {
		log_error(logMuse, "No se pudo crear el file descriptor \n");
	}

	return fileDescriptor;
}

int crearSocketServidorMemoria(int puerto)	{
	struct sockaddr_in miDireccionServidor;
	int socketDeEscucha = crearSocketMEmoria();

	miDireccionServidor.sin_family = AF_INET;			//Protocolo de conexion
	miDireccionServidor.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0 y significa que usa la IP actual de la maquina
	miDireccionServidor.sin_port = htons(puerto);		//Puerto en el que escucha
	memset(&(miDireccionServidor.sin_zero), '\0', 8);		//Pone 0 a la estructura
		//Veamos si el puerto está en uso
	int puertoEnUso = 1;
	int puertoYaAsociado = setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &puertoEnUso, sizeof(puertoEnUso));

	if (puertoYaAsociado == ERROR) {
		log_error(logMuse, "El puerto asignado ya está siendo utilizado \n");
	}
		//Turno del bind
	int activado = 1;
	setsockopt(socketDeEscucha,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));//Para evitar que falle el bind, al querer usar un mismo puerto

	int valorBind = bind(socketDeEscucha,(void*) &miDireccionServidor, sizeof(miDireccionServidor));

	if ( valorBind !=0) {

		log_error(logMuse, "El bind no funcionó, el socket no se pudo asociar al puerto \n");
		return 1;
	}

	return socketDeEscucha;
}

int crearSocketEscuchaMemoria (int puerto) {

	int socketDeEscucha = crearSocketServidorMemoria(puerto);

	//Escuchar conexiones
		int valorListen;
		valorListen = listen(socketDeEscucha, SOMAXCONN);/*
					SOMAXCONN como segundo parámetro, y significaría el máximo tamaño de la cola*/
		if(valorListen == ERROR) {

			log_error(logMuse, "El servidor no pudo recibir escuchar conexiones de clientes.\n");
		} else	{

			log_info(logMuse, "¡Hola, estoy escuchando! \n");
		}

	return socketDeEscucha;
}


int aceptarClienteMemoria(int fd_socket){

	struct sockaddr_in unCliente;
	memset(&unCliente, 0, sizeof(unCliente));
	unsigned int addres_size = sizeof(unCliente);

	int fd_Cliente = accept(fd_socket, (struct sockaddr*) &unCliente, &addres_size);
	if(fd_Cliente == ERROR)  {
		log_error(logMuse, "El servidor no pudo aceptar la conexión entrante \n");
	} else	{
		log_info(logMuse, "Se conectó un proceso \n");
	}

	return fd_Cliente;

}



void crearHiloParalelos(){

	socket_escucha = crearSocketEscuchaMemoria(config_muse->puerto);

	if(socket_escucha > 0){
	int cliente = 0;

		while(( cliente = aceptarClienteMemoria(socket_escucha)) > 0 ){

			pthread_t hilo;
			pthread_create(&hilo, NULL, (void*)atenderConexiones, (void*)cliente );
			//pthread_join(hilo, NULL);

		}
	}
	else {
		log_error(logMuse, "Error al crear el socket de escucha");
		exit(1);
	}
}




void inicializarMemoria(){

	cantidad_paginas = config_muse->tamanio_total/config_muse->tamanio_pagina;

	config_muse->paginas_totales = cantidad_paginas +  (config_muse->tamanio_swap / config_muse->tamanio_pagina);
//  BIG MALLOC
	punteroMemoria = malloc(config_muse->tamanio_total);

	bitmap_marcos 	= string_repeat('0', cantidad_paginas);

	tabla_archivos = list_create();
	tabla_procesos = list_create();

	//Iniciar tabla clock, no se vacia hasta que muere el programa
	tabla_clock    = list_create();

	//El primero es distinto porque tiene el puntero
	t_clock* clock = malloc(sizeof(t_clock));
			clock->p  =  1;
			clock->marco = 0;
			clock->id = -1;
	list_add(tabla_clock, clock);

	for(int i=1; i<cantidad_paginas; i++){
		clock = malloc(sizeof(t_clock));

		clock->id = -1;
		clock->marco = i;
		list_add(tabla_clock, clock);
	}

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->bloquesLibres = NULL;
	proceso->cliente       = -1;
	proceso->id			   = -1;
	proceso->ip            = "0.0.0";
	proceso->segmentos     = list_create();
	pthread_mutex_lock(&sem_procesos);
	list_add(tabla_procesos, proceso);
	pthread_mutex_unlock(&sem_procesos);

	pthread_mutex_init(&sem_bitmap_swap ,NULL);
	pthread_mutex_init(&sem_bitmap_marco ,NULL);
	pthread_mutex_init(&sem_paginas,NULL);
	pthread_mutex_init(&sem_cant_pag,NULL);
	pthread_mutex_init(&sem_procesos,NULL);
	pthread_mutex_init(&sem_clock,NULL);
	pthread_mutex_init(&sem_mapeo,NULL);


	lista_marcos_memoria = list_create();

	for(int i =0; i<cantidad_paginas; i++){
		t_semaforo* semaforo = malloc(sizeof(t_semaforo));
		pthread_mutex_t marco_memoria;
		pthread_mutex_init(&marco_memoria, NULL);
		semaforo->marco = marco_memoria;
		list_add(lista_marcos_memoria, semaforo);
	}

//	pthread_mutex_init(&multi ,NULL);
//	pthread_mutex_init(&wt ,NULL);
//	pthread_mutex_init(&global ,NULL);


}


void* inicializarSwap(){

	remove(config_muse->rutaSwap);

	paginas_swap = config_muse->tamanio_swap/config_muse->tamanio_pagina;
	bitmap_swap  = string_repeat('0', paginas_swap);



	if(config_muse->tamanio_swap>0){
	char mode[] = "0777"; // Permisos totales
	int permisos = strtol(mode, 0, 8); // Administración para los permisos

	int fd_archivo = open(config_muse->rutaSwap, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);

		chmod(config_muse->rutaSwap, permisos); // Aplico permisos al archivo
		truncate(config_muse->rutaSwap, config_muse->tamanio_swap);
		void* archivo_mapeado = mmap(NULL, config_muse->tamanio_swap, PROT_READ | PROT_WRITE, MAP_SHARED, fd_archivo, 0);


		if ( archivo_mapeado == MAP_FAILED){
			printf("Error al mapear el archivo %s . \n", config_muse->rutaSwap);
			return NULL;
		}
    	char caracter = '\0';
    	int i;
    	for(i=0; i < config_muse->tamanio_swap; i++){
    		write(fd_archivo, &caracter, 1);
    	}

		close(fd_archivo); // Cerramos el archivo pues ya lo tenemos mapeado en memoria, no es necesario mantenerlo abierto

		lista_marcos_swap = list_create();
		for(int i =0; i<paginas_swap; i++){
			t_semaforo* semaforo = malloc(sizeof(t_semaforo));
			pthread_mutex_t marco_swap;
			pthread_mutex_init(&marco_swap, NULL);
			semaforo->marco = marco_swap;

			list_add(lista_marcos_swap, semaforo);
		}


		return archivo_mapeado;
	}


	return NULL;

}


bool existeArchivoConfig(char* ruta) {
	FILE * file = fopen(ruta, "r");
	if (file!=NULL){
		fclose(file);
		return true;//
	} else {
	return false;
	}
}

int crearConfigMemoria(){

	logMuse = log_create("MUSE.txt", "LOG", true, LOG_LEVEL_INFO);

    log_info(logMuse, "Se creó correctamente el Log. \n");

	//Valido que exista la ruta de configuracion

	if (!existeArchivoConfig(rutaConfigMuse)) {
		log_error(logMuse, "Verificar path del archivo \n");
	    return -1;
	}
	//Voy a levantar el archivo y guardar los valores en mi work area

	    	config_muse = malloc(sizeof(t_configuracion));
	        config_ruta = config_create(rutaConfigMuse);

	        if (config_ruta != NULL){

	        	config_muse->puerto 	    = config_get_int_value(config_ruta, "LISTEN_PORT");
	        	config_muse->tamanio_total 	= config_get_int_value(config_ruta, "MEMORY_SIZE");
	        	config_muse->tamanio_pagina	= config_get_int_value(config_ruta, "PAGE_SIZE");
	        	config_muse->tamanio_swap 	= config_get_int_value(config_ruta, "SWAP_SIZE");
	        	config_muse->rutaSwap       = config_get_string_value(config_ruta, "SWAP");


	        }

	        if(config_muse == NULL){
	        	log_error(logMuse, "ERROR No se pudo levantar el archivo de configuración de la MUSE \n");
	        	return -1;
	        }

	        return 0;


}
static void libres_destroy(t_libres *self) {

    free(self);
}

static void seg_destroy(t_segmento *self) {
    if(self->path != NULL){
	free(self->path);
    }
    list_destroy_and_destroy_elements(self->paginas, (void*) pag_destroy);
    free(self);
}

static void proc_destroy(t_proceso *self) {
	if(self->ip != NULL) free(self->ip);
    if(self->bloquesLibres != NULL){
    list_destroy_and_destroy_elements(self->bloquesLibres, (void*) libres_destroy);}
    if(self->segmentos != NULL){
    list_destroy_and_destroy_elements(self->segmentos, (void*) seg_destroy);}
    free(self);
}

static void clock_destroy(t_clock *self) {

    free(self);
}

static void sem_destroy(t_semaforo *self) {
	pthread_mutex_destroy(&self->marco);
    free(self);
}
static void ip_destroy(t_ip_id *self) {
	free(self->ip);
    free(self);
}
static void pag_destroy(t_pagina *self) {
	if(self->p == 1){
		t_clock* clock = list_get(tabla_clock, self->marco);//buscar_clock(proceso, seg, pag);
			free(clock->ip);
	}
    free(self);
}
static void archivos_destroy(t_archivo *self) {
	free(self->nombre);
	list_destroy_and_destroy_elements(self->proceso, (void*) ip_destroy);
	list_destroy_and_destroy_elements(self->puntero_a_pag, (void*) pag_destroy);
    free(self);
}

void handler(){
	log_info(logMuse, "Cerrando MUSE \n");

	if ( munmap(punteroSwap, config_muse->tamanio_swap) == -1 ){
		log_error(logMuse, "Error en el unmaping del archivo de swap");
	}
	free(config_muse);
	free(punteroMemoria);
	free(bitmap_marcos);
	free(bitmap_swap);

	list_destroy_and_destroy_elements(tabla_archivos, (void*) archivos_destroy);

	t_proceso* fantasma = list_remove(tabla_procesos, 0);
//	free(fantasma->ip);
	list_destroy_and_destroy_elements(fantasma->segmentos, (void*) seg_destroy);

	int i=0;
	while(i< list_size(tabla_procesos)){
		t_proceso* aux = list_remove(tabla_procesos, 0);
		int x =0;
		while(x<list_size(aux->segmentos)){
			t_segmento* seg_aux = list_remove(aux->segmentos, 0);
			if(!seg_aux->dinamico && seg_aux->shared){
				free(seg_aux->path);
				free(seg_aux);
				continue;
			}
			list_destroy_and_destroy_elements(seg_aux->paginas, (void*) pag_destroy);

			if(!seg_aux->dinamico){
			free(seg_aux->path);
			}
			free(seg_aux);
		}
		list_destroy(aux->segmentos);

	    list_destroy_and_destroy_elements(aux->bloquesLibres, (void*)libres_destroy);
	    free(aux->ip);

	}
	list_destroy(tabla_procesos);

	list_destroy_and_destroy_elements(tabla_clock, (void*) clock_destroy);

	pthread_mutex_destroy(&sem_bitmap_marco);
	pthread_mutex_destroy(&sem_bitmap_swap);
	pthread_mutex_destroy(&sem_cant_pag);
	pthread_mutex_destroy(&sem_clock);
	pthread_mutex_destroy(&sem_paginas);
	pthread_mutex_destroy(&sem_procesos);

	list_destroy_and_destroy_elements(lista_marcos_memoria, (void*) sem_destroy);
	list_destroy_and_destroy_elements(lista_marcos_swap, (void*) sem_destroy);


	exit(0);
}

int main() {

	signal(SIGINT, &handler);
		remove("MUSE.txt");
		int v_error = crearConfigMemoria();
 		if(v_error == 0){

		inicializarMemoria();

		punteroSwap = inicializarSwap();

		if(punteroSwap != NULL || config_muse->tamanio_swap ==0){
			crearHiloParalelos();


		} else{
			log_error(logMuse, "Error al levantar el area de Swap");
			free(config_muse);
			free(punteroMemoria);
			free(bitmap_marcos);
			free(bitmap_swap);
			list_destroy(tabla_archivos);
			list_destroy(tabla_procesos);


			for(int i=0; i>list_size(tabla_clock);i++){
				t_clock * clock = list_get(tabla_clock, i);
				free(clock->ip);
				free(clock);
			}
			list_destroy(tabla_clock);
		}

		} //esta llave cierra luego de ver que se levanto ok el config
	}
