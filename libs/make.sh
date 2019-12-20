rm -rf  ./conexion.o ./libs.o ./mensajes.o  ./conexion.d ./libs.d ./mensajes.d  liblibs.so

gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"conexion.d" -MT"conexion.o" -o "conexion.o" "conexion.c"

gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"libs.d" -MT"libs.o" -o "libs.o" "libs.c"

gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"mensajes.d" -MT"mensajes.o" -o "mensajes.o" "mensajes.c"

gcc -shared -o "liblibs.so"  ./conexion.o ./libs.o ./mensajes.o

echo ""
echo "**COMPILACION LIBS FINALIZADA**"
echo ""
