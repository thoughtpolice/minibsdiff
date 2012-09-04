all:
	$(CC) -Wextra -Wall -std=c99 -O2 minibsdiff.c -o minibsdiff
clean:
	rm -f minibsdiff *~ *.o
