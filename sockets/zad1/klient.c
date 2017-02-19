#include "czat.h"

int soketID;
int typ;
char login[MAX_DLUGOSC_NAZWY], sciezka[MAX_DLUGOSC_NAZWY+ 2], sciezkaKlienta[MAX_DLUGOSC_NAZWY+ 2];
pthread_t watek;
struct wiadomosc w;
struct wiadomosc w2;
struct sockaddr *aKlient, *aSerwer;
socklen_t rozmiarStruktury;
struct pollfd fds[1];
struct pollfd fds2[1]; // do czytania stdin
int flagaWyjscia = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // do wyjscia z petli

void wyjscieZCzatu(){

	w.typWiadomosci = 3;
	strcpy(w.loginKlienta, login);
	sendto(soketID, &w, sizeof(struct wiadomosc), 0, aSerwer, rozmiarStruktury);
	flagaWyjscia = 1;
	close(fds[0].fd);
	close(fds2[0].fd);
	if (typ == LOKALNY){
		remove(sciezkaKlienta);
	}
}

void dodajKlienta(){
	printf("dodanie klienta do serwera\n");
	int liczbaWyslanych;
	memset(&w, 0, sizeof(struct wiadomosc));
	strcpy(w.loginKlienta, login);
	w.typWiadomosci = 1;

	liczbaWyslanych = sendto(soketID, &w, sizeof(struct wiadomosc), 0, aSerwer, rozmiarStruktury);
	if (liczbaWyslanych == -1){
		perror("sendto");
		printf("blad sendto w dodawaniu klienta\n");
	}
}

void* czytaj(void *argv){
	printf("rozpoczecie watku czytajacego\n");
	printf("wyjscie poprzez exit\n");
	int ret;
	fds2[0].fd = 0; // czekanie na stdin
	fds2[0].events = POLLIN | POLLPRI;
	while (!flagaWyjscia) {
		ret = poll(fds2, 1, -1);
		if (ret > 0){
			if (fds2[0].revents & ( POLLERR | POLLHUP | POLLNVAL )){
				perror("blad eventu czytajacego");
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
					przeczytane = sendto(soketID, &w, sizeof(struct wiadomosc), 0, aSerwer, rozmiarStruktury);
					if (przeczytane < 0){
						perror(" sendto blad wysylania wiadomosci");
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
	printf("rozpoczecie dzialania klienta \n");
	int ret;
	struct klient klient;
	int port;
	memset(&fds, 0, sizeof(fds));
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
		port = strtoul(argv[4], NULL, 0);
	} else {
		printf("sposob moze byc tylko local lub inet\n");
		return -1;
	}
	
	printf("zakoncz swoja wiadomosc poprzez wcisniecie enter\n");

	getdomainname(domena,MAX_DLUGOSC_NAZWY);
	gethostname(host, MAX_DLUGOSC_NAZWY);
	getcwd(tmpSc, sizeof(sciezkaKlienta));
	sprintf(login, "%s:%s%d", host, domena, atoi(argv[1]));
	strcpy(klient.login, login);
	sprintf(sciezkaKlienta, "%s/%s", tmpSc, login);

	if (typ == LOKALNY){
		printf("inicjalizacja lokalnych struktur\n");
		soketID = socket(AF_UNIX, SOCK_DGRAM, 0);

		// czesc dla serwera 
		struct sockaddr_un serwerAdres;
		memset(&serwerAdres, 0, sizeof(struct sockaddr_un));
		serwerAdres.sun_family = AF_UNIX;
		strcpy(serwerAdres.sun_path, sciezka);
		aSerwer = (struct sockaddr *) &serwerAdres;

		// czesc dla klienta
		struct sockaddr_un klientAdres;
		memset(&klientAdres, 0, sizeof(struct sockaddr_un));
		klientAdres.sun_family = AF_UNIX;
		strcpy(klientAdres.sun_path, sciezkaKlienta);
		aKlient = (struct sockaddr *) &klientAdres;
		rozmiarStruktury = sizeof(struct sockaddr_un);

	}else if (typ == INTERNETOWY){
		printf("inicjalizacja internetowych struktur\n");
		soketID = socket(AF_INET, SOCK_DGRAM, 0);
		
		// czesc dla serwera
		struct sockaddr_in serwerAdres;
		memset(&serwerAdres, 0, sizeof(struct sockaddr_in));
		serwerAdres.sin_family = AF_INET;
		serwerAdres.sin_port = htons(port);
		serwerAdres.sin_addr.s_addr = inet_addr(argv[3]);
		aSerwer = (struct sockaddr *) &serwerAdres;

		// czesc dla klienta
		struct sockaddr_in klientAdres;
		memset(&klientAdres, 0, sizeof(struct sockaddr_in));
		klientAdres.sin_family = AF_INET;
		klientAdres.sin_port = htons(1024 + rand() % (65535 - 1024));
		klientAdres.sin_addr.s_addr = htonl(INADDR_ANY);
		aKlient = (struct sockaddr *) &klientAdres;
		rozmiarStruktury = sizeof(struct sockaddr_in);
	}

	if(bind(soketID, aKlient, rozmiarStruktury) == -1) {
		perror("bind");
		if(typ == LOKALNY) {
			printf("%s\n", sciezkaKlienta);
		}
	}
	if(connect(soketID, aSerwer, rozmiarStruktury) == -1) {
		perror("connect");
	}

	fds[0].fd = soketID;
  	fds[0].events = POLLIN | POLLPRI;
	
	dodajKlienta();

	pthread_mutex_lock(&mutex);
	pthread_create(&watek, NULL, czytaj, argv);				
	pthread_mutex_unlock(&mutex);
	
	int liczba;
	while (!flagaWyjscia) {
		//printf("petla nasluchujaca\n");
		ret = poll(fds, 1, 30*1000);
		if (ret > 0) {
			printf("zarejestrowano dzialanie na sokecie\n");
			if (fds[0].revents & (POLLIN | POLLPRI)) {
        		liczba = recvfrom(soketID, &w2, sizeof(struct wiadomosc), 0, NULL, NULL);
        	}
        	if (liczba < 0) {
          		printf("blad: recvfrom");
          		break;
        	}
        	if (liczba > 0) {
          		printf("uzytkownik %s napisal: %s \n", w2.loginKlienta, w2.tresc);
        	}
        	ret = ret - 1;
		}
		else if (ret == 0){
			dodajKlienta(aSerwer, rozmiarStruktury, typ);
		}
	}

	pthread_join(watek, NULL);


	return 0;
}
