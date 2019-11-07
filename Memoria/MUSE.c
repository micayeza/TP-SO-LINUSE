/*
 * MUSE.c
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */
#include "MUSE.h"

void*  convertir(uint32_t logica, int seg, int pag, int marco){

	return punteroMemoria + (marco*config_muse->tamanio_pagina) + (logica%config_muse->tamanio_pagina);

}

void desperdicio(uint32_t sobrante, void* posicion, t_pagina* pag){
	  char* desperdicio = string_repeat('#', sobrante);
	  memmove(posicion, desperdicio, sobrante);
	  pag->ultimo_header  = 0;
	  pag->tamanio_header = 0;
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

	return pagina;

}

int swap(int pag_swap){
return 0;
}


void actualizar_header(int segmento, int pagina,uint32_t posicion, uint32_t tamAnterior, uint32_t tamanio,  t_list* tabla_segmentos, t_list* bloquesLibres){
//no actualizo aca el header de la ultima pagina salvo que entre exactamente el tamanio
  t_pagina* pag = buscar_segmento_pagina(tabla_segmentos,segmento, pagina );
	  if(pag->p !=1){ //Necesito swapear y traer el marco correcto
		  pag->marco = swap(pag->marco);
		  pag->p     = 1;

	  }

//Logicamente tengo pag->ultimo_header pero como es la fisica?
	  t_header* header = convertir(posicion, segmento, pagina, pag->marco);
  	 // t_header* header = fisica;
  	  header->isFree = false;
  	  header->size   = tamanio;

  	  if(tamAnterior >= tamanio +5){
      //if(headers == 1){ // tamanio libre >=
//Que pagina es la del ultimo header? el segmento es el mismo pero podria variar la pagina
  		  pag->esFinPag = 0;
    	  uint32_t nueva = posicion + tamanio + 5;
    	  int pag_final  = nueva/config_muse->tamanio_pagina;

    	  pag = buscar_segmento_pagina(tabla_segmentos,segmento, pag_final );
    	  if(pag->p !=1){ //Necesito swapear y traer el marco correcto
    	  		  pag->marco = swap(pag->marco);
    	  		  pag->p     = 1;

    	  	  }
    	  t_header* cola = convertir(nueva, segmento, pag->numero, pag->marco);

  		  cola->isFree   = true;
  		  cola->size     = tamAnterior - tamanio;
  		if(pag->esFinPag == 0){
  		  t_libres* newLibre = malloc(sizeof(t_libres));
  		  newLibre->pagina   = pag->numero;
  		  newLibre->segmento = segmento;
  		  newLibre->posicion = nueva;
  		  newLibre->tamanio  = cola->size;
  		  newLibre->id       = contador++;
  		  list_add(bloquesLibres, newLibre);
  		 }
  		else{
  		  		  pag->ultimo_header  = nueva;
  		  		  pag->tamanio_header = cola->size;
  		  	  }
  	  }
  	  if(tamAnterior - tamanio< 5 && pag->esFinPag == 1){

    	  desperdicio(tamAnterior - tamanio,header + 5 + tamanio,pag );
      }

}


int calcular_paginas_malloc(uint32_t tamanio){

	int cociente   = (tamanio+5)/config_muse->tamanio_pagina;
	int resto      = (tamanio+5)%config_muse->tamanio_pagina;

	if(resto == 0){
		return cociente;
	}
	if(resto >= 5){
		return cociente +1;
	} else{ //	resto < 5
		return cociente +1; //Significa que no me alcanzo para el nuevo header, LO DESPERDICIO
	}
}

int buscar_marco_libre(char* bitmap){
//	if(!string_contains("0", bitmap_marcos)){
//		return -1;}
	//reviso si hay un marco libre
	int i;
	for( i = 0; i<strlen(bitmap_marcos); i++){
		if(bitmap_marcos[i] == '0'){
			return i;
			}
	}
		return -1;
}

uint32_t crearPaginas(int cant_pag, uint32_t tamanio, t_segmento* segmento, t_list* bloques_libres){
	//Necesito una tabla de marcos libres y swaps libres;

	int size_tabla = list_size(segmento->paginas) -1;
	uint32_t retorno;
	uint32_t faltante;
	uint32_t sobrante;

//
	tamanio +=5;
	for(int i = 1; i<=cant_pag; i++){

		size_tabla++;
		t_pagina* pagina = malloc(sizeof(t_pagina));

		if(size_tabla>0 && i ==1){

			t_pagina* ultima_pagina = list_get(segmento->paginas, size_tabla);
			if (ultima_pagina->ultimo_header>=5){
				free(pagina);
				size_tabla--;
					if(ultima_pagina->p==0){
						ultima_pagina->marco = swap(ultima_pagina->marco);
					}
				ultima_pagina->p=1;

				t_header* head = convertir(ultima_pagina->ultimo_header, segmento->segmento,  ultima_pagina->numero, ultima_pagina->marco);

				retorno = ultima_pagina->ultimo_header + 5;
				head->isFree = false;
				head->size   = tamanio;
				tamanio = tamanio - 5 - (ultima_pagina->tamanio_header);
				ultima_pagina->esFinPag = 0;
				ultima_pagina->tamanio_header = 0;
				ultima_pagina->ultimo_header  = 0;

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

			t_header* head = convertir(segmento->base * size_tabla, segmento->segmento,  size_tabla, pagina->marco);
			retorno = segmento->base * size_tabla + 5;
			head->isFree = false;
			head->size   = tamanio;
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
						pagina->ultimo_header = segmento->base + tamanio;
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


	int tam = calcular_paginas_malloc(tamanio);
	if(paginas_usadas + tam > config_muse->paginas_totales){
		return -1;
	}
	int ind = list_size(tabla_segmentos) -1;

	t_segmento* seg_nuevo = malloc(sizeof(t_segmento));

	if(ind>0){
		t_segmento* seg_anterior = list_get(tabla_segmentos, ind);
		seg_nuevo->base          = seg_anterior->limite;
	}else{
		seg_nuevo->base          = 0;
	}

	seg_nuevo->segmento = ind +1;
	seg_nuevo->limite   = seg_nuevo->base + tam*config_muse->tamanio_pagina;
	seg_nuevo->dinamico = 0;
	seg_nuevo->shared   = 0;
	seg_nuevo->paginas  = list_create();



	uint32_t res = crearPaginas(tam, tamanio, seg_nuevo, bloques_libres);
	if (res >=0){
	seg_nuevo->empty = 1;
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

	//Busco en los bloques libres
	    t_libres*  valor_libre = list_find(bloquesLibres, (void* )mayor_tamanio );
		if(valor_libre!=NULL){

			actualizar_header(valor_libre->segmento, valor_libre->pagina,valor_libre->posicion, valor_libre->tamanio, tamanio, tabla_segmentos, bloquesLibres );
			remover_bloque_libre(bloquesLibres,valor_libre->id);
			uint32_t retorno =  valor_libre->posicion + 5;
			return retorno;
		}

		//reviso si en el utimo header hay lugar
		int i = list_size(tabla_segmentos) - 1;
		if(i != -1){

			t_segmento* ult_segmento = list_get(tabla_segmentos, i);
			//SI el segmento es dinámico, voy a extenderlo (IGual a CERO)

			if(ult_segmento->dinamico == 0 && ult_segmento->empty != 0){
				i = list_size(ult_segmento->paginas) - 1;

				t_pagina* ult_pag = list_get(ult_segmento->paginas, i);

						if(ult_pag->tamanio_header >= (tamanio)){

							uint32_t retorno = 	ult_pag->ultimo_header + 5;
						    actualizar_header(ult_segmento->segmento, ult_pag->numero,ult_pag->ultimo_header, ult_pag->tamanio_header, tamanio, tabla_segmentos, bloquesLibres);

							return retorno;

						} else{
							uint32_t extra ;
							if(ult_pag->tamanio_header!=0){ //compenso xq en la funcion sumo 5
								extra = tamanio -5- ult_pag->tamanio_header;
							}

							int tam = calcular_paginas_malloc(extra);

								if(paginas_usadas + tam > config_muse->paginas_totales){
									return -1;
								}
								ult_segmento->limite += tam*config_muse->tamanio_pagina;

									uint32_t res = crearPaginas(tam, tamanio, ult_segmento,bloquesLibres);
									if (res == -1){
										return -1;
									}
									return res;

						}



			}else{

		//---------------------------------------------------------------------------------
				//CREAR SEGMENTO
				//antes fijarme si no tengo uno vacio
				uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres);
				if(res == -1){
					return -1;
				}
				return res;
		//---------------------------------------------------------------------------------

			}
	} else { // no existia ningun segmento --> creo uno nuevo
		//antes fijarme si no tengo uno vacio
		uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres);
					if(res == -1){
						return -1;
					}
				  return res;
	}
	return -1;
}





void atenderConexiones(int parametros){

	t_list* bloquesLibres;
	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->cliente = parametros;
	proceso->ip 	 = malloc(INET6_ADDRSTRLEN);
	proceso->segmentos = list_create();

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

			bloquesLibres = list_create();

			enviarInt(proceso->cliente, 0);

		}break;
		case CERRAR:{
			//TEngo que liberar absolutamente todo

			log_info(logMuse,"Se desconecto el proceso: \n");
			pthread_exit("CHAU");
		}break;
		case RESERVAR: {
			uint32_t tamanio = recibirUint32_t(proceso->cliente);
			uint32_t posicion = mallocMuse(tamanio, bloquesLibres, proceso->segmentos);

			enviarUint32_t(proceso->cliente,  posicion);

		} break;
		case LIBERAR:{

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

	// hasta que no salga del listen, nunca va a retornar el socket del servidor ya que el listen es bloqueante
//		t_list* segmentos;
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
//	tabla_inicial = malloc(sizeof(t_inicial) * cantidad_paginas);
//
//	for(int i = 0; i < cantidad_paginas; i++){
//
//		tabla_inicial[i].marco = malloc(config_muse->tamanio_pagina);
//	}

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
