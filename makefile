#=-------------------------------------------=#
# Makefile for lexical and syntactic analyzer #
#                    FORT320                  #
#=-------------------------------------------=#
OBJ_FILES =	fort320_ast_t.o fort320_utils.o fort320_list.o fort320_types.o\
		fort320_sa.o fort320_la.o\
		fort320_hash_t.o fort320_errcheck.o fort320_strdup.o fort320_main.o
EXEC	= ./fort320
# Paths #
INCLUDE	= ./include
LIB	= ./src/lib
SURFACE	= ./src
BUILD	= ./build
OBJS	= $(addprefix $(BUILD)/, $(OBJ_FILES))

# Compiler stuff #
CC	= clang
CFLAGS	= -std=c89 -Iinclude/ -g -Wall -Wextra
CLIBS	= -lfl -lm

fort320: info build_dir $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(CLIBS)

build_dir:
	mkdir -p $(BUILD)

info:
	$(info  + ===========================================================)
	$(info  | Compiling and building *fort320*                           )
	$(info  + ===========================================================)


$(BUILD)/%.o:
	$(CC) $(CFLAGS) -c -o $@ $<

# Our extra libraries #
$(BUILD)/fort320_strdup.o: $(LIB)/Utils/strdup.c $(INCLUDE)/Utils/strdup.h
$(BUILD)/fort320_utils.o: $(LIB)/Utils/utils.c $(INCLUDE)/Utils/utils.h
$(BUILD)/fort320_list.o: $(LIB)/InputBuffer/mylist.c $(INCLUDE)/InputBuffer/mylist.h
$(BUILD)/fort320_hash_t.o: $(LIB)/SymbolTable/hash_t.c  $(INCLUDE)/SymbolTable/hash_t.h
$(BUILD)/fort320_types.o: $(LIB)/Types/types.c $(INCLUDE)/Types/types.h
$(BUILD)/fort320_ast_t.o: $(LIB)/IR/AST.c  $(INCLUDE)/IR/AST.h
$(BUILD)/fort320_errcheck.o: $(LIB)/DebugInfo/errcheck.c $(INCLUDE)/DebugInfo/errcheck.h

# Syntactical Parser #
$(BUILD)/fort320_sa.o: $(SURFACE)/fort320_sa.c

$(SURFACE)/fort320_sa.c: $(SURFACE)/fort320_sa.y
	bison -dv $(SURFACE)/fort320_sa.y
	mv fort320_sa.output $(SURFACE)/fort320_sa.output
	mv fort320_sa.tab.c $(SURFACE)/fort320_sa.c
	cmp -s fort320_sa.tab.h $(SURFACE)/tok.h ||\
		mv fort320_sa.tab.h $(SURFACE)/tok.h

# Lexical Analyzer #
$(BUILD)/fort320_la.o: $(SURFACE)/fort320_la.c

$(SURFACE)/fort320_la.c: $(SURFACE)/fort320_la.l
	flex $(SURFACE)/fort320_la.l
	mv lex.yy.c $(SURFACE)/fort320_la.c

# The main program #
$(BUILD)/fort320_main.o: $(SURFACE)/fort320_main.c
	$(CC) $(CFLAGS) -c $(SURFACE)/fort320_main.c -o $(BUILD)/fort320_main.o

#fort320_la.o fort320_sa.o fort320_main.o: $(INCLUDE)/InputBuffer/mylist.h
#fort320_la.o fort320_main.o		: tok.h

clean:
	$(info  + ===========================================================)
	$(info  | Cleaning build directory and temporary files               )
	$(info  + ===========================================================)
	rm -f	$(SURFACE)/*~ $(INCLUDE)/*~ $(LIB)/*~ $(INCLUDE)/DebugInfo/*~\
		$(INCLUDE)/InputBuffer/*~ $(INCLUDE)/IR/*~ $(INCLUDE)/SymbolTable/*~\
		$(INCLUDE)/Utils/*~ $(LIB)/DebugInfo/*~ $(LIB)/InputBuffer/*~\
		$(INCLUDE)/Types/*~ $(LIB)/Types/*~\
		$(LIB)/IR/*~ $(LIB)/SymbolTable/*~ $(LIB)/Utils/*~
	rm -f -r $(BUILD)
	rm -f 	$(SURFACE)/fort320_la.c $(SURFACE)/lex.yy.c $(SURFACE)/fort320_sa.c\
		$(SURFACE)/tok.h $(SURFACE)/fort320_sa.tab.c $(SURFACE)/fort320_sa.tab.h\
		$(SURFACE)/fort320_sa.output ./fort320
