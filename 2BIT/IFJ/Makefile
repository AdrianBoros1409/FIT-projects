#Makefile
PRJ=ifj18_112
TEST_DIR=tests
TEST_SH=test.sh
PROGS=$(PRJ)
FILES=main.c parser.c scanner.c string.c symtable.c expressions.c generator.c paramlist.c stack.c
CC=gcc
CFLAGS=-Wall -W -Wextra -g -std=c99 -pedantic

all: $(PROGS)

$(PRJ): $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES) -lm

clean:
	rm -f *.o *.out $(PROGS)
