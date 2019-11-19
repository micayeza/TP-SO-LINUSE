LIBS = -lcommons -lpthread -llibs
CFLAGS = -g -Wall -I /home/utnso/workspace/libs
LDFLAGS = -L /home/utnso/workspace/libs/Debug
TARGET= SUSE

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

$(TARGET): $(OBJECTS) 
	gcc $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

clean: 
	-rm -f *.o
