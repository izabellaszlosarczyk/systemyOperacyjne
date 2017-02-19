#ifndef LAB7_GLOWNY_H
#define LAB7_GLOWNY_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define KLUCZ_K 2
#define LICZBA_KONSUMENTOW 5
#define LICZBA_PRODUCENTOW 5
#define ROZMIAR_BUFORA_WSPOLNEGO 10
#define MAX_LICZBA_ZADAN 12


int stworzSemafor();
int czyIstnieje();
int stworzBuforWspolny();
int stworzSemaforPu();
int stworzSemaforPe();
int stworzSemaforKrytycznyProducent();
int stworzSemaforKrytycznyKonsument();
char *generujCzas();

struct zadanie{
    int liczba;	
    int status; // -1 przetworzony/ pusty oraz  0 utworzony i czeka 
};

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

#endif //LAB7_GLOWNY_H

// poza pusty,pelny jeszcze dodatkowe dwa sem do kontroli indeksu, sekcje krytyczne dla kon i prod - sem = 1
// inc i%n i jak przejdziemy do konca to od 0, zablokowanie na sem - nic niee robimy - krążymy