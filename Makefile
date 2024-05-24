main: main.o admin.o examens.o modalites.o
	gcc -Wall -Wextra -std=c99 -o main main.o admin.o examens.o modalites.o -lcrypto

%.o: %.c
	gcc -Wall -Wextra -std=c99 -c $<

clean:
	rm -f *.o main

