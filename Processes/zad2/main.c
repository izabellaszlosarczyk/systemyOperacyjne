#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>




int licznikPlikow(char *sciezka, int argc, char **argumenty, int licznikP, int licznikWartosciDzieci, int liczbaDzieci){
    printf("badam sciezke: %s\n", sciezka);
    DIR *katalog;
    struct dirent *elementKatalogu;
    struct stat statystykiPliku;
    char *sciezkaDoPliku;
    pid_t pid;

    if((katalog = opendir(sciezka)) == NULL){
        perror("Blad funkcji opendir");
        return -1; }

    while((elementKatalogu = readdir(katalog))){

        if (strcmp(elementKatalogu->d_name, ".") == 0 || strcmp(elementKatalogu->d_name, "..") == 0){
            continue;
        }
        sciezkaDoPliku = (char*) calloc(strlen(sciezka) + strlen(elementKatalogu->d_name) + 2, sizeof(char));
        strcpy(sciezkaDoPliku, sciezka);
        strcat(sciezkaDoPliku, "/");
        strcat(sciezkaDoPliku, elementKatalogu->d_name);
        stat(sciezkaDoPliku, &statystykiPliku);


        if(S_ISREG(statystykiPliku.st_mode)) {
            ++licznikP;

        }
        else if(S_ISDIR(statystykiPliku.st_mode)){
            liczbaDzieci++;
            pid = fork();
            if(pid < 0){
                perror("Blad funkcji fork");
                return -1;
            }
            if(pid==0){
                argumenty[1] = sciezkaDoPliku;
                if (execlp(argumenty[0], argumenty[0], argumenty[1], argumenty[2], "-child", NULL) == -1) {
                    perror("Blad funkcji execvp");
                    return -1;
                }
            }
        }

        free(sciezkaDoPliku);
    }
    if(strcmp(argumenty[2], "-w") == 0){
        sleep(15);
    }

    int status;
    int i;
    for(i = 0; i < liczbaDzieci -1 ; i++){
        wait(&status);
        licznikWartosciDzieci += WEXITSTATUS(status);
    }

    if(strcmp(argumenty[argc-1], "-child") != 0) {
        return licznikP+licznikWartosciDzieci;
    } else {
        exit(licznikP+licznikWartosciDzieci);
    }
}

int main(int argc, char **argv){
    int licznikP = 0;
    int licznikWartosciDzieci = 0;
    int liczbaDzieci = 0;
    if (argc < 2){
        printf("%s\n", "Zla liczba arg");
        int tmp = 0;
        while (tmp < argc -1){
            printf("%s\n", argv[tmp]);
            tmp = tmp - 1;
        }
        return -1;
    }


    char *sciezkaPliku;
    sciezkaPliku = argv[1];
    if(sciezkaPliku == NULL){
        sciezkaPliku = ".";
    }

    licznikP = licznikPlikow(sciezkaPliku, argc, argv, licznikP, licznikWartosciDzieci, liczbaDzieci);
    printf("liczba plikow to :%d\n", licznikP);
    return 0;
}