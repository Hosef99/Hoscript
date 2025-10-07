CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LDFLAGS = -lm
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
TARGET = bin/cal.exe

# Detect OS mkdir command
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
