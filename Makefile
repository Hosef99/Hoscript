CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS = -lm -lreadline -lhistory
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))

# Detect OS for output binary
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    TARGET = bin/hos
else ifeq ($(UNAME_S),Darwin)
    TARGET = bin/hos
else
    TARGET = bin/hos.exe
endif

# Detect OS for mkdir/rm commands
ifeq ($(OS),Windows_NT)
    MKDIR = if not exist $(1) mkdir $(1)
    RM = rmdir /s /q
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf
endif

# Default build rule
$(TARGET): $(OBJ)
	$(call MKDIR,bin)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .c to .o inside build/
build/%.o: src/%.c
	$(call MKDIR,build)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean run debug sanitize

clean:
	-$(RM) build bin

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g
debug: clean $(TARGET)

sanitize: CFLAGS += -fsanitize=address -g
sanitize: clean $(TARGET)

