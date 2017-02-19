#include "glowny.h"

char wiadomosc[1024];

int czyPierwsza(int liczba){
    if(liczba<2)
        return 0; //gdy liczba jest mniejsza niż 2 to nie jest pierwszą
    
    int i;
    for(i = 2; i*i <= liczba;i++)
        if(liczba%i==0)
            return 0; //gdy znajdziemy dzielnik, to dana liczba nie jest pierwsza
    return 1;
}

int konsument(int argc, char *argv[]){

    printf("inicjalizuje pamiec w i semafory\n");
    int flaga = czyIstnieje();

    int semPu = stworzSemaforPu();
    int semPe = stworzSemaforPe();
    int semafor = stworzSemafor();
    int semaforKrytycznyK = stworzSemaforKrytycznyKonsument();
    int bufor = stworzBuforWspolny();

    int i;
    struct zadanie* wyprodukowaneZadania = (struct zadanie *)shmat(bufor, NULL, 0);

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
    semctl(semaforKrytycznyK, 0, SETVAL, tmpUnia);
    struct sembuf temp1, temp2; 

    printf("wchode do petli while\n");
    i = 0;
    while (1){
        
        // sprawdzenie czy krytyczny na 1 
        printf("sprawdzam krytyczny\n");
        int wartosc4 = (int)semctl(semaforKrytycznyK, 0, GETVAL, tmpUnia);
        if (wartosc4 != 0) {
            // zablokowanie krytycznego 
            temp2.sem_flg = IPC_NOWAIT;
            temp2.sem_num = 0;
            temp2.sem_op = -1;    
            
            if(semop(semaforKrytycznyK, &temp2, 1) == -1) {
                continue;
            }

	        int wartosc2 = (int)semctl(semPu, 0, GETVAL, tmpUnia);
            int wartosc3 = (int)semctl(semafor, i, GETVAL, tmpUnia);

            printf("sprawdzam czy moge dostac sie do zadania\n");
            printf("%d %d %d\n", wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status, wartosc2 != 0, wartosc3 != 0);
            if ((wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status == 0) && (wartosc2 != 0) && (wartosc3 != 0)){ 
                // zablokuj i-ty semafor
                temp1.sem_flg = 0;
                temp1.sem_num = i;
                temp1.sem_op = -1;
                semop(semafor, &temp1, 1); 

                // zmniejsz pusty
                temp1.sem_flg = 0;
                temp1.sem_num = 0;
                temp1.sem_op = -1;
                semop(semPu, &temp1, 1);

                int wartosc = (int)semctl(semPu, i, GETVAL, tmpUnia);
                if (czyPierwsza(wyprodukowaneZadania[i].liczba)== 0){
                    sprintf(wiadomosc, "%d %s Sprawdzilem liczbe: %d -zlozona. Pozostalo %d zadan oczekujacych", getpid(), generujCzas(), wyprodukowaneZadania[i].liczba, wartosc);
                }else{
                    sprintf(wiadomosc, "%d %s Sprawdzilem liczbe: %d -pierwsza. Pozostalo %d zadan oczekujacych", getpid(), generujCzas(), wyprodukowaneZadania[i].liczba, wartosc);
                }

                printf("%s\n", wiadomosc);

                wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status = -1;

                //zwiekszamy pelny
                temp1.sem_flg = 0;
                temp1.sem_num = 0;
                temp1.sem_op = 1;
                semop(semPe, &temp1, 1);
               
                // odblokuj zadanie
                temp1.sem_flg = 0;
                temp1.sem_num = i;
                temp1.sem_op = 1;
                semop(semafor, &temp1, 1);
            }
            else {
                printf("nie dostaje sie do itego wiec przechodze dalej\n");
            }

            i = i + 1;

            temp2.sem_flg = 0;
            temp2.sem_num = 0;
            temp2.sem_op = 1;
            // ustawienie semfkry na 1; 
            semop(semaforKrytycznyK, &temp2, 1);
        }
        else {
            printf("nie dostaje sie do itego wiec przechodze dalej\n");
            i = i + 1;
        }

    }

    return 0;
}
