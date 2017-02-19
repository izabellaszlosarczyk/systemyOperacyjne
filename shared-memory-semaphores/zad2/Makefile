CC = gcc
CFLAGS = -Wall -g
LFLAGS = -I. -lrt -lpthread

all: main.c
	gcc $(CFLAGS) -o main.o main.c producent.c konsument.c glowny.h glowny.c $(LFLAGS)

clean: 
	@rm -f *~ *.o