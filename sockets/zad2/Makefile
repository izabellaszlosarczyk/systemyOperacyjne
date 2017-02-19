all: klient.c serwer.c
	gcc -Wall klient.c -I. -lrt -o klient -lpthread
	gcc -Wall serwer.c -I. -lrt -o serwer -lpthread
		
clean: 
	@rm -f *~ *.o
