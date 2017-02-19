#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

//biblioteki używane do pomiaru czasu
#include <time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>

// do bibliotek
#ifdef DYNAMICLIB
#include <dlfcn.h>
#endif

struct list *(*initL)();
struct node *(*create_nodeL)(char *n, char *s, char *b, char *e, long int c, char *a);
void (*insert_at_the_endL)(struct list *list, struct node *tmp);
void (*insert_at_the_beginningL)(struct list *list, struct node *tmp);
void (*remove_from_listL)(struct list *list, struct node *rmv);
struct node *(*mergesortL)(struct node* head);
void (*show_listL)(struct list* list);
void (*show_list_reverseL)(struct list* list);
void (*set_new_listL)(struct list* lista, struct node* head);
void (*show_node2L)(struct node *tmp);
struct node *(*finding_nodeL)(struct list *list, char *name, char *surname);

 //elementy potrzebne do pomiaru czasu
clock_t checkpoints[4];
clock_t start, end;
struct tms myTimes[4];
/* struktura tms- zwraca - zwraca inf o procesie: 
	utime- od wlaczenia programu mojego ile zuzyl
	stime- ile zuzyl na wyowalnie procesow moich
	cstime i cutime 
*/
void checkpoint(int number){
	double ticks = CLOCKS_PER_SEC; // liczba taktow zegara na sek
	checkpoints[number] =  times(&myTimes[number]); // wypelnia myTimes i na tej podstawie checkpoints
	printf("numer chcekpointa: %d\n", number);// "%ju", (uintmax_t)(clock_t)
	printf("real time: %lf\n\n", (uintmax_t)(clock_t)(checkpoints[number]-start)/ticks);
	printf("user time: %lf\n\n", (double)(myTimes[number].tms_stime)/ticks);
    printf("system time: %lf\n", (double)(myTimes[number].tms_utime)/ticks);
    printf("ticks : %lf\n", ticks);
    if(number >= 1){
        printf("Od pierwszego punktu kontrolnego:\n");
        printf("real time: %lf\n", (double)(checkpoints[number]-checkpoints[0])/ticks);
        printf("user time: %lf\n\n", (double)(myTimes[number].tms_utime-myTimes[0].tms_utime)/ticks);
        printf("system time: %lf\n", (double)(myTimes[number].tms_stime-myTimes[0].tms_stime)/ticks);
 
        if(number >= 2){
            printf("Od ostatniego punktu kontrolnego:\n");
            printf("real time: %lf\n", (uintmax_t)(clock_t)(checkpoints[number]-checkpoints[number-1])/ticks);
            printf("user time: %lf\n\n", (uintmax_t)(clock_t)(myTimes[number].tms_utime-myTimes[number-1].tms_utime)/ticks);
            printf("system time: %lf\n", (uintmax_t)(clock_t)(myTimes[number].tms_stime-myTimes[number-1].tms_stime)/ticks);
           }
    }
}

clock_t _start_clock, _last_clock;
struct tms _start_tms, _last_tms;
int _ticks;



int main() 
{	
	start = times(NULL);
  #ifdef DYNAMICLIB
    char *error;
       //nie przejmuje sie nie zdefiniowanymi symbolami
    void * listLib = dlopen("liblist.so", RTLD_LAZY);
    if (!listLib) { 
      fprintf(stderr, "%s\n", dlerror());
      exit(1);
    }
    initL = dlsym(listLib, "init");
    if ((error = dlerror()) != NULL)  
    { 
      fprintf(stderr, "%s\n", error);
      exit(1); 
    }
    //zwraca adres do funkcji ktora zaladowana z shared l.
    initL = dlsym(listLib, "init");
    create_nodeL = dlsym(listLib, "create_node");
    mergesortL = dlsym(listLib, "mergesort");
    show_listL = dlsym(listLib, "show_list");
    show_list_reverseL = dlsym(listLib, "show_list_reverse");
    remove_from_listL = dlsym(listLib, "remove_from_list");
    set_new_listL = dlsym(listLib, "set_new_list");
    insert_at_the_endL = dlsym(listLib, "insert_at_the_end");
    insert_at_the_beginningL = dlsym(listLib, "insert_at_the_beginning");
    show_node2L = dlsym(listLib, "show_node2");
    finding_nodeL= dlsym(listLib, "finding_node");

  #else
    initL = init;
    create_nodeL = create_node;
    mergesortL = mergesort;
    show_listL = show_list;
    show_list_reverseL = show_list_reverseL;
    remove_from_listL = remove_from_list;
    set_new_listL = set_new_list;
    insert_at_the_endL = insert_at_the_end;
    insert_at_the_beginningL = insert_at_the_beginning;
    show_node2L = show_node2;
    finding_nodeL = finding_node;
  #endif  

	checkpoint(0);
  struct node *s1 = create_nodeL("adam", "lis", "01-01-1940", "agh123@pap.pl", 662, "Krakow");
  struct node *s2 = create_nodeL("patrycja", "waran", "01-01-1926", "s@student.agh.edu.pl", 659, "Radom");
  struct node *s3 = create_nodeL("marysia", "kot", "01-01-1957", "kluska@on.pl", 660, "Warszawa");
  struct node *s4 = create_nodeL("grzegorz", "papuga", "01-01-1973", "iaff@my.pl", 510, "Poznan");
  struct node *s5 = create_nodeL("patryk", "kot", "01-01-1962", "ser@aol.com", 708, "Opole");
  struct node *s6 = create_nodeL("bronislaw", "pies", "01-01-1982", "ptr@onet.pl", 613, "Zakopane");
  struct node *s7 = create_nodeL("izabella", "krolik", "01-01-1954", "stek@interia.pl", 657, "Gdansk");
  struct node *t1 = create_nodeL("adam", "lis", "01-01-1940", "agh123@pap.pl", 662, "Krakow");
  struct node *t2 = create_nodeL("patrycja", "waran", "01-01-1926", "s@student.agh.edu.pl", 659, "Radom");
  struct node *t3 = create_nodeL("marysia", "kot", "01-01-1957", "kluska@on.pl", 660, "Warszawa");
  struct node *t4 = create_nodeL("grzegorz", "papuga", "01-01-1973", "iaff@my.pl", 510, "Poznan");
  struct node *t5 = create_nodeL("patryk", "kot", "01-01-1962", "ser@aol.com", 708, "Opole");
  struct node *t6 = create_nodeL("bronislaw", "pies", "01-01-1982", "ptr@onet.pl", 613, "Zakopane");
  struct node *t7 = create_nodeL("izabella", "krolik", "01-01-1954", "stek@interia.pl", 657, "Gdansk");
  struct node *u1 = create_nodeL("adam", "lis", "01-01-1940", "agh123@pap.pl", 662, "Krakow");
  struct node *u2 = create_nodeL("patrycja", "waran", "01-01-1926", "s@student.agh.edu.pl", 659, "Radom");
  struct node *u3 = create_nodeL("marysia", "kot", "01-01-1957", "kluska@on.pl", 660, "Warszawa");
  struct node *u4 = create_nodeL("grzegorz", "papuga", "01-01-1973", "iaff@my.pl", 510, "Poznan");
  struct node *u5 = create_nodeL("patryk", "kot", "01-01-1962", "ser@aol.com", 708, "Opole");
  struct node *u6 = create_nodeL("bronislaw", "pies", "01-01-1982", "ptr@onet.pl", 613, "Zakopane");
  struct node *u7 = create_nodeL("izabella", "krolik", "01-01-1954", "stek@interia.pl", 657, "Gdansk");
  struct list *lista = initL();
  struct list *lista2 = initL();
  struct list *lista3 = initL();

  checkpoint(1);
  insert_at_the_endL(lista, s2);
  insert_at_the_endL(lista, s3);
  insert_at_the_endL(lista, s4);
  insert_at_the_endL(lista, s5);
  insert_at_the_endL(lista, s6);
  insert_at_the_endL(lista, s7);

  insert_at_the_endL(lista, t2);
  insert_at_the_endL(lista, t3);
  insert_at_the_endL(lista, t4);
  insert_at_the_endL(lista, t5);
  insert_at_the_endL(lista, t6);
  insert_at_the_endL(lista, t7);

  insert_at_the_endL(lista, u2);
  insert_at_the_endL(lista, u3);
  insert_at_the_endL(lista, u4);
  insert_at_the_endL(lista, u5);
  insert_at_the_endL(lista, u6);
  insert_at_the_endL(lista, u7);

  remove_from_listL(lista, lista->head);
  struct node *tmp; 
  tmp = finding_nodeL(lista, "bronislaw" , "pies");
  if (tmp != NULL){
   	printf("%s\n", tmp->surname);}

  checkpoint(2);
  insert_at_the_beginningL(lista, s1);
  insert_at_the_beginningL(lista, t1);
  insert_at_the_beginningL(lista, u1);

	struct node* nowy = mergesortL(lista->head);
	struct node* nowy2 = mergesortL(lista2->head);
	struct node* nowy3 = mergesortL(lista3->head);
	set_new_listL(lista, nowy);
	set_new_listL(lista2, nowy2);
	set_new_listL(lista3, nowy3);

	//show_list(lista);
	//show_list(lista2);
	//show_list_reverse(lista3);
  #ifdef DYNAMICLIB
	 dlclose(listLib);
  #endif
  checkpoint(3);
	return 0;
 }


/* makefile:
-Ldir - dodaje katalog do scieżki gdzie linker szuka bilbiotek
- lname libname.so linkuje wbrew statycznej lub dzielonej jesli obie znalezione to blad
*/