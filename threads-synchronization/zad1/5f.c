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
#include <ctype.h>

#define GLODNY 1
#define MYSLI 0
#define JE 2

pthread_t filozofowie[5];
pthread_cond_t warunkowe[5];
pthread_mutex_t mutexJedzenia = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lokaj;
int ileZjedli[5];
int stanFilozofa[5];
int ileJe;


void inicjalizacja(){
	int i;
	if (pthread_cond_init(&lokaj, NULL)!= 0 ){
		perror("blad cond init");
		exit(1);
	}
	for (i = 0; i <5 ; i = i +1){
		if (pthread_cond_init(&warunkowe[i], NULL) != 0){
			perror("blad cond init");
			exit(1);
		}	
		stanFilozofa[i] = MYSLI;
	}
}

int lewy(int id){
	return ((id+4)%5);
}

int prawy(int id){
	return ((id+1)%5);
}

int testJedzenia(int i){
	int errno1;
	if ((stanFilozofa[lewy(i)] != JE) && (stanFilozofa[prawy(i)] != JE) && (stanFilozofa[i] == GLODNY)){
		if ((errno1 = pthread_cond_signal(&warunkowe[i])) != 0){
			perror("blad cond signal jedzenie\n");
			return -1;
		}
		stanFilozofa[i] = JE;
		printf("filozof nr: %d je\n", i);
		ileZjedli[i] = ileZjedli[i] + 1;
		ileJe = ileJe + 1;
	}
	return 0;
}
int testLokaj(){
	int errno1;
	if (ileJe < 4){
		if ((errno1 = pthread_cond_signal(&lokaj))!= 0){
			perror("blad cond signal lokaj\n");
		}
		return 1;
	}
	return 0;
}

void podnies(int i){
	int errno1;
	if ((errno1 =  pthread_mutex_lock(&mutexJedzenia))!= 0){
		perror("blad mutex lock\n");
		exit(1);
	}
	testLokaj();
	while (ileJe >= 4){
		if ((errno1 = pthread_cond_wait(&lokaj, &mutexJedzenia))!= 0){
			perror("blad cond wait\n");
			exit(1);
		}
	}
	stanFilozofa[i] = GLODNY;
	testJedzenia(i);
	while(stanFilozofa[i] == GLODNY){
		pthread_cond_wait(&warunkowe[i], &mutexJedzenia);
	}
	if ((errno1 = pthread_mutex_unlock(&mutexJedzenia)) != 0){
		perror("blad unlock\n");
		exit(1);
	}
}

void odloz(int i){
	int errno1;
	if ((errno1 =  pthread_mutex_lock(&mutexJedzenia))!= 0){
		perror("blad mutex lock\n");
		exit(1);
	}
	stanFilozofa[i] = MYSLI;
	ileJe = ileJe - 1;
	testJedzenia(lewy(i));
	testJedzenia(prawy(i));
	if ((errno1 = pthread_mutex_unlock(&mutexJedzenia)) != 0){
		perror("blad unlock\n");
		exit(1);
	}
	sleep(5);
}

void *filozof(void *tmp){
	int i = *((int*)tmp);
	int j = 0;
	while(1){
		if (j > 20){
			break;
		}
		podnies(i);
		odloz(i);
		j = j + 1; // dla testow
	}
	return NULL;
}

int main(int argc , char *argv[]){
	int i, errno1;
	ileJe = 1;
	printf("%s\n", "inicjalizuje");
	for (i = 0; i < 5; i = i +1){
		ileZjedli[i] = 0;
	}
	inicjalizacja();
	printf("%s\n", "wchodze do tworzenia watkow");

	for (i = 0; i <5 ; i = i +1){
		if ((errno1 = pthread_create(&filozofowie[i], NULL, filozof, &i)) != 0){
			perror("blad create");
			return -1;
		} 
	}

	for (i = 0; i <5 ; i = i +1){
		if ((errno1 = pthread_join(filozofowie[i], NULL)) != 0){
			perror("blad join");	
			return -1;
		}
	}

	for (i = 0; i <5 ; i = i +1){
		printf("filozof nr %d zjadl %d razy\n", i + 1, ileZjedli[i]);
	}

	pthread_mutex_destroy(&mutexJedzenia);
	return 0;
}