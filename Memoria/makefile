#TARGET = MEMORIA
#LIBS = -lcommons -pthread
#CFLAGS = -g -Wall

#.PHONY: default all clean

#default: $(TARGET) # la regla por defecto
#all: default # Si ejecutan all, que ejecute la default

#OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
#HEADERS = $(wildcard *.h)

#%.o: %.c $(HEADERS)
#	gcc $(CFLAGS) -c $< -o $@

#$(TARGET): $(OBJECTS) 
#	gcc  $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

default: MEMORIA

MEMORIA: MUSE.c ../libs/libs.c 
	gcc -g $? -o $@ -lcommons -pthread 
	

clean: 
	-rm -f *.o