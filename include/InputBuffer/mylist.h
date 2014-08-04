#ifndef MYLIST
#define MYLIST

#define NULL_CHAR_SIZE	1

typedef struct list{
	char *name;	/* the lexeme */
	
	struct list *next;
	struct list *prev;
} line_T;

line_T *root;	/* The head of the list */

/* Function declarations */
void list_init (void);
void list_insert (const char *name);
void list_delete (void);
int list_is_empty (void);
void print_list (void);
line_T *search_list_by_name (const char *name);

#endif	/* MYLIST */
