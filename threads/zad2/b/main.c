#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <signal.h>


int iloscWatkow, iloscRekordow;
int rozmiarBufora = 1024;
char *nazwaPliku, *str;
FILE* uchwytF;
pthread_t *watki;
pthread_key_t buforKlucz;

char *str;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t czekaj = PTHREAD_MUTEX_INITIALIZER;

int flagaIni = 0;
int numerTestu, sig;

int dzieleniePrzezZero = 0;

void handler1(int sig) {
	printf("Przechwycono sygnal: %d, PID: %ld, TID: %lu\n", sig, (long) getpid(), (long) pthread_self());
}

void handler2(int sig, siginfo_t *info, void *ptr) {
	printf("Przechwycono sygnal: %d, PID: %ld, TID: %lu\n", sig, (long) getpid(), (long) pthread_self());
}


void czyszczenieMutex(void *sygFunk){
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

	int ile = 0;
	while(ile){
    	sleep(1);
		int przesuniecie;

		pthread_mutex_lock(&mutex);
		przesuniecie = ftell(uchwytF);
		ile = fread(bufor, sizeof(char), rozmiarBufora*iloscRekordow, uchwytF);

		//printf("%s, %d<--%s\n", bufor, ile, (char*)string);		

		bufor[ile]= 0;

		char *dopasowanie;
		if ((dopasowanie = strstr(bufor, str)) != NULL ){
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

void *funkcja2(void *sygFunk){
	int sigFlaga;
	sigset_t zbior;
	struct sigaction sa;

	sigFlaga = *((int *) sygFunk);
	free(sygFunk);

	char *bufor = (char*)malloc((rozmiarBufora + 1)*iloscRekordow);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	unsigned long int watekId = pthread_self();

	pthread_mutex_lock(&czekaj);

	pthread_mutex_unlock(&czekaj);	

	if (sigFlaga) {
		if(numerTestu == 4){
			sigemptyset(&zbior);
			sigaddset(&zbior, sig);
			sigprocmask(SIG_SETMASK, &zbior, NULL);
		}

		if (numerTestu == 5) {
			memset(&sa, 0, sizeof(sa));
			sa.sa_handler = handler1;
			if (sigaction(sig, &sa, NULL) == -1) {
				perror("sigaction(...) failed");
				exit(1);
			}
		}

		if (numerTestu == 4 || numerTestu == 5) {
			pthread_kill(pthread_self(), sig);
		}
	}
	pthread_sigmask(SIG_BLOCK, &zbior, NULL);

	while(flagaIni != 1){
		printf("watek :%lu", watekId);
		printf(" czekam na inicjalizacje\n");
	}
	
	printf("Uruchomiono watek o id: %lu\n", watekId);
	int ile = 1;
	while(ile){
		pthread_mutex_lock(&mutex);
		printf("Uruchomiono watek o id: %lu\n", watekId);
		int ile = 1;
		if (dzieleniePrzezZero == 0){
			dzieleniePrzezZero = 1;
			printf("uwaga dziele przez zero\n");
			int liczba = 5/0;
		}

		int przesuniecie = ftell(uchwytF);
		ile = fread(bufor, sizeof(char), rozmiarBufora*iloscRekordow, uchwytF);
		bufor[ile]= 0;
		pthread_mutex_unlock(&mutex);

		char *dopasowanie;
		if ((dopasowanie = strstr(bufor, str)) != NULL ){

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

void *funkcja3(void *sygFunk){
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
		if ((dopasowanie = strstr(bufor, str)) != NULL ){

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
	if (argc != 3){
		perror("zla liczba arg");
		return -1;
	}
	numerTestu = atoi(argv[1]);
	sig = atoi(argv[2]);
	iloscWatkow = 5;
	nazwaPliku = "test1.txt";
	iloscRekordow = 3;
	str = "A";
	int typ; 
	typ = 2;
	struct sigaction sa;
	sigset_t zbior;
	int *sigFlaga;

	printf("nr testu :%d\n", numerTestu);
	//printf("zadane parametry to: %d - ilosc watkow, %s - nazwa pliku, %d - ilosc rekordow, %s - szukany string %d - typ \n", iloscWatkow, nazwaPliku, iloscRekordow, str, typ);
	uchwytF = fopen(nazwaPliku, "r");
	watki = (pthread_t *)malloc(sizeof(pthread_t)*iloscWatkow);


    pthread_key_create(&buforKlucz, freeBufor);

	pthread_mutex_lock(&czekaj);

    if(numerTestu == 2){
		sigemptyset(&zbior);
		sigaddset(&zbior, sig);
		sigprocmask(SIG_BLOCK, &zbior, NULL);
    }

	if (numerTestu == 3) {
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler = handler1;
		if (sigaction(sig, &sa, NULL) == -1) {
			perror("sigaction(...) failed");
			exit(1);
		}
	}
	printf("tworzenie watkow\n");
	int i = 0;
	while (i < iloscWatkow) {
		sigFlaga = calloc(1, sizeof(int)); 
		if (i == 0) {
			*sigFlaga = 1; 
		} else {
			*sigFlaga = 0; 
		}
		if (typ == 1){
    		pthread_create(&watki[i], NULL, funkcja, sigFlaga);
    	}else if (typ == 2){
    		pthread_create(&watki[i], NULL, funkcja2, sigFlaga);
    	}else {
    		pthread_create(&watki[i], NULL, funkcja3, sigFlaga);
    	}
		i = i + 1;
	}    

    pthread_mutex_unlock(&czekaj);

	if(numerTestu == 1 || numerTestu == 2 || numerTestu == 3){
		if(kill(getpid(), sig) != 0){
			perror("Blad funkcji kill");
			exit(-1);
		}
	}

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
