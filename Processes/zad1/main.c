#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
// ############################################### struktury pomocnicze ################################
int licznik;
#define ROZMIARSTOSU 512
int tablicaN[4] = {4000 , 20000, 85000, 100000};
FILE *uchwytPliku;

struct czasS {
    struct timeval sys;
    struct timeval usr;
    struct timeval real;
    struct timeval sumUsSys;
};

struct czasS rodzic, dziecko;

// 0 linia - fork  1 linia - vfork  2 linia - clonefork 3 linia - clonevfork
// kolejne kolumny to wartosci n
struct czasS tablicaWynikowM[4][4];
struct czasS tablicaWynikowD[4][4];
struct czasS tablicaWynikow[4][4];

// ################################################funkcje programowe ########################################
int zwiekszLiczbe(){
    licznik = licznik + 1;
    return 0;
}

void wynikiDoStruktur(struct czasS macierzysty, struct czasS potomny, int i, int typ){
    struct rusage tmp1;
    struct czasS sumaryczny;
    getrusage(RUSAGE_SELF, &tmp1);
    macierzysty.sys.tv_sec = tmp1.ru_stime.tv_sec;
    macierzysty.sys.tv_usec = tmp1.ru_stime.tv_usec;
    macierzysty.usr.tv_sec = tmp1.ru_utime.tv_sec;
    macierzysty.usr.tv_usec = tmp1.ru_utime.tv_usec;
    macierzysty.sumUsSys.tv_sec = tmp1.ru_stime.tv_sec + tmp1.ru_utime.tv_sec;
    macierzysty.sumUsSys.tv_usec = tmp1.ru_stime.tv_usec + tmp1.ru_utime.tv_usec;

    sumaryczny.sys.tv_sec = tmp1.ru_stime.tv_sec;
    sumaryczny.sys.tv_usec = tmp1.ru_stime.tv_usec;
    sumaryczny.usr.tv_sec = tmp1.ru_utime.tv_sec;
    sumaryczny.usr.tv_usec = tmp1.ru_utime.tv_usec;
    sumaryczny.sumUsSys.tv_sec = tmp1.ru_stime.tv_sec + tmp1.ru_utime.tv_sec;
    sumaryczny.sumUsSys.tv_usec = tmp1.ru_stime.tv_usec + tmp1.ru_utime.tv_usec;

    getrusage(RUSAGE_CHILDREN, &tmp1);
    potomny.sys.tv_sec = tmp1.ru_stime.tv_sec;
    potomny.sys.tv_usec = tmp1.ru_stime.tv_usec;
    potomny.usr.tv_sec = tmp1.ru_utime.tv_sec;
    potomny.usr.tv_usec = tmp1.ru_utime.tv_usec;
    potomny.sumUsSys.tv_sec = tmp1.ru_stime.tv_sec + tmp1.ru_utime.tv_sec;
    potomny.sumUsSys.tv_usec = tmp1.ru_stime.tv_usec + tmp1.ru_utime.tv_usec;

    sumaryczny.sys.tv_sec += tmp1.ru_stime.tv_sec;
    sumaryczny.sys.tv_usec += tmp1.ru_stime.tv_usec;
    sumaryczny.usr.tv_sec += tmp1.ru_utime.tv_sec;
    sumaryczny.usr.tv_usec += tmp1.ru_utime.tv_usec;
    sumaryczny.sumUsSys.tv_sec += tmp1.ru_stime.tv_sec + tmp1.ru_utime.tv_sec;
    sumaryczny.sumUsSys.tv_usec += tmp1.ru_stime.tv_usec + tmp1.ru_utime.tv_usec;

    tablicaWynikowM[typ][i] = macierzysty;
    tablicaWynikowD[typ][i] = potomny;
    tablicaWynikow[typ][i] = sumaryczny;
}

// i - ktory typ testu, typ - ktore n
void testFork(int N, int i, int typ){
    int tmp, status;
    pid_t pid;
    struct timeval startTm, koniecTm;
    dziecko.real = (struct timeval){0, 0};
    gettimeofday(&rodzic.real, NULL);

    for (tmp = 0; tmp < N; tmp++){
        gettimeofday(&startTm, NULL);
        pid = fork();
        if(pid == 0){ //w procesie potomnym
            zwiekszLiczbe();
            _exit(0);
        } else if(pid < 0){
            printf("Błąd fork");
            _exit(-1);
        } else {
            waitpid(pid, &status, 0);
        }
        gettimeofday(&koniecTm, NULL);

        dziecko.real.tv_sec += (koniecTm.tv_sec - startTm.tv_sec);
        dziecko.real.tv_usec += (koniecTm.tv_usec - startTm.tv_usec);
    }
    gettimeofday(&koniecTm, NULL);
    rodzic.real.tv_sec = koniecTm.tv_sec - rodzic.real.tv_sec;
    rodzic.real.tv_usec = koniecTm.tv_usec - rodzic.real.tv_usec;
    wynikiDoStruktur(rodzic, dziecko,i,typ);
    printf("Licznik: %d\n", licznik);

}

void testVfork(int N, int i, int typ){
    int tmp, status;
    pid_t pid;
    struct timeval startTm, koniecTm;
    dziecko.real = (struct timeval){0, 0};
    gettimeofday(&rodzic.real, NULL);

    for (tmp = 0; tmp < N; tmp++){
        gettimeofday(&startTm, NULL);
        pid = vfork();
        if(pid == 0){ //w procesie potomnym
            zwiekszLiczbe();
            _exit(0);
        } else if(pid < 0){
            printf("Błąd fork");
            _exit(-1);
        } else {
            waitpid(pid, &status, 0);
        }
        gettimeofday(&koniecTm, NULL);

        dziecko.real.tv_sec += (koniecTm.tv_sec - startTm.tv_sec);
        dziecko.real.tv_usec += (koniecTm.tv_usec - startTm.tv_usec);
    }
    gettimeofday(&koniecTm, NULL);
    rodzic.real.tv_sec = koniecTm.tv_sec - rodzic.real.tv_sec;
    rodzic.real.tv_usec = koniecTm.tv_usec - rodzic.real.tv_usec;
    wynikiDoStruktur(rodzic, dziecko,i,typ);
    printf("Licznik: %d\n", licznik);
}

void testCloneFork(int N, int i, int typ){
    int tmp;
    void *stosDlaDzieckaW = malloc(ROZMIARSTOSU);

    if(stosDlaDzieckaW == NULL){
        perror("Nie udalo sie zaalokowac stosu dla procesu potomnego.");
        _exit(-1);
    }
    void *stosDlaDziecka = stosDlaDzieckaW + ROZMIARSTOSU;
    struct timeval startTm, koniecTm;
    dziecko.real = (struct timeval){0, 0};
    gettimeofday(&rodzic.real, NULL);

    for (tmp = 0; tmp < N; tmp++){
        gettimeofday(&startTm, NULL);
        pid_t pid = clone(&zwiekszLiczbe, stosDlaDziecka, SIGCHLD | CLONE_FS, NULL);
        if (pid == -1){
            perror("Funkcja clone (fork) zawiodla");
            _exit(-1);
        }
        if (waitpid(pid, NULL, 0) == -1)
            perror("waitpid");
        gettimeofday(&koniecTm, NULL);

        dziecko.real.tv_sec += (koniecTm.tv_sec - startTm.tv_sec);
        dziecko.real.tv_usec += (koniecTm.tv_usec - startTm.tv_usec);
    }
    gettimeofday(&koniecTm, NULL);
    rodzic.real.tv_sec = koniecTm.tv_sec - rodzic.real.tv_sec;
    rodzic.real.tv_usec = koniecTm.tv_usec - rodzic.real.tv_usec;
    wynikiDoStruktur(rodzic, dziecko,i,typ);
    printf("Licznik: %d\n", licznik);
}

void testCloneVfork(int N, int i , int typ){
    int tmp;
    void *stosDlaDzieckaW = malloc(ROZMIARSTOSU);
    if(stosDlaDzieckaW == NULL){
        perror("Nie udalo sie zaalokowac stosu dla procesu potomnego.");
        _exit(-1);
    }
    struct timeval startTm, koniecTm;
    dziecko.real = (struct timeval){0, 0};
    gettimeofday(&rodzic.real, NULL);
    void *stosDlaDziecka = stosDlaDzieckaW + ROZMIARSTOSU;

    for (tmp = 0; tmp < N; tmp++){
        gettimeofday(&startTm, NULL);
        pid_t pid = clone(&zwiekszLiczbe, stosDlaDziecka, SIGCHLD | CLONE_FS | CLONE_VM | CLONE_VFORK | CLONE_FILES, NULL);
        if (pid == -1){
            perror("Funkcja clone (fork) zawiodla");
            _exit(-1);
        }
        if (waitpid(pid, NULL, 0) == -1)
            perror("waitpid");
        gettimeofday(&koniecTm, NULL);

        dziecko.real.tv_sec += (koniecTm.tv_sec - startTm.tv_sec);
        dziecko.real.tv_usec += (koniecTm.tv_usec - startTm.tv_usec);
    }
    gettimeofday(&koniecTm, NULL);
    rodzic.real.tv_sec = koniecTm.tv_sec - rodzic.real.tv_sec;
    rodzic.real.tv_usec = koniecTm.tv_usec - rodzic.real.tv_usec;
    wynikiDoStruktur(rodzic, dziecko,i,typ);
    printf("Licznik: %d\n", licznik);
}

void test(int N, int i){
    testFork(N,i,0);// drugi int opcja zeby wiedziec do ktorego wiersza
    testVfork(N,i,1);
    testCloneFork(N,i,2);
    testCloneVfork(N,i,3);
}

void zapisDoPliku(){
    int i , j , k;
    for (i = 0; i < 3; i = i + 1){
        if (i == 0) fprintf(uchwytPliku, "%s\n", "Wyniki dla Macierzystego");
        if (i == 1) fprintf(uchwytPliku, "%s\n", "Wyniki dla Potomnych");
        if (i == 2) fprintf(uchwytPliku, "%s\n", "Wyniki Sumaryczne ");
        for (j = 0; j < 4; j = j + 1){ // dla ktorego N
            fprintf(uchwytPliku, "wartosc N to: %d\n", tablicaN[j]);
            fprintf(uchwytPliku, "%s\n", " ");
            for (k = 0; k < 4; k = k + 1){
                fprintf(uchwytPliku, "metoda nr: %d\n", (k + 1));
                if (i == 0){
                   // printf("%ld.%06ld\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
                    fprintf(uchwytPliku, "%s", "Systemowy:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowM[k][j].sys.tv_sec, tablicaWynikowM[k][j].sys.tv_usec);
                    fprintf(uchwytPliku, "%s", "Użytkownika:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowM[k][j].usr.tv_sec, tablicaWynikowM[k][j].usr.tv_usec);
                    fprintf(uchwytPliku, "%s", "Rzeczywisty:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowM[k][j].real.tv_sec, tablicaWynikowM[k][j].real.tv_usec);
                    fprintf(uchwytPliku, "%s", "Suma Sys i Usr:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowM[k][j].sumUsSys.tv_sec, tablicaWynikowM[k][j].sumUsSys.tv_usec);
                    fprintf(uchwytPliku, "%s\n", " ");
                    fprintf(uchwytPliku, "%s\n", " ");
                }
                if (i == 1){
                    fprintf(uchwytPliku, "%s", "Systemowy:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowD[k][j].sys.tv_sec, tablicaWynikowD[k][j].sys.tv_usec);
                    fprintf(uchwytPliku, "%s", "Użytkownika:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowD[k][j].usr.tv_sec, tablicaWynikowD[k][j].usr.tv_usec);
                    fprintf(uchwytPliku, "%s", "Rzeczywisty:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowD[k][j].real.tv_sec, tablicaWynikowD[k][j].real.tv_usec);
                    fprintf(uchwytPliku, "%s", "Suma Sys i Usr:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikowD[k][j].sumUsSys.tv_sec, tablicaWynikowD[k][j].sumUsSys.tv_usec);
                    fprintf(uchwytPliku, "%s\n", " ");
                    fprintf(uchwytPliku, "%s\n", " ");
                }
                if (i == 2){
                    fprintf(uchwytPliku, "%s", "Systemowy:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikow[k][j].sys.tv_sec, tablicaWynikow[k][j].sys.tv_usec);
                    fprintf(uchwytPliku, "%s", "Użytkownika:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikow[k][j].usr.tv_sec, tablicaWynikow[k][j].usr.tv_usec);
                    fprintf(uchwytPliku, "%s", "Rzeczywisty:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikow[k][j].real.tv_sec, tablicaWynikow[k][j].real.tv_usec);
                    fprintf(uchwytPliku, "%s", "Suma Sys i Usr:");
                    fprintf(uchwytPliku, "%ld.%06ld\n", tablicaWynikow[k][j].sumUsSys.tv_sec, tablicaWynikow[k][j].sumUsSys.tv_usec);
                    fprintf(uchwytPliku, "%s\n", " ");
                    fprintf(uchwytPliku, "%s\n", " ");
                }
            }
        }
    }
}

int main(int argc, char* argv[]){
    int i;
    licznik = 0;
    for (i = 0; i < 4 ; i = i +1){
        test(tablicaN[i],i); // ktory N
    }
    uchwytPliku = fopen("wyniki.txt", "w");
    if (uchwytPliku == NULL){
        perror("Problem z funkcja open");}
    else {
        printf("%s","no i co");
        zapisDoPliku();
    }
    fclose(uchwytPliku);
    return 0;
}