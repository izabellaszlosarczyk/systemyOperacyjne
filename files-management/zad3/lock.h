#ifndef LOCK_H
#define LOCK_H

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>


void ustawRL(int deskryptorPliku, int przesuniecie);
void ustawWL(int deskryptorPliku, int przesuniecie);
void odblokuj(int deskryptorPliku, int przesuniecie);
int przeczytajB(int deskryptorPliku, int przesuniecie, char *c);
int napiszB(int deskryptorPliku, int przesuniecie, char *c);
int wyswietlL(int deskryptorPliku);

#endif