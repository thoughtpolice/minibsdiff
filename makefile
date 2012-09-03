all:
	$(CC) -Wextra -Wall -std=c99 -O2 bsdiff.c bspatch.c test.c -o minibsdiff
clean:
	rm -f minibsdiff *~ *.o
