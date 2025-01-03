# Compiler
CC = g++

# Compiler flags
CFLAGS = -I./include/ -I./common/thirdparty/

# Libraries
LIBS = -lSDL2 -ldl

# Source files
SRC = $(wildcard src/*.cpp src/glad.c)

# Output binary
OUT = prog

# Default target
all: $(OUT)

# Build target
$(OUT): $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(OUT) $(LIBS)

# Clean target
.PHONY: clean
clean:
	rm -f $(OUT)

