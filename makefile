all: spin.o

spin.o: spin.c
	gcc spin.c -lpthread -o spin.o -g

clean:
	rm -f spin.o
