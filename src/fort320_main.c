#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Utils/utils.h"
#include "InputBuffer/mylist.h"
#include "SymbolTable/hash_t.h"
#include "IR/AST.h"

void yyparse(void);

int main (int argc, char **argv)
{
	extern FILE *yyin;
	extern hash_table_t *my_hashtable;

	argv++, argc--;  /* skip over program name */
	if (argc > 0) {
		if (NULL == (yyin = fopen (argv[0], "r"))) {
			perror("fort320: error:");
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stdout, "fort320 usage: fort320 in_file.f\n");
		exit(EXIT_FAILURE);
	}

	if (NULL == (INPUT_FILE_NAME = malloc (NULL_CHAR_SIZE + strlen(argv[0])))) {
		perror("fort320: error:");
		exit(EXIT_FAILURE);
	}
	/* For error messages */
	strcpy(INPUT_FILE_NAME, argv[0]);
	list_init();

	/* Initialize my_hashtable (defined in parser)*/
	my_hashtable = create_hash_table(8);	/*custom size of 8 lists*/
	if (NULL == my_hashtable) {
		printf("Hash table allocation error\n");
		return -1;
	}

	/* Initialize AST */
	AST_init();

	/* Scan the file */
	yyparse();
	printf("======    Parse Completed   ==============\n\n");

        printf("======    Intermediate Representation  ===\n");
        print_ast();

        return 0;
}
