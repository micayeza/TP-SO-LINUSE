export LD_LIBRARY_PATH=../../libs/

rm -rf  ./SACCLI.o  ./SACCLI.d  SACCli

gcc -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -I"../../libs" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"SACCLI.d" -MT"SACCLI.o" -o "SACCLI.o" "SACCLI.c"

gcc -L"../../libs" -o "SACCli"  ./SACCLI.o   -llibs -lfuse -lcommons

echo ""
echo "**COMPILACION SACCLIENTE FINALIZADA**"
echo ""
