SRC=src/
TEMP=tmp/
OBJ=obj/
BIN=bin/
PROGRAM=shell
MKDIR = -@mkdir $(@D) 2>/dev/null || true # use || true to hide the error
SH_OBJS=ArgumentList.o builtins.o main.o shell.o job.o command.o
SH_LIST=$(addprefix $(OBJ), $(SH_OBJS))
RM=rm -f

CC=cc
LD=cc

READLINE_FLAG := $(shell test "x" = "x`locate readline.h`"; echo $$?)
ifneq ($(READLINE_FLAG), 0)
	READLINE_LIB = -lreadline
	READLINE_FLAGS = -DHAS_READLINE
else
	READLINE_LIB = 
	READLINE_FLAGS =
endif
CFLAGS=-c -std=c99 -D_GNU_SOURCE $(READLINE_FLAGS)
LDFLAGS = -std=c99 -I $(SRC) -D_GNU_SOURCE $(READLINE_FLAGS) $(READLINE_LIB)

all: $(PROGRAM)

$(PROGRAM): $(TEMP)lex.yy.c $(TEMP)y.tab.c $(SH_LIST)
	$(MKDIR)
	$(LD) $(LDFLAGS) $(TEMP)lex.yy.c $(TEMP)y.tab.c $(SH_LIST) -o $(PROGRAM)

$(TEMP)lex.yy.c: $(SRC)lexer.l
	$(MKDIR)
	lex --outfile $(TEMP)lex.yy.c $(SRC)lexer.l 

$(TEMP)y.tab.c:	$(SRC)parser.y
	$(MKDIR)
	yacc -d $(SRC)parser.y --output-file $(TEMP)y.tab.c

$(OBJ)%.o: $(SRC)%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $< -o $@
	

clean:
	$(RM) $(SH_LIST) $(TEMP)lex.yy.c $(TEMP)y.tab.c $(PROGRAM) $(TEMP)y.output $(TEMP)y.tab.h
	rmdir $(TEMP) $(OBJ)
	




