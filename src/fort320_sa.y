%{
	#include <stdio.h>
	#include "../include/SymbolTable/hash_t.h"
	#include "../include/IR/AST.h"
	#include "../include/Utils/utils.h"
	#include "../include/Types/types.h"
	
	/* variable declaration */
	int scope = 0;	/* scope (or nesting depth) defines where each variable
			 * is defined within the program. The visibility of each
			 * variable can also be extracted (i think)
			 */
	hash_table_t *my_hashtable;	/* Symbol Table declaration */
	/* Sematic process spam variables --- */
	/* helpers */
	char * pch;
	list_t *curr;
	/**/
	init_values *head_init;	/* head of the list */
	init_values *init;	/* init :list that holds the initializations of id 
				* if a single var is initialized init holds only 
				* one node init is created in the context of
				* sematic process --> the list is initialized
				* every time
				*/
	/* --- Sematic process spam variables */

	
	void yyerror         (char *msg);
	list_t *context_check(/*enum code_ops operation, */char *sym_name);
	list_t *install      (Type type, Complex_Type c_type, char *sym_name);
	char   *str_append   (char *source_1, char *source_2);
	init_values *create_init_node(void);
	init_values *initialize_node (init_values *node,
	                              Type type_,
	                              initialization_t constant,
	                              int no_of_occurences);
%}

%union
{
	/* Basic Types */
	int           intval;
	double        realval;
	unsigned char charval;
	char         *stringval;

	/* Type and Category */
	Type t;
	Complex_Type c_t;

	struct {
		Type type;
		Complex_Type c_type;
		/* other fields */
	} v;	/* for expressions */

	/* info_struct
	 * it is used to pass more information through the sematic values 
	 * of the (non terminal) symbols. 
	 * <<Not all fields are used for every non terminal>>
	 * value_list : type,cat,params
	 * value : type,cat
	 */
	struct {
		Type type;		/* type */
		Complex_Type c_type;	/* category */
		char *str;		/* save multiple ID's...more or less */
		int params;		/* block has at least (params) parameters */

		/* basic types */
		/*union {
			int intval;
			double realval;
			unsigned char charval;
			char *string;
			struct{
				double c_real;
				double c_imag;
			} complex;
		} basic_types;
		*/
		initialization_t basic_types;
	} info_str;

	/*A. function param list */
	struct params_t *params;
}

%start program

%type<t> type
%type<stringval> vars
%type<stringval> undef_variable
%type<c_t> listspec
%type<stringval> dims
%type<stringval> dim
%type<stringval> id_list
%type<info_str> value_list
%type<info_str> values
%type<info_str> value
%type<v> expression
%type<v> expressions
%type<v> listexpression
%type<info_str> simple_constant
%type<info_str> complex_constant	/* mporei kai na mh xreiazetai */
%type<info_str> constant
%type<t> variable
%type<intval> repeat
%type<charval> sign

%type<params> formal_parameters

%token<stringval> OROP
%token<stringval> ANDOP
%token<stringval> NOTOP
%token<stringval> RELOP
%token<charval> ADDOP
%token MULOP
%token DIVOP
%token POWEROP
%token LISTFUNC

%token LPAREN
%token RPAREN
%token COMMA
%token ASSIGN
%token COLON
%token LBRACK
%token RBRACK

%token<stringval> ID
%token<intval> ICONST
%token<realval> RCONST
%token<charval> LCONST
%token<charval> CCONST
%token<stringval> SCONST

%token FUNCTION
%token SUBROUTINE
%token END
%token COMMON
%token INTEGER
%token REAL
%token COMPLEX  
%token LOGICAL
%token CHARACTER
%token STRING
%token LIST
%token DATA
%token CONTINUE
%token GOTO
%token CALL
%token READ
%token WRITE
%token LENGTH
%token NEW
%token IF
%token THEN
%token ELSE
%token ENDIF
%token DO
%token ENDDO
%token STOP
%token RETURN

/* Operator Precedence */
%nonassoc	T_BRACKETS
%nonassoc	T_COMPLEX
%left		OROP
%left		ANDOP
%nonassoc	NOTOP
%nonassoc	RELOP
%left		ADDOP
%left		MULOP DIVOP
%right		POWEROP
%nonassoc	LPAREN RPAREN


%%
program		: body END subprograms
		;
body		:
/*$1*/		{ scope++; /* put */ }
/*$2,$3*/	declarations statements
/*$4*/		{
			delete_scope(my_hashtable, scope);
			scope--;
		}
		;
declarations	: declarations type vars
		{
			pch = strtok ($3,"%");
			while (pch != NULL) {
				curr = lookup_identifier(my_hashtable, pch, scope);
				curr->type = $2;	/*gia ayth th malakia ta kanw ola*/
				pch = strtok (NULL, "%");
			}
			free($3);
		}
		| declarations COMMON cblock_list
		| declarations DATA vals
		| /* empty */
		;
type		: INTEGER	{ $$ = TY_integer; }
		| REAL		{ $$ = TY_real; }
		| COMPLEX	{ $$ = TY_complex; }
		| LOGICAL	{ $$ = TY_logical; }
		| CHARACTER	{ $$ = TY_character; }
		| STRING	{ $$ = TY_string; }
		;
vars		: vars COMMA undef_variable
      		{
			if (NULL == $1 && $3 != NULL)
				$$ = $3;
			else if ($3 != NULL $$ $1 != NULL)
				$$ = str_append($1, $3);
			else
				$$ = $1;
		}
		| undef_variable
		{
			if ($1 != NULL)
				$$ = 1;
			else
				$$ = NULL;
		}
		;
undef_variable	: listspec ID LPAREN dims RPAREN
		{
	       		if ($1 == C_list) {
				if (NULL == install(TY_unknown, $1, $2)) {
					$$ = $2;
					/* pass the dimensions of id in hash table */
					curr = context_check($2);
					curr->id_info.init_n.dimensions = strdup($4);
					/* the above maybe not so needed */
				} else {
					$$ = NULL;
				}
			} else {
				if (install(NULL == TY_unknown, C_array, $2)) {
					$$ = $2;
					/* pass the dimensions of id in hash table */
					curr = context_check($2);
					curr->id_info.init_n.dimensions = strdup($4);
				} else {
					$$ = NULL;
				}
			}
		}
		| listspec ID
		{
			if (NULL == install(TY_unknown, $1, $2))
				$$ = $2;
			else
				$$ = NULL;
		}
		;
listspec	: LIST		{ $$ = C_list; }
		| /* empty */	{ $$ = C_variable; }
		;
dims		: dims COMMA dim
		{	/*same as vars rule*/
			if (NULL == $1 && $3 != NULL)
				$$ = $3;
			else if ($3 != NULL && $1 != NULL)
				$$ = str_append($1, $3);
			else
				$$ = $1;
		}
		| dim
		{
			if ($1 != NULL)
				$$ = $1;
			else
				$$ = NULL;
		}
		;
dim		: ICONST
		{
			/* allocate enough bytes to satisfy even the largest
			 * integer in fort320
			 */
			$$ = (char *)malloc(20);
			sprintf($$, "%d", $1);	
		}
		| ID
		{
			if (context_check($1) != NULL)
				$$ = $1;
			else
				$$ = NULL;
		}
		| error
		{
			ERROR(stderr, ER_UNKNWN_CONST(FRED("")));
			SEM_ERROR = 1;
		}
		;
cblock_list	: cblock_list cblock
		| cblock
		;
cblock		: DIVOP ID DIVOP id_list
		{
			install(TY_unknown, C_common, $2);
		}
		;
id_list		: id_list COMMA ID
		{
			/* check Symbol Table */
			if (context_check($3) != NULL)
				/* keep the id for the next rules */
				$$ = str_append($1, $3);
		}
		| ID
		{	/* check Symbol Table */
			if (context_check($1) != NULL) 
				$$ = $1;
		}
		| error
		{
			ERROR(stderr, "Unknown type of constant");
			SEM_ERROR = 1;
		}
		;
vals		: vals COMMA ID value_list
		{	/* Initialization block*/
			curr = context_check($3);
			if (curr != NULL) {
				/* validate the consistency of the initializations */
				if (curr->type != $4.type) {
					ERROR(stderr, "Sematic Error2. Incorrect type");
					SEM_ERROR = 1;
				}
				/* list - array check*/
				if (curr->cat != $4.c_type) {
					if (curr->cat == C_list &&
					    $4.c_type == C_array) {
					/* check whether values == 0 */
						
						/* !_insert_list_! */
						curr->id_info.init_n.init = head_init; /*&($4.init);*/
					} else if (curr->cat == C_array &&
						   $4.c_type == C_variable) {
					/*Initialize all other elements with 0 */
					
						/* !_insert_list_! */
						curr->id_info.init_n.init = head_init;/*&($4.init);*/
					
					} else {
						ERROR(stderr, "Sematic Error2. Incorrect category");
						SEM_ERROR = 1;
					}
				} else {
					/* !_insert_list_! */
					curr->id_info.init_n.init = head_init;/*&($4.init);*/
				}
				/* pre initializations */
				/* 2.2 if id is variable we want to assign its initialization value to symbol table */
				/* if id is array or list we follow different procedures (me tous xwrous dedomenwn?)*/
				/*if(initialize_id(curr,$4.str)){
					ERROR(stderr, "Memory Allocation Error.");
					SEM_ERROR = 1;
				}*/
				
			}
		}
		| ID value_list
		{ 
			curr = context_check($1);
			if (curr != NULL) {
				/*validate the consistency of the initializations*/
				if(curr->type != $2.type){
					/*printf("type %s and cat %s\n",typeNames[$2.type],catNames[$2.cat]);*/
					ERROR(stderr, "Sematic Error1. Initialization");
					SEM_ERROR = 1;
				}
				if (curr->cat != $2.c_type) {
					if (curr->cat == C_list &&
					    $2.c_type == C_array) {
					/* check whether values == 0*/
						
						/* !_insert_list_! */
						curr->id_info.init_n.init = head_init;/*&($2.init);*/
						
					} else if (curr->cat == C_array &&
					           $2.c_type == C_variable) {
					/*Initialize all other elements with 0*/
					
						/* !_insert_list_! */
						curr->id_info.init_n.init = head_init;/*&($2.init);*/	
					} else {
						ERROR(stderr, "Sematic Error1. Incorrect "
							"category ID %s, type %s", catNames[curr->cat],
										   catNames[$2.c_type]);
						SEM_ERROR = 1;
					}
				} else{	/* !_insert_list_! */
					curr->id_info.init_n.init = head_init;/*&($2.init);*/
				}
				/* pre initializations */
				/* 2.2 if id is variable we want to assign its initialization value to symbol table */
				/* if id is array or list we follow different procedures (me tous xwrous dedomenwn?)*/
				/*if(initialize_id(curr,$2.str)){
					ERROR(stderr, "Memory Allocation Error.");
					SEM_ERROR = 1;
				}*/
			}
		}
		;
value_list	: DIVOP values DIVOP
		{
			$$ = $2;
		}
		;
values		: values COMMA value
		{
			init_values *curr;

			if ($1.type == $3.type) {
				$$.type = $1.type;
				/* I have many values therefore array (or list?)*/
				$$.c_type = C_array;
				$$.params = $1.params + $3.params;
				
				/*if(head_init == NULL){
					printf("Say sth bro \n");
				}*/
				
				/* time to form a list ! */
				/* first initializatins go to the back of the list */
				for (curr = head_init; curr != NULL; curr = curr->next) {
					if( NULL == curr->next) {
						curr->next = init;
						break;	/* xxaxaxaxa kalo ayto to ksexasa :P */
					}
				}
				/*head->next = init;	 9gag meme here -genius- */
			}
		}
		| value
		{
			
			$$.type = $1.type;
			$$.c_type = $1.c_type;
			$$.params = 1;

			/* initializations node :: first on the list */
			head_init = init;
		}
		;
value		: repeat sign constant
		{	/* we definately have many elements here::> either list or array */
			
			/* we assume here it is an array .. if the id initialized here
			 * is list we fix it in the above levels of parsing
			 */
			$$.c_type = C_array;		
			$$.type = $3.type;
			
			if($1 < 0 && $1 != -1){
				ERROR(stderr, "Negative operator in initialization semantics");
				SEM_ERROR = 1;
			} else {
				/* init node initialization */
				init = create_init_node();
				init = initialize_node(init,$3.type,$3.basic_types,$1);
				
				/* check sign */
				if ($2 != '%') {
					if ($3.type == TY_character ||
					    $3.type == TY_logical ||
					    $3.type == TY_string) {
						ERROR(stderr, "Sematic fault. Incorrect type");
						SEM_ERROR = 1;
					} else {
						if ($2 == '+') { /* nothing */ }
						else {	/* ADDOP is '-' */
							if ($3.type == TY_integer) {
								init->initialization.intval =
									-init->initialization.intval;
							} else if ($3.type == TY_real) {
								init->initialization.realval =
									-init->initialization.realval;
							} else if ($3.type == TY_complex) {
								init->initialization.complex.c_real =
									-init->initialization.complex.c_real;
								init->initialization.complex.c_imag = 
									-init->initialization.complex.c_imag;
							}
						}
					}
				}
			}
		}
		| ADDOP constant
		{	
			if ($2.type == TY_character || $2.type == TY_logical ||
			    $2.type == TY_string) {
				ERROR(stderr, "Sematic fault. Incorrect type");
				SEM_ERROR = 1;
			} else {
				$$.type = $2.type;
				$$.c_type = C_variable;
				
				init = create_init_node();
				init = initialize_node(init,$2.type,$2.basic_types,1);
				
				if ($1 == '+') { /* nothing */ }
				else {	/* ADDOP is '-' */
					if ($2.type == TY_integer) {
						init->initialization.intval =
							-init->initialization.intval;
					} else if ($2.type == TY_real) {
						init->initialization.realval =
							-init->initialization.realval;
					}
					else if ($2.type == TY_complex) {
						init->initialization.complex.c_real =
							-init->initialization.complex.c_real;
						init->initialization.complex.c_imag =
							-init->initialization.complex.c_imag;
					}
				}
			}
		}
		| constant
		{
			$$.type = $1.type;
			$$.c_type = C_variable;

			/* init node initialization */
			init = create_init_node();
			init = initialize_node(init, $1.type, $1.basic_types, 1);
		}
		;
repeat		: ICONST MULOP	{ $$ = $1; }
		| MULOP		{ $$ = -1; /* -1:for infinite */ }
		;
sign		: ADDOP		{ $$ = $1; }
		| /* empty */	{ $$ = '%'; }
		;
constant	: simple_constant	{ $$ = $1; }
		| complex_constant	{ $$ = $1; }
		;
simple_constant	: ICONST
		{
			$$.type = TY_integer;
			$$.basic_types.intval = $1;
			mkleaf_int($1);
		}
		| RCONST
		{
			$$.type = TY_real;
			$$.basic_types.realval = $1;
			mkleaf_real($1);
		}
		| LCONST
		{
			$$.type = TY_logical;
			$$.basic_types.charval = $1;
			mkleaf_bool($1);
		}
		| CCONST
		{
			$$.type = TY_character;
			$$.basic_types.charval = $1;
			mkleaf_char($1);
		}
		| SCONST
		{
			$$.type = TY_string;
			$$.basic_types.string = $1;
			mkleaf_string($1);
		}
		| error
		{
			ERROR(stderr, ER_UNKNWN_CONST(FRED("")));
			$$.type = TY_invalid;
			FLAG_ERROR = 1;
			SEM_ERROR = 1;
		}
		;
complex_constant: LPAREN RCONST COLON sign RCONST RPAREN %prec T_COMPLEX
		{
			$$.type = TY_complex;
			$$.basic_types.complex.c_real = $2;
			if ($4 == '%' || $4 == '+')
				$$.basic_types.complex.c_imag = $5;
			else
				$$.basic_types.complex.c_imag = -$5;

			AST_expr_T *reall = mkleaf_real($2);
			AST_expr_T *realr = mknode_nsign(mkleaf_real($5));
			$$.ast.expr_node = mknode_paren(NULL, mknode_colon(reall, realr));
 		}
		;
statements	: statements labeled_statement
		| labeled_statement
		| error
		{
			ERROR(stderr, "Watch out the statements");
			SEM_ERROR = 1;
		}
		;
labeled_statement: label statement
		| statement
		;
label		: ICONST { $$ = mkleaf_int($1); }
		;
statement	: simple_statement
		| compound_statement
		;
simple_statement: assignment
		| goto_statement
		| if_statement
		| subroutine_call
		| io_statement
		| CONTINUE
		| RETURN
		| STOP
		;
assignment	: variable ASSIGN expression
		{
			/*print_expr($3, 0);*/
			mkcmd_assign($3);	/*evala cmd node edw !!!*/
		}
		;
variable	: ID LPAREN expressions RPAREN
		{
			/*list_t *id = install($1, 99);*/
			$$ = mknode_paren(mkleaf_id(id), $3);
		}
		| LISTFUNC LPAREN expression RPAREN
		{
			$$ = $3;
		}
		| ID
		{
			/*list_t *id = install($1, 99);*/
			$$ = mkleaf_id(id);
		}
		;
expressions	: expressions COMMA expression	{ $$ = mknode_comma($1, $3);}
		| expression			{ $$ = $1; }
		;
expression	: expression OROP expression	{ $$ = mknode_or($1, $3);   }
		| expression ANDOP expression	{ $$ = mknode_and($1, $3);  }
		| expression RELOP expression	{ $$ = mknode_gt($1, $3);   }
		| expression ADDOP expression	{ $$ = mknode_plus($1, $3); }
		| expression MULOP expression	{ $$ = mknode_mul($1, $3);  }
		| expression DIVOP expression	{ $$ = mknode_div($1, $3);  }
		| expression POWEROP expression	{ $$ = mknode_pow($1, $3);  }
		| NOTOP expression		{ $$ = mknode_not($2);      }
		| ADDOP expression		{ $$ = mknode_psign($2);    }
		| variable			{ $$ = $1; }
		| simple_constant		{ $$ = $1; }
		| LENGTH LPAREN expression RPAREN	{ $$ = $3; }
		| NEW LPAREN expression RPAREN		{ $$ = $3; }
		| LPAREN expression RPAREN		{ $$ = $2; }
		| LPAREN expression COLON expression RPAREN	{ $$ = mknode_colon($2, $4); }
		| listexpression			{ $$ = $1; }
		;
listexpression	: LBRACK expressions RBRACK %prec T_BRACKETS	{ $$ = $2; }
		| LBRACK RBRACK %prec T_BRACKETS		{ $$ = NULL; }
		;
goto_statement	: GOTO label { mkcmd_assign($2); }	/*evala cmd node edw !!!*/
		| GOTO ID COMMA LPAREN labels RPAREN {
			/*add_identifier(my_hashtable, $2, scope, 1);*/
			mkcmd_goto($5);		/*evala cmd node edw !!!*/
		}
		;
labels		: labels COMMA label { $$ = mknode_comma($1, $3); }	/*/evala cmd node edw !!!*/
		| label { $$ = $1; }
		;
if_statement	: IF LPAREN expression RPAREN label COMMA label COMMA label
		| IF LPAREN expression RPAREN simple_statement
		;
subroutine_call	: CALL variable
		;
io_statement	: READ read_list
		| WRITE write_list
		;
read_list	: read_list COMMA read_item
		| read_item
		;
read_item	: variable
		| LPAREN read_list COMMA ID ASSIGN iter_space RPAREN {/*add_identifier(my_hashtable, $4, scope, 1);*/}
		;
iter_space	: expression COMMA expression step
		;
step		: COMMA expression
		| /* empty */
		;
write_list	: write_list COMMA write_item
		| write_item
		;
write_item	: expression
		| LPAREN write_list COMMA ID ASSIGN iter_space RPAREN {/*add_identifier(my_hashtable, $4, scope, 1);*/}
		;
compound_statement: branch_statement
		| loop_statement
		;
branch_statement: IF LPAREN expression RPAREN THEN body tail
		;
tail		: ELSE body ENDIF
		| ENDIF
		;
loop_statement	: DO ID ASSIGN iter_space body ENDDO {/*add_identifier(my_hashtable, $2, scope, 1);*/}
		;
subprograms	: subprograms subprogram
		| /* empty */
		;
subprogram	: {scope++;/*put*/} header {scope--;/*do not pop anything*/} body END
		| error END { ERROR(stdout, "Syntax error in subprogram"); }
		;
header		: type listspec FUNCTION ID LPAREN formal_parameters RPAREN {/*add_identifier(my_hashtable, $4, scope, 3);*/}
		| SUBROUTINE ID LPAREN formal_parameters RPAREN {/*add_identifier(my_hashtable, $2, scope, 4);*/}
		| SUBROUTINE ID {/*add_identifier(my_hashtable, $2, scope, 4);*/}
		;
formal_parameters: type vars COMMA formal_parameters
		| type vars
		;
%%

/* void install (Type type, Complex_Type c_type, char *sym_name)
 *
 * Checks if the identifier is already defined. If not, install him.
 */
list_t *install(Type type, Complex_Type c_type, char *sym_name)
{
	list_t *id = NULL;
	id = lookup_identifier(my_hashtable, sym_name, scope);
	if (!id) {
		add_identifier(my_hashtable, sym_name, scope, type, c_type);
	} else {
		ERROR(stderr, "%s is already defined", sym_name);
	}
	return id;
}

/* void context_check (char * sym_name)
 *
 * Checks if the identifier is defined. If yes, generate code.
 */
list_t *context_check(/*enum code_ops operation, */char *sym_name)
{
	list_t *id = NULL;
	id = lookup_identifier(my_hashtable, sym_name, scope);
	
	if (!id) {
		ERROR(stderr, "%s is undefined", sym_name);
	} else {
		/* gen_code(operation, id->offset); */
	}
	return id;
}

/*  char *str_append(char *source_1, char *source_2)
 *
 *	Appends 2 strings into one bigger string. The two strings are 
 *	seperated from the special character '%'
 */
char* str_append(char *source_1, char *source_2)
{
	char *hlp_string;

	hlp_string = malloc(strlen(source_1) + strlen(source_2) + NULL_CHAR_SIZE + 1);
	strcpy(hlp_string, source_1);
	strcat(hlp_string, "%");
	strcat(hlp_string, source_2);
	/* free .. ayto i polu kala tha paei i kata diaolou ... de peirazei .... */
	/*free(source_1);*/
	free(source_2);

	return hlp_string;
}

/* init_values *create_init_node(char *type)
 *
 * Creates a new node that has the initialization value of one element
 */
init_values *create_init_node(void)
{
	init_values *node;

	node = malloc(sizeof(init_values));
	if (NULL == node) {
		ERROR(stderr, "Memory Allocation Error\n");
		SEM_ERROR = 1;
	}			

	return node ;
}

/* init_values *initialize_node(init_values *node, Type type_,
 *                              initialization_t constant, int no_of_occurences)
 *
 * Sets the values of an init_node element
 */
init_values *initialize_node(init_values *node,
                             Type type_,
                             initialization_t constant,
                             int no_of_occurences)
{

	if (NULL == node) {
		ERROR(stderr, "Warning cast to null structure\n ");
		SEM_ERROR = 1;
		return NULL;
	}

	/* initialize value node */
	node->num = no_of_occurences;
	node->next = NULL;

	switch (type_) {
	case TY_integer:
		node->initialization.intval = constant.intval;
		break;
	case TY_real:
		node->initialization.realval = constant.realval;
		break;
	case TY_character:
		node->initialization.charval = constant.charval;
		break;
	case TY_logical:
		node->initialization.charval = constant.charval;
		break;
	case TY_string:
		node->initialization.string = strdup(constant.string);
		break;
	case TY_complex:
		node->initialization.complex.c_real = constant.complex.c_real;
		node->initialization.complex.c_imag = constant.complex.c_imag; 
		break;
	default:
		/*nothing -> should not go here*/
	}

	return node;
}

void yyerror(char *msg)
{
	fprintf(stdout, "Error from default: %s\n", msg);
}
