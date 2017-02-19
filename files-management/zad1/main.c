#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// ----------------------------------struktury ---------------------------------------------
int rozmiarStruktury;
int ileStruktur;
int typ;
FILE *uchwytF = NULL;
int uchwytS = -1;

struct rekord {
	int numer;
	char *dane;
};
// --------------------------------funkcje pomocnicze ------------------------------------
struct rekord* initRekord(int rozmiarStruktury){
    struct rekord* nowyRekord = malloc(sizeof(struct rekord));
    nowyRekord->dane = malloc(sizeof(char)*rozmiarStruktury + 1);
    return nowyRekord;
}

struct rekord* wypelnijRekord(int rozmiarStruktury, int licznikStruktur){
    struct rekord* rekordNowy = initRekord(rozmiarStruktury);
    rekordNowy->numer = licznikStruktur;
    int i = rozmiarStruktury-1;
    while (i != 0){
        rekordNowy->dane[i] = (char)(((int)'0')+rand()%77); //77 - od 48 kodu ascii do 125
        i = i - 1;
    }
    return rekordNowy;
}

char* konwertujNaString(int a) {
    char *str = (char*)malloc(sizeof(char)*10);
    sprintf(str, "%d", a);
    return str;
}

int konwertujNaInt(char* str) {
    int a;
    sscanf(str, "%d", &a);
    return a;
}

void wyswietlStrukture(struct rekord* struktura) {
    printf("%p\n", &(struktura->numer));
   // printf("%s\n", struktura->dane);
}

// ----------------------------------funkcje do dzialania na pliku ------------------------------
void mojClose(int wyborUzytkownika){
	if (wyborUzytkownika == 0){
		fclose(uchwytF);
	}
	else if (wyborUzytkownika == 1){
		close(uchwytS);
	}
}
// rozmiar tego ktorym operujemy - zapisujemy /czytamy 
void mojWrite(void* bufor, size_t rozmiarOperujacy, size_t licznik, int wyborUzytkownika){
    if (wyborUzytkownika == 0){
        fwrite(bufor, rozmiarOperujacy, licznik, uchwytF); }
    else if (wyborUzytkownika == 1){
        write(uchwytS, bufor, rozmiarOperujacy*licznik);}
}

void mojRead(void* bufor, size_t rozmiarOperujacy, size_t licznik, int wyborUzytkownika){
	if (wyborUzytkownika == 0){
        if (fread(bufor, rozmiarOperujacy, licznik, uchwytF) == -1){
            perror("File Read Error");
            exit(1);
        }
    }
    else if (wyborUzytkownika == 1){
        if (read(uchwytS, bufor, rozmiarOperujacy*licznik) == -1 ){
            perror("File Read Error");
            exit(1);
        }
    }

}

void mojOpenWrite(char *nazwaPliku, int wyborUzytkownika){
	if (wyborUzytkownika == 0){
		uchwytF = fopen(nazwaPliku, "w");
	}
	else if (wyborUzytkownika == 1){
		uchwytS = open(nazwaPliku, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
	} 
	if (uchwytS < 0 && uchwytF == NULL) {
      	fprintf(stderr, "Value of errno: %d\n", errno);
      	if (uchwytF == NULL) {
            perror("Problem z funkcja biblioteczna");
        } else {
            perror("Problem z funkcja systemowa");
        }
	}
}

void mojOpenReadWrite(char *nazwaPliku, int wyborUzytkownika) {
    if (wyborUzytkownika == 0){
        uchwytF = fopen(nazwaPliku, "r+");
    }
    else if (wyborUzytkownika == 1){
        uchwytS = open(nazwaPliku, O_RDWR,  S_IRUSR | S_IWUSR | S_IXUSR );
    }
    if (uchwytS < 0 && uchwytF == NULL) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        if (uchwytF == NULL) {
            perror("Problem z funkcja biblioteczna");
        } else {
            perror("Problem z funkcja systemowa");
        }
    }
}

void generujPlik(int rozmiarStruktury, int ileStruktur, char* nazwaPliku, int wyborUzytkownika){
    int i = 0;
    struct rekord *tmp;
    while (i != ileStruktur - 1){
        tmp = wypelnijRekord(rozmiarStruktury, i);
        wyswietlStrukture(tmp);
        mojWrite(tmp->dane, sizeof(char), (size_t)rozmiarStruktury, wyborUzytkownika);
        i = i + 1;
    }
}


void czytanieDoStruktur(char *nazwaPliku, int wyborUzytkownika){
    int i = 0;
    while (i != ileStruktur - 1){
    	struct rekord *nowy = initRekord(rozmiarStruktury);
    	mojRead(nowy->dane, sizeof(char), (size_t)rozmiarStruktury, wyborUzytkownika);
        nowy->numer = i;
        wyswietlStrukture(nowy);
    	i = i + 1;
    }
}

void ustawPrzesuniecie(int numer, int wyborUzytkownika, int przesuniecie) {
    if (wyborUzytkownika == 1) {
        lseek(uchwytS, numer * (rozmiarStruktury*sizeof(char)), przesuniecie);
    }
    else if (wyborUzytkownika == 0) {
        fseek(uchwytF, numer * (rozmiarStruktury*sizeof(char)), przesuniecie);
    }
}

void sort(char *nazwaPliku, int wyborUzytkownika) {
    if (wyborUzytkownika == 1) {
        lseek(uchwytS, 0, SEEK_SET);
    }
    else if (wyborUzytkownika == 0) {
        fseek(uchwytF, 0, SEEK_SET);
    }

    struct rekord *s1, *s2;
    s1 = (struct rekord *) malloc(sizeof(int) + rozmiarStruktury);
    s2 = (struct rekord *) malloc(sizeof(int) + rozmiarStruktury);
    s1->dane = (char *) malloc(sizeof(char) * (rozmiarStruktury+ 1));
    s2->dane = (char *) malloc(sizeof(char) * (rozmiarStruktury+ 1));

    int n = ileStruktur;
    do {
        int i;
        for(i = 0; i < n-1; i++) {
            ustawPrzesuniecie(i, wyborUzytkownika, SEEK_SET);
            mojRead(s1->dane, sizeof(char) * rozmiarStruktury, (size_t) rozmiarStruktury, wyborUzytkownika);
            s1->numer = i;
            ustawPrzesuniecie(i+1, wyborUzytkownika, SEEK_SET);
            mojRead(s2->dane, sizeof(char) * rozmiarStruktury, (size_t) rozmiarStruktury, wyborUzytkownika);
            s2->numer = i;

            if (strcmp(s1->dane,s2->dane) > 0 ){

                ustawPrzesuniecie(-2, wyborUzytkownika, SEEK_CUR);
                mojWrite(s2->dane, sizeof(char) * rozmiarStruktury, (size_t) rozmiarStruktury, wyborUzytkownika);
                mojWrite(s1->dane, sizeof(char) * rozmiarStruktury, (size_t) rozmiarStruktury, wyborUzytkownika);
            }
        }
        ustawPrzesuniecie(-1,wyborUzytkownika,SEEK_CUR);
        n--;
    } while(n > 1);
    free(s1);
    free(s2);
}


int main(){
    srand( (unsigned int) time(NULL) );
	rozmiarStruktury = 9000;
    ileStruktur = 20;
    typ = 1;
    mojOpenWrite("tmp.txt", typ);
	generujPlik(rozmiarStruktury, ileStruktur, "tmp.txt", typ);
    mojClose(typ);

    printf("-------------------------------\n");

    mojOpenReadWrite("tmp.txt", typ);
    //sort("tmp.txt", typ);
    mojClose(typ);

    mojOpenReadWrite("tmp.txt", typ);
    czytanieDoStruktur("tmp.txt", typ);
    mojClose(typ);
    return 0;
}