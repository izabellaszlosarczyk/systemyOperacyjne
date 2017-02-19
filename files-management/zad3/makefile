CC = gcc
FLAGS =  -Wall -g


all: main.o

main.o: main.c lock.c
	$(CC) $(FLAGS) -I. $^ -o $@ 

clean: 
	rm -f *.o
	rm -f *.out
