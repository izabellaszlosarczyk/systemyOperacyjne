#define _XOPEN_SOURCE

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define BUFFER_MAX_SIZE 1024

int koniecCzytania = 1;

void koniec(int sig){
    koniecCzytania = 0;
}

int main(int argc, char *argv[]){
    int fd;
    //int flaga;
    char *wiadomosc;
    char *c;

    wiadomosc = (char*)malloc(sizeof(char)*BUFFER_MAX_SIZE + 64);
    wiadomosc[0] = '\0';

    if (argc < 2){
        printf("%s", "niewlasciwa liczba argumentow");
        return 1;
    }
    char *nKolejki = argv[1];
    struct sigaction sygnal;
    sygnal.sa_handler = koniec;
    if(sigaction(SIGTSTP, &sygnal, NULL) != 0){
        perror("Blad sigaction");
        return 1;
    }

    if(mkfifo(nKolejki, 0666) != 0){
        perror("Blad mkfifo");
        return 1;
    }
    if ((fd = open(nKolejki, O_RDONLY)) == -1){
        perror("Blad open");
        return 1;
    }
    //flaga = fcntl(fd, F_GETFL);
    //fcntl(fd, F_SETFL, flaga | O_NONBLOCK);

    printf("%s", "koniec czytania CTRL+Z");
    c = (char*)malloc(sizeof(char));

    while(koniecCzytania){
        wiadomosc[0] = '\0';
        while (read(fd, c, sizeof(char)) != 0){
            if(strcmp(c, "\n") == 0) {
                break;
            }
            printf(">>>> %s\n", c);

            strcat(wiadomosc, c);
        }
        if(wiadomosc[0] != '\0') {
            strcat(wiadomosc, "\n\0");

            time_t tm = time(NULL);
            char *timeString = ctime(&tm);
            printf("%s - %s", timeString, wiadomosc);
        }
    }
    free(wiadomosc);
    close(fd);
    unlink(nKolejki);

    return 0;
}
