#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>
#include <sys/ioctl.h>


#define MAX_LICZBA_KLIENTOW 10
#define MAX_DLUGOSC_NAZWY 256
#define MAX_ROZMIAR_WIADOMOSCI 256
#define LOKALNY 1
#define INTERNETOWY 2

#define REJESTRACJA 1
#define ZWYKLA_WIAD 2
#define WYLOGOWYWANIE 3

struct klient{
	char login[MAX_DLUGOSC_NAZWY];
	int soket;
	pthread_t watek;
	struct sockaddr_un lokalny;
	struct sockaddr_in internetowy;
};

struct wiadomosc {
	char loginKlienta[MAX_DLUGOSC_NAZWY];
	int typWiadomosci;
	char tresc[MAX_ROZMIAR_WIADOMOSCI];
} ;
