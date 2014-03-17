all: spin.o

spin.o: spin.c
	gcc spin.c -lpthread -o spin.o

