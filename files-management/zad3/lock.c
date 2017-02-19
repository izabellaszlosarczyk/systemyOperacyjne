#include <lock.h>

// set_lk ustawia badz czysci locka
void ustawRL(int deskryptorPliku, int przesuniecie){
	struct flock mojLock;
	mojLock.l_type = F_RDLCK;
	mojLock.l_whence = SEEK_SET;
	mojLock.l_start = przesuniecie;
	mojLock.l_len = 1;
	mojLock.l_pid = getpid();
	if(fcntl(deskryptorPliku, F_SETLK, &mojLock) == -1){
        perror("ustawienie locka, czytanie:");
        exit(-1);
	}
}

void ustawWL(int deskryptorPliku, int przesuniecie){
	struct flock mojLock;
	mojLock.l_type = F_WRLCK;
	mojLock.l_whence = SEEK_SET;
	mojLock.l_start = przesuniecie;
	mojLock.l_len = 1;
	mojLock.l_pid = getpid();
	if(fcntl(deskryptorPliku, F_SETLK, &mojLock) == -1){
        perror("ustawienie locka, pisanie:");
        exit(-1);}
}

void odblokuj(int deskryptorPliku, int przesuniecie){
	struct flock mojLock;
	mojLock.l_type = F_UNLCK;
	mojLock.l_whence = SEEK_SET;
	mojLock.l_start = przesuniecie;
	mojLock.l_len = 1;
	mojLock.l_pid = getpid();

	if(fcntl(deskryptorPliku, F_SETLK, &mojLock) == -1){
        perror("odblokowanie locka:");
        exit(-1);}
}


int przeczytajB(int deskryptorPliku, int przesuniecie, char *c){
	if(lseek(deskryptorPliku, przesuniecie, SEEK_SET)==-1) return -1;

    struct flock mojLock;
	mojLock.l_type = F_RDLCK;
	mojLock.l_whence = SEEK_SET;
	mojLock.l_start = przesuniecie;
	mojLock.l_len = 0;

	if(fcntl(deskryptorPliku, F_GETLK, &mojLock) == -1) {
		perror("odczytywanie locka");
		return -1;
	}
	if(mojLock.l_type == F_RDLCK) {
		printf("Ustawiony read lock");
		return -1;
	}
	if(read(deskryptorPliku, c, sizeof(char)) == -1) {
		perror("błąd read");
		return -1;
	}
	
	return 0;
}

int napiszB(int deskryptorPliku, int przesuniecie, char *c){
	if(lseek(deskryptorPliku, przesuniecie, SEEK_SET)==-1) {
		printf("Nie znaleziono znaku\n");
		return -1;
	}

    struct flock mojLock;
	mojLock.l_type = F_WRLCK;
	mojLock.l_whence = SEEK_SET;
	mojLock.l_start = przesuniecie;
	mojLock.l_len = 0;

	if(fcntl(deskryptorPliku, F_GETLK, &mojLock) == -1) {
		perror("Podczas F_GETLK");
		return -1; // sprawdzamy czy takie blokady sa, jesli nie to zostanie struktura nadpisana
	}
	if(mojLock.l_type == F_WRLCK){
		printf("Jest lock typu F_WRLCK\n");
		return -1;
	}

	if(write(deskryptorPliku, c, sizeof(char)) == -1) {
		perror("Podczas zapisu");
		return -1;
	}
	return 0;
}

int wyswietlL(int deskryptorPliku){
	int i, n;
    n = lseek(deskryptorPliku, 0, SEEK_END);
    struct flock mojLock;
    int licznikLockow = 0;

    for (i = 0; i < n; i++) {
    	mojLock.l_type = F_WRLCK;
    	mojLock.l_whence = SEEK_SET;
    	mojLock.l_start = i;
        mojLock.l_len = 1;
        if(fcntl(deskryptorPliku, F_GETLK, &mojLock) == -1){
			return -1;
		}
        if (mojLock.l_type == F_WRLCK){
            printf("blokada zapisu - pozycja: %d,\tpid: %ld\n", i, (long)mojLock.l_pid);
            ++licznikLockow;
        }
        if (mojLock.l_type == F_RDLCK){
            printf("blokada odczytu - pozycja: %d,\tpid: %ld\n", i, (long)mojLock.l_pid);
            ++licznikLockow;
        }
    }
    if(licznikLockow == 0){
        printf("Nie ma blokad .\n");
    }
    return 0;
}
