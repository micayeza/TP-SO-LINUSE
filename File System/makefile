TARGET = SACServer
LIBS = -lreadline -lcommons -pthread
CFLAGS = -g -Wall -std=c99

.PHONY: default all clean

default: $(TARGET) # la regla por defecto
all: default # Si ejecutan all, que ejecute la default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) 


clean: 
	-rm -f *.o