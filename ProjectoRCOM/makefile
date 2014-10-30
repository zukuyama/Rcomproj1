CC = gcc
PROG = main
CFLAGS =
SRCS = programa.c
BIN_DIR = bin

all: bin other

bin:
	mkdir -p ${BIN_DIR}
other:
	$(CC) $(SRCS) -o $(BIN_DIR)/$(PROG) $(CFLAGS)
