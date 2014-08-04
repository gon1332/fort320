#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../../include/InputBuffer/mylist.h"


/* void list_init (void)
 * 
 * Used to initiliase a double-linked circular list with sentinel.
 */
void list_init (void)
{
	root = (line_T *) malloc(sizeof(line_T));
	if (!root) {
		perror("fort320: list_init: error");
		exit(EXIT_FAILURE);
	}
	root->next = root;
	root->prev = root;
}

/* void list_insert (char *name)
 * 
 * Used to insert a node in the end of the list with specific member values.
 */
void list_insert (const char *name)
{
	line_T *new_node;
	
	new_node = (line_T *) malloc(sizeof(line_T));
	if (!new_node) {
		perror("fort320: list_insert (new_node): error");
		exit(EXIT_FAILURE);
	}
	
	new_node->name = malloc(NULL_CHAR_SIZE + strlen(name));
	if (!new_node->name) {
		perror("fort320: list_insert (new_node->name): error");
		exit(EXIT_FAILURE);
	}
	
	strcpy(new_node->name, name);
	
	/* insert element in the end of the list */
	new_node->next = root;
	new_node->prev = root->prev;
	root->prev->next = new_node;
	root->prev = new_node;
}

/* void list_delete (void)
 *
 * Deletes all nodes of the list
 */
void list_delete (void)
{
	line_T *curr, *next_curr;
	
	for (curr = root->next; curr != root;) {
		next_curr = curr->next;
		free(curr->name);
		free(curr);
		curr = next_curr;
	}
	free(root);
}

/* int list_is_empty (void)
 * 
 * Checks if the list is empty. If it is returns 1, otherwise 0.
 */
int list_is_empty (void)
{
	if (root->next == root && root->prev == root)
		return (1);
	return (0);
}

/* void fprint_list (const char *before, const char *after)
 * 
 * Prints the list between 'before' and 'after' strings.
 */
void print_list (void)
{
	line_T *curr;
	
	for (curr = root->next; curr != root; curr = curr->next)
		fprintf(stdout, "%s", curr->name);
}

/* line_T *search_list_by_name (char *name)
 * 
 * Searches the list for an element with the given name.
 * If succeeded returns the pointer to the element, otherwise
 * returns NULL pointer.
 */
line_T *search_list_by_name (const char *name)
{
	line_T *curr;
	
	root->name = malloc(NULL_CHAR_SIZE + strlen(name));
	if (!root->name) {
		perror("fort320: search_list_by_name: error");
		exit(EXIT_FAILURE);
	}
	strcpy(root->name, name);

	for (curr = root->next; strcmp(curr->name, name); curr = curr->next);
	
	return (curr != root ? curr : NULL);	/* If it was found, return element */
}
