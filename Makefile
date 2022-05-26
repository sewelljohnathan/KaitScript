CC = gcc
SRC_CODE = driver.c

all: $(SRC_CODE)
	$(CC) $(SRC_CODE) -o kait