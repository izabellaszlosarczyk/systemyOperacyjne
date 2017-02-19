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
int main(int argc, char *argv[]) {
    printf("%s\n", "wchodze do dziecka");
    struct sigaction sygnal1, sygnal2;

    koniec = 0; 
    licznikSygnalow = 0;

    sygnal1.sa_handler = zwiekszLSygnalow;
    sygnal2.sa_handler = zakoncz;

   // if ((SIGRTMIN+SIGUSR2 < SIGRTMAX) && (SIGRTMIN+SIGUSR1 < SIGRTMAX)) {
    //        printf("%s", "nie przekracza zakresu");
    //}

        printf("%s\n","dziecko sigfillset");
        if (sigfillset(&(sygnal1.sa_mask)) != 0) {
            perror("Blad funkcji sigfillset dla sygnalu1");
            return -1;
        }

        if (sigfillset(&(sygnal2.sa_mask)) != 0) {
            perror("Blad funkcji sigfillset dla sygnalu2");
            return -1;
        }
        printf("%s\n","dziecko sigaction");
        if (sigaction(SIGRTMIN+SIGUSR1, &sygnal1, NULL) != 0 || sigaction(SIGRTMIN+SIGUSR2, &sygnal2, NULL) != 0) {
            perror("Blad funkcji sigaction");
            return -1;
        }

        sigset_t maska;
        //sigset_t maska_tmp;
        printf("%s\n","dziecko sigemptyset");
        if (sigemptyset(&maska) != 0) {
            perror("Blad funkcji sigemptyset");
            return -1;
        }
        printf("%s\n","dziecko sigaddset");
        if (sigaddset(&maska, SIGRTMIN+SIGUSR1) != 0 || sigaddset(&maska, SIGRTMIN+SIGUSR2) != 0) {
            perror("Blad funkcji sigaddset");
            return -1;
        }
        printf("%s\n","dziecko sigprocmask");
        if (sigprocmask(SIG_UNBLOCK, &maska, NULL) != 0) {
            perror("Blad funkcji sigprocmask");
            return -1;
        }

        printf("%s\n","dziecko oczekwanie");
        while (koniec == 0) {
            sigsuspend(&maska);
            //pause();
        }

        if (sigprocmask(SIG_UNBLOCK, &maska, NULL) != 0) {
            perror("Blad funkcji sigprocmask");
            return -1;
        }

        int i;
        for (i = 0; i < licznikSygnalow; ++i) {
            if (kill(getppid(), SIGRTMIN+SIGUSR1) != 0) {
                perror("Blad funkcji kill");
                return -1;
            }
            printf("potomek>> %s\n", "wysylam SIGUSR1 z dziecka");
        }

        printf("potomek>> wysylam sigusr2\n");
        if (kill(getppid(), SIGRTMIN+SIGUSR2) != 0) {
            perror("Blad funkcji kill");
            return -1;
        }
      
    return 0;
}

