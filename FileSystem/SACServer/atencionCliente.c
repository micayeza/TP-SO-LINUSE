#include "SACServer.h"

void atenderCliente(t_cliente* cliente){
	//Me mantengo a la espera de Recibir Mensaje de este cliente
	while(1){
		TipoOperacion operacionRecibida = (TipoOperacion) recibirEntero(cliente->socket, log_interno); //Chequear que no se bloquea, debuggear.
		switch(operacionRecibida){
			case SYS_WRITE:{

				break;
			}
			case SYS_OPEN:{

				break;
			}
			case SYS_READ:{

				break;
			}
			case SYS_RMDIR:{

				break;
			}
			case SYS_UNLINK:{
				char* path = recibirTexto(cliente->socket, log_interno);
				int resultadoUnLink = SacServerUnlink(path);//Hacer los pasos para un Create Hilo.
				enviarEntero(cliente->socket, resultadoUnLink,  log_interno);
				break;
			}
			case SYS_MKDIR:{
				printf("SERVER: UN MKDIR. \n");
				char* path = recibirTexto(cliente->socket, log_interno);
				int resultado = crearNodoDirectorio(path);
				enviarEntero(cliente->socket, resultado,  log_interno);
				break;
			}
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
				printf("SERVER: UN GETATTR. \n");
				break;
			}
			case SYS_READDIR:{
				char* path = recibirTexto(cliente->socket, log_interno);
				char* nombresArchivos = obtenerArchivosDeDirectorio(path);
				int resEnvioTexto = enviarTexto(cliente->socket, nombresArchivos, log_interno);
				printf("SERVER: UN READDIR. \n");
				break;
			}
		}
	}
}

int FS_unlink(char* path){

	return 0;
}


