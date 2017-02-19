#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

/*Za pomocą funkcji popen napisz program, który realizuje polecenie
ls -l | grep ^d > folders.txt
 */


int main(int argc, char *argv[]){
    int rozmiarL = 0;
    char linia[BUFFER_SIZE];
    char *wyjscie1 = 0;

    //open files
    FILE *f = fopen("folders.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }


    if (argc != 1){
        perror("Program nie przyjmuje arg.");
        return 1;
    }
    FILE* tmpW = popen("ls -l", "r");
    if(tmpW == NULL){
        perror("Blad funkcji popen");
        return 1;
    }
    while(fgets(linia, BUFFER_SIZE, tmpW)){
        if(linia[0] == 'd') {
            rozmiarL += strlen(linia);
            wyjscie1 = realloc(wyjscie1, rozmiarL);
            strcat(wyjscie1, linia);
        }
    }

    fwrite(wyjscie1, sizeof(char), rozmiarL, f);
    fclose(f);
    pclose(tmpW);
    return 0;
}
