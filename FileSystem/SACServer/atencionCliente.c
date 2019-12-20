#include "SACServer.h"

void atenderCliente(t_cliente* cliente){
	//Me mantengo a la espera de Recibir Mensaje de este cliente
	while(1){
		TipoOperacion operacionRecibida = (TipoOperacion) recibirEntero(cliente->socket, log_interno); //Chequear que no se bloquea, debuggear.
		switch(operacionRecibida){
			case SYS_GETATTR:{
				char* path = recibirTexto(cliente->socket, log_interno);
				pthread_mutex_lock(&m_acceso_fs);
				t_nodo* nodo = obtenerNodoDePath(path);
				pthread_mutex_unlock(&m_acceso_fs);
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
				log_info(log_resultados, "***GETATTR--> Path: %s.", path);
				printf("SERVER: GETATTR. \n");
				free_nodo(nodo);
				free(path);
				break;
			}
			case SYS_READDIR:{
				char* path = recibirTexto(cliente->socket, log_interno);
				char* nombresArchivos = obtenerArchivosDeDirectorio(path);
				pthread_mutex_lock(&m_acceso_fs);
				int resEnvioTexto = enviarTexto(cliente->socket, nombresArchivos, log_interno);
				pthread_mutex_unlock(&m_acceso_fs);
				log_info(log_resultados, "***READDIR--> Path: %s.", path);
				printf("SERVER: READDIR. \n");
				free(path);
				free(nombresArchivos);
				break;
			}
			case SYS_MKDIR:{
				char* path = recibirTexto(cliente->socket, log_interno);
				pthread_mutex_lock(&m_acceso_fs);
				int resultado = crearNodoDirectorioArchivo(path, 1);
				pthread_mutex_unlock(&m_acceso_fs);
				enviarEntero(cliente->socket, resultado,  log_interno);
				log_info(log_resultados, "***MKDIR--> Path: %s.", path);
				printf("SERVER: MKDIR. \n");
				free(path);
				break;
			}
			case SYS_CREATE:{
				char* path = recibirTexto(cliente->socket, log_interno);
				pthread_mutex_lock(&m_acceso_fs);
				int resultado = crearNodoDirectorioArchivo(path, 0);
				pthread_mutex_unlock(&m_acceso_fs);
				enviarEntero(cliente->socket, resultado,  log_interno);
				log_info(log_resultados, "***CREATE--> Path: %s.", path);
				printf("SERVER: CREATE. \n");
				free(path);
				break;
			}
			case SYS_UTIMES:{
				char* path = recibirTexto(cliente->socket, log_interno);
				struct timespec tiempoCreacion =  recibirTiempo(cliente->socket, log_interno);
				struct timespec tiempoModificacion =  recibirTiempo(cliente->socket, log_interno);
				pthread_mutex_lock(&m_acceso_fs);
				int resultado = modificarFechas(path, tiempoCreacion, tiempoModificacion);
				pthread_mutex_unlock(&m_acceso_fs);
				enviarEntero(cliente->socket, resultado,  log_interno);
				log_info(log_resultados, "***UTIMES--> Path: %s.", path);
				printf("SERVER: UTIMES. \n");
				free(path);
				break;
			}
			case SYS_TRUNCATE:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int nuevoSize = recibirEntero(cliente->socket, log_interno);
				pthread_mutex_lock(&m_acceso_fs);
				int resultado = cambiarTamanioArchivo(path, nuevoSize);
				pthread_mutex_unlock(&m_acceso_fs);
				enviarEntero(cliente->socket, resultado,  log_interno);
				log_info(log_resultados, "***TRUNCATE--> Path: %s.", path);
				printf("SERVER: TRUNCATE. \n");
				free(path);
				break;
			}
			case SYS_WRITE:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int offset = recibirEntero(cliente->socket, log_interno);
				int size = recibirEntero(cliente->socket, log_interno);
				void* datos = malloc(size);
				recibirDatos(cliente->socket, datos, log_interno);
				pthread_mutex_lock(&m_acceso_fs);
				int resultado = escribirArchivo(path, offset, size, datos);
				pthread_mutex_unlock(&m_acceso_fs);
				enviarEntero(cliente->socket, resultado,  log_interno);
				log_info(log_resultados, "***WRITE--> Path: %s.", path);
				log_info(log_resultados, "      Datos: %s.", (char*)datos);
				printf("SERVER: WRITE. \n");
				free(path);
				free(datos);
				break;
			}
			case SYS_READ:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int offset = recibirEntero(cliente->socket, log_interno);
				int size = recibirEntero(cliente->socket, log_interno);
				void* datos = malloc(size);
				pthread_mutex_lock(&m_acceso_fs);
				int resultado = leerArchivo(path, offset, size, datos);
				pthread_mutex_unlock(&m_acceso_fs);
				enviarDatos(cliente->socket, datos, size, log_interno);
				enviarEntero(cliente->socket, resultado,  log_interno);
				log_info(log_resultados, "***READ--> Path: %s.", path);
				log_info(log_resultados, "      Datos: %s.", (char*)datos);
				printf("SERVER: READ. \n");
				free(path);
				free(datos);
				break;
			}
			case SYS_MOVE:{
				char* path = recibirTexto(cliente->socket, log_interno);
				char* newPath = recibirTexto(cliente->socket, log_interno);
				int resultado = cambiarUbicacion(path,newPath);
				enviarEntero(cliente->socket, resultado,  log_interno);
				log_info(log_resultados, "***MOVE--> Path: %s.", path);
				log_info(log_resultados, "      New Path: %s.", newPath);
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


