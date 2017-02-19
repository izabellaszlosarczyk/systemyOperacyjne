#include "glowny.h"

char wiadomosc[1024];

int generujLiczbe(){
    int liczba;
    srand (time(NULL));
    liczba =  ((rand()%100)/10);
    return liczba;
}

void zakonczenie(int sig){
    sem_unlink("/semPe");
    sem_unlink("/semPu");
    sem_unlink("/semaforKrytycznyK");
    int i;
    for (i = 0; i < ROZMIAR_BUFORA_WSPOLNEGO; i = i + 1){
        char str[2];
        sprintf(str, "%s", "/");
        sprintf(str, "%d", i);
        sem_unlink(str);
    }
    exit(0);
}


int producent(int argc, char *argv[]){

    struct sigaction sa;
    sa.sa_handler = zakonczenie;
    sigaction(SIGTSTP, &sa, NULL);

	printf("producent: generuje pamiec dzielona\n");

    int shm_mem = shm_open("/shared_mem", O_RDWR, 0);
	ftruncate(shm_mem, sizeof(struct zadanie)*ROZMIAR_BUFORA_WSPOLNEGO);
	
	struct zadanie* wyprodukowaneZadania = (struct zadanie*)mmap(NULL, sizeof(struct zadanie)*ROZMIAR_BUFORA_WSPOLNEGO, PROT_READ, MAP_SHARED, shm_mem, 0);

	printf("producent: generuje semafory\n");
	sem_t* semPe = sem_open("/semPe", O_CREAT, 0644, ROZMIAR_BUFORA_WSPOLNEGO);
	sem_t* semPu = sem_open("/semPu", O_CREAT, 0644, 0);
	sem_t* semaforKrytycznyP = sem_open("/semaforKrytycznyP", O_CREAT, 0644, 1);

	sem_t* tablicaSemaforow[ROZMIAR_BUFORA_WSPOLNEGO];
    int i;
    for (i = 0; i < ROZMIAR_BUFORA_WSPOLNEGO; i = i + 1){
    	char str[5];
    	sprintf(str, "%s", "/");
		sprintf(str, "%d", i);
		tablicaSemaforow[i] = sem_open(str, O_CREAT, 0644, 0);	
    }

    printf("producent: wchodze do while\n");
    i = 0;
    while(1){
    	printf("producent: sprawdzam sem. krytyczny\n");
        int wartosc;
        sem_getvalue(semaforKrytycznyP, &wartosc);
        if (wartosc == 1) {
            // zablokowanie krytycznego 
            if (sem_trywait(semaforKrytycznyP) == -1){
            	perror("blad przy dec- semaforKrytycznyP");
            }

            int wartosc2;
        	sem_getvalue(semPe, &wartosc2);
        	printf("producent: sprawdzam sem. pelny\n");

        	int wartosc3;
        	sem_getvalue(tablicaSemaforow[i%ROZMIAR_BUFORA_WSPOLNEGO], &wartosc3);
        	printf("producent: sprawdzam sem. zadania\n");

            if ((wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status == -1) && (wartosc2 != 0) && (wartosc3 != 0)) { 
                // zablokuj i-ty semafor
                sem_trywait(tablicaSemaforow[i]); 
               
                // zmniejsz pelny

                if (sem_trywait(semPe) == -1){
            	perror("blad przy dec- semPu");
            	}

                wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].liczba = generujLiczbe();
                wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status = 0;
                sem_getvalue(semPu, &wartosc);
                sprintf(wiadomosc, "%d %s Dodalem liczbe: %d. Pozostalo %d zadan oczekujacych", getpid(), generujCzas(), wyprodukowaneZadania[i].liczba, wartosc);

                printf("%s\n", wiadomosc);

                //zwiekszamy pusty
                sem_post(semPu);
               
                // odblokuj zadanie
                sem_post(tablicaSemaforow[i]);
            }
            else {
                i = i + 1;
            }
            sem_post(semaforKrytycznyP);
        }
    }

    return 0;
}