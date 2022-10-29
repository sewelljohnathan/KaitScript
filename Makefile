CC = gcc
SRC_CODE = src/driver.c src/lexgen.c src/interpreter.c src/interpreterTools.c src/standard.c

all: $(SRC_CODE)
	$(CC) $(SRC_CODE) -o kait