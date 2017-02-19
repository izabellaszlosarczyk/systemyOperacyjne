CC = gcc
FLAGS = -Wall -g

INCLUDE=./list

all: compile clean

compile: list_static list_dynamic list_shared

list_static: list_test.c
	$(CC) $^ -o list_test_static.o ${FLAGS} -L. -llist -I$(INCLUDE)
	./list_test_static.o

list_dynamic: list_test.c
	gcc list_test.c -o list_test_dynamic.o ${FLAGS} -L. -Wl,-R. -DDYNAMICLIB -ldl -I$(INCLUDE)
	./list_test_dynamic.o

list_shared: list_test.c
	gcc list_test.c -o list_test_shared.o ${FLAGS} -L. -Wl,-R. -llist -ldl -I$(INCLUDE)
	./list_test_shared.o
	
clean: 
	rm -f *.o
	rm -f *.out
