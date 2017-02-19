CC = gcc
FLAGS =  -Wall -g


all: main-macierzysty.o main-potomek.o

main-macierzysty.o: main-macierzysty.c 
	$(CC) $(FLAGS) -I. $^ -o $@ 

main-potomek.o: main-potomek.c 
	$(CC) $(FLAGS) -I. $^ -o $@ 

clean: 
	rm -f *.o
	rm -f *.out