#ifndef _FORTERRORS_
#define _FORTERRORS_

/* LEXICAL ANALYSIS ERRORS */
#define ER_UNTERM_STR		"unterminated string constant"
#define ER_LARGE_STR		"string literal length bigger than 256 bytes"
#define ER_UNKNWN_LEX(x)	"unknown lexeme " x " found"

/* SYNTACTIC ANALYSIS ERRORS */
#define ER_ID_REDEF(x)		x " is already defined"
#define ER_UNDEF_ID(x)		x " is undeclared"
#define ER_UNKNWN_CONST(x)	"unknown type of constant " x

/* SEMANTIC ANALYSIS ERRORS */
#define ER_TP_MSMCH(x,y)	"type missmatch: "  "wrong use of " x " operation: " y

#endif	/* _FORTERRORS_ */
