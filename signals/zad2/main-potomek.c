#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>


int licznikSygnalow;
int koniec;
int potwierdzenieOtrzymania;

void zwiekszLSygnalow(int sygnal){
    printf("dziecko, otrzymalem sygnal: %d\n", sygnal);
    licznikSygnalow = licznikSygnalow + 1;
    if(potwierdzenieOtrzymania){
        printf("dziecko, wysylam potwierdzenie\n");
        if(kill(getppid(), SIGUSR1) != 0){
            perror("in function kill()");
            exit(1);
        }
    }
}

void zakoncz(int sygnal){
    printf("%s", "dziecko, koncze");
    koniec = 1;
}

// liczbasygnalow - ile z polecenia przekazujemy, liczniksygnalow - ile faktycznie bylo sygnalow
int main(int argc, char *argv[]){

    struct sigaction sygnal1, sygnal2;

    sygnal1.sa_handler = zwiekszLSygnalow;
    sygnal2.sa_handler = zakoncz;

    koniec = 0;
    licznikSygnalow = 0;
    potwierdzenieOtrzymania = 1;

    printf("%s\n", "dziecko, sigfillset");
    if(sigfillset(&(sygnal1.sa_mask)) != 0 ){
        perror("Blad funkcji sigfillset dla sygnalu1");
        return -1;
    }

    if (sigfillset(&(sygnal2.sa_mask)) != 0){
        perror("Blad funkcji sigfillset dla sygnalu2");
        return -1;
    }
    printf("%s\n", "dziecko, sigaction");
    if(sigaction(SIGUSR1, &sygnal1, NULL) != 0 || sigaction(SIGUSR2, &sygnal2, NULL) != 0){
        perror("Blad funkcji sigaction");
        return -1;
    }

    sigset_t maska, maska_tmp;
    if(sigemptyset(&maska) != 0){
        perror("Blad funkcji sigemptyset");
        return -1;
    }
    printf("%s\n", "dziecko, sigaddset");
    if(sigaddset(&maska, SIGUSR1) != 0 || sigaddset(&maska, SIGUSR2) != 0){
        perror("Blad funkcji sigaddset");
        return -1;
    }
    printf("%s\n", "dziecko, sigprocmask");
    if(sigprocmask(SIG_UNBLOCK, &maska, &maska_tmp) != 0){
        perror("Blad funkcji sigprocmask");
        return -1;
    }

    while(koniec == 0) {
        printf("%s\n", "dziecko, usypiam");
        //sigsuspend(&maska_tmp);
        pause();
    }
/*
    if(sigprocmask(SIG_UNBLOCK, &maska, NULL) != 0){
        perror("Blad funkcji sigprocmask");
        return -1;
    }*/

    printf("dziecko, wysylam %d sygnalow", licznikSygnalow);

    int i;
    for(i=0; i<licznikSygnalow; ++i){
        printf("%s\n", "dziecko, wysylam");
        if(kill(getppid(), SIGUSR1) != 0){
            perror("Blad funkcji kill");
            return -1;
        }
    }

    if(kill(getppid(), SIGUSR2) != 0){
        perror("Blad funkcji kill");
        return -1;
    }


    return 0;
}