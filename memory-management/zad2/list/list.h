#ifndef LIST_H
#define LIST_H


struct node {
	char *name;
	char *surname;
	char *bdate;
	char *email;
	long int contact;
	char *address; 
	struct node *next;
	struct node *prev;
} ;

struct list {
	struct node *head;
	struct node *tail;
};

struct list *init(void);
struct node *create_node(char *n, char *s, char *b, char *e, long int c, char *a);
void insert_at_the_end(struct list *list, struct node *tmp);
void insert_at_the_beginning(struct list *list, struct node *tmp);
int length_list(struct list *list, struct node *h, struct node *t);
int two_string_compare(struct node *node1, struct node *node2);
struct node *finding_node(struct list *list, char *name, char *surname);
struct node* merge(struct node* a, struct node* b);
struct node* mergesort(struct node* head);
struct node* find_middle(struct node* head);
void remove_from_list(struct list *list, struct node *rmv);
void show_list(struct list* list);
void show_list_reverse(struct list* list);
void show_node(struct list *list, int i);
void show_node2(struct node *tmp);
void set_new_list(struct list* lista, struct node* head);
void delete_list(struct list *list);
#endif