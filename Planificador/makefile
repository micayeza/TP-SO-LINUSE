LIBS =  -lcommons
CFLAGS = -g -Wall
TARGET= SUSE

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) 
	gcc ../libs/*.c $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean: 
	-rm -f *.o
