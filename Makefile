CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS = -lm

# Entry point (default to main.c, can override with MAINFILE)
MAINFILE ?= main.c

# Source files: all .c except main.c/test.c, plus selected entry point
SRC = $(wildcard src/*.c)
SRC := $(filter-out src/main.c src/test.c, $(SRC))
SRC += src/$(MAINFILE)

OBJ = $(patsubst src/%.c, build/%.o, $(SRC))

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    TARGET = bin/hos
    LDFLAGS += -lreadline -lhistory
else ifeq ($(UNAME_S),Darwin) # macOS
    TARGET = bin/hos
    LDFLAGS += -lreadline -lhistory
else
    TARGET = bin/hos.exe
endif

# Detect OS for mkdir/rm commands
ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$(1)" mkdir "$(1)"
    RM = rmdir /s /q "$(1)"
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
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
	-$(call RM,build)
	-$(call RM,bin)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g
debug: clean $(TARGET)

sanitize: CFLAGS += -fsanitize=address -g
sanitize: clean $(TARGET)
