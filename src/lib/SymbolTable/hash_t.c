#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SymbolTable/hash_t.h"
#include "Utils/strdup.h"


hash_table_t *create_hash_table(int size)
{
        int i;
        hash_table_t *new_table;

        if (size<1) return NULL; /* invalid size for table */

        /* Attempt to allocate memory for the table structure */
        if ((new_table = malloc(sizeof(hash_table_t))) == NULL) {
              return NULL;
        }

        /* Attempt to allocate memory for the table itself */
        if ((new_table->table = malloc(sizeof(list_t *) * size)) == NULL) {
                return NULL;
        }

        /* Initialize the elements of the table */
        for(i=0; i<size; i++)
                new_table->table[i] = NULL;

        /* Set the table's size */
        new_table->size = size;

        return new_table;
}

/* Hash function
*
*/
unsigned int hash(hash_table_t *hashtable, char *str)
{
	unsigned int hashval = 0;

	for(; *str != '\0'; str++) hashval = *str + (hashval << 5) - hashval;

        /* return the hash value mod the hashtable size so that it will
         * fit into the necessary range
         */
        return hashval % hashtable->size;
}

list_t *lookup_identifier(hash_table_t *hashtable, char *str,int embeleia)
{
    list_t *list;
    unsigned int hashval = hash(hashtable, str);

    /* Go to the correct list based on the hash value and see if str is
     * in the list.  If it is, return a pointer to the list element.
     * If it isn't, the item isn't in the table, so return NULL.
     */
    for(list = hashtable->table[hashval]; list != NULL; list = list->next) {
        if (strcmp(str, list->str) == 0 && list->reach <= embeleia)
		return list;
    }
    return NULL;
}
/*	add_identifier(hash_table_t *hashtable, char *str, int embeleia)
 *		Adds a new identifier to the hash table. The node is put in the first position of each
 *		respective list_t.
 */
int add_identifier(hash_table_t *hashtable, char *str, int embeleia, Type type, Complex_Type cat)
{
    list_t *new_list;
    list_t *current_list;
    unsigned int hashval = hash(hashtable, str);

    /* Attempt to allocate memory for list */
    if ((new_list = malloc(sizeof(list_t))) == NULL) return 1;

    /* Does item already exist? */
    current_list = lookup_identifier(hashtable, str, embeleia);
        /* item already exists, don't insert it again. */
    if (current_list != NULL && current_list->reach == embeleia) return -1;

    /* Insert into list */
    new_list->str = strdup(str);
    new_list->reach = embeleia;
    new_list->type = type;
    new_list->cat = cat;
    new_list->is_function = 0;				/* intial value set to 0:: if func we set it in grammar rules*/

    /* NOTE: id_info represents a union structure representing different data based on variables cat and is_func
     * and therefore not initialized here.
     */

	/* if first node in list */
	if(hashtable->table[hashval] == NULL){
		hashtable->table[hashval] = new_list;
		new_list->next = NULL;
		new_list->prev = NULL;
	}
	/* insert node in list */
	else{
		hashtable->table[hashval]->prev = new_list;
		new_list->next = hashtable->table[hashval];
		hashtable->table[hashval] = new_list;
	}

	/* Refresh stack */
	if(	add_to_stack(hashtable, new_list, hashval) != 0 ){
		printf("Error in stack allocation\n");
		return -2;
	}

	/*TEST CODE Prints*/
	/*printf("------------HASHme-_------------\n");
	print_hashtable(hashtable);
	printf("________________________________\n");*/
	/*printf("------------STACKme-------------\n");
	print_stack(hashtable);
	printf("--------------------------------\n");*/

    return 0;
}
/* int delete_id(hash_table_t *hashtable ,list_t *d_node, int table_num)
		deletes the specified node from the hash table
		if the user does not know the table number it must be set to -1
 */
int delete_id(hash_table_t *hashtable, list_t *d_node, int table_num)
{
	int i = 0;	
	
	/*We only care for the table number when the node is the first element of the list*/
	if(table_num == -1){
		/*go through the hashtable lists to check the first nodes*/
		for (i = 0; i < hashtable->size ; i++){
			if(hashtable->table[i] == d_node){
				table_num = i;
				break;
			}
		}
	}	

	/* A. if d_node points to a function we must delete the parameter list as well*/
	if(d_node->is_function != 0){
		remove_params(d_node);
	}
	/* C,D,F. if d_node points to a variable/array/list we must free the initializations list*/
	else if(d_node->cat == C_variable || d_node->cat == C_array || d_node->cat == C_list){
		remove_inits(d_node);
	}
	/**/
	
	/* remove node */
	/*if d_node is the first node in list*/
	if(hashtable->table[table_num] == d_node){
		hashtable->table[table_num] = d_node->next;
		free(d_node->str);
		free(d_node);
	}
	else{
		d_node->prev->next = d_node->next;
		d_node->next->prev = d_node->prev;
		free(d_node->str);
		free(d_node);
	}
	return 0;
}

/* free_table(hash_table_t *hashtable) 
 * 	Frees all the nodes from the hash table,including the hash table itself
 */
void free_table(hash_table_t *hashtable)
{
	int i;
	list_t *list, *temp;

	if (hashtable==NULL) return;

	/* Free the memory for every item in the table, including the 
	* strings themselves.
	*/
	for(i=0; i<hashtable->size; i++) {
		list = hashtable->table[i];
		while(list!=NULL) {
			temp = list;
			list = list->next;
			free(temp->str);
			free(temp);
		}
	}

	/* Free the table itself */
	free(hashtable->table);
	free(hashtable);
}

void print_hashtable(hash_table_t *hashtable){

	list_t **curr_table;
	int i = 0;	
	list_t *curr;	
	
	struct params_t *curr_param;
	
	printf("------------HASHme-_------------\n");
	for(curr_table = hashtable->table ; i < hashtable->size; i++ ){
		
		for(curr = hashtable->table[i]; curr != NULL; curr = curr->next) {
        	
			printf("List %d , node_msg %s with nesting depth %d and Type %s and Category %s \n",i, 					curr->str,curr->reach,typeNames[curr->type],catNames[curr->cat]);
			
			/* Print id_info*/
			/* A. print function/subroutine parameters*/
			if(curr->is_function != 0){
				printf("\t\t Parameter list \n");
				for(curr_param = curr->id_info.params; curr_param != NULL; curr_param = curr_param->next){
					printf("\t\t>>> id: %s , type: %s and eval: %d\n", curr_param->p_name, 
						typeNames[curr_param->p_type], curr_param->eval_t);
				}
			}
			/* C,D,F. initializations print */
			else if(curr->cat == C_variable || curr->cat == C_array || curr->cat == C_list){
				printf("\t\t >>> Intitial Value: ");
				print_value(curr);
			}
    		}	
		curr_table++;
	}	
}
/*STACK FUNCTIONS*/

/* int add_to_stack(hash_table_t *hashtable, list_t node, int nesting_depth)
 *   Adds a node reference to the stack (sorted by nesting depth)
 */
int add_to_stack(hash_table_t *hashtable, list_t *node, int table_n){

	nd_stack_t *curr_node;
	
	/* Attempt to allocate memory for the node */
	if( (curr_node = malloc(sizeof(nd_stack_t))) == NULL ){
		printf("Error in stack node memory allocation\n");
		return -1;
	}
	
	/* Point to the node in the hash table*/
	curr_node->info_node = node;		
	
	curr_node->table_num = table_n;
	curr_node->next = NULL;
	curr_node->prev = NULL;
	
	/*Always put the new nodes in the beggining of the list*/
	if(hashtable->nd_stack == NULL){
		hashtable->nd_stack = curr_node;
	}
	else{
		hashtable->nd_stack->prev = curr_node;
		curr_node->next = hashtable->nd_stack;				/* if it works remind me .....*/
		hashtable->nd_stack = curr_node;   				/*k ama doulepsei na m to thimitheite.....*/
	}

	return 0;
}

/* int delete_scope(hash_table_t *hashtable,int scope)
 *   Deletes all the nodes with scope( nesting depth ) from the hash table and the stack itself
*/
int delete_scope(hash_table_t *hashtable,int scope){
	
	nd_stack_t *curr_node, *temp;

	for(curr_node = hashtable->nd_stack; curr_node != NULL;){
		
		/* If the id has the specified scope(n.d.) we delete it 
		 *	from both the hash table and remove its stack pointer 
		 */ 	
		if(curr_node->info_node->reach == scope){
			delete_id(hashtable, curr_node->info_node, curr_node->table_num);
			
			/* delete from stack */
			if(hashtable->nd_stack == curr_node){				/*Maybe overreacting it is always going to hit that part */
				hashtable->nd_stack = curr_node->next;			/* On the other hand we give full control to the parser to f. the id's*/
				temp = curr_node;								/*.... that means if i haven't done it already here !!! :P*/
				curr_node = curr_node->next;
				free(temp);
			}
			else{											/*this chunk of code is never executed.....*/
				curr_node->next->prev = curr_node->prev;
				curr_node->prev->next = curr_node->next;
				temp = curr_node;
				curr_node = curr_node->next;
				free(temp);
			}
		}
		else{
			break;											/* it depends on whether you want a stack or a list basically....*/
		}
	}
	
	return 0;
}

/*	void print_stack(hash_table_t *hashtable)
		Prints the nodes sorted by nesting depth..... (and without segmentations i hope)
*/
void print_stack(hash_table_t *hashtable){			/*throws Segmentation fault :P .... it should have a catch block*/

	nd_stack_t *curr_node = NULL;
	
	
	printf("------------STACKme-_------------\n");
	for( curr_node = hashtable->nd_stack; curr_node != NULL; curr_node = curr_node->next ){
		
		printf("Stack:: ID %s with nd %d resides in table %d\n",curr_node->info_node->str,curr_node->info_node->reach,curr_node->table_num);
	}
	printf("endprint\n");
}
/* struct params_t* insert_params(struct params *node, Type type, int eval, char *name)
 * insererts a parameter's information into the list
 * reterns a reference to the new list... sort of
 */
struct params_t* insert_params(struct params_t *node, Type type, int eval, char *name){

	struct params_t *curr;
		
	/* if you understand what this does you get a cookie */
	/* if list is empty create node */
	if (node == NULL){
		
		curr = (struct params_t *)malloc(sizeof(struct params_t));
		/* store parameter info */
		/* <INSERT EPIC CODE HERE> */
		curr->p_name = strdup(name); 
		curr->p_type = type;
		curr->eval_t = eval;
		curr->next = NULL;
		node = curr;
	}
	else{
		curr = (struct params_t *)malloc(sizeof(struct params_t));
		curr->p_name = strdup(name);
		curr->p_type = type;
		curr->eval_t = eval;
		curr->next = node;
		node = curr;		/* sth fishy is going on here ....*/
	}
	
	return node;
}
/* void remove_params(list_t *node){
 * just in case we want to remove the id node we want to remove the list nodes as well...
 * must be connected to above functions when removing an id...(check id if function then call this)
 */
void remove_params(list_t *node){
	
	struct params_t *curr,*curr_2;
	/*printf("!!!!!!!!!!!!REMOVE FUNC!!!!!!!!!!!\n");*/
	for(curr = node->id_info.params; curr != NULL;){
		curr_2 = curr->next;
		free(curr->p_name);
		free(curr);
		curr = curr_2;
	}
	node->id_info.params = NULL;
}

/* void remove_inits(list_t *node)
 * Removes the nodes allocated for the initialization values of each structure (var/array/list)	
 */
void remove_inits(list_t *node){

	init_values *curr,*curr_2;
	
	/*printf("!!!!!!!!!!!!REMOVE INITS!!!!!!!!!!! for node %s\n",node->str);*/
	
	for(curr = node->id_info.init_n.init; curr != NULL;){
		/*printf("vr\n");*/
		curr_2 = curr->next;
		free(curr);
		
		curr = curr_2;
	}
	
	free(node->id_info.init_n.dimensions);
	/* reset pointers */
	node->id_info.init_n.init = NULL;
	node->id_info.init_n.dimensions = NULL;
}

/* void print_value(list_t *curr)
 * Prints initial values stored in hash table
 */
void print_value(list_t *node){

	init_values *head_init,*curr;	

	if(node->id_info.init_n.init == NULL){
		printf(" ::NULL ");		/* tha prepei na yparxoun akoma k gia unititialized var */
	}
	
	head_init = node->id_info.init_n.init;

	switch(node->type){
			case TY_integer:
				for (curr = head_init; curr != NULL; curr = curr->next){
					if(&(curr->initialization.intval) != NULL)
						printf("%d \t",curr->initialization.intval);
					else
						printf(" ::NULL ");
				}
				break; 
			case TY_real:
				for (curr = head_init; curr != NULL; curr = curr->next){
					if(&curr->initialization.realval != NULL)
						printf("%f \t",curr->initialization.realval);
					else
						printf(" ::NULL ");
				}
				break;
			case TY_complex:
				for (curr = head_init; curr != NULL; curr = curr->next){
					if(curr->initialization.string != NULL){
						printf("%g :",curr->initialization.complex.c_real);
						printf(" %g \t",curr->initialization.complex.c_imag);
					}
					else{
						printf(" ::NULL ");
					}
				}
				break;
			case TY_string:
				for (curr = head_init; curr != NULL; curr = curr->next){
					if(curr->initialization.string != NULL)
						printf("%s \t",curr->initialization.string);
					else
						printf(" ::NULL ");
				}
				break;
			case TY_character:
				for (curr = head_init; curr != NULL; curr = curr->next){
					if(&curr->initialization.charval != NULL)
						printf("%c \t",curr->initialization.charval);
					else
						printf(" ::NULL ");
				}
				
				break;
			case TY_logical:
				for (curr = head_init; curr != NULL; curr = curr->next){
					if(&curr->initialization.charval != NULL)
						printf("%c \t",curr->initialization.charval);
					else
						printf(" ::NULL ");
				}
				break;
			default:
				return;		
		}	
		puts("");
		return;
}




