CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -Iinclude
LDFLAGS =

BIN = allocator_sim
SRC = src/main.c src/allocator.c src/allocator_helpers.c src/allocator_print.c src/mem_sim.c
OBJ = $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)
