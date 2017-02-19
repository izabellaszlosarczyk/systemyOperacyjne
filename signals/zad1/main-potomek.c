#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int licznikSygnalow;

void zwiekszLSygnalow(int sygnal){
    printf("otrzymalem sygnal: %d\n", sygnal);
    licznikSygnalow = licznikSygnalow + 1;
}
int koniec;

void zakoncz(int sygnal){
    printf("otrzymalem sygnal zakonczenia: %d\n", sygnal);
    koniec = 1;
}

// liczbasygnalow - ile z polecenia przekazujemy, liczniksygnalow - ile faktycznie bylo sygnalow
int main(int argc, char *argv[]){
    //setbuf(stdout, NULL);

    printf("potomek>> %s", "wchodze do potomka\n");
    struct sigaction sygnal1, sygnal2;

    licznikSygnalow = 0;
    koniec = 0;
    
    sygnal1.sa_handler = zwiekszLSygnalow;
    sygnal2.sa_handler = zakoncz;

    if(sigfillset(&(sygnal1.sa_mask)) != 0 ){
        perror("potomek>> Blad funkcji sigfillset dla sygnalu1");
        return -1;
    }

    if (sigfillset(&(sygnal2.sa_mask)) != 0){
        perror("potomek>> Blad funkcji sigfillset dla sygnalu2");
        return -1;
    }
    if(sigaction(SIGUSR1, &sygnal1, NULL) != 0 || sigaction(SIGUSR2, &sygnal2, NULL) != 0){
        perror("potomek>> Blad funkcji sigaction");
        return -1;
    }

    printf("potomek>> pierwsza czesc done\n");

    sigset_t maska, maska_tmp;

    printf("potomek>> sigemptyset\n");
    if(sigemptyset(&maska) != 0){
        perror("potomek>> Blad funkcji sigemptyset");
        return -1;
    }

    printf("potomek>> sigaddset\n");
    if(sigaddset(&maska, SIGUSR1) != 0 || sigaddset(&maska, SIGUSR2) != 0){
        perror("potomek>> Blad funkcji sigaddset");
        return -1;
    }

    printf("potomek>> sig_block\n");
    if(sigprocmask(SIG_UNBLOCK, &maska, &maska_tmp) != 0){
        perror("potomek>> Blad funkcji sigprocmask");
        return -1;
    }

    printf("%s", "potomek>> czekam w potomku\n");

    while(koniec == 0) {
        printf("%d", koniec);
        sigsuspend(&maska);
    }

    if(sigprocmask(SIG_UNBLOCK, &maska, NULL) != 0){
        perror("potomek>> Blad funkcji sigprocmask");
        return -1;
    }

    int i;
    printf("potomek>> skonczylem czekac. liczba sygnalow: %d\n", licznikSygnalow);
    if (licznikSygnalow != 0){
        for(i=0; i<licznikSygnalow; ++i){
            if(kill(getppid(), SIGUSR1) != 0){
                perror("Blad funkcji kill");
                return -1;
            }
            printf("potomek>> %s\n", "wysylam SIGUSR1 z dziecka");
        }
    }
    else {
        printf("potomek>> %s\n", "nie zlapano sygnalow");
    }

    printf("potomek>> wysylam sigusr2\n");
    if(kill(getppid(), SIGUSR2) != 0){
        perror("potomek>> Blad funkcji kill");
        return -1;
    }

    printf("potomek>> koniec\n");

    return 0;
}

