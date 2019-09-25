DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS = -g -Wall -pedantic
else
    CFLAGS =
endif

CC = gcc
CC += $(CFLAGS)

FILENAME = VMTranslator

all: main.o heck.o hash.o utils.o
	$(CC) main.o heck.o hash.o utils.o -o $(FILENAME)
	rm *.o

main: main.c
	$(CC) -c main.c -o main.o

heck: heck.c heck.h
	$(CC) -c heck.c -o heck.o

hash: hash.c hash.h
	$(CC) -c hash.c -o hash.o

utils: utils.c utils.h
	$(CC) -c utils.c -o utils.o

run: 
	$(FILENAME)

