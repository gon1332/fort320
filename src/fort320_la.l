/*
 * Description: Lexical Analysis for FORTRAN320
 * Usage: $ flex test.l
 *	  $ gcc -o a.out lex.yy.c -lfl
 *	  $ ./a.out file.f
 */

%{
	#include "../include/InputBuffer/mylist.h"
	#include "../include/Utils/utils.h"
	#include "../include/SymbolTable/hash_t.h"
	#include "../include/IR/AST.h"
	#include "tok.h"

	#define YY_USER_ACTION { FR_COL = TO_COL; FR_COL++; TO_COL += yyleng; }

	int LINENO = 1;		/* line numbering */
	int FR_COL = 0;		/* column numbering */
	int TO_COL = 0;

	int FLAG_ERROR = 0;
	char *INPUT_FILE_NAME = NULL;

	char *input_string = NULL;
%}

/* definitions */
%x IN_STRING
%x IN_STRING_ERROR

BIN_0	[01]
BIN_1	[1]
DEC_0	[0-9]
DEC_1	[1-9]
OCT_0	[0-7]
OCT_1	[1-7]
HEX_0	[0-9A-Fa-f]
HEX_1	[1-9A-Fa-f]

EXP	[Ee][+-]?{DEC_0}+

LETTER		[A-Za-z]
PRNT_ASCII	[ -~]
W		[ \t]
ALPHANUM	{LETTER}|{DEC_0}

CCONST		('{PRNT_ASCII}')|('\\[nftrbv]')

DEC_ICONST	0|{DEC_1}{DEC_0}*
HEX_ICONST	0[Xx]{HEX_1}{HEX_0}*
OCT_ICONST	0[Oo]{OCT_1}{OCT_0}*
BIN_ICONST	0[Bb]{BIN_1}{BIN_0}*
ICONST		{DEC_ICONST}|{HEX_ICONST}|{OCT_ICONST}|{BIN_ICONST}

DEC_RCONST	((0|{DEC_1}{DEC_0}*)[.])|((0[.]|{DEC_1}{DEC_0}*[.]|[.])({DEC_1}{DEC_0}*|0+{DEC_1}{HEX_0}*))
DEC_RCONST_EXP	{DEC_RCONST}{EXP}|{DEC_1}{DEC_0}*{EXP}
HEX_RCONST	0[Xx](((0|{HEX_1}{HEX_0}*)[.])|((0[.]|{HEX_1}{HEX_0}*[.]|[.])({HEX_1}{HEX_0}*|0+{HEX_1}{HEX_0}*)))
OCT_RCONST	0[Oo](((0|{OCT_1}{OCT_0}*)[.])|((0[.]|{OCT_1}{OCT_0}*[.]|[.])({OCT_1}{OCT_0}*|0+{OCT_1}{OCT_0}*)))
BIN_RCONST	0[Bb](((0|{BIN_1}{BIN_0}*)[.])|((0[.]|{BIN_1}{BIN_0}*[.]|[.])({BIN_1}{BIN_0}*|0+{BIN_1}{BIN_0}*)))
RCONST		{DEC_RCONST}|{DEC_RCONST_EXP}|{HEX_RCONST}|{OCT_RCONST}|{BIN_RCONST}

%%
FUNCTION|function	{ list_insert(yytext); return (FUNCTION); }
SUBROUTINE|subroutine	{ list_insert(yytext); return (SUBROUTINE); }
END|end			{ list_insert(yytext); return (END); }
COMMON|common		{ list_insert(yytext); return (COMMON); }
INTEGER|integer		{ list_insert(yytext); return (INTEGER); }
REAL|real		{ list_insert(yytext); return (REAL); }

COMPLEX|complex		{ list_insert(yytext); return (COMPLEX); }
LOGICAL|logical		{ list_insert(yytext); return (LOGICAL); }
CHARACTER|character	{ list_insert(yytext); return (CHARACTER); }
STRING|string		{ list_insert(yytext); return (STRING); }
LIST|list		{ list_insert(yytext); return (LIST); }
DATA|data		{ list_insert(yytext); return (DATA); }

CONTINUE|continue	{ list_insert(yytext); return (CONTINUE); }
GOTO|goto		{ list_insert(yytext); return (GOTO); }
CALL|call		{ list_insert(yytext); return (CALL); }
READ|read		{ list_insert(yytext); return (READ); }
WRITE|write		{ list_insert(yytext); return (WRITE); }
LENGTH|length		{ list_insert(yytext); return (LENGTH); }
NEW|new			{ list_insert(yytext); return (NEW); }

IF|if			{ list_insert(yytext); return (IF); }
THEN|then		{ list_insert(yytext); return (THEN); }
ELSE|else		{ list_insert(yytext); return (ELSE); }
ENDIF|endif		{ list_insert(yytext); return (ENDIF); }
DO|do			{ list_insert(yytext); return (DO); }
ENDDO|enddo		{ list_insert(yytext); return (ENDDO); }
STOP|stop		{ list_insert(yytext); return (STOP); }
RETURN|return		{ list_insert(yytext); return (RETURN); }

{ICONST}	{	/* ICONST */
	list_insert(yytext);
	yylval.intval = convert_hex_bin_oct_to_dec(yytext);
	return (ICONST);
}

{RCONST}	{	/* RCONST */
	list_insert(yytext);
	yylval.realval = convert_hex_bin_oct_to_rconst(yytext);
	return (RCONST);
}

{CCONST}	{	/* CCONST */
	list_insert(yytext);
	yylval.charval = char_value(yytext);
	return (CCONST);
}

C(AD*|D+)R	{	/* LISTFUNC */
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (LISTFUNC);
}

{LETTER}(([_]?{ALPHANUM})*[_]|{ALPHANUM}*)	{	/* ID */
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (ID);
}

\.(GT|gt)\.	{	/* RELOP GT*/
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (RELOP);
}

\.(GE|ge)\.	{	/* RELOP GE*/
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (RELOP);

}

\.(LT|lt)\.	{	/* RELOP LT*/
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (RELOP);

}

\.(LE|le)\.	{	/* RELOP LE*/
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (RELOP);

}

\.(EQ|eq)\.	{	/* RELOP EQ*/
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (RELOP);
}

\.(NE|ne)\.	{	/* RELOP NE*/
	list_insert(yytext);
	yylval.stringval = strdup(yytext);
	return (RELOP);
}

\.(TRUE|true)\.	{	/* LCONST_TRUE */
	list_insert(yytext);
	yylval.charval = 1;
	return (LCONST);
}


\.(FALSE|false)\.	{	/* LCONST_FALSE */
	list_insert(yytext);
	yylval.charval = 0;
	return (LCONST);
}

\.(OR|or)\.	{	/* OROP */
	list_insert(yytext);
	return (OROP);
}

\.(AND|and)\.	{	/* ANDOP */
	list_insert(yytext);
	return (ANDOP);
}

\.(NOT|not)\.	{	/* NOTOP */
	list_insert(yytext);
	return (NOTOP);
}

"+"|"-"	{	/* ADDOP */
	list_insert(yytext);
	yylval.charval = strcmp(yytext, "+") ? '-' : '+' ;
	return (ADDOP);
}

"*"	{	/* MULOP */
	list_insert(yytext);
	return (MULOP);
}

"/"	{	/* DIVOP */
	list_insert(yytext);
	return (DIVOP);
}

"**"	{	/* POWEROP */
	list_insert(yytext);
	return (POWEROP);
}

"("	{	/* LPAREN */
	list_insert(yytext);
	return (LPAREN);
}

")"	{	/* RPAREN */
	list_insert(yytext);
	return (RPAREN);
}

","	{	/* COMMA */
	list_insert(yytext);
	return (COMMA);
}

"="	{	/* ASSIGN */
	list_insert(yytext);
	return (ASSIGN);
}

":"	{	/* COLON */
	list_insert(yytext);
	return (COLON);
}

"["	{	/* LBRACK */
	list_insert(yytext);
	return (LBRACK);
}

"]"	{	/* RBRACK */
	list_insert(yytext);
	return (RBRACK);
}

"$".*\n	{	/* COMMENTS */
	INFO_REFRESH();
}

<INITIAL>"\""	{	/* Begin a string literal */
	input_string = malloc (MAX_STR_LEN);
	strcpy(input_string, "");
	list_insert(yytext);
	BEGIN(IN_STRING);
}
<IN_STRING>{	/* Valid lexemes in a string */
	"\""	{	/* End of a string literal */
		list_insert(yytext);
		yylval.stringval = strdup(input_string);
		free(input_string);
		BEGIN(INITIAL);
		return (SCONST);
	}
	"\\"\n	{	/* '\' character and continue in a new line */
		list_insert(yytext);
		INFO_REFRESH();
	}
	"\\"{PRNT_ASCII}	{	/* '\' followed by any printable ASCII character */
		if (strlen(input_string) + strlen(yytext) + NULL_CHAR_SIZE > MAX_STR_LEN) {
			BEGIN(IN_STRING_ERROR);
		} else {
			strcat(input_string, yytext);
			list_insert(yytext);
		}
	}
	{PRNT_ASCII}	{	/* Any printable ASCII character */
		if (strlen(input_string) + strlen(yytext) + NULL_CHAR_SIZE > MAX_STR_LEN) {
			BEGIN(IN_STRING_ERROR);
		} else {
			strcat(input_string, yytext);
			list_insert(yytext);
		}
	}
	\\\\{W}*	{	/* The character '\' */ /* MAYBE ISN'T NEEDED: it's the same as "\\"[ -~] */
		if (strlen(input_string) + strlen(yytext) + NULL_CHAR_SIZE > MAX_STR_LEN) {
			BEGIN(IN_STRING_ERROR);
		} else {
			strcat(input_string, yytext);
			list_insert(yytext);
		}
	}
	\n	{	/* ERROR - unterminated string constant */
		FLAG_ERROR = 1;
		ERROR(stdout, "unterminated string constant");
		INFO_REFRESH();
	}
}
<IN_STRING_ERROR>{
	"\""	{	/* End of a string literal */
		FLAG_ERROR = 1;
		ERROR(stdout, "string literal length bigger than 256 bytes");
		free(input_string);
		BEGIN(INITIAL);
	}
	\n	{	/* ERROR - unterminated string constant */
		FLAG_ERROR = 1;
		INFO_REFRESH();
	}
	.	{ /* nothing */ }
}

{W}+	{	/* WHITESPACES */
	list_insert(yytext);
}

\n	{	/* NEW LINE */
	if (FLAG_ERROR) {
		FLAG_ERROR = 0;
		LINE("  ", "\n\n");
	}
	INFO_REFRESH();
}

.	{	/* If the lexeme is not supported */
	FLAG_ERROR = 1;
	list_insert(yytext);
	ERROR(stdout, "unknown lexeme \'%s\' found", yytext);
}

%%

