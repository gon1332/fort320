#ifndef AST_H
#define AST_H

typedef enum {
	EXPR_ID=0, EXPR_INT, EXPR_REAL,  EXPR_BOOL,   EXPR_STR,   EXPR_CHAR,
       	EXPR_OR,   EXPR_AND, EXPR_NOT,   EXPR_GT,     EXPR_GE,    EXPR_LT,
	EXPR_LE,   EXPR_EQ,  EXPR_NE,    EXPR_PLUS,   EXPR_MINUS, EXPR_MUL,
	EXPR_DIV,  EXPR_POW, EXPR_PAREN, EXPR_ASSIGN, EXPR_COLON, EXPR_BRACK,
	EXPR_COMMA
} ExprNodeTag;

typedef enum {
	CMD_ASSIGN=0, CMD_GOTO, CMD_S_IF, CMD_C_IF, CMD_CALL, CMD_IO,
	CMD_C_FLOW,   CMD_LOOP
} CmdNodeTag;

extern char *expr_lookup[];
extern char *cmds_lookup[];

struct ExprNode {
	ExprNodeTag kind;
	union {
		int              intval;	/* ---- */
		double           realval;	/*      */
		char             charval;	/* leaf */
		char            *stringval;	/*      */
		void            *id_entry;	/* ---- */
		struct ExprNode *opds[2];	/* node */
	} description;
};

typedef struct ExprNode AST_expr_T;

struct CmdNode {
	CmdNodeTag kind;
	struct CmdNode *next;

	AST_expr_T     *expr;
	struct CmdNode *cmds[2];
};

typedef struct CmdNode AST_cmd_T;

/*  -----   GLOBAL INDECES   ----------------------------------------------  */
AST_cmd_T  *AST_head,   /* Points to the first command of the program. */
           *AST_tail;   /* Points to the last command of the program. */

/*  -----   MACRO DEFINITIONS   -------------------------------------------  */
#define mknode_plus(y,z)	mknode(EXPR_PLUS,y,z)
#define mknode_minus(y,z)	mknode(EXPR_MINUS,y,z)
#define mknode_mul(y,z)		mknode(EXPR_MUL,y,z)
#define mknode_div(y,z)		mknode(EXPR_DIV,y,z)
#define mknode_pow(y,z)		mknode(EXPR_POW,y,z)
#define mknode_paren(y,z)	mknode(EXPR_PAREN,y,z)	/* y(z) */
#define mknode_assign(y,z)	mknode(EXPR_ASSIGN,y,z)
#define mknode_colon(y,z)	mknode(EXPR_COLON,y,z)
#define mknode_brack(y,z)	mknode(EXPR_BRACK,y,z)  /* y[z] */
#define mknode_comma(y,z)	mknode(EXPR_COMMA,y,z)
#define mknode_or(y,z)		mknode(EXPR_OR,y,z)
#define mknode_and(y,z)		mknode(EXPR_AND,y,z)
#define mknode_not(y)		mknode(EXPR_NOT,y,NULL)
#define mknode_gt(y,z)		mknode(EXPR_GT,y,z)
#define mknode_ge(y,z)		mknode(EXPR_GE,y,z)
#define mknode_lt(y,z)		mknode(EXPR_LT,y,z)
#define mknode_le(y,z)		mknode(EXPR_LE,y,z)
#define mknode_ne(y,z)		mknode(EXPR_NE,y,z)
#define mknode_psign(y)		mknode(EXPR_PLUS,y,NULL)
#define mknode_nsign(y)		mknode(EXPR_MINUS,y,NULL)

#define mkcmd_assign(x)		mkcmd(CMD_ASSIGN,x,NULL,NULL)
#define mkcmd_goto(x)		mkcmd(CMD_GOTO,x,NULL,NULL)
#define mkcmd_simple_if(x,y)	mkcmd(CMD_S_IF,x,y,NULL)
#define mkcmd_if(x,y,z)		mkcmd(CMD_C_IF,x,y,z)
#define mkcmd_call(x)		mkcmd(CMD_CALL,x,NULL,NULL)
#define mkcmd_io(x,y)		mkcmd(CMD_IO,x,y,NULL)
#define mkcmd_ctrl_flow(x)	mkcmd(CMD_C_FLOW,NULL,NULL,NULL)
#define mkcmd_loop(x,y)		mkcmd(CMD_LOOP,x,y,NULL)

/*  -----   EXTERNAL FUNCTION DECLARATIONS   ------------------------------  */
extern AST_expr_T *mkleaf_id    (                  void   *id_entry);
extern AST_expr_T *mkleaf_int   (register          int     number);
extern AST_expr_T *mkleaf_real  (register          double  number);
extern AST_expr_T *mkleaf_char  (register unsigned char    character);
extern AST_expr_T *mkleaf_bool  (register unsigned char    logical);
extern AST_expr_T *mkleaf_string(register const    char   *string);
extern AST_expr_T *mknode       (ExprNodeTag  op,
                                 AST_expr_T  *ch_1,
                                 AST_expr_T  *ch_2);
extern AST_cmd_T  *mkcmd        (CmdNodeTag   cmd,
                                 AST_expr_T  *ch_1,
                                 AST_cmd_T   *ch_2,
                                 AST_cmd_T   *ch_3);
extern void AST_init (void);
extern void print_ast(void);
#endif	/* AST_H */
