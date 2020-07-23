CC = gcc
CFLAGS = -Wall -pedantic 
LDLIBS = -lpthread

OBJ = client server

.PHONY: all clean

all : $(OBJ)

clean : 
	@ echo "rimozione file compilati"
	-rm $(OBJ)
