/*
 * MUSE.c
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */
#include "MUSE.h"

void agregar_bloque_libre(t_list* bloquesLibres, int pagina,int segmento,uint32_t posicionEnPagina, uint32_t tamanioLibre){
	contador++;
	t_libres* newLibre = malloc(sizeof(t_libres));
	  		  newLibre->pagina   = pagina;
	  		  newLibre->segmento = segmento;
	  		  newLibre->posicion = posicionEnPagina;
	  		  newLibre->tamanio  = tamanioLibre;
	  		  newLibre->id       = contador;
	  		  list_add(bloquesLibres, newLibre);
}

t_segmento* buscar_segmento(uint32_t logica, t_list* tabla_segmentos){
	bool buscar_segmento(void* seg){

			return (((t_segmento*)seg)->base >= logica && ((t_segmento*)seg)->base +((t_segmento*)seg)->tamanio  < logica );

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

void desperdicio(uint32_t sobrante, void* posicion, t_pagina* pag){
	  char* desperdicio = string_repeat('#', sobrante);
	  memmove(posicion, desperdicio, sobrante);
	  pag->ultimo_header  = 0;
	  pag->tamanio_header = 0;
	  free(desperdicio);
}

t_pagina* buscar_segmento_pagina(t_list* segmentos , int seg, int pag){

	bool buscar_seg(void* elSeg){

			return (((t_segmento*)elSeg)->segmento == seg);

		}
	bool buscar_pag(void* laPag){

			return (((t_pagina*)laPag)->numero == pag);

		}
	t_segmento* segmento  = list_find(segmentos, &(buscar_seg));
	t_pagina*   pagina    = list_find(segmento->paginas, &(buscar_pag));

	if(pagina->p != 1){
		pagina->marco = swap(pagina->marco);
		pagina->p = 1;
	}

	return pagina;

}

int swap(int pag_swap){
return 0;
}

void vaciarSegmento(t_segmento* segmento, t_list* bloquesLibres ){
	segmento->dinamico = 0;
	segmento->empty    = true;
	for(int i = 0; i<list_size(segmento->paginas);i++){
		t_pagina* pagina = list_remove(segmento->paginas, i);
		free(pagina);
	}
	bool buscar_por_segmento(void* bloque_libre){

		return (((t_libres*)bloque_libre)->segmento == segmento->segmento);
		}
	t_list* nuevos = list_filter(bloquesLibres, &buscar_por_segmento);

	for(int i=0; i<list_size(nuevos);i++){
		t_libres* nuevoLibre = list_get(nuevos, i);
		bool buscar_bloque_id(void* bloque){
			return(((t_libres*)bloque)->id == nuevoLibre->id);
		}
		t_libres* libre = list_remove_by_condition(bloquesLibres, &buscar_bloque_id);
		free(libre);
	}
	void liberar(t_libres* libre){
		free(libre);
	}
	list_clean_and_destroy_elements(nuevos, &liberar);

}


void freeMuse(uint32_t posicionAliberar,t_list* tabla_segmentos,t_list* bloquesLibres){
	//La posicion que me mandan es despues del header
	posicionAliberar -= 5;
	//Primero libero lo que pidieron
	t_segmento* segmento = buscar_segmento(posicionAliberar, tabla_segmentos);
	if(segmento == NULL){
		return;
	}
	uint32_t desplazamiento1,desplazamiento2 , posicion2;
	t_header* head1;
	t_header* head2;
	t_pagina* pagina1;
	t_pagina* pagina2;
	int pag1, pag2, marco2;

	pag1 = posicionAliberar/config_muse->tamanio_pagina;

	pagina1         = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag1);
    desplazamiento1 = posicionAliberar%config_muse->tamanio_pagina;
	head1 		    = punteroMemoria + (config_muse->tamanio_pagina * pagina1->marco + desplazamiento1);
	head1->isFree   = true;
		//Veo si hay desperdicio
		if(config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size) < 5 ){
			head1->size += (config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size));
		}
			//Si era el ultimo header aca muere mi operación de la ultima pag
			if(pagina1->esFinPag == 1 && pagina1->ultimo_header == desplazamiento1){
				pagina1->tamanio_header = head1->size;
				return;
			}

			posicion2 = posicionAliberar + 5 + head1->size;
			pag2      = posicion2/config_muse->tamanio_pagina;
			//Si no era la ultima pagina hay otra y esa puede tener desperdicio
			 if(desplazamiento1 + 5 + head1->size == config_muse->tamanio_pagina){
				 pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2);
				 for(int i= 0; i<5; i++){
					 if((char*)(punteroMemoria + (config_muse->tamanio_pagina * pagina1->marco + (i)))!='#'){
							posicion2 += i;
							head1->size += i;
							break;
					 	}
				 }
			 }

					if(pag2 == pag1){
						marco2 = pagina1->marco;
					} else {
						pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2);
						marco2 = pagina2->marco;
					}
	desplazamiento2 = posicion2%config_muse->tamanio_pagina;
	head2 = punteroMemoria + (config_muse->tamanio_pagina * marco2 + desplazamiento2);

	if(head2->isFree == true){
		//Si son bloques de la misma pagina veo si el segundo header es el ultimo del segmento
				if(pag1 == pag2){
					if(pagina1->esFinPag == 1){
						//Si el segundo header es el ultimo , pasa a ser el primero, sumo tamanio
						if(desplazamiento2 == pagina1->ultimo_header){
							head1->size += (head2->size + 5);

							pagina1->ultimo_header = desplazamiento1;
							pagina1->tamanio_header = head1->size;
							return;
						}
					}

				} else{ // Si son paginas distintas pero la segunda es la ultima
					if(pagina2->esFinPag == 1){
						if(pagina2->ultimo_header == desplazamiento2){
//Si entra acá significa que en la siguiente pagina solo estaba un header free, asique LA ELIMINO sin importar desperdicio
							if(posicionAliberar == segmento->base){
								vaciarSegmento(segmento, bloquesLibres);
								return;
							}
							head1->size -=(config_muse->tamanio_pagina - (desplazamiento1 + 5));
							pagina1->esFinPag = 1;
							pagina1->ultimo_header = desplazamiento1;
							pagina1->tamanio_header = head1->size;
//Bien, el nuevo size es solo de la pagina pero, cuantas paginas ocupaba antes el size? debo eliminarlas
							for(int i= pag1+1; i<=pag2;i++){
							t_pagina* eliminar = list_remove(segmento->paginas, i);
							free(eliminar);
							segmento->tamanio -= (config_muse->tamanio_pagina);
							}
							if(!segmento->ultimo){

								t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
								nuevoSegmento->base = segmento->base + segmento->tamanio;
								nuevoSegmento->dinamico = 0;
								nuevoSegmento->empty    = true;
								nuevoSegmento->paginas  = list_create();
								nuevoSegmento->segmento = list_size(tabla_segmentos);
								nuevoSegmento->shared   = 0;
								nuevoSegmento->tamanio  = config_muse->tamanio_pagina * (pag2-pag1);
								nuevoSegmento->ultimo   = false;
								list_add(tabla_segmentos, nuevoSegmento);
							}

							return;

						}
					}
				}

		bool buscar_bloque(void* bloque_libre){

			return (((t_libres*)bloque_libre)->posicion == desplazamiento2);
			}

		t_libres* libre = list_remove_by_condition(bloquesLibres,&buscar_bloque);
		if(libre != NULL){free(libre);}


		head1->size += (5 + head2->size);
	}
	agregar_bloque_libre(bloquesLibres, pag1, segmento->segmento, desplazamiento1, head1->size);
	return;

}




void compactar(t_list*  tabla_segmentos, t_list* bloquesLibres){
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
		if(segmento->dinamico==1 || segmento->empty){
			continue;
		}

		posicion1 = segmento->base;
		fin 	  = false;

		while(fin){
			pag1 = posicion1/config_muse->tamanio_pagina;

			pagina1 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag1);
				if(pagina1->esFinPag == 1 && pagina1->ultimo_header == posicion1){
					fin = true;
					continue;
				}

			desplazamiento1 = posicion1%config_muse->tamanio_pagina;
			head1 = punteroMemoria + (pagina1->marco * config_muse->tamanio_pagina + desplazamiento1 );

			posicion2 = posicion1 + 5 + head1->size;
			if(!head1->isFree){ //Si no esta libre avanzo a la siguiente posicion, con o sin desperdicio?
						if(desplazamiento1 + 5 + head1->size < config_muse->tamanio_pagina){
							if(config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size) < 5){
							   posicion2 += (config_muse->tamanio_pagina - (desplazamiento1 + 5 + head1->size));

							}
						}
						//Se desperdicio lo primero?
						if(posicion2 == config_muse->tamanio_pagina){
							pag2 = posicion2/config_muse->tamanio_pagina;
							pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2);
							for(int i = 0; i<5;i++){
								if((char*)(punteroMemoria + (config_muse->tamanio_pagina * pagina1->marco + (i)))!='#'){
									posicion2 += i;
									break;
								}
							}
							}
						posicion1 = posicion2;
						continue;
						}

			pag2 = posicion2/config_muse->tamanio_pagina;
					if(pag2 == pag1){
						marco2 = pagina1->marco;
					} else {
						pagina2 = buscar_segmento_pagina(tabla_segmentos, segmento->segmento, pag2);
						marco2 = pagina2->marco;
					}

			desplazamiento2 = posicion2%config_muse->tamanio_pagina;
			head2 = punteroMemoria + (marco2 * config_muse->tamanio_pagina + desplazamiento2);

			if(!head2->isFree){

				posicion1 = posicion2;
				continue;

			}



			if(head1->isFree && head2->isFree  ){
				head1->size += (5 + head2->size);

				if(pag1 == pag2){
					if(pagina1->esFinPag == 1){
						//Si el segundo header es el ultimo , pasa a ser el primero, sumo tamanio
						if(desplazamiento2 == pagina1->ultimo_header){
							head1->size += (head2->size + 5);

							pagina1->ultimo_header = desplazamiento1;
							pagina1->tamanio_header = head1->size;

							if(posicion1 ==  segmento->base){
								vaciarSegmento(segmento, bloquesLibres);
								fin = true;
								continue;
							}
							//Pero el primero estaba en libres entonces lo elimino
							bool buscar_bloque(void* bloque_libre){

								return (((t_libres*)bloque_libre)->posicion == desplazamiento1);
								}

							t_libres* libre = list_remove_by_condition(bloquesLibres,&buscar_bloque);
							if(libre != NULL){free(libre);}

						}
					}

				} else{ // Si son paginas distintas pero la segunda es la ultima
					if(pagina2->esFinPag == 1){
						if(pagina2->ultimo_header == desplazamiento2){
//Si entra acá significa que en la siguiente pagina solo estaba un header free, asique LA ELIMINO sin importar desperdicio
							if(posicion1 == segmento->base){
								vaciarSegmento(segmento, bloquesLibres);
								fin = true;
								continue;
							}
							head1->size -=(config_muse->tamanio_pagina - (desplazamiento1 + 5));
							pagina1->esFinPag = 1;
							pagina1->ultimo_header = desplazamiento1;
							pagina1->tamanio_header = head1->size;
							//El header 1 estaba en los libres
							bool buscar_bloque(void* bloque_libre){

								return (((t_libres*)bloque_libre)->posicion == desplazamiento1);
								}

							t_libres* libre = list_find(bloquesLibres,&buscar_bloque);
							if(libre != NULL){libre->tamanio = head1->size;}

//Bien, el nuevo size es solo de la pagina pero, cuantas paginas ocupaba antes el size? debo eliminarlas
							for(int i= pag1+1; i<=pag2;i++){
							t_pagina* eliminar = list_remove(segmento->paginas, i);
							free(eliminar);
							segmento->tamanio -= (config_muse->tamanio_pagina);
							}
							if(!segmento->ultimo){

								t_segmento* nuevoSegmento = malloc(sizeof(t_segmento));
								nuevoSegmento->base = segmento->base + segmento->tamanio;
								nuevoSegmento->dinamico = 0;
								nuevoSegmento->empty    = true;
								nuevoSegmento->paginas  = list_create();
								nuevoSegmento->segmento = list_size(tabla_segmentos);
								nuevoSegmento->shared   = 0;
								nuevoSegmento->tamanio  = config_muse->tamanio_pagina * (pag2-pag1);
								nuevoSegmento->ultimo   = false;
								list_add(tabla_segmentos, nuevoSegmento);
							}
							fin = true;
							continue;

						}
					}
				}
			//Si ni la pag1 ni la 2 son ultimas , pero estan libres, elimino la 2 de los libres y edito la 1
				bool buscar_bloque(void* bloque_libre){

				return (((t_libres*)bloque_libre)->posicion == desplazamiento1);
				}
				t_libres* libre = list_remove_by_condition(bloquesLibres, &buscar_bloque);
				free(libre);
				t_libres* libre1 = list_find(bloquesLibres, &buscar_bloque);
				libre->tamanio = head1->size;

			}


		}

	}
}





void actualizar_header(int segmento, int pagina,uint32_t posicion, uint32_t tamAnterior, uint32_t tamanio,  t_list* tabla_segmentos, t_list* bloquesLibres, int fin){

	//Busco la pagina a actualizar y la traigo a memoria
	t_pagina* pag = buscar_segmento_pagina(tabla_segmentos,segmento, pagina );


	//Actualizo el header que ya existia, empieza en posicion
	t_header* head = convertir(posicion, pag->marco);
	head->isFree   = false;
	head->size     = tamanio;

	//Tengo que actualizar la nueva "Cola" primero veo si necesito una cola.
	if(tamAnterior == tamanio){
		if(pag->esFinPag == 1){
			pag->tamanio_header = 0;
			pag->ultimo_header  = posicion;
		}
		return;
	}

	//Necesito cola --> entra en la pagina actual?
//	uint32_t finPag        = (config_muse->tamanio_pagina * pag->numero) + config_muse->tamanio_pagina;
	uint32_t ubicacionCola = posicion + 5 + tamanio;
	uint32_t sobrantePag   = config_muse->tamanio_pagina - posicion;
	if(config_muse->tamanio_pagina > ubicacionCola){
		uint32_t sobrante = config_muse->tamanio_pagina - ubicacionCola;
		//si lo que sobra de la pagina actual es menor que 5 lo desperdicio
		if(sobrante < 5){
			void* puntero = punteroMemoria + (config_muse->tamanio_pagina * pag->marco + ubicacionCola);
			desperdicio(sobrante, puntero, pag);
			//sumando al tamanio lo desperdiciado, todavia sobra tamAnterior? esta en la otra pag
			if(tamAnterior >sobrantePag){
				tamAnterior -= sobrantePag;
				t_pagina* sigPagina = buscar_segmento_pagina(tabla_segmentos, segmento, pag->numero + 1);

				//Entra en el sobrante de esta pagina un header?
				if(tamAnterior < 5){
					void* puntero = punteroMemoria + (config_muse->tamanio_pagina*sigPagina->marco);
					desperdicio(tamAnterior,puntero, sigPagina);
				}else{
					t_header* cola = convertir(0, sigPagina->marco);
					cola->isFree   = true;
					cola->size     = tamAnterior -5;
					if(fin == 1){
						pag->esFinPag       = 0;
						sigPagina->esFinPag = 1;
						sigPagina->tamanio_header = tamAnterior -5;
						sigPagina->ultimo_header  = 0;
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
			if(fin ==1){
				pag->tamanio_header = tamAnterior - tamanio -5;
				pag->ultimo_header  = ubicacionCola;
			}else{
				agregar_bloque_libre(bloquesLibres, pag->numero, segmento, ubicacionCola,tamAnterior - tamanio -5 );
			}
		}


	}
	//Si la cola no entra en la pag inicial, busco la pagina final de este header
	if(config_muse->tamanio_pagina <= ubicacionCola){
		//Necesito saber cuanto me falta ubicar despues de la pagina actual
		uint32_t reservado = tamanio;
		tamanio     -= sobrantePag;
		int cociente = tamanio/config_muse->tamanio_pagina;

		tamanio -= (config_muse->tamanio_pagina * cociente);
		cociente ++;
		t_pagina* ultPagina = buscar_segmento_pagina(tabla_segmentos, segmento, cociente);

			uint32_t sobrante     = config_muse->tamanio_pagina - tamanio;
			uint32_t libreOtraPag = tamAnterior-reservado-sobrante;
			if(sobrante <5 ){
				void* puntero = punteroMemoria + (config_muse->tamanio_pagina*ultPagina->marco + tamanio);

				desperdicio(sobrante, puntero,ultPagina);
				//Me quedo tamanio libre del espacio original?

				if(libreOtraPag > 0){
					ultPagina = buscar_segmento_pagina(tabla_segmentos, segmento, cociente+1);

						puntero = punteroMemoria + config_muse->tamanio_pagina*ultPagina->marco;
				}
				if(libreOtraPag > 0 && libreOtraPag<5){
					desperdicio(libreOtraPag, puntero, ultPagina);

				}
				if(libreOtraPag >=5){
					t_header* cola = puntero;
					cola->isFree   = true;
					cola->size     = libreOtraPag-5;
					agregar_bloque_libre(bloquesLibres, ultPagina->numero, segmento, 0, libreOtraPag-5);
				}
			}
			if(sobrante >= 5){
				t_header* cola = punteroMemoria + (config_muse->tamanio_pagina*ultPagina->marco + tamanio);
				cola->isFree   = true;
				cola->size     = libreOtraPag-5;
				agregar_bloque_libre(bloquesLibres, ultPagina->numero, segmento, tamanio, libreOtraPag-5);
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
			return cociente +2;
		}else{
		return cociente +1;

		}
	}
}

int buscar_marco_libre(char* bitmap){
//	if(!string_contains("0", bitmap_marcos)){
//		return -1;}
	//reviso si hay un marco libre
	int i;
	for( i = 0; i<strlen(bitmap_marcos); i++){
		if(bitmap_marcos[i] == '0'){
			bitmap_marcos[i] = '1';
			return i;
			}
	}
		return -1;
}

uint32_t crearPaginas(int cant_pag, uint32_t tamanio, t_segmento* segmento, t_list* bloques_libres){


	int size_tabla = list_size(segmento->paginas) -1;
	uint32_t retorno;
	uint32_t faltante;
	uint32_t sobrante;


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
					if(ultima_pagina->p==0){
						ultima_pagina->marco = swap(ultima_pagina->marco);
					}
				ultima_pagina->p=1;

				t_header* head = convertir(ultima_pagina->ultimo_header, ultima_pagina->marco);

retorno = segmento->base + (config_muse->tamanio_pagina*ultima_pagina->numero + (ultima_pagina->ultimo_header + 5));
				head->isFree = false;
				head->size   = tamanio;

				uint32_t sob_pag = config_muse->tamanio_pagina - (ultima_pagina->ultimo_header + 5);
				if(sob_pag<tamanio){
					tamanio = tamanio  - sob_pag;
					ultima_pagina->esFinPag = 0;
					ultima_pagina->tamanio_header = 0;
					ultima_pagina->ultimo_header  = 0;

				}else{
					sob_pag -= tamanio;
					if(sob_pag>=5){
						t_header* cola = convertir(ultima_pagina->ultimo_header + 5+tamanio,ultima_pagina->marco );
						cola->isFree   = true;
						cola->size     = sob_pag-5;
						ultima_pagina->esFinPag = 1;
						ultima_pagina->tamanio_header = sob_pag-5;
						ultima_pagina->ultimo_header  = ultima_pagina->ultimo_header + 5+tamanio;
					}else{
						void* puntero = punteroMemoria + ultima_pagina->marco*config_muse->tamanio_pagina + (ultima_pagina->ultimo_header + 5 + tamanio);
						desperdicio(sob_pag, puntero, ultima_pagina);

					ultima_pagina->esFinPag = 1;
					ultima_pagina->tamanio_header = 0;
//					ultima_pagina->ultimo_header  = 0;
					}

				}



				continue;
			}
		}

		pagina->marco = buscar_marco_libre(bitmap_marcos);
		pagina->p=1;

		if(pagina->marco == -1){
			pagina->marco  = buscar_marco_libre(bitmap_swap);
		   pagina->p = 0;

		   	   if(i==cant_pag || i==1){
		   		    pagina->marco = swap(pagina->marco);
					pagina->p=1;
		   			}

				}
		if(i==1){

			t_header* head = convertir(0, pagina->marco);
			retorno = segmento->base + (config_muse->tamanio_pagina* size_tabla + 5);
			head->isFree = false;
			head->size   = tamanio - 5;
	   }
		pagina->numero   = size_tabla;
		pagina->esFinPag = 0;

		if(tamanio>config_muse->tamanio_pagina){
					faltante = tamanio - config_muse->tamanio_pagina;
					tamanio  = faltante;
					}
				else{
					sobrante = config_muse->tamanio_pagina -tamanio;
					if(sobrante >= 5){
						t_header* cola = punteroMemoria + (pagina->marco*config_muse->tamanio_pagina + tamanio);
						cola->isFree = true;
						cola->size   = sobrante-5;
						pagina->ultimo_header = tamanio;
						pagina->tamanio_header = sobrante-5;
						pagina->esFinPag =1;

					}else{
						//Voy a desperdiciar si sobra menos de 5 #$%$#
						desperdicio(sobrante,  punteroMemoria + (pagina->marco*config_muse->tamanio_pagina + tamanio), pagina);

					}

				}

		list_add(segmento->paginas, pagina);


	}


	return retorno;
}



uint32_t crearSegmentoDinamico(uint32_t tamanio, t_list* tabla_segmentos, t_list* bloques_libres){

	uint32_t extra = tamanio +5;
	int tam = calcular_paginas_malloc(extra);
	if(paginas_usadas + tam > config_muse->paginas_totales){
		return 0;
	}
	int ind = list_size(tabla_segmentos) -1;

	t_segmento* seg_nuevo = malloc(sizeof(t_segmento));

	if(ind>0){
		t_segmento* seg_anterior = list_get(tabla_segmentos, ind);
		seg_nuevo->base          = seg_anterior->base + seg_anterior->tamanio;
	}else{
		seg_nuevo->base          = 0;
	}

	seg_nuevo->segmento = ind +1;
	seg_nuevo->tamanio  = tam*config_muse->tamanio_pagina;
	seg_nuevo->dinamico = 0;
	seg_nuevo->shared   = 0;
	seg_nuevo->ultimo   = true;
	seg_nuevo->paginas  = list_create();



	uint32_t res = crearPaginas(tam, tamanio, seg_nuevo, bloques_libres);
	if (res >=0){
	seg_nuevo->empty = false;
	 list_add(tabla_segmentos, seg_nuevo);
		}
	return res;
}


void remover_bloque_libre(t_list* bloque_libre , int id){ //int pag, int seg){

	bool buscar_bloque(void* bloque_libre){

//			return (((t_libres*)bloque_libre)->segmento == seg && ((t_libres*)bloque_libre)->pagina == pag);
		return (((t_libres*)bloque_libre)->id == id);
		}

	t_libres* libre  = list_remove_by_condition(bloque_libre, &buscar_bloque);
	free(libre);

}


uint32_t mallocMuse(uint32_t tamanio, t_list* bloquesLibres, t_list* tabla_segmentos){
//No olvidar, yo NO agrego los ultimos espacios libres de las paginas en bloques libres


	bool mayor_tamanio(t_libres* bloque_libre){
		int tamanio_cinco = tamanio + 5;
		return bloque_libre->tamanio >= tamanio_cinco || bloque_libre->tamanio == tamanio;
	}

	//Si lo que reservan es 0 se devuelve null
	if(tamanio == 0){
		return 0;
	}
	//Busco en los bloques libres
	    t_libres*  valor_libre = list_find(bloquesLibres, (void* )mayor_tamanio );
		if(valor_libre!=NULL){
			bool buscar_seg(void* elSeg){

					return (((t_segmento*)elSeg)->segmento == valor_libre->segmento);

				}


			actualizar_header(valor_libre->segmento, valor_libre->pagina,valor_libre->posicion, valor_libre->tamanio, tamanio, tabla_segmentos, bloquesLibres, 0 );

			t_segmento* segmento  = list_find(tabla_segmentos, &(buscar_seg));
			uint32_t retorno = segmento->base + (config_muse->tamanio_pagina * valor_libre->pagina + (valor_libre->posicion + 5));
			remover_bloque_libre(bloquesLibres,valor_libre->id);
			return  retorno;

		}

		//reviso si en el utimo header hay lugar
		int i = list_size(tabla_segmentos) - 1;
		if(i != -1){

			t_segmento* ult_segmento = list_get(tabla_segmentos, i);
			//SI el segmento es dinámico, voy a extenderlo (IGual a CERO)

			if(ult_segmento->dinamico == 0 && !ult_segmento->empty){
				i = list_size(ult_segmento->paginas) - 1;

				t_pagina* ult_pag = list_get(ult_segmento->paginas, i);

						if(ult_pag->tamanio_header >= (tamanio)){

							uint32_t retorno = ult_segmento->base + (ult_pag->numero*config_muse->tamanio_pagina + (ult_pag->ultimo_header + 5));
						    actualizar_header(ult_segmento->segmento, ult_pag->numero,ult_pag->ultimo_header, ult_pag->tamanio_header, tamanio, tabla_segmentos, bloquesLibres, 1);

							return retorno;

						} else{
							uint32_t extra ;
							if(ult_pag->tamanio_header!=0){
								extra = tamanio - ult_pag->tamanio_header;
							}else{
								extra = tamanio +5;
							}

							int tam = calcular_paginas_malloc(extra);

								if(paginas_usadas + tam > config_muse->paginas_totales){
									return -1;
								}

								ult_segmento->tamanio += tam*config_muse->tamanio_pagina;
								if(ult_pag->tamanio_header!=0){
									tam++;
								}

									uint32_t res = crearPaginas(tam, tamanio, ult_segmento,bloquesLibres);

									return res;

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

				uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres);
				if(res == 0){
					return 0;
				}
				return res;
				}
				else{
					t_segmento* segmentoVacio = list_get(tabla_segmentos, seg);
					int tam = calcular_paginas_malloc(tamanio+5);
					uint32_t res = crearPaginas(tam, tamanio, segmentoVacio, bloquesLibres);
						if (res >=0){
						segmentoVacio->empty = false;
						 list_add(tabla_segmentos, segmentoVacio);

						}
						return res;
				}

		//---------------------------------------------------------------------------------

			}
	} else { // no existia ningun segmento --> creo uno nuevo
		//antes fijarme si no tengo uno vacio, si no habia segmento poque habria uno vacio? ajajjajaajajaj

						uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres);
						if(res == 0){
							return 0;
						}
						return res;


	}
	return 0;
}





void atenderConexiones(int parametros){


	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->cliente = parametros;
	proceso->ip 	 = malloc(INET6_ADDRSTRLEN);
	proceso->segmentos = list_create();
	proceso->bloquesLibres = list_create();

	ip_de_programa(proceso->cliente, proceso->ip);
	activo = true;

	while(activo){
		int operacion = recibirInt(proceso->cliente);
		switch (operacion) {
		case SALUDO:{

			proceso->id = recibirInt(proceso->cliente);
			if(proceso->id<0){
				enviarInt(proceso->cliente, -1);
				pthread_exit("char");
			}

			list_add(tabla_procesos, proceso);

			enviarInt(proceso->cliente, 0);

		}break;
		case CERRAR:{
			//TEngo que liberar absolutamente todo

			log_info(logMuse,"Se desconecto el proceso: \n");
			pthread_exit("CHAU");
		}break;
		case RESERVAR: {
			uint32_t tamanio = recibirUint32_t(proceso->cliente);
			uint32_t posicion = mallocMuse(tamanio, proceso->bloquesLibres, proceso->segmentos);

			enviarUint32_t(proceso->cliente,  posicion);

		} break;
		case LIBERAR:{
			uint32_t posicionAliberar = recibirUint32_t(proceso->cliente);
			freeMuse(posicionAliberar, proceso->segmentos, proceso->bloquesLibres);
		} break;
		case OBTEBER:{

		}break;
		case COPIAR:{

		}break;
		case MAPEAR: {

		}break;
		case SINCRO:{

		} break;
		case DESMAP:{

		}break;
		case VER:{
			char* cero = string_new();
//			cero = '\0';
			memcpy(punteroMemoria+config_muse->tamanio_total, cero, 1);
//			char* string = malloc(config_muse->tamanio_total);
//			memcpy(string, punteroMemoria, config_muse->tamanio_total);
//			(char*)punteroMemoria +  config_muse->tamanio_total = '\0';
			printf("%s ", (char*)punteroMemoria);

		}break;

		default:{

		}


		}


	}



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

	log_info(logMuse, "Peer IP address: %s \n", ip);

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
	tabla_clock    = list_create();

	contador = 0;

}


void inicializarSwap(){
	remove("archivoSwap.txt");
	archivoSwap = fopen(rutaSwap,"wb");
	if(archivoSwap == NULL){ log_error(logMuse, "No se pudo crear el archivo swap" ); exit(1); }

	rewind(archivoSwap);
	int i = 0;
	vaciar = malloc(strlen("\n")); //LLeno todo el archivo con \n
	vaciar = "\n";

	paginas_swap = config_muse->tamanio_swap/config_muse->tamanio_pagina;
	while(i < paginas_swap){
		fputs(vaciar, archivoSwap);
		i++;
	}

	bitmap_swap 	= string_repeat('0', paginas_swap);

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


	        }

	        if(config_muse == NULL){
	        	log_error(logMuse, "ERROR No se pudo levantar el archivo de configuración de la MUSE \n");
	        	return -1;
	        }

	        return 0;


}

	int main() {

		remove("MUSE.txt");
		int v_error = crearConfigMemoria();
		if(v_error == 0){

		inicializarMemoria();

		inicializarSwap();

		crearHiloParalelos();



		} //esta llave cierra luego de ver que se levanto ok el config
	}
