export LD_LIBRARY_PATH=../../libs/

rm -rf  ./SACServer.o ./atencionCliente.o ./config_management.o  ./SACServer.d ./atencionCliente.d ./config_management.d  SACServer

gcc -I"../../libs" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"SACServer.d" -MT"SACServer.o" -o "SACServer.o" "SACServer.c"

gcc -I"../../libs" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"atencionCliente.d" -MT"atencionCliente.o" -o "atencionCliente.o" "atencionCliente.c"

gcc -I"../../libs" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"config_management.d" -MT"config_management.o" -o "config_management.o" "config_management.c"

gcc -L"../../libs" -o "SACServer"  ./SACServer.o ./atencionCliente.o ./config_management.o   -llibs -lm -lpthread -lcommons

echo ""
echo "**COMPILACION SACSERVER FINALIZADA**"
echo ""
