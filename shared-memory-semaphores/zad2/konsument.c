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

void zakonczenie2(int sig){
    
    exit(0);
}

int konsument(int argc, char *argv[]){
	
    struct sigaction sa;
    sa.sa_handler = zakonczenie2;
    sigaction(SIGTSTP, &sa, NULL);

    printf("klient: generuje pamiec dzielona\n");

    int shm_mem = shm_open("/shared_mem", O_RDONLY, 0);
	ftruncate(shm_mem, sizeof(struct zadanie)*ROZMIAR_BUFORA_WSPOLNEGO);
	
	struct zadanie* wyprodukowaneZadania = (struct zadanie*)mmap(NULL, sizeof(struct zadanie)*ROZMIAR_BUFORA_WSPOLNEGO, PROT_READ, MAP_SHARED, shm_mem, 0);

	printf("klient: generuje semafory\n");
	sem_t* semPe = sem_open("/semPe", O_CREAT, 0644, ROZMIAR_BUFORA_WSPOLNEGO);
	sem_t* semPu = sem_open("/semPu", O_CREAT, 0644, 0);
	sem_t* semaforKrytycznyK = sem_open("/semaforKrytycznyK", O_CREAT, 0644, 1);

	sem_t* tablicaSemaforow[ROZMIAR_BUFORA_WSPOLNEGO];
    int i;
    for (i = 0; i < ROZMIAR_BUFORA_WSPOLNEGO; i = i + 1){
    	char str[2];
    	sprintf(str, "%s", "/");
		sprintf(str, "%d", i);
		tablicaSemaforow[i] = sem_open(str, O_CREAT, 0644, 0);	
    }

    printf("klient: wchodze do while\n");
    i = 0;
    while (1){
    	printf("klient: sprawdzam sem. krytyczny\n");
    	int wartosc;
        sem_getvalue(semaforKrytycznyK, &wartosc);
        if (wartosc == 1) {
            // zablokowanie krytycznego 
            if (sem_trywait(semaforKrytycznyK) == -1){
            	perror("blad przy dec- semaforKrytycznyK");
            }

            int wartosc2;
        	sem_getvalue(semPu, &wartosc2);
        	printf("klient: sprawdzam sem. pusty\n");

        	int wartosc3;
        	sem_getvalue(tablicaSemaforow[i%ROZMIAR_BUFORA_WSPOLNEGO], &wartosc3);
        	printf("klient: sprawdzam sem. zadania\n");

            if ((wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].status == 1) && (wartosc2 != 0) && (wartosc3 != 0)){ 

                // zablokuj i-ty semafor
                sem_trywait(tablicaSemaforow[i%ROZMIAR_BUFORA_WSPOLNEGO]); 

                // zmniejsz pusty
                if (sem_trywait(semPu) == -1){
            	perror("blad przy dec- semPu");
            	}

                sem_getvalue(semPu, &wartosc);
                if (czyPierwsza(wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].liczba)== 0){
                    sprintf(wiadomosc, "%d %s Sprawdzilem liczbe: %d -zlozona. Pozostalo %d zadan oczekujacych", getpid(), generujCzas(), wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].liczba, wartosc);
                }else{
                    sprintf(wiadomosc, "%d %s Sprawdzilem liczbe: %d -pierwsza. Pozostalo %d zadan oczekujacych", getpid(), generujCzas(), wyprodukowaneZadania[i%ROZMIAR_BUFORA_WSPOLNEGO].liczba, wartosc);
                }

                printf("%s\n", wiadomosc);

                //zwiekszamy pelny
                sem_post(semPe);
               
                // odblokuj zadanie
                sem_post(tablicaSemaforow[i%ROZMIAR_BUFORA_WSPOLNEGO]);
            }
            else {
                i = i + 1;
            }
            sem_post(semaforKrytycznyK);
            i = i + 1;
        }
    }

    return 0;
}