# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -I./include

# Linker flags
LDFLAGS = -lhdf5 -lm

# Source and object file directories
SRC_DIR = src
OBJ_DIR = build/object_files

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Additional source file (main.c)
MAIN_SRC = main.c
MAIN_OBJ = $(OBJ_DIR)/main.o

# OUTPUT name
OUTPUT = main

# Main target
all: $(OUTPUT)

# Linking step
$(OUTPUT): $(OBJ_FILES) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compilation step for main.c
$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Compilation step for other source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR)/*.o $(OUTPUT)

# Phony targets
.PHONY: all clean
