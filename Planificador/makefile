LIBS =  -lcommons
CFLAGS = -g -Wall

.PHONY: default all clean

default: SUSE
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

SUSE: $(OBJECTS) 
	gcc $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean: 
	-rm -f *.o
