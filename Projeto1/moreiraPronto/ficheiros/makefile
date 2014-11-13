CC = gcc
PROG = main
CFLAGS =
SRCS = file.c
BIN_DIR = bin

all: bin other

bin:
	mkdir -pp ${BIN_DIR}
other:
	$(CC) $(SRCS) -o $(BIN_DIR)/$(PROG) $(CFLAGS)
