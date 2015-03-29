SRC=shell/
TEMP=tmp/
BINARY=bin/shell
OBJ=obj/
MKDIR = -@mkdir $(@D) 2>/dev/null || true # use || true to hide the error
SH_OBJS=ArgumentList.o builtins.o main.o shell.o
SH_LIST=$(addprefix $(OBJ), $(SH_OBJS))
RM=rm -f

CC=cc
LD=cc
CFLAGS=-c
LDFLAGS=-I $(SRC)

all: $(BINARY)

$(BINARY): $(TEMP)lex.yy.c $(TEMP)y.tab.c $(SH_LIST)
	$(MKDIR)
	$(LD) $(LDFLAGS) $(TEMP)lex.yy.c $(TEMP)y.tab.c $(SH_LIST) -o $(BINARY)

$(TEMP)lex.yy.c: $(TEMP) $(SRC)lexer.l
	$(MKDIR)
	lex --outfile $(TEMP)lex.yy.c $(SRC)lexer.l 

$(TEMP)y.tab.c:	$(TEMP)	$(SRC)parser.y
	$(MKDIR)
	yacc -d $(SRC)parser.y --output-file $(TEMP)y.tab.c

$(OBJ)%.o: $(SRC)%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $< -o $@
	
	




