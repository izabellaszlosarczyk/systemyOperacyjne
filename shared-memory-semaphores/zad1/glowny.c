#include "glowny.h"

int stworzSemafor(){
    key_t semaforKlucz = ftok(".", 's');

    return semget(semaforKlucz, ROZMIAR_BUFORA_WSPOLNEGO , IPC_CREAT | 0666);
}

int stworzBuforWspolny(){
    key_t buforKlucz = KLUCZ_K;
    return shmget(buforKlucz, sizeof(struct zadanie)*ROZMIAR_BUFORA_WSPOLNEGO, IPC_CREAT | 0666);
}

int czyIstnieje(){
    key_t buforKlucz = KLUCZ_K;

    shmget(buforKlucz, sizeof(struct zadanie)*ROZMIAR_BUFORA_WSPOLNEGO, IPC_CREAT | IPC_EXCL | 0666);
    if (errno != EEXIST){
        return 0;
    }
    return 1;
}

int stworzSemaforPu(){
    key_t semaforPuKlucz = ftok(".", 'u');

    return semget(semaforPuKlucz, 1, IPC_CREAT | 0666);
}

int stworzSemaforPe(){
    key_t semaforPeKlucz = ftok(".", 'e');

    return semget(semaforPeKlucz, 1, IPC_CREAT | 0666);
}

int stworzSemaforKrytycznyProducent(){
    key_t semaforPeKlucz = ftok(".", 'p');

    return semget(semaforPeKlucz, 1, IPC_CREAT | 0666);
}

int stworzSemaforKrytycznyKonsument(){
    key_t semaforPeKlucz = ftok(".", 'k');

    return semget(semaforPeKlucz, 1, IPC_CREAT | 0666);
}

char *generujCzas(){
    time_t czas = time(NULL);
    ctime(&czas);
    return ctime(&czas);
}