#include "glowny.h"

char wiadomosc[1024];

int generujLiczbe(){
    int liczba;
    srand (time(NULL));
    liczba =  ((rand()%100)/10);
    return liczba;
}

int producent(int argc, char *argv[]){

    printf("inicjalizuje pamiec w i semafory\n");
    int flaga = czyIstnieje();
    int semPu = stworzSemaforPu();
    int semPe = stworzSemaforPe();
    int semafor = stworzSemafor();
    int semaforKrytycznyP = stworzSemaforKrytycznyProducent();
    int bufor = stworzBuforWspolny();
    
    int i;

    struct zadanie* wyprodukowaneZadania = (struct zadanie *)shmat(bufor, NULL, 0);

    printf("%s", "inicjalizowanie bufora\n");
    union semun tmpUnia;
    if (flaga == 0){
        
        for (i = 0; i < ROZMIAR_BUFORA_WSPOLNEGO; i++) {
        wyprodukowaneZadania[i].status = -1;
        wyprodukowaneZadania[i].liczba = 0;
        }   
        tmpUnia.val = 1;
        for (i = 0; i < ROZMIAR_BUFORA_WSPOLNEGO; i++) {
            semctl(semafor, i, SETVAL, tmpUnia);
        }
        tmpUnia.val = ROZMIAR_BUFORA_WSPOLNEGO;
        semctl(semPe, 0, SETVAL, tmpUnia);

        tmpUnia.val = 0;
        semctl(semPu, 0, SETVAL, tmpUnia);
    }

    tmpUnia.val = 1;
    semctl(semaforKrytycznyP, 0, SETVAL, tmpUnia);

    struct sembuf temp1, temp2; 

    printf("wchode do petli while\n");
    i = 0;
    while(1){

        // sprawdzenie czy krytyczny na 1 
        printf("sprawdzam krytyczny\n");
        int wartosc4 = (int)semctl(semaforKrytycznyP, 0, GETVAL, tmpUnia);
        if (wartosc4 != 0) {
            // zablokowanie krytycznego 
            temp2.sem_flg = 0;
            temp2.sem_num = 0;
            temp2.sem_op = -1;    

            if (semop(semaforKrytycznyP, &temp2, 1) == -1) {
                 perror("semop");
            }
            

            int wartosc2 = (int)semctl(semPe, 0, GETVAL, tmpUnia);
            int wartosc3 = (int)semctl(semafor, i, GETVAL, tmpUnia);

            printf("sprawdzam czy moge dostac sie do zadania\n");
            if ((wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status == -1) && (wartosc2 != 0) && (wartosc3 != 0)){ 
                // zablokuj i-ty semafor
                temp1.sem_flg = 0;
                temp1.sem_num = i;
                temp1.sem_op = -1;
                semop(semafor, &temp1, 1); 

                // zmniejsz pelny
                temp1.sem_flg = 0;
                temp1.sem_num = 0;
                temp1.sem_op = -1;
                semop(semPe, &temp1, 1);

                wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].liczba = generujLiczbe();
                wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status = 0;
                int wartosc = semctl(semPu, i, GETVAL, tmpUnia);
                 // wieksz pusty
                temp1.sem_flg = 0;
                temp1.sem_num = 0;
                temp1.sem_op = 1;
                semop(semPu, &temp1, 1);

                sprintf(wiadomosc, "%d %s Dodalem liczbe: %d. Pozostalo %d zadan oczekujacych", getpid(), generujCzas(), wyprodukowaneZadania[i].liczba, wartosc);
                printf("%s\n", wiadomosc);
               
                // odblokuj zadanie
                temp1.sem_flg = 0;
                temp1.sem_num = i;
                temp1.sem_op = 1;
                semop(semafor, &temp1, 1);
            }
            else {
                printf("nie dostaje sie do itego wiec przechodze dalej\n");
            }
            temp2.sem_flg = 0;
            temp2.sem_num = 0;
            temp2.sem_op = 1;
            // ustawienie semfkry na 1; 
            semop(semaforKrytycznyP, &temp2, 1);
            i = i + 1;
        }
        else {
            printf("nie dostaje sie do itego wiec przechodze dalej\n");
            i = i + 1;
        }

    }

    return 0;
}
