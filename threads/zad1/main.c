#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

int iloscWatkow, iloscRekordow;
int rozmiarBufora = 1024;
char *nazwaPliku, *str;
FILE* uchwytF;
pthread_t *watki;
pthread_key_t buforKlucz;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t czekaj = PTHREAD_MUTEX_INITIALIZER;

int flagaIni = 0;


void czyszczenieMutex(void *arg){
	if((errno = pthread_mutex_unlock(&mutex)) != 0){
		perror("Blad funckji pthread_mutex_unlock");
		exit(-1);
	}
}

void freeBufor(void* bufor) {
	free(bufor);
}

void *funkcja(void *string){
	int wczesniejszy = 0;

	char *bufor = (char*)malloc((rozmiarBufora + 1)*iloscRekordow);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &wczesniejszy);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &wczesniejszy);

	unsigned long int watekId = pthread_self();

	pthread_setspecific(buforKlucz, bufor);

	//czeka na watki
	pthread_mutex_lock(&czekaj);
	pthread_mutex_unlock(&czekaj);	

	
	printf("Uruchomiono watek o id: %lu\n", watekId);
	int ile = 1;

	while(ile){
    	sleep(1);
		int przesuniecie;

		pthread_mutex_lock(&mutex);
		przesuniecie = ftell(uchwytF);
		ile = fread(bufor, sizeof(char), rozmiarBufora*iloscRekordow, uchwytF);

		//printf("%s, %d<--%s\n", bufor, ile, (char*)string);		

		bufor[ile]= 0;

		char *dopasowanie;
		if ((dopasowanie = strstr(bufor, string)) != NULL ){
			// jakis ofset
			przesuniecie = przesuniecie + (dopasowanie - bufor);

			printf("watek o id:%lu odnalazl tekst, id rekordu %d\n ", watekId, bufor[przesuniecie/1024]);
			int j = 0;
			while (j < iloscWatkow){
				printf("%d\n", j);
				if (!pthread_equal(watki[j], pthread_self())){
					printf("watek nr %d konczy prace\n", j+1);
					pthread_cancel(watki[j]);
				}
				j = j + 1;
			}
		}

		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

void *funkcja2(void *string){

	char *bufor = (char*)malloc((rozmiarBufora + 1)*iloscRekordow);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	unsigned long int watekId = pthread_self();

	pthread_mutex_lock(&czekaj);

	pthread_mutex_unlock(&czekaj);	

	while(flagaIni != 1){
		printf("watek :%lu", watekId);
		printf(" czekam na inicjalizacje\n");
	}
	
	printf("Uruchomiono watek o id: %lu\n", watekId);
	int ile = 1;
	while(ile){
		pthread_mutex_lock(&mutex);
		int przesuniecie = ftell(uchwytF);
		ile = fread(bufor, sizeof(char), rozmiarBufora*iloscRekordow, uchwytF);
		bufor[ile]= 0;
		pthread_mutex_unlock(&mutex);

		char *dopasowanie;
		if ((dopasowanie = strstr(bufor, string)) != NULL ){

			// jakis ofset
			przesuniecie = przesuniecie + (dopasowanie - bufor);
			printf("watek o id:%lu odnalazl tekst, id rekordu %d\n ", watekId, bufor[przesuniecie/1024]);
		}
		pthread_testcancel();
	}
	int j = 0;
	while (j < iloscWatkow){
		if (watki[j] != pthread_self()){
			printf("watek nr %d konczy prace\n", j+1);
			pthread_cancel(watki[j]);
		}
		j = j + 1;
	}

	return NULL;
}

void *funkcja3(void *string){
	int flagaZnalezienia = 0;
	char *bufor = (char*)malloc((rozmiarBufora + 1)*iloscRekordow);

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	unsigned long int watekId = pthread_self();

	pthread_mutex_lock(&czekaj);

	pthread_mutex_unlock(&czekaj);	

	while(flagaIni != 1){
		printf("watek :%lu", watekId);
		printf("czekam na inicjalizacje\n");
	}
	
	printf("Uruchomiono watek o id: %lu\n", watekId);
	int ile = 1;
	while(ile || !flagaZnalezienia){
		pthread_mutex_lock(&mutex);
		int przesuniecie = ftell(uchwytF);
		ile = fread(bufor, sizeof(char), rozmiarBufora*iloscRekordow, uchwytF);
		bufor[ile]= 0;
		pthread_mutex_unlock(&mutex);

		char *dopasowanie;
		if ((dopasowanie = strstr(bufor, string)) != NULL ){

			// jakis ofset
			przesuniecie = przesuniecie + (dopasowanie - bufor);
			printf("watek o id:%lu odnalazl tekst, id rekordu %d\n ", watekId, bufor[przesuniecie/1024]);
		}
		flagaZnalezienia = 1;
		pthread_testcancel();
	}
	int j = 0;
	while (j < iloscWatkow){
		if (watki[j] != pthread_self()){
			printf("watek nr %d konczy prace\n", j+1);
			pthread_cancel(watki[j]);
		}
		j = j + 1;
	}

	return NULL;
}


int main(int argc, char *argv[]){
	if (argc != 6){
		perror("zla liczba arg 1: ilosc watkow , 2: nazwa pliku , 3: ilosc rekordow do przeszukania 4 : szukany string 5 : typ zakonczenia watkow\n");
		return -1;
	}
	iloscWatkow = atoi(argv[1]);
	nazwaPliku = argv[2];
	iloscRekordow = atoi(argv[3]);
	str = argv[4];
	int typ;
	typ = atoi(argv[5]);

	printf("zadane parametry to: %d - ilosc watkow, %s - nazwa pliku, %d - ilosc rekordow, %s - szukany string %d - typ \n", iloscWatkow, nazwaPliku, iloscRekordow, str, typ);
	uchwytF = fopen(nazwaPliku, "r");
	

	// do tworzenia pliku z rekordami
	/*uchwytF = fopen(nazwaPliku, "w+");
	int k;
	for (k = 0; k < iloscWatkow*iloscRekordow + 3; k = k + 1){
		char c;
		srand(time(NULL));
		c = rand()%100; // int dwa bajty
		fputc(c, uchwytF);
		int m;
		for (m = 0; m < 1023; m = m + 1){
			c = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random () % 26];
			fputc(c, uchwytF);
		}
	}*/

	watki = (pthread_t *)malloc(sizeof(pthread_t)*iloscWatkow);


    pthread_key_create(&buforKlucz, freeBufor);

	pthread_mutex_lock(&czekaj);

    int i = 0; 
    printf("tworzenie watkow\n");
    while(i < iloscWatkow){
    	if (typ == 1){
    		pthread_create(&watki[i], NULL, funkcja, str);
    	}else if (typ == 2){
    		pthread_create(&watki[i], NULL, funkcja2, str);
    	}else {
    		pthread_create(&watki[i], NULL, funkcja3, str);
    	}
    	i = i + 1;
    }

    pthread_mutex_unlock(&czekaj);

    flagaIni = 1;
    i = 0;
    while(i < iloscWatkow){
    	if(pthread_join(watki[i], NULL) != 0) {
    		perror("pthread_join");
    	}
    	i = i + 1;
    }

    fclose(uchwytF);
    printf("%s\n","program zostanie zakonczony");
	return 0;
}
