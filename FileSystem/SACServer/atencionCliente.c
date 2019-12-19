#include "SACServer.h"

void atenderCliente(t_cliente* cliente){
	//Me mantengo a la espera de Recibir Mensaje de este cliente
	while(1){
		TipoOperacion operacionRecibida = (TipoOperacion) recibirEntero(cliente->socket, log_interno); //Chequear que no se bloquea, debuggear.
		switch(operacionRecibida){
			case SYS_GETATTR:{
				char* path = recibirTexto(cliente->socket, log_interno);
				t_nodo* nodo = obtenerNodoDePath(path);
				if(nodo == NULL){
					int cero = 0;
					enviarEntero(cliente->socket, &cero,  log_interno); //Se envia que no se encontró.
				}else{
					enviarEntero(cliente->socket, nodo->estado,  log_interno);
					if(nodo->estado == 1){
						enviarEntero(cliente->socket, nodo->tam_archivo,  log_interno);
						enviarTiempo(cliente->socket, nodo->fecha_creacion,  log_interno);
						enviarTiempo(cliente->socket, nodo->fecha_modificacion,  log_interno);
					}
				}
				free_nodo(nodo);
				free(path);
				printf("SERVER: GETATTR. \n");
				break;
			}
			case SYS_READDIR:{
				char* path = recibirTexto(cliente->socket, log_interno);
				char* nombresArchivos = obtenerArchivosDeDirectorio(path);
				int resEnvioTexto = enviarTexto(cliente->socket, nombresArchivos, log_interno);
				free(path);
				free(nombresArchivos);
				printf("SERVER: READDIR. \n");
				break;
			}
			case SYS_MKDIR:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int resultado = crearNodoDirectorioArchivo(path, 1);
				enviarEntero(cliente->socket, resultado,  log_interno);
				free(path);
				printf("SERVER: MKDIR. \n");
				break;
			}
			case SYS_CREATE:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int resultado = crearNodoDirectorioArchivo(path, 0);
				enviarEntero(cliente->socket, resultado,  log_interno);
				free(path);
				printf("SERVER: CREATE. \n");
				break;
			}
			case SYS_UTIMES:{
				char* path = recibirTexto(cliente->socket, log_interno);
				struct timespec tiempoCreacion =  recibirTiempo(cliente->socket, log_interno);
				struct timespec tiempoModificacion =  recibirTiempo(cliente->socket, log_interno);
				int resultado = modificarFechas(path, tiempoCreacion, tiempoModificacion);
				enviarEntero(cliente->socket, resultado,  log_interno);
				free(path);
				printf("SERVER: UTIMES. \n");
				break;
			}
			case SYS_TRUNCATE:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int nuevoSize = recibirEntero(cliente->socket, log_interno);
				int resultado = cambiarTamanioArchivo(path, nuevoSize);
				enviarEntero(cliente->socket, resultado,  log_interno);
				free(path);
				printf("SERVER: TRUNCATE. \n");
				break;
			}
			case SYS_WRITE:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int offset = recibirEntero(cliente->socket, log_interno);
				int size = recibirEntero(cliente->socket, log_interno);
				void* datos = malloc(size);
				recibirDatos(cliente->socket, datos, log_interno);
				int resultado = escribirArchivo(path, offset, size, datos);
				enviarEntero(cliente->socket, resultado,  log_interno);
				free(path);
				//FREE VOID* (datos) !!!!!!!!!!!!!!!?
				printf("SERVER: WRITE. \n");
				break;
			}
			case SYS_READ:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int offset = recibirEntero(cliente->socket, log_interno);
				int size = recibirEntero(cliente->socket, log_interno);
				void* datos = malloc(size);
				int resultado = leerArchivo(path, offset, size, datos);
				enviarDatos(cliente->socket, datos, size, log_interno);
				enviarEntero(cliente->socket, resultado,  log_interno);
				free(path);
				//FREE VOID* (datos) !!!!!!!!!!!!!!!?
				printf("SERVER: READ. \n");
				break;
			}
			case SYS_RMDIR:{

				break;
			}
			case SYS_UNLINK:{
				break;
			}
		}
	}
}

int FS_unlink(char* path){

	return 0;
}


