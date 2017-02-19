CC = gcc
FLAGS =  -Wall -g


all: serwer.o klient.o

serwer.o: serwer.c 
	$(CC) $(FLAGS) -I. $^ -o $@ 

klient.o: klient.c 
	$(CC) $(FLAGS) -I. $^ -o $@ 

clean: 
	rm -f *.o
	rm -f *.out
