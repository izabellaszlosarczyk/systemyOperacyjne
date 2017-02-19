#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include "lock.h"

static int uchwytDoPliku;

void menu(){
    printf("\n\n1. Ustaw read lock\n");
    printf("2. Ustaw write lock\n");
    printf("3. Wyświetl rygle\n");
    printf("4. Zwolnij rygiel\n");
    printf("5. Czytaj znak\n");
    printf("6. Zapisz znak\n");
    printf("7. Koniec\n\n");
}
char znak(){
    char c;

   	printf("Znak: ");
    scanf(" %c", &c);

    return c;
}

int pozycja(){
	int pozycja;

    printf("Pozycja znaku: ");
    scanf("%d", &pozycja);

    return pozycja;
}

void loop(){
    int type;
    int pozycjaTmp;
    char c;

    while(type != 7){
        menu();
        scanf("%d", &type);
        printf("\n\n");

        switch(type) {
            case 1:
                pozycjaTmp = pozycja();
                ustawRL(uchwytDoPliku, pozycjaTmp);
                break;
            case 2:
                pozycjaTmp = pozycja();
                ustawWL(uchwytDoPliku, pozycjaTmp);
                break;
            case 3:
                wyswietlL(uchwytDoPliku);
                break;
            case 4:
                pozycjaTmp = pozycja();
                odblokuj(uchwytDoPliku, pozycjaTmp);
                break;
            case 5:
                pozycjaTmp = pozycja();
                przeczytajB(uchwytDoPliku, pozycjaTmp, &c);
                printf("Znak: %c\n", c);
                break;
            case 6:
                c = znak();
                pozycjaTmp = pozycja();
                napiszB(uchwytDoPliku, pozycjaTmp, &c);
                break;
        }
    }
}

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Użycie: program_name filename\n");
        return -1;
    }

    uchwytDoPliku = open(argv[1], O_RDWR);

    if(uchwytDoPliku == 0){
        perror(argv[1]);
    }

    loop();

    close(uchwytDoPliku);
    return 0;
}