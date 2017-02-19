#include "czat.h"

int soketID;
int typ;
char login[MAX_DLUGOSC_NAZWY], sciezka[MAX_DLUGOSC_NAZWY+ 2], sciezkaKlienta[MAX_DLUGOSC_NAZWY+ 2];
pthread_t watek;
struct sockaddr *aKlient, *aSerwer;
struct pollfd fds[1];
struct pollfd fds2[1];
int flagaWyjscia = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct wiadomosc w;


void wyjscieZCzatu(){

	w.typWiadomosci = 3;
	strcpy(w.loginKlienta, login);
	send(soketID, &w, sizeof(struct wiadomosc), 0);
	flagaWyjscia = 1;
	close(fds[0].fd);
	close(fds2[0].fd);
	if (typ == LOKALNY){
		remove(sciezkaKlienta);
	}
}

void sig_handler(int signo) {
	remove(sciezka);

	exit(EXIT_SUCCESS);
}


void dodajKlienta(){
	printf("dodaje klienta\n");
	struct wiadomosc w1;

	memset(&w1, 0, sizeof(struct wiadomosc));
	strcpy(w1.loginKlienta, login);
	w1.typWiadomosci = 1;
	if (send(soketID, &w1, sizeof(struct wiadomosc), 0) != sizeof(struct wiadomosc)){
		perror("blad send w dodawaniu klienta\n");
	}
}

void* czytaj(void *argv){
	printf("rozpoczecie watku czytajacego\n");
	printf("wyjscie poprzez exit\n");
	int ret;
	struct wiadomosc w;
	fds2[0].fd = 0; // czekanie na stdin
	fds2[0].events = POLLIN | POLLPRI;
	while (!flagaWyjscia) {
		ret = poll(fds2, 1, -1);
		if (ret > 0){
			if (fds2[0].revents && ( POLLERR | POLLHUP | POLLNVAL )){
				perror("blad");
				exit(0);
			}
			if (fds2[0].revents & (POLLIN | POLLPRI)){
				int przeczytane;
				przeczytane = read(0, w.tresc, MAX_ROZMIAR_WIADOMOSCI);
				if (przeczytane < 0){
					perror("read blad");
					exit(0);
				}
				else if (strncmp(w.tresc, "exit", 4)== 0){
					pthread_mutex_lock(&mutex);
					flagaWyjscia =1;
					pthread_mutex_unlock(&mutex);
					exit(0);
				}
				else {
					printf("wiadomosc do wyslania:%s\n", w.tresc);
					w.typWiadomosci = 2;
					strcpy(w.loginKlienta, login);
					przeczytane = send(soketID, &w, sizeof(struct wiadomosc), 0);
					if (przeczytane < 0){
						perror(" send blad");
						exit(0);
					}
				}
				
			}	
		}
	}
	wyjscieZCzatu();
	return 0;
}
int main(int argc, char *argv[]) {
	printf("----------------------------------------------------------\n");
	printf("rozpoczynam prace klienta\n");
	socklen_t rozmiarStruktury;
	struct pollfd fds[1];
	int ret;
	struct klient klient;
	int port = 0;
	struct wiadomosc w2;
	char domena[MAX_DLUGOSC_NAZWY], host[MAX_DLUGOSC_NAZWY];
	char tmpSc[MAX_ROZMIAR_WIADOMOSCI];

	if (strcmp("-local", argv[2]) && (argc < 5)) {
		printf("id uzytkownika, typ serwera -local | -inet , dla local - sciezka do soketu serwera \n");
		return -1;
	}

	if (strcmp("-inet", argv[2]) && (argc < 4)) {
		printf("id uzytkownika, typ serwera -local | -inet , dla intet - adres IP , port \n");
		return -1;
	}

	if (!strcmp("-local", argv[2])) {
		typ = LOKALNY;
		strcpy(sciezka, argv[3]);
	} 
	else if (!strcmp("-inet", argv[2])) {
		typ = INTERNETOWY;
		port = atoi(argv[4]);
	} else {
		printf("sposob moze byc tylko local lub inet\n");
		return -1;
	}
	printf("wejscie zainicjalizowane, przechodze dalej\n");

	signal(SIGINT, sig_handler);
	printf("zakoncz swoja wiadomosc poprzez wcisniecie enter\n");

	getdomainname(domena,MAX_DLUGOSC_NAZWY);
	gethostname(host, MAX_DLUGOSC_NAZWY);
	getcwd(tmpSc, sizeof(sciezkaKlienta));
	sprintf(login, "%s:%s%d", host, domena, atoi(argv[1]));
	strcpy(klient.login, login);
	sprintf(sciezkaKlienta, "%s/%s", tmpSc, login);

	printf("pobrane odpowiednie struktury\n");
	if (typ == LOKALNY){

		printf("inicjalizuje dzialanie dla trybu lokalnego\n");
		if ((soketID = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
			printf("blad socket lokalny\n");
		}

		// czesc dla serwera 
		struct sockaddr_un serwerAdres;
		memset(&serwerAdres, 0, sizeof(struct sockaddr_un));
		serwerAdres.sun_family = AF_UNIX;
		strcpy(serwerAdres.sun_path, sciezka);
		aSerwer = (struct sockaddr *) &serwerAdres;
		rozmiarStruktury = sizeof(struct sockaddr_un);

	}else if (typ == INTERNETOWY){

		printf("inicjalizuje dzialanie dla trybu internetowego\n");
		if ((soketID = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			printf("blad socket internetowy\n");
		}
		
		// czesc dla serwera
		struct sockaddr_in serwerAdres;
		memset(&serwerAdres, 0, sizeof(struct sockaddr_in));
		serwerAdres.sin_family = AF_INET;
		serwerAdres.sin_port = htons(port);
		serwerAdres.sin_addr.s_addr = inet_addr(argv[3]);
		aSerwer = (struct sockaddr *) &serwerAdres;
		rozmiarStruktury = sizeof(struct sockaddr_in);
	}

	if (connect(soketID, aSerwer, rozmiarStruktury) == -1){
		perror("blad connect");
		return -1;
	}

	fds[0].fd = soketID;
  	fds[0].events = POLLIN | POLLPRI;
	
	dodajKlienta();
	pthread_create(&watek, NULL, czytaj, argv);

	int liczba;
	printf("czekamy na rozne \n");
	while (!flagaWyjscia) {
		ret = poll(fds, 1, 30 * 1000);
		if (ret > 0) {
			if (fds[0].revents & (POLLIN | POLLPRI)) {
        		liczba = recv(soketID, &w2, sizeof(struct wiadomosc), 0);
        	}
        	if (liczba < 0) {
          		printf("blad: recvfrom");
          		break;
        	}
        	if (liczba > 0) {
          		printf("uzytkownik %s napisal: %s \n", w2.loginKlienta, w2.tresc);
        	}
		}
		if (ret == 0){
			dodajKlienta(aSerwer, rozmiarStruktury, typ);
		}
	}

	pthread_join(watek, NULL);

	return 0;
}
