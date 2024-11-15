# Compiler
CC = gcc

#Flags
CFLAGS = -g -Wall -Wno-unused-variable

# Source Files
SRC = main.c vaderSentiment.c

# Header Files
HEADERS = utility.h

# Output Binary
OUTPUT = vaderSentiment

# Build the executable
all: $(OUTPUT)

$(OUTPUT): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(OUTPUT)

# Clean the build directory
clean:
	rm -f $(OUTPUT)

# Check memory leaks using Valgrind
valgrind: $(OUTPUT)
	valgrind --leak-check=full --track-origins=yes ./$(OUTPUT)

.PHONY: all clean valgrind
