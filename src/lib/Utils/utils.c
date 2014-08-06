#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "../../../include/InputBuffer/mylist.h"
#include "../../../include/Utils/utils.h"

/* void fprint_list (const char *before, const char *after)
 *
 * Prints the list between 'before' and 'after' strings.
 */
void LINE (const char *before, const char *after)
{
	fprintf(stdout, "%s", before);
	print_list();
	fprintf(stdout, "%s", after);
}


/* int ERROR (FILE *stream, const char *fmsg, ...)
 *
 * Prints a formatted error message in the specified stream and returns the total
 * number of characters written.
 */
int ERROR (FILE *stream, const char *fmsg, ...)
{
	int ret;
	va_list arg;

	ret = fprintf(stream, "%s:%d:%d.%d: error: ", INPUT_FILE_NAME, LINENO, FR_COL, TO_COL);

	va_start(arg, fmsg);
	ret += vfprintf (stream, fmsg, arg);
	va_end(arg);

	ret += fprintf(stream, "\n");

	return (ret);
}


/* void inline INFO_REFRESH(void)
 *
 * - Initiates the list that keeps track of a single line
 * - Increments the line counter
 * - Initiates the column counters
 */
void INFO_REFRESH(void)
{
	list_delete();
	list_init();
	LINENO++;
	FR_COL = TO_COL = 0;
}


/* int convert_hex_bin_oct_to_dec (const char *input)
 *
 * Given a hexadecimal, binary, or octal number in string, returns it in its
 * decimal representation and as a type of integer.
 *
 * In terms of security, it does not check if the digits are valid for the given
 * representation. This function is made to support the parsing so it takes for
 * granted the validity of the data the lexer passes for computation.
 */
int convert_hex_bin_oct_to_dec (const char *input)
{
	int result = 0;
	int power = 0;
	int i = 0;

	if (NULL == input) {
		fprintf(stderr, "fort320: convert_hex_bin_oct_to_dec: error: NULL input.\n");
		exit(EXIT_FAILURE);
	}

	if ('0' == input[0]) {
		if ('B' == input[1] || 'b' == input[1]) {
			for (i = 2, power = strlen(input)-2-1; power >= 0 && input[i] != '\0'; power--, i++)
				if ('1' == input[i])
					result += pow((double) BIN_base, (double) power);
		}
		else if ('X' == input[1] || 'x' == input[1]) {
			for (i = 2, power = strlen(input)-2-1; power >= 0 && input[i] != '\0'; power--, i++) {
				if (ASCII_is_in_a_f(input[i]))
					result += (input[i] - ASCII_a) * pow((double) HEX_base, (double) power);
				else if (ASCII_is_in_A_F(input[i]))
					result += (input[i] - ASCII_A) * pow((double) HEX_base, (double) power);
				else if (ASCII_is_in_0_9(input[i]))
					result += (input[i] - ASCII_0) * pow((double) HEX_base, (double) power);
				else {
					fprintf(stderr, "fort320: convert_hex_bin_oct_to_dec: \
								error: Not a hex digit.\n");
					exit(EXIT_FAILURE);
				}
			}
		} else if ('O' == input[1] || 'o' == input[1]) {
			for (i = 2, power = strlen(input)-2-1; power >= 0 && input[i] != '\0'; power--, i++)
				result += (input[i] - ASCII_0) * pow((double) OCT_base, (double) power);
		}
	} else {
		result = atoi(input);
	}
	return (result);

}


double convert_hex_bin_oct_to_rconst (const char *input)
{
	double result = 0;
	int dot_pos = 0;
	int power = 0;
	int i = 0;

	if (NULL == input) {
		fprintf(stderr, "fort320: convert_hex_bin_oct_to_dec: error: NULL input.\n");
		exit(EXIT_FAILURE);
	}

	if ('0' == input[0]) {
		for (dot_pos = 0; input[dot_pos] != '.'; dot_pos++);
		dot_pos++;

		if ('B' == input[1] || 'b' == input[1]) {
			for (i = 2, power = dot_pos-3-1; input[i] != '\0'; i++) {
				if ('.' != input[i]) {
					if ('1' == input[i])
						result += pow((double) BIN_base, (double) power);
					power--;
				}
			}
		} else if ('O' == input[1] || 'o' == input[1]) {
			for (i = 2, power = dot_pos-3-1; input[i] != '\0'; i++)
				if ('.' != input[i]) {
					result += (input[i] - ASCII_0) * pow((double) OCT_base, (double) power);
					power--;
				}
		} else {
			result = atof(input);
		}
	} else {
		result = atof(input);
	}
	return (result);

}


char char_value (const char *input)
{
	if (4 == strlen(input)) {
		switch (input[2]) {
		case 'n':	return ('\n');
		case 'f':	return ('\f');
		case 't':	return ('\t');
		case 'r':	return ('\r');
		case 'b':	return ('\b');
		case 'v':	return ('\v');
		}
	}
	return (input[1]);

}
