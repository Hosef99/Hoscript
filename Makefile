CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = calc

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

.PHONY: clean
clean:
	rm -f src/*.o $(TARGET)
