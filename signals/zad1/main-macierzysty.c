#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int licznikSygnalow;

void zwiekszLSygnalow(int sygnal){
    licznikSygnalow = licznikSygnalow + 1;
}
int koniec;

void zakoncz(int sygnal){
    koniec = 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr, "Niewlasciwa liczba argumentow\n");
        return -1;
    }
    int liczbaSygnalow = atoi(argv[1]);
    if (liczbaSygnalow < 0){
        fprintf(stderr, "Niewlasciwa liczba sygnalow, musi byc nieujemna\n");
        return -1;
    }
    licznikSygnalow = 0;
    koniec = 0;
    struct sigaction sygnal1 , sygnal2;

    sygnal1.sa_handler = zwiekszLSygnalow;
    sygnal2.sa_handler = zakoncz;

    if (sigfillset(&(sygnal1.sa_mask)) != 0){
        perror("Blad funkcji sigfillset dla sygnal1");
        exit(1);
    }
    if (sigfillset(&(sygnal2.sa_mask)) != 0){
        perror("Blad funkcji sigfillset dla sygnal2");
        exit(1);
    }
    if(sigaction(SIGUSR1, &sygnal1, NULL) != 0 || sigaction(SIGUSR2, &sygnal2, NULL) != 0){
        perror("Blad funkcji sigaction dla sygnal1 lub sygnal2");
        return -1;
    }

    sigset_t maska, maska_tmp;
    sigemptyset(&maska);

    if (sigaddset(&maska, SIGUSR1) != 0 || sigaddset(&maska, SIGUSR2) != 0){
        perror("Blad funkcji sigaddset");
        return -1;
    }
    //ustawienie zeby nie blokowalo sigusr1 i sigusr2
    sigprocmask(SIG_BLOCK, &maska, NULL);

    pid_t pid = fork();

    if(pid == -1){
        perror("Blad funkcji fork");
        return -1;
    }

    if(pid == 0){
        //printf("");
        if(execl("./main-potomek.o", "main-potomek.o", NULL) == -1){
            perror("Blad funkcji execl");
            return -1;
        } 
    }

    printf("pid: %d\n", pid);

    if(sigprocmask(SIG_UNBLOCK, &maska, NULL) != 0){
        perror("Blad funkcji sigprocmask");
        return -1;
    }

    printf("Wysylanie %d sygnalow SIGUSR1 do dziecka.\n", liczbaSygnalow);
    //sleep(10);

    int i;
    for(i=0; i < liczbaSygnalow; i++){
        printf("wysylam: %d\n", i);
        if(kill(pid, SIGUSR1) != 0)	{
            perror("Blad funkcji kill");
            return -1;
        }
    }

    printf("Wysylanie sygnalu SIGUSR2 do dziecka.\n");

    if(kill(pid, SIGUSR2) != 0) {
        perror("Blad funkcji kill");
        return -1;
    }

    while(!koniec){
        printf ("%s\n","czekam");
        sigsuspend(&maska_tmp);
    }
    if(sigprocmask(SIG_UNBLOCK, &maska, NULL) != 0){
        perror("Blad funkcji sigprocmask");
        return -1;
    }

    printf("Rodzic odebral %d sygnal(y/ow) SIGUSR1 z %d spodziewanych.\n", licznikSygnalow, liczbaSygnalow);

    return 0;
}
