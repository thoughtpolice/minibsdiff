all:
	$(CC) -Wall -std=gnu99 -O2 bsdiff.c bspatch.c test.c -o minibsdiff
clean:
	rm -f minibsdiff *~ *.o
