#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char *prawoDostepu;

void strmode(mode_t prawa, char* wynik) {
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

int licznikPlikow(const char *sciezka, char *prawo, int licznikP){
	/*struct dirent - struktura wskazująca na element w katalogu (plik/folder) zawierająca
	 ino_t d_ino - numer seryjny pliku
  	char d_name[] - nazwa wejściowa
	*/
	struct dirent *katalog_element;
	struct stat statystyki;
	DIR *katalog;
	char calaSciezka[1024];
	if((katalog = opendir(sciezka)) == NULL){
		fprintf(stderr,"Nie mozna otworzyc katalogu %s\n", sciezka);
		exit(errno);
	}
	while ((katalog_element = readdir(katalog)) != NULL){
		if(strcmp(katalog_element->d_name, ".") != 0 && strcmp(katalog_element->d_name, "..") != 0){
			strcpy(calaSciezka, sciezka);
			strcat(calaSciezka, "/");
			strcat(calaSciezka, katalog_element->d_name);
			stat(calaSciezka, &statystyki); // 
		 // czy jest plikiem
			char *prawoPliku = (char*)malloc(sizeof(char)*11);
			strmode(statystyki.st_mode, prawoPliku);
			if(S_ISREG(statystyki.st_mode) && strcmp(prawoPliku, prawo)== 0){
				licznikP = licznikP + 1;
				printf("%s\n", katalog_element->d_name); }
		}
		else if (S_ISDIR(statystyki.st_mode)){
				licznikP = licznikPlikow(calaSciezka, prawo,licznikP);
		}
	}
	closedir(katalog);
	return licznikP;
}



int main(int argc, char ** argv){
	if(argc != 3){
		fprintf(stderr, "Niewlasciwa liczba argumentow.\n");
		exit(1);
	}
	prawoDostepu = argv[2];
	struct stat statystyki;
	if(stat(argv[1], &statystyki) == -1){
		fprintf(stderr, "Nie udalo sie wykonac operacji stat.\n");
		exit(errno);
	}
	char sciezka_absolutna[PATH_MAX+1], *s_ab;
	if((s_ab = realpath(argv[1], sciezka_absolutna)) == NULL){
		fprintf(stderr, "Nie udalo sie uzyskac bezwzglednej sciezki.\n");
		exit(errno);
	}
	int licznikP;
	licznikP = 0;
	licznikP = licznikPlikow(sciezka_absolutna, prawoDostepu, licznikP);
	printf("Laczna liczba plikow spelniajacych kryteria: %d.\n", licznikP);
	return 0;
}