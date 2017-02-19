#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define MAX 100
#define LICZBA_CZYTELNIKOW 25
#define ROZMIAR_BUFFORA 20
#define LICZBA_PISARZY 24 // lepsza opcja niz czytanie jako argv[] ze wzgledu na funkcje pthread_create

pthread_t czytelnicy[LICZBA_CZYTELNIKOW];
int bufor[ROZMIAR_BUFFORA];
pthread_t pisarze[LICZBA_PISARZY];
sem_t semCz, semKolejnosc, semDostep; 
// semKolejnosc - dba aby nie bylo zaglodzen
unsigned int ile = 0; // ile aktualnie jest w czytelni

int randomowaLiczba(){  
  return (int)rand();
}

void zadaniePisarza(){
  int indeks; 
  indeks = randomowaLiczba()%ROZMIAR_BUFFORA;
  bufor[indeks] = randomowaLiczba()%10; // losowana z mniejszego zakresu- wieksza szansa na znalezienie przez klienta
  printf("Pisarz: wrzucilem liczbe: %d na miejscu %d\n", bufor[indeks], indeks);
}

void zadanieCzytelnika(int liczba){
  int i, licznik = 0;
  for (i = 0; i < ROZMIAR_BUFFORA; i = i + 1) {
    if (bufor[i] == liczba){
      printf("Czytelnik: znalazlem liczbe: %d na miejscu %d\n", liczba, i);
      licznik = licznik + 1;
    }
  }
  if (licznik == 0)
  	printf("Czytelnik: nie znalazlem liczby: %d\n", liczba);
  else 
  	printf("Znalazlem liczbe %d %d razy\n", liczba, licznik);
  return ;
}

void* pisarz(void *ptr) {
  int i, r;
  i = 0;
  r = randomowaLiczba()%5; // ile razy ma zmieniac wrzucone na zew zeby bylo widac przeplatanie
  while(i < r) {
    sem_wait(&semKolejnosc);
    sem_wait(&semDostep); 
    sem_post(&semKolejnosc); 

    zadaniePisarza();

    sem_post(&semDostep);
    i = i + 1;
  }
  pthread_exit(0);
}

void* czytelnik(void *l) {
  int liczba;
  liczba = *((int*)l);

  sem_wait(&semKolejnosc);
  sem_wait(&semCz);        
  if (ile  == 0)        
    sem_wait(&semDostep);      
  ile = ile + 1;
  sem_post(&semKolejnosc);
  sem_post(&semCz);

  zadanieCzytelnika(liczba);     

  sem_wait(&semCz);         
  ile = ile - 1;          
  if (ile == 0) 
    sem_post(&semDostep); 
  sem_post(&semCz);  

  pthread_exit(0);
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  int r, i ;

  sem_init(&semDostep, 0 , 1);
  sem_init(&semCz, 0, 1); 
  sem_init(&semKolejnosc, 0, 1); 

  for (i = 0 ; i < ROZMIAR_BUFFORA; i = i + 1){
    bufor[i] = 0;
  }
  for (i = 0; i <LICZBA_PISARZY; i = i +1){
    pthread_create(&pisarze[i], NULL, pisarz, NULL);

    r = randomowaLiczba()%10; // liczba do wyszukania w tablicy 
    pthread_create(&czytelnicy[i], NULL, czytelnik, &r);
  }

  for (i = 0; i < LICZBA_PISARZY; i = i +1){
    pthread_join(czytelnicy[i], NULL);
    pthread_join(pisarze[i], NULL);
  }
  sem_destroy(&semDostep);
  sem_destroy(&semCz);
  sem_destroy(&semKolejnosc);

  return 0;
}