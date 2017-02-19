CC = gcc
FLAGS = -fPIC -Wall -g
LDFLAGS = -shared
VPATH=./:./list

INCLUDE=./list

all: static shared clean

list.o: list.c 
	$(CC) $(FLAGS) -I$(INCLUDE) -c $^ 

static: list.o
	ar rc liblist.a list.o

shared: list.o
	gcc -fPIC -shared -o liblist.so list.o

clean: 
	rm -f *.o
	rm -f *.out