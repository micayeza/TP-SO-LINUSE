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
				printf("PINTO UN MKDIR PERRO");
				break;
			}
			case SYS_READDIR:{

				break;
			}
		}
	}
}

int FS_unlink(char* path){

	return 0;
}


