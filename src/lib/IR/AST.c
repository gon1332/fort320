#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IR/AST.h"
#include "Utils/strdup.h"
#include "DebugInfo/errcheck.h"


char *expr_lookup[26] = {
	"ID",  "INT", "REAL",  "BOOL",  "STR", "CHAR", "OR",    "AND",   "NOT",
        "GT",  "GE",  "LT",    "LE",    "EQ",  "NE",   "PLUS",  "MINUS", "MUL",
        "DIV", "POW", "PAREN", "ASSIGN", "COLON", "BRACK", "COMMA"
};

char *cmds_lookup[8] = {
	"ASSIGN", "GOTO", "S_IF", "C_IF", "CALL", "IO", "C_FLOW", "LOOP"
};


/*  -----   INTERNAL FUNCTION DECLARATIONS   ------------------------------  */
/*static void print_expr(AST_expr_T *root);*/
static void print_cmd (AST_cmd_T  *root);


/**************************************************************************//**
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

/**************************************************************************//**
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

/**************************************************************************//**
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

/**************************************************************************//**
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

/**************************************************************************//**
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

/**************************************************************************//**
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
	ret->description.stringval = strdup(string);

	return(ret);
}

/**************************************************************************//**
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

/**************************************************************************//**
 * AST_cmd_T *mk_cmd (CmdNodeTag, AST_expr_T, AST_cmd_T, AST_cmd_T)
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

/**************************************************************************//**
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

/**************************************************************************//**
 * void print_ast (void)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Prints the AST.
 *****************************************************************************/
void print_ast(void)
{
        AST_cmd_T *curr;
        for (curr = AST_head; curr != AST_tail; curr = curr->next)
                print_cmd(curr);
}

/**************************************************************************//**
 * static void print_expr (AST_expr_T)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Prints the given expression recursively.
 *****************************************************************************/
                /*
static void print_expr(AST_expr_T *root)
{
	if (root->kind < 6) {
		printf("%s", expr_lookup[root->kind]);
		switch (root->kind) {
		case ID:
                        printf("(%s) ", root->description.id_entry->str);
                        break;
		case EXPR_INT:
                        printf("(%d) ", root->description.intval);
                        break;
		case EXPR_REAL:
                        printf("(%g) ", root->description.realval);
                        break;
		case EXPR_BOOL:
                        printf("(%d) ", root->description.charval);
                        break;
		case EXPR_STR:
                        printf("(%s) ", root->description.stringval);
                        break;
		case EXPR_CHAR:
                        printf("(%c) ", root->description.charval);
                        break;
		default:
                        printf("!ERROR!\n");
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
                */
/*****************************************************************************/
#define UN_COMPACT
int _print_t(AST_expr_T *tree, int is_left, int offset, int depth, char s[20][255])
{
        char b[20] = "";
        int width = 5;

        if (!tree) return 0;

        /*printf("<%s>\n", expr_lookup[tree->kind]);
        */
        sprintf(b, "(%s)", expr_lookup[tree->kind]);

        int left, right;
        if (tree->kind > 5) {
                left  = _print_t(tree->description.opds[0], 1, offset,                depth + 1, s);
                right = _print_t(tree->description.opds[1], 0, offset + left + width, depth + 1, s);
        } else {
                left  = width;
                right = width;
        }
#ifdef COMPACT
        for (int i = 0; i < width; i++)
                s[depth][offset + left + i] = b[i];

        if (depth && is_left) {
                for (int i = 0; i < width + right; i++)
                s[depth - 1][offset + left + width/2 + i] = '-';

                s[depth - 1][offset + left + width/2] = '.';
        } else if (depth && !is_left) {
                for (int i = 0; i < left + width; i++)
                s[depth - 1][offset - width/2 + i] = '-';

                s[depth - 1][offset + left + width/2] = '.';
        }
#else
        for (int i = 0; i < width; i++)
                s[2 * depth][offset + left + i] = b[i];

        if (depth && is_left) {
                for (int i = 0; i < width + right; i++)
                s[2 * depth - 1][offset + left + width/2 + i] = '-';

                s[2 * depth - 1][offset + left + width/2] = '+';
                s[2 * depth - 1][offset + left + width + right + width/2] = '+';
        } else if (depth && !is_left) {

                for (int i = 0; i < left + width; i++)
                s[2 * depth - 1][offset - width/2 + i] = '-';

                s[2 * depth - 1][offset + left + width/2] = '+';
                s[2 * depth - 1][offset - width/2 - 1] = '+';
        }
#endif

        return left + width + right;
}


void print_expr(AST_expr_T *root)
{
        int i;
        char s[20][255];
        for (i = 0; i < 20; i++)
                sprintf(s[i], "%80s", " ");

        _print_t(root, 0, 4, 0, s);

        for (i = 0; i < 20; i++)
                printf("%s\n", s[i]);
}
/*****************************************************************************/


/* TODO: gon1332 Print command nodes. Sat 09 Aug 2014 06:01:37 PM UTC */
static void print_cmd(AST_cmd_T *root)
{
        ;
}


