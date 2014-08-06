#ifndef UTILS
#define UTILS

#define NULL_CHAR_SIZE	1
#define MAX_STR_LEN	256

#define ASCII_a	97
#define ASCII_A	65
#define ASCII_0	48

#define ASCII_is_in_0_9(a)	(((a) >= ASCII_0) && ((a) <= (ASCII_0 + 9)))
#define ASCII_is_in_A_F(a)	(((a) >= ASCII_A) && ((a) <= (ASCII_A + 5)))
#define ASCII_is_in_a_f(a)	(((a) >= ASCII_a) && ((a) <= (ASCII_a + 5)))

#define HEX_base	16
#define BIN_base	2
#define OCT_base	8

#include <stdio.h>

extern int LINENO;		/* line numbering */
extern int FR_COL;		/* column numbering */
extern int TO_COL;

extern int FLAG_ERROR;	/* error flag */
extern char *INPUT_FILE_NAME;

void LINE (const char *before, const char *after);
int ERROR (FILE *stream, const char *fmsg, ...);
void INFO_REFRESH(void);

int convert_hex_bin_oct_to_dec (const char *input);
double convert_hex_bin_oct_to_rconst (const char *input);

char char_value (const char *input);

#endif	/* UTILS */
