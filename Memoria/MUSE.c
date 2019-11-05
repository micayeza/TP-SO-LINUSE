/*
 * MUSE.c
 *
 *  Created on: 26 sep. 2019
 *      Author: utnso
 */
#include "MUSE.h"


int swap(int pag_swap){

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
	if(!string_contains('0', bitmap_marcos)){
		return -1;}
	//reviso si hay un marco libre
	for( int i = 0; i<strlen(bitmap_marcos); i++){
		if(bitmap_marcos[i] == '0'){
			return i;
			}
	}

}

int crearPaginas(int cant_pag,t_list* paginas, uint32_t tamanio, int segmento, t_list* bloques_libres){
	//Necesito una tabla de marcos libres y swaps libres;

 //Si es la unica pagina significa que entra el header ahi o no necesita
//Para la primer pagina
	int size_tabla = list_size(paginas) -1;

	uint32_t faltante;
	uint32_t sobrante;
	tamanio +=5;
	for(int i = 1; i<=cant_pag; i++){
		size_tabla++;

		t_pagina* pagina = malloc(sizeof(t_pagina));

		int marco = buscar_marco_libre(bitmap_marcos);
		pagina->p=1;

		if(marco == -1){
		   marco  = buscar_marco_libre(bitmap_swap);
		   pagina->p = 0;

		   	   if(i==cant_pag || i==1){
					marco = swap(marco);
					pagina->p=1;
		   			}
				}
		pagina->numero = size_tabla;
		pagina->marco  = marco;

		if(tamanio>config_muse->tamanio_pagina){
					faltante = config_muse->tamanio_pagina -tamanio;
					tamanio  = faltante;
					}
				else{
					sobrante = tamanio - config_muse->tamanio_pagina;
					if(sobrante >= 5){
						t_header* cola = punteroMemoria + (marco*config_muse->tamanio_pagina + tamanio);
						cola->isFree = true;
						cola->size   = sobrante-5;
						pagina->ultimo_header = punteroMemoria + (marco*config_muse->tamanio_pagina + tamanio);
						pagina->tamanio_header = sobrante-5;
//						list_add(paginas, pagina);
//						return 0;
						//Tengo que agrergarlo a mi tabla de bloques libres
						t_libres* libre = malloc(sizeof(t_libres));
						libre->pagina   = size_tabla;
						libre->posicion = punteroMemoria + (marco*config_muse->tamanio_pagina + tamanio);
						libre->segmento = segmento;
						libre->tamanio  = sobrante-5;
						list_add(bloques_libres, libre);

					}else{
						//Voy a desperdiciar si sobra menos de 5 #$%$#
						char* desperdicio = string_repeat('#', sobrante);
						memmove(punteroMemoria + (marco*config_muse->tamanio_pagina + tamanio), desperdicio, sobrante);
						pagina->ultimo_header  = 0;
						pagina->tamanio_header = 0;
//						return 0;
					}

				}

		list_add(paginas, pagina);


	}


	return 0;
}



uint32_t crearSegmentoDinamico(uint32_t tamanio, t_list* tabla_segmentos, t_list* bloques_libres){


	int tam = calcular_paginas_malloc(tamanio);
	if(paginas_usadas + tam > config_muse->paginas_totales){
		return -1;
	}
	int ind = list_size(tabla_segmentos) -1;
	t_segmento* seg_anterior = list_get(tabla_segmentos, ind);


	t_segmento* seg_nuevo = malloc(sizeof(t_segmento));
	seg_nuevo->segmento = ind +1;
	seg_nuevo->base     = seg_anterior->base + seg_anterior->tamanio;
	seg_nuevo->tamanio  = tam * config_muse->tamanio_pagina;
	seg_nuevo->dinamico = 0;
	seg_nuevo->shared   = 0;
	seg_nuevo->paginas  = list_create();



	int res = crearPaginas(tam,seg_nuevo->paginas, tamanio, seg_nuevo->segmento, bloques_libres);
	if (res >=0){
	res =  list_add(tabla_segmentos, seg_nuevo);
		}
	return res;
}


void remover_bloque_libre(t_list* bloque_libre , int pag, int seg){

	bool buscar_bloque(void* bloque_libre){

			return (((t_libres*)bloque_libre)->segmento == seg && ((t_libres*)bloque_libre)->pagina == pag);

		}
	t_libres* libre  = list_remove_by_condition(bloque_libre, &buscar_bloque);
	free(libre);

}


uint32_t mallocMuse(uint32_t tamanio, t_list* bloquesLibres, t_list* tabla_segmentos){


	bool igual_tamanio(t_libres* bloque_libre){
		return bloque_libre->tamanio == tamanio;
	}

	bool mayor_tamanio(t_libres* bloque_libre){
		int tamanio_cinco = tamanio + 5;
		return bloque_libre->tamanio >= tamanio_cinco;
	}

	//Busco si hay lugar justo para el maloc
	t_libres* valor_libre = list_find(bloquesLibres, (void* ) igual_tamanio)  ;
	if(valor_libre != NULL){

// FUNCION QUE ACTUALIZE EL HEADER DE LA PAGINA, SI NO ESTA EN MEMORIA SWAPPEAR		(1)
		uint32_t retorno =  valor_libre->posicion + 5;
		remover_bloque_libre(bloquesLibres,valor_libre->pagina,valor_libre->segmento);
		return retorno;
	}

	//SI no hay un lugar igual para el maloc,

		//Busco un FREE en donde haya lugar para el maloc + header
		valor_libre = list_find(bloquesLibres, (void* )mayor_tamanio );
		if(valor_libre!=NULL){

			// FUNCION QUE ACTUALIZE EL HEADER DE LA PAGINA Y CREE UN NUEVO HEADER ADELANTE, SI NO ESTA EN MEMORIA SWAPPEAR  (2)
			remover_bloque_libre(bloquesLibres,valor_libre->pagina,valor_libre->segmento);
			uint32_t retorno =  valor_libre->posicion + 5;
			return valor_libre->posicion + 5;
		}

//reviso si en el utimo header hay lugar
		int i = list_size(tabla_segmentos) - 1;
		if(i != -1){
		t_segmento* ult_segmento = list_get(tabla_segmentos, i);
		//SI el segmento es dinámico, voy a extenderlo (IGual a CERO)

		if(ult_segmento->dinamico == 0){
			i = list_size(ult_segmento->paginas) - 1;

			t_pagina* ult_pag = list_get(ult_segmento->paginas, i);
			if(ult_pag->tamanio_header >= (tamanio + 5)){
				// FUNCION QUE ACTUALIZE EL HEADER DE LA PAGINA Y CREE UN NUEVO HEADER ADELANTE, SI NO ESTA EN MEMORIA SWAPPEAR(2)
				return ult_pag->ultimo_header + 5;

			} else {

//---------------------------------------------------------------------------------
//CREAR NUEVA PAGINA, YA SE QUE ES EXTENSIBLE EL SEGMNTO SOLO HAY QUE VER QUE HAYA LUGAR EN LA MEMORIA, DEVOLVERÁ -1 SI NO HAY MAS LUGAR EN LA MEMORIA
				int tam = calcular_paginas_malloc(tamanio);
					if(paginas_usadas + tam > config_muse->paginas_totales){
						return -1;
					}
				int res = crearPaginas(tam,ult_segmento->paginas, tamanio, ult_segmento->segmento,bloquesLibres);
				if (res == -1){
					return -1;
				}
//---------------------------------------------------------------------------------
			}
		}else{

//---------------------------------------------------------------------------------
			//CREAR SEGMENTO
			uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres);
			if(res == -1){
				return -1;
			}
//---------------------------------------------------------------------------------

		}
	} else {
		uint32_t res = crearSegmentoDinamico(tamanio, tabla_segmentos, bloquesLibres);
					if(res == -1){
						return -1;
					}
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
		t_list* segmentos;
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
