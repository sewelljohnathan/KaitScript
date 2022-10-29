CC = gcc
SRC_CODE = driver.c lexgen.c interpreter.c interpreterTools.c standard.c

all: $(SRC_CODE)
	$(CC) $(SRC_CODE) -o kait