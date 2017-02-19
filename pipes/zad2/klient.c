#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define BUFFER_MAX_SIZE 1024

char* getTime() {
    time_t rawtime;
    struct tm * timeinfo;        
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    char *timeString = (char*)malloc(sizeof(char)*8);
    strftime(timeString, 8, "%H:%M:%S", timeinfo);

    return timeString;
}

char* getWiadomosc(char *tekst, char *wiadomosc) {
    char *tmp;
    if((tmp = (char *)malloc(sizeof(char))) == NULL){
        perror("Blad alokacji pamieci 2");
        return NULL;
    }
    wiadomosc = (char *)realloc(wiadomosc, (strlen(wiadomosc) + strlen(tekst) + 10) * sizeof(char));
    tmp = realloc(tmp, ((strlen(tmp))+ 10) * sizeof(char));
    sprintf(tmp, "%ld", (long)getpid());
    strcat(wiadomosc, tmp);
    strcat(wiadomosc, "-");

    char* timeString = getTime();

    wiadomosc = (char *)realloc(wiadomosc, (strlen(wiadomosc)+ strlen(timeString)+ 2) * sizeof(char));
    strcat(wiadomosc, timeString);
    strcat(wiadomosc, "-");
    strcat(wiadomosc, tekst);
    strcat(wiadomosc, "\n");
    return wiadomosc;
}

int main(int argc, char *argv[]){
    int fd;
    //pid_t pid;
    size_t rozmiarWiadomosci;

    if (argc < 2){
        printf("%s", "niewlasciwa liczba argumentow");
        return 1;
    }
    char *nKolejki = argv[1];
    if ((fd = open(nKolejki, O_WRONLY)) == 1){
        perror("Blad open");
        return 1;
    }
    char *wiadomosc, tekst[BUFFER_MAX_SIZE];
    if((wiadomosc =(char*) malloc(sizeof(char))) == NULL){
        perror("Blad alokacji pamieci");
        close(fd);
        return 1;
    }
    wiadomosc[0]= '\0'; // na poczaktu czy na koncu?

    printf("Komunikat max - 1024, Zakonczenie - Ctrl+D.\n");

    // czytanie przez klientow, tworzenie wiadomosci
    while (fgets(tekst, BUFFER_MAX_SIZE, stdin) != NULL){
        wiadomosc = getWiadomosc(tekst, wiadomosc);

    }

    printf("\n>>>%s", wiadomosc);
    rozmiarWiadomosci = strlen(wiadomosc)*sizeof(char);
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror(0);
        exit(1);
    }
    write(fd, wiadomosc, rozmiarWiadomosci);
    syncfs(fd);
    free(wiadomosc);
    close(fd);

    //unlink(nKolejki);
    return 0;
}