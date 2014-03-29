CC=gcc
CFLAGS=-g

all : test

spin.o : spin.c
	$(CC) $(CFLAGS) -c -fpic spin.c

counter.o : counter.c
	$(CC) $(CFLAGS) -c -fpic counter.c
        
libspin.so: spin.o
	$(CC) $(CFLAGS) -shared -o $@ $<

libcounter.so : counter.o
	$(CC) $(CFLAGS) -shared -o $@ $<

test : test.o libcounter.so libspin.so
	$(CC) $(CFLAGS) $< -o $@ -lcounter -lspin -L. -lpthread


clean :
	rm -rf *.o test *.so
