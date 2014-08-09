#ifndef __HASH_T_H__
#define __HASH_T_H__
/*Hash table*/
#include "Types/types.h"

/*************************** crappy initial id value info that i think goes to hash table **********************************/
/* Basic Types
 */
typedef union {
	int intval;
	double realval;
	unsigned char charval;
	char *string;
	struct{
		double c_real;
		double c_imag;
	} complex;
} initialization_t;

/* struct params
 *	A. simple list that stores information on function parameters
 */
struct params_t {
	char *p_name;
	Type p_type;
	int eval_t; 	/* Evaluation strategy >> by value::0 or by reference::1 */
	struct params_t *next;
};

/* init_values_t
 * list that stores initial values for: (C.) lists ,(D.) C_arrays, (F.) C_variable
 */
typedef struct init_values_t{
	int num;				/* num*int:: num is initialized num times default is 1 */
	initialization_t initialization;
	struct init_values_t *next;
} init_values;


/*union declaration
 * Stores important information (params, callables..) associated with the id.
 */
typedef union {
	struct params_t *params;	/* A. parameter list - used in function calls */
	int const_value;		/* B. const values */
	/*char *dimensions;*/		/* C. Dimensions of Lists and Arrays */

	/*init_values *init;*/		/* F. Initializations */

	/*C,D,F proposed solution */
	struct{
		char *dimensions;		/* for variables is custom NULL */
		init_values *init;		/* F. Initializations */
	}init_n;

} info_str;

/********************************************** Hash Table ************************************************************/

/*doubly-linked list that points to nodes of the hash table*/
typedef struct _list_t_ {
	char *str;			/* identifier name */
	int reach;			/* nesting depth */
	Type type;			/* type of id (basic types: int,real...) */
	Complex_Type cat;		/* category of id (common_block, lists, arrays etc.) */
	int is_function;		/* 0: variable, 1: function, 2: subroutine */

	info_str id_info;		/* information regarding id::union points to many id_types */

        struct _list_t_ *next;
	struct _list_t_ *prev;
} list_t;

/*Stack*/
/*The stack is a double linked list which has the pointers to the hash table nodes, sorted by nesting depth*/
typedef struct _nd_stack_t {
	list_t *info_node;
	int table_num;

	struct _nd_stack_t *next;
	struct _nd_stack_t *prev;
} nd_stack_t;
/*Stack*/

typedef struct _hash_table_t_ {
    int size;       				/* the size of the table */
    list_t **table; 				/* the table elements */

	nd_stack_t *nd_stack;			/* nesting depth stack */
} hash_table_t;
/*Hash Table*/

/********************************************* Functions ****************************************************************/
/*function declarations for hash_table*/
hash_table_t *create_hash_table(int size);
unsigned int hash(hash_table_t *hashtable, char *str);
list_t *lookup_identifier(hash_table_t *hashtable, char *str, int embeleia);
int add_identifier(hash_table_t *hashtable, char *str, int embeleia, Type type, Complex_Type cat);
void free_table(hash_table_t *hashtable);
int delete_id(hash_table_t *hashtable, list_t *d_node, int table_num);
void print_hashtable(hash_table_t *hashtable);
/*function declarations for hash_table*/

/*function declarations for stack*/
int add_to_stack(hash_table_t *hashtable, list_t *node, int table_n);
int delete_scope(hash_table_t *hashtable, int scope);
void print_stack(hash_table_t *hashtable);
/*function declarations for stack*/

/* function declarations for id information attachment */
struct params_t* insert_params(struct params_t *node, Type type, int eval, char *name);
void remove_params(list_t *node);
void remove_inits(list_t *node);
void print_value(list_t *node);

#endif
