CC = gcc
FLAGS = -Wall -g


all: main.o 

main.o: main.c 
	$(CC) $(FLAGS) $^ -o $@ 


clean: 
	rm -f *.o
	rm -f *.out
