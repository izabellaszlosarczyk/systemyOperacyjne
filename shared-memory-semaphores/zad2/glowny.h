#ifndef LAB7_GLOWNY_H
#define LAB7_GLOWNY_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LICZBA_KONSUMENTOW 5
#define LICZBA_PRODUCENTOW 5
#define ROZMIAR_BUFORA_WSPOLNEGO 10
#define MAX_LICZBA_ZADAN 12


char *generujCzas();

struct zadanie{
    int liczba;	
    int status; // -1 przetworzony/ pusty oraz  0 utworzony i czeka 
};



#endif //LAB7_GLOWNY_H

// poza pusty,pelny jeszcze dodatkowe dwa sem do kontroli indeksu, sekcje krytyczne dla kon i prod - sem = 1
// inc i%n i jak przejdziemy do konca to od 0, zablokowanie na sem - nic niee robimy - krążymy