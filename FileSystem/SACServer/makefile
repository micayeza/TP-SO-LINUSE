TARGET = SACServer
LIBS = -lreadline -lcommons -pthread -lpthread -lfuse 
CFLAGS = -g -Wall -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27  -std=c11 

.PHONY: default all clean

default: $(TARGET) # la regla por defecto
all: default # Si ejecutan all, que ejecute la default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c  $< -o $@ 

$(TARGET): $(OBJECTS) 
	gcc  $(OBJECTS) $(CFLAGS) $(LIBS) -o $@


clean: 
	-rm -f *.o