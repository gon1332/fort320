#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IR/AST.h"
#include "Utils/strdup.h"
#include "DebugInfo/errcheck.h"


char *expr_lookup[26] = {
	"ID", "INT", "REAL", "BOOL", "STR", "CHAR", "OR", "AND", "NOT", "GT",
	"GE", "LT", "LE", "EQ", "NE", "PLUS", "MINUS", "MUL", "DIV", "POW",
	"PAREN", "ASSIGN", "COLON", "BRACK", "COMMA"
};

char *cmds_lookup[8] = {
	"ASSIGN", "GOTO", "S_IF", "C_IF", "CALL", "IO", "C_FLOW", "LOOP"
};


/*************************************************************************//**
 * AST_expr_T *mkleaf_id (list_t *):
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a leaf node for an id containing a pointer to the id entry in the
 * symbol table.
 * Returns the pointer to the node.
 *****************************************************************************/
AST_expr_T *mkleaf_id(void *id_entry)
{
	AST_expr_T *ret;

	SafeCall( ret = malloc(sizeof(AST_expr_T)) );

	/* Initialize the new node's fields */
	ret->kind = EXPR_ID;
	ret->description.id_entry = id_entry;

	return(ret);
}

/*************************************************************************//**
 * AST_expr_T *mkleaf_int (register int):
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a leaf node for an integer containing the value of it.
 * Returns the pointer to the node.
 *****************************************************************************/
AST_expr_T *mkleaf_int(register int number)
{
	AST_expr_T *ret;

	SafeCall( ret = malloc(sizeof(AST_expr_T)) );

	/* Initialize the new node's fields */
	ret->kind = EXPR_INT;
	ret->description.intval = number;

	return(ret);
}

/*************************************************************************//**
 * AST_expr_T *mkleaf_double (register double):
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a leaf node for a double containing the value of it.
 * Returns the pointer to the node.
 *****************************************************************************/
AST_expr_T *mkleaf_real(register double number)
{
	AST_expr_T *ret;

	SafeCall( ret = malloc(sizeof(AST_expr_T)) );

	/* Initialize the new node's fields */
	ret->kind = EXPR_REAL;
	ret->description.realval = number;

	return(ret);
}

/*************************************************************************//**
 * AST_expr_T *mkleaf_char (register unsigned char)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a leaf node for a character containing the value of it.
 * Returns the pointer to the node.
 *****************************************************************************/
AST_expr_T *mkleaf_char(register unsigned char character)
{
	AST_expr_T *ret;

	SafeCall( ret = malloc(sizeof(AST_expr_T)) );

	/* Initialize the new node's fields */
	ret->kind = EXPR_CHAR;
	ret->description.charval = character;

	return(ret);
}

/*************************************************************************//**
 * AST_expr_T *mkleaf_bool (register unsigned char)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a leaf node for a boolean containing the value of it.
 * Returns the pointer to the node.
 *****************************************************************************/
AST_expr_T *mkleaf_bool(register unsigned char logical)
{
	AST_expr_T *ret;

	SafeCall( ret = malloc(sizeof(AST_expr_T)) );

	/* Initialize the new node's fields */
	ret->kind = EXPR_BOOL;
	ret->description.charval = logical;

	return(ret);
}

/*************************************************************************//**
 * AST_expr_T *mkleaf_string (register char *)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a leaf node for a string containing the value of it.
 * Returns the pointer to the node.
 *****************************************************************************/
AST_expr_T *mkleaf_string(register const char *string)
{
	AST_expr_T *ret;

	SafeCall( ret = malloc(sizeof(AST_expr_T)) );

	/* Initialize the new node's fields */
	ret->kind = EXPR_STR;
	ret->description.stringval =  strdup(string);

	return(ret);
}

/*************************************************************************//**
 * AST_expr_T *mknode (ExprNodeTag, AST_expr_T, AST_expr_T)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a node for the ExprNodeTag expression which includes at most two
 * other expressions.
 * Returns the pointer to the node.
 *****************************************************************************/
AST_expr_T *mknode(
		ExprNodeTag  op,
		AST_expr_T  *ch_1,
		AST_expr_T  *ch_2
)
{
	AST_expr_T *ret;

	SafeCall( ret = malloc(sizeof(AST_expr_T)) );

	/* Initialize the new node's fields */
	ret->kind = op;
	ret->description.opds[0] = ch_1;
	ret->description.opds[1] = ch_2;

	return(ret);
}

/*************************************************************************//**
 * AST_expr_T *mk_cmd (CmdNodeTag, AST_expr_T, AST_cmd_T, AST_cmd_T)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Creates a command node for the CmdNodeTag command which includes one
 * expression and at most two other recursive commands (for the if-then-else
 * and loop statements).
 * Returns the pointer to the node.
 *****************************************************************************/
AST_cmd_T *mkcmd(
		CmdNodeTag  cmd,
		AST_expr_T *ch_1,
 		AST_cmd_T  *ch_2,
		AST_cmd_T  *ch_3
)
{
	AST_cmd_T *ret;

	SafeCall( ret = malloc(sizeof(AST_cmd_T)) );

	/* Populate the single-linked list setting the head and tail */
	if (NULL == AST_head) {	/* if the cmd is the first in the list */
		AST_head = ret;
		AST_tail = ret;
	} else {		/* if there are cmds already in the list */
		AST_tail->next = ret;
		AST_tail = ret;
	}

	/* Initialize the new cmd node's fields */
	ret->kind = cmd;
	ret->next = NULL;
	ret->expr = ch_1;
	ret->cmds[0] = ch_2;	/* if the cmd is not a branch(excldng goto) */
	ret->cmds[1] = ch_3;	/* the cmd children are NULL */

	return(ret);
}

/*************************************************************************//**
 * void AST_init (void)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Initializes the single-linked list without sentinel.
 * Sets *head and *tail pointers.
 *****************************************************************************/
void AST_init(void)
{
	AST_head = NULL;
	AST_tail = NULL;
	return;
}

/*************************************************************************//**
 * void print_expr (AST_expr_T)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Prints the given expression recursively.
 *****************************************************************************/
void print_expr(AST_expr_T *root)
{
	if (root->kind <= 5) {
		printf("%s", expr_lookup[root->kind]);
		switch (root->kind) {
		/*case ID:	printf("(%s) ", root->description.id_entry->str);*/
		case EXPR_INT:	printf("(%d) ", root->description.intval);
		case EXPR_REAL:	printf("(%g) ", root->description.realval);
		case EXPR_BOOL:	printf("(%d) ", root->description.charval);
		case EXPR_STR:	printf("(%s) ", root->description.stringval);
		case EXPR_CHAR:	printf("(%c) ", root->description.charval);
		default:	printf("!ERROR!\n");
		}
		return;
	} else {
		if (root->description.opds[0] != NULL) {
			print_expr(root->description.opds[0]);
		}
		printf("__%s__ ", expr_lookup[root->kind]);
		if (root->description.opds[1] != NULL) {
			print_expr(root->description.opds[1]);
		}
		puts("");
	}
}
