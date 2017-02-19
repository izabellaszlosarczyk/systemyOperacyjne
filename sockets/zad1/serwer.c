#include "czat.h"

struct klient klienci[MAX_LICZBA_KLIENTOW];
time_t tablicaCzasowPolaczenia[MAX_LICZBA_KLIENTOW];
char sciezka[MAX_ROZMIAR_WIADOMOSCI + 2];
struct pollfd fds[2];

void wylogujSerwer(){
	close(fds[0].fd);
	close(fds[1].fd);
	remove(sciezka);
}

void wyslijWiadomosc(int soket, struct wiadomosc w, int typ){
	printf("przygotowanie do wysylania wiadomosci\n");
	int i, rozmiarStruktury;
	struct sockaddr *soketWyslanieAd;
	printf("odebrano wiadomosc od : %s %s\n", w.loginKlienta, w.tresc);
	for (i = 0; i < MAX_LICZBA_KLIENTOW ; i = i + 1 ){
		//printf("%s %s<-\n", w.loginKlienta, klienci[i].login);
		if (strcmp(w.loginKlienta, klienci[i].login) != 0 && strlen(klienci[i].login) > 0){
			if (typ == LOKALNY){
				soketWyslanieAd = (struct sockaddr*)&(klienci[i].lokalny);
				rozmiarStruktury =  sizeof(struct sockaddr_un);
			}else if (typ == INTERNETOWY){
				soketWyslanieAd = (struct sockaddr*)&(klienci[i].internetowy);
				rozmiarStruktury =  sizeof(struct sockaddr_in);
			}
			if (sendto(soket, &w, sizeof(struct wiadomosc), 0, soketWyslanieAd, rozmiarStruktury) == -1) {
				perror("blad sendto - wyslanie wiadomosci");
				return;
			}
		}
	}
}
int porownanie(struct sockaddr_un *str1, struct sockaddr_un *str2){
	if ((str1->sun_path == str2->sun_path) && (str1->sun_family == str2->sun_family)){
		return 1;
	} 
	return 0;
}
int porownanie2(struct sockaddr_in *str1, struct sockaddr_in *str2){
	if ((str1->sin_family == str2->sin_family) && (str1->sin_addr.s_addr == str2->sin_addr.s_addr) && (str1->sin_port == str2->sin_port)){
		return 1;
	}
	return 0;
}

void wylogujKlienta(struct sockaddr_un *klientAdLokalny, struct sockaddr_in *klientAdInter){
	printf("wylogowywanie klienta\n");
	int i = 0;
	while(i < MAX_LICZBA_KLIENTOW){
		if ( porownanie(klientAdLokalny, (struct sockaddr_un*)&(klienci[i].lokalny)) || porownanie2(klientAdInter, (struct sockaddr_in*)&(klienci[i].internetowy) )){
			strcpy(klienci[i].login, "");
		}
	}
}

int dodajKlienta(struct wiadomosc w, int typ, struct sockaddr_un *addLokalny, struct sockaddr_in *addInter) {
	printf("dodaje klienta\n");
	int i = 0;
	int flagaCzyJuzJest = 0;
	time_t czas;
	// prawdzam czy klient juz nie zarejestrowal sie i chce tylko odswiezyc
	while ((i < MAX_LICZBA_KLIENTOW) && (strlen(klienci[i].login) > 1)){
		if (strcmp(klienci[i].login, w.loginKlienta) != 0){
			i++;
		}else {
			flagaCzyJuzJest = 1;
		}
	}
	if (flagaCzyJuzJest == 1){
		tablicaCzasowPolaczenia[i] = time(&czas);
	} 
	else {
		if (i < MAX_LICZBA_KLIENTOW) {
			strcpy(klienci[i].login, w.loginKlienta);
			tablicaCzasowPolaczenia[i] = time(&czas);
			if (typ == LOKALNY){
				memcpy(&(klienci[i].lokalny), addLokalny, sizeof(struct sockaddr_un));
			} else {
				memcpy(&(klienci[i].internetowy), addInter, sizeof(struct sockaddr_in));
			}
			printf("Dodano klienta");
			return 0;
		}
		else {
			printf("Przekroczono maksymalna liczbe klientow.");
			return -1;
		}		
	}
	return 0;
}

void uruchomSerwer(int lokalnyID, int internID, struct sockaddr_un *klientAdLokalny, struct sockaddr_in *klientAdInter){
	printf("serwer zostaje uruchomiony\n");
	struct wiadomosc w;
	socklen_t rozmiarStrukturyL = sizeof(struct sockaddr_un);
	socklen_t rozmiarStrukturyI = sizeof(struct sockaddr_in);
	fds[0].fd = lokalnyID;
  	fds[0].events = POLLIN | POLLPRI;
  	fds[1].fd = internID;
  	fds[1].events = POLLIN | POLLPRI;
  	int ret;
  	printf("wchodze do petli czytania\n");
	while(1){
		printf("jestem w while\n");
		ret = poll(fds, 2, 30*1000);
		if (ret > 0){
			printf("cos sie stalo\n");
			if (fds[0].revents & (POLLIN | POLLPRI)) {
				if (recvfrom(lokalnyID, (void *) &w, sizeof(struct wiadomosc), 0 , (struct sockaddr *) klientAdLokalny, &rozmiarStrukturyL) > 0) {
					if (w.typWiadomosci == 1){
						printf("odebrano prosbe o logowanie\n");
						if (dodajKlienta(w, LOKALNY, klientAdLokalny, NULL) == -1){
							break;
						}
					}
					if (w.typWiadomosci == 2){
						printf("odebrano wiadomosc\n");
						wyslijWiadomosc(lokalnyID, w, LOKALNY);
					}
					if (w.typWiadomosci == 3){
						wylogujKlienta(klientAdLokalny, NULL);
					}
				}
			}
			if (fds[1].revents & (POLLIN | POLLPRI)) {	
				if (recvfrom(internID, (void *) &w, sizeof(struct wiadomosc), 0, (struct sockaddr *) klientAdInter, &rozmiarStrukturyI) > 0){
					if (w.typWiadomosci == 1){
						if (dodajKlienta(w, INTERNETOWY, NULL, klientAdInter) == -1){
							break;
						}
					}
					if (w.typWiadomosci== 2){
						wyslijWiadomosc(internID, w, INTERNETOWY);
					}
					if (w.typWiadomosci == 3){
						wylogujKlienta(NULL, klientAdInter);
					}
				}
			}	
		}
	}
}


int main(int argc, char *argv[]){

	if (argc < 3){
		printf("program powinien zostac wywolany z argumentami : numer portu, sciezka\n");
		return -1;
	}
	printf("wyruchomienie serwera\n");
	struct sockaddr_un serwerAdLokalny, klientAdLokalny;
	struct sockaddr_in serwerAdInter, klientAdInter;
	int lokalnyID, internID;
	int port = atoi(argv[1]);
	strcpy(sciezka, argv[2]);

	memset(&serwerAdLokalny, 0, sizeof(struct sockaddr_un));
	memset(&serwerAdInter, 0, sizeof(struct sockaddr_in));

	
	lokalnyID = socket(AF_UNIX, SOCK_DGRAM, 0);
	serwerAdLokalny.sun_family = AF_UNIX;
	strcpy(serwerAdLokalny.sun_path, sciezka);
	if(bind(lokalnyID, (struct sockaddr *)&serwerAdLokalny, sizeof(struct sockaddr_un)) == -1) {
		perror("bind local");
		printf("%s\n", sciezka);
	}

	internID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	serwerAdInter.sin_family = AF_INET;
	serwerAdInter.sin_addr.s_addr = htonl(INADDR_ANY);
	serwerAdInter.sin_port = htons(port);
	if(bind(internID, (struct sockaddr *) &serwerAdInter, sizeof(struct sockaddr_in)) == -1) {
		perror("bind intern");	
	}

	int k;
	for (k = 0; k < MAX_LICZBA_KLIENTOW; k = k + 1){
		klienci[k].login[0] = (char)0;
	}

	printf("zainicjalizowano struktury serwera\n");
	uruchomSerwer(lokalnyID, internID, &klientAdLokalny, &klientAdInter);

	wylogujSerwer();

	return 0;
}