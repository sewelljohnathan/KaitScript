CC = gcc
SRC_CODE = driver.c lexgen.c interpreter.c

all: $(SRC_CODE)
	$(CC) $(SRC_CODE) -o kait