# Compiler and flags
CC = gcc
CFLAGS = `pkg-config --cflags gtk4`
LDFLAGS = `pkg-config --libs gtk4`

# Source and object files
SRC_DIR = src
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/gui.c
OBJ_FILES = $(SRC_FILES:.c=.o)
TARGET = main

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compiling source to object
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean
clean:
	rm -f $(OBJ_FILES) $(TARGET)

# Run
run: $(TARGET)
	./$(TARGET)
