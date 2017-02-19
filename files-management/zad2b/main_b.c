#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

char *prawoDostepu;
int licznikP = 0;

void strmode(mode_t prawa, char* wynik) {
	//wynik = (char*)malloc(sizeof(char)*11);
	int *tmp = (int*)malloc(sizeof(int)*11);
    tmp[0] = (S_ISDIR(prawa)) ? 'd' : '-';
    tmp[1] = (prawa & S_IRUSR) ? 'r' : '-';
    tmp[2] =(prawa & S_IWUSR) ? 'w' : '-';
    tmp[3] =(prawa & S_IXUSR) ? 'x' : '-';
    tmp[4] =(prawa & S_IRGRP) ? 'r' : '-';
    tmp[5] =(prawa & S_IWGRP) ? 'w' : '-';
    tmp[6] =(prawa & S_IXGRP) ? 'x' : '-';
    tmp[7] =(prawa & S_IROTH) ? 'r' : '-';
    tmp[8] =(prawa & S_IWOTH) ? 'w' : '-';
    tmp[9] =(prawa & S_IXOTH) ? 'x' : '-';
	int i;
	for (i = 0; i< 11; i = i+1){
		wynik[i] = (char)tmp[i];
	}
    wynik[10] = '\0';
}

int szukajPlikow(const char *sciezka, const struct stat *statystyka, int typ){
	char *prawoPliku = (char*)malloc(sizeof(char)*11);
	strmode(statystyka->st_mode, prawoPliku);
	printf("[szukajPlikow]: %s %s\n", prawoPliku, prawoDostepu);
	if (typ == FTW_F && strcmp(prawoPliku, prawoDostepu) == 0){
		//printf("sp: %s %s\n", sciezka, prawoPliku);
		printf("prawo: %s\n", sciezka);
		licznikP = licznikP + 1;
		// trzeba wypisywac bezw
	}
	return 0;
} 

int licznikPlikow(const char *sciezka, char *prawoDostepu){
	/*struct dirent - struktura wskazująca na element w katalogu (plik/folder) zawierająca
	 ino_t d_ino - numer seryjny pliku
  	char d_name[] - nazwa wejściowa
	*/ 

	if (ftw(sciezka, szukajPlikow , 1024) == -1 ){
		perror("Blad w funkcji ftw.");
		exit(errno);
	}
	return licznikP;
}

int main(int argc, char ** argv){
	if(argc != 3){
		fprintf(stderr, "Niewlasciwa liczba argumentow.\n");
		exit(1);
	}
	prawoDostepu = argv[2];
	printf("prawo dostepu :%s\n", prawoDostepu);
	//char *p;
    //prawoDostepu = (int)strtoul(argv[2], &p, 8);
    //printf("%d\n", prawoDostepu);

	char sciezka_absolutna[PATH_MAX+1], *s_ab;
    if((s_ab = realpath(argv[1], sciezka_absolutna)) == NULL){
        fprintf(stderr, "Nie udalo sie uzyskac bezwzglednej sciezki.\n");
		exit(errno);
    }
    licznikPlikow(sciezka_absolutna, prawoDostepu);
	printf("Laczna liczba plikow spelniajacych kryteria: %d.\n", licznikP);
	return 0;
}