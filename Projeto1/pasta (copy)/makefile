CC = gcc
PROG = main
CFLAGS =
SRCS = appLayer.c auxiliar.c linkLayer.c noncanon.c protocol.c main.c
BIN_DIR = bin

all: bin other

bin:
	mkdir -p ${BIN_DIR}
other:
	$(CC) $(SRCS) -o $(BIN_DIR)/$(PROG) $(CFLAGS)
