#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "list.h"

//obsulga bledow
void show_errno(void)
{
    if(errno==1)  printf("operacja niedozwolona");
    if(errno==12) printf("przeciazenie pamieci");    
    if(errno==22) printf("zly argument");
    if(errno==0)  printf("wszystko ok!");
    printf(" occurred\n");
}


//inicjalizuje liste
struct list *init(void) {
	struct node *head = malloc(sizeof(struct node));
	struct list *list = malloc(sizeof(struct list));
	list->head = head;
	list->head->next = NULL;
	list->tail = head;
	return list;
}
//tworzy pojedynczego node'a z zadanymi elementami
struct node *create_node(char *n, char *s, char *b, char *e, long int c, char *a) {
	 struct node *p = malloc(sizeof(struct node));
	 p->name = n;
	 p->surname = s;
	 p->bdate = b;
	 p->email = e;
	 p->contact = c;
	 p->address = a;
	 return p;
}

void insert_at_the_end(struct list *list, struct node *tmp){
	if (tmp!= NULL && list != NULL){
		list->tail->next = tmp;
 		tmp->prev = list->tail;
		tmp->next = NULL;
		list->tail = tmp;
	}
	else {
		show_errno();
	}
}

void insert_at_the_beginning(struct list *list, struct node *tmp){
	if (tmp!= NULL && list != NULL){
		tmp->next = list->head;
 		tmp->prev = NULL;
		list->head->prev = tmp;
		list->head = tmp;
	}
	else {
		show_errno();
	}
}

// sprawdza dlugosc listy pomiedzy h a t
int length_list(struct list *list, struct node *h, struct node *t){
	if (list!= NULL){
		struct node *tmp = h;
		int i = 1;
		while (tmp != t && tmp != NULL) {
			tmp = tmp->next;
			i = i+1;
		}
		return i;
	}
	else {
		show_errno();
        return -1;
	}
}
// wyszukuje po danym imieniu i nazwisku node'a i zwraca wsk do niego 
struct node *finding_node(struct list *list, char *name, char *surname){
	if (list != NULL){
		if (list->head->surname == surname){ return list->head;}
		else {
			struct node *tmp = list->head;

			while (tmp != NULL && tmp->name != name && tmp->surname != surname){
				tmp = tmp->next; 
			}
			if (tmp == NULL) {
				return NULL;
			}
			return tmp;
		}
	}

	show_errno();
    return NULL;
}

// taki delete
void remove_from_list(struct list *list, struct node *rmv){
	if (list!= NULL && list->head != NULL){
		if (rmv == list->head) {
			list->head->next->prev = NULL; //dodane
			list->head = list->head->next;

		} else if (rmv == list->tail){
			list->tail->prev->next = NULL; // dodane
			list->tail = list->tail->prev;
		} else {
			rmv->prev->next = rmv->next;
			rmv->next->prev = rmv->prev;
		}
		free(rmv);
	}
	else {
		show_errno();
	}
}

// do funkcji merge sort
int two_string_compare(struct node *node1, struct node *node2){
	if (node1 != NULL && node2 != NULL){
		if ( strcmp(node1->surname,node2->surname) < 0){
			return 1;
		}else if(strcmp(node1->surname,node2->surname) == 0){
			if (strcmp(node1->name,node2->name) < 0){
				return 1;
			}
		}
		else {
			return -1;
		}
		return 0;
	}
	else {
		show_errno();
        return -1;
	}
}

struct node* mergesort(struct node* head) {
    if(head == NULL || head->next == NULL) {
        return head;
    }
 	else {
    	struct node* middle = find_middle(head);
    	struct node* secondList = middle->next;
 
    	middle->next = NULL;
 
    	return merge(mergesort(head), mergesort(secondList));
    }
}
 // szuka srodkowego elementu, potrzebne do tworzenia partycji-mergesort
struct node* find_middle(struct node* head) {
    if(head == NULL) {
        return head;
    }
 	else {
    	struct node* slow = head;
    	struct node* fast = head;
 
    	while(fast->next != NULL && fast->next->next != NULL) {
        	slow = slow->next;
        	fast = fast->next->next;
        }

        return slow;
    }
}
 
struct node* merge(struct node* a, struct node* b) {
    struct node* newList = malloc(sizeof(struct node));
    struct node* current = newList;
 
    while(a != NULL && b != NULL) {
        if(two_string_compare(a,b) < 0) {
            current->next = a;
            a->prev = current;
            a = a->next;
        } else {
            current->next = b;
            b->prev = current;
            b = b->next;
        }
        current = current->next;
    }
    if(a != NULL) {
        current->next = a;
        a->prev = current;
    }
    if(b != NULL) {
        current->next = b;
        b->prev = current;
    }

    current = newList->next;
    current->prev = NULL;
    free(newList);
 
    return current;
}
// pokazuje ity node na liscie
void show_node(struct list *list, int i){
	struct node *tmp = list->head;
	while (i != 0){
		tmp = tmp->next; }
	printf("Name: %s\n", tmp->name);
}
// pokazuje zadany node(imie)
void show_node2(struct node *tmp){
	if (tmp != NULL){
		printf("Name: %s\n", tmp->name);
	}
	else {
		show_errno();
	}
}
//wypisuje liste
void show_list(struct list* list ){
	if (list != NULL){
    	struct node *p = list->head;
    	int i = 1;
    	printf("\n\n\n------------------------------------\n");
    	while(p != NULL){
        	printf("number of node: %d\n", i);
        	printf("Name: %s\n", p->name);
        	printf("Surname: %s\n", p->surname);
        	printf("Address: %s\n", p->address);
        	i = i + 1; 
        	p = p->next;
    	}
    	printf("\n------------------------------------\n\n\n");
    }
    else {
    	show_errno();
    }	
}

void show_list_reverse(struct list* list ){
    struct node *p = list->tail;

    int i = 1;

    printf("\n\n\n------------------------------------\n");

    while(p != NULL){
        printf("number of node: %d\n", i);
        printf("Name: %s\n", p->name);
        printf("Surname: %s\n", p->surname);
        printf("Address: %s\n", p->address);
        i = i + 1; 
        p = p->prev;
    }
    printf("\n------------------------------------\n\n\n");
}

void set_new_list(struct list* lista, struct node* head) {
    lista->head = head;

    struct node* p = head;

    while(p->next != NULL){
        p = p->next;
    }

    lista->tail = p;
}

void delete_list(struct list *list){
	struct node *tmp, *tmp2;
	tmp = list->tail;
	while (tmp != NULL){
		tmp2= tmp->prev;
		free(tmp);
		tmp = tmp2;
	}
}