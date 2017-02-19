#include "glowny.h"


char *generujCzas(){
    time_t czas = time(NULL);
    ctime(&czas);
    return ctime(&czas);
}


