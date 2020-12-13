
aes: aeslib.o
	gcc -Wall main.c aeslib.o -o main -std=c99 ; ./main

aeslib.o: aeslib.c aeslib.h
	gcc -Wall -c aeslib.c -std=c99

clean:
	rm aeslib.o main
