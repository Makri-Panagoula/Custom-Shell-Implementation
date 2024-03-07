# Paths
SRC = ./src
HEADERS = ../headers

# Compiler
CC = gcc

# Compile Options
CFLAGS = -Wall -Werror -I -g -I$(HEADERS)

# Executable
EXEC = mysh

# Object files 
OBJ = $(SRC)/aliases.o $(SRC)/redirections.o $(SRC)/commands.o $(SRC)/mysh.o

all: $(EXEC)

$(EXEC): $(OBJ)

	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)  -lrt -g

run: $(EXEC)
	./$(EXEC) 

clean:
	rm -f $(OBJ) $(EXEC) 
