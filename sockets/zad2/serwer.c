#include "czat.h"

struct klient klienci[MAX_LICZBA_KLIENTOW];
time_t tablicaCzasowPolaczenia[MAX_LICZBA_KLIENTOW];
char *sciezka;
struct pollfd fds[MAX_LICZBA_KLIENTOW + 2];
int licznikKlientow = 0;

void sig_handler(int signo) {
	remove(sciezka);

	exit(EXIT_SUCCESS);
}

void zmniejszTablice(){
	int i, j;
	for (i = 0; i < licznikKlientow; i++)
      {
        if (fds[i].fd == -1)
        {
          for(j = i; j < licznikKlientow; j++)
          {
            fds[j].fd = fds[j+1].fd;
          }
          licznikKlientow--;
        }
      }
}

void wylogujKlienta(int i){
	printf(" wylogowywanie klienta \n");
	close(fds[i].fd);
	fds[i].fd = -1;
	zmniejszTablice();
}


void wyslijWiadomosc(int soket, struct wiadomosc w){
	int i, tmp;
	printf("wysylanie wiadomosci \n");
	for (i = 0; i < MAX_LICZBA_KLIENTOW ; i = i + 1 ){
		if (soket != klienci[i].soket){
			tmp = send(klienci[i].soket, &w, sizeof(struct wiadomosc), 0);
			if (tmp < 0) {
				wylogujKlienta(i);
				perror("blad sendto\n");
				return;
			}
		}
	}
}

int dodajKlienta(int soketID, struct wiadomosc w) {
	printf("serwer dodaje klienta\n");
	int i = 0, flagaCzyJuzJest = 0, soketKlienta = -1;
	time_t czas;
	while ((i < MAX_LICZBA_KLIENTOW) && (strlen(klienci[i].login) > 1)){
		if (strcmp(klienci[i].login, w.loginKlienta) != 0){
			i++;
		}else {
			flagaCzyJuzJest = 1;
		}
	}
	if (flagaCzyJuzJest == 1){
		tablicaCzasowPolaczenia[i] = time(&czas);
		return klienci[i].soket;
	} 
	else {
		if (i < MAX_LICZBA_KLIENTOW) {
			strcpy(klienci[i].login, w.loginKlienta);
			tablicaCzasowPolaczenia[i] = time(&czas);
			soketKlienta = accept(soketID, NULL, NULL);
			if (soketKlienta < 0){
				printf("blad accept\n");
			}
			fds[licznikKlientow].fd = soketKlienta;
			fds[licznikKlientow].revents = POLLIN;
			klienci[i].soket = soketKlienta;
			printf("Dodano klienta");
			licznikKlientow = licznikKlientow + 1;
			return soketKlienta;
		}
		else {
			printf("Przekroczono maksymalna liczbe klientow.");
		}		
	}
	return soketKlienta;
}

void uruchomSerwer(int lokalnyID, int internID){
	printf("uruchom serwer \n");
	struct wiadomosc w;
	int klientSoket = 0;

	listen(lokalnyID, MAX_LICZBA_KLIENTOW);
	listen(internID, MAX_LICZBA_KLIENTOW);
	memset(fds, 0 , sizeof(fds));

	fds[0].fd = lokalnyID;
  	fds[0].revents = POLLIN;
  	fds[1].fd = internID;
  	fds[1].revents = POLLIN;
  	int ret, i;
  	printf("petla ktora oczekuje na wiadomosci \n");
	while(1){								// w milisekundach
		ret = poll(fds, MAX_LICZBA_KLIENTOW, 30* 1000);

		if (ret > 0){
			for (i = 2; i < licznikKlientow; i = i + 1){
				if (fds[i].revents == 0){
					continue;
				}
				else if (fds[i].revents & (POLLIN | POLLPRI)){
					if (fds[i].fd == fds[0].fd){
						printf("deskryptor lokalny nasluchujacy czytelny\n");
						while (klientSoket > 0){
							if (recv(lokalnyID, (void *) &w, sizeof(struct wiadomosc), 0) > 0) {
								if (w.typWiadomosci == 1){
									if ( (klientSoket = dodajKlienta(lokalnyID, w)) < 0){
										if (errno !=  EWOULDBLOCK){
											printf("blad funckji accept");
										}
										break;
									}
								}	
							}
						}	
					}
					else if (fds[i].fd == fds[1].fd){
						printf("deskryptor inten nasluchujacy czytelny\n");
						while (klientSoket > 0){
							if (recv(internID, (void *) &w, sizeof(struct wiadomosc), 0) > 0){
								if (w.typWiadomosci == 1){
									if ( (klientSoket = dodajKlienta(internID, w)) == -1){
										break;
									}
								}	
							}
						}		
					}
					else {
						printf("inny deskryptor gotowy do czytnaia\n");
						int liczP = 0;
						do {
							liczP = recv(fds[i].fd, (void *) &w, sizeof(struct wiadomosc), 0);
							if ( liczP < 0){
								break;
							}
							if (liczP == 0){
								printf("zamknieto polaczenie\n");
								break;
							}
							wyslijWiadomosc(fds[i].fd, w);
						}while(1);
					}	
				}
			}
		}
	}
	for (i = 0; i < licznikKlientow; i++){
    	if(fds[i].fd >= 0)
      		close(fds[i].fd);
 	}
}


int main(int argc, char *argv[]){
	printf("----------------------------------------------------------\n");
	printf("rozpoczynam prace serwera\n");
	licznikKlientow = 0;
	if (argc < 3){
		printf("program powinien zostac wywolany z argumentami : numer portu, sciezka\n");
		return -1;
	}
	printf(" inicjalizowanie struktur \n");
	struct sockaddr_un serwerAdLokalny;
	struct sockaddr_in serwerAdInter;
	int lokalnyID, internID;
	int port = atoi(argv[1]), on1 = 1, on2 = 1;
	sciezka = argv[2];

	signal(SIGINT, sig_handler);
	
	remove(sciezka);
	
	lokalnyID = socket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP);
	internID = socket(AF_INET,  SOCK_STREAM, IPPROTO_TCP);
	setsockopt(lokalnyID, SOL_SOCKET,  SO_REUSEADDR, (char *)&on1, sizeof(on1));
	ioctl(lokalnyID, FIONBIO, (char *)&on1);
	setsockopt(internID, SOL_SOCKET,  SO_REUSEADDR, (char *)&on2, sizeof(on2));
	ioctl(internID, FIONBIO, (char *)&on2);

	memset(&serwerAdLokalny, 0, sizeof(struct sockaddr_un));
	memset(&serwerAdInter, 0, sizeof(struct sockaddr_in));

	serwerAdLokalny.sun_family = AF_UNIX;
	strcpy(serwerAdLokalny.sun_path, sciezka);
	
	serwerAdInter.sin_family = AF_INET;
	serwerAdInter.sin_addr.s_addr = htonl(INADDR_ANY);
	serwerAdInter.sin_port = htons(port);
	
	bind(lokalnyID, (struct sockaddr *) &serwerAdLokalny, sizeof(struct sockaddr_un));
	bind(internID, (struct sockaddr *) &serwerAdInter, sizeof(struct sockaddr_in));
	printf("struktury zainicjalizowane\n");
	
	uruchomSerwer(lokalnyID, internID);

	return 0;
}