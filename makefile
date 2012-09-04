ifeq ($(MINGW),YES)
EXT=.exe
else
EXT=
endif

PREFIX?=$(HOME)
SRC=minibsdiff
CCOPTS?=-Wall -Wextra -std=c99

SCANBUILD?=scan-build
SCANOPTS?=-analyze-headers --use-analyzer $(shell which clang)

all:
	$(CC) $(CCOPTS) -pipe -march=native -O3 $(SRC).c -o $(SRC)$(EXT)
debug:
	$(CC) $(CCOPTS) -g -O0 $(SRC).c -o $(SRC)$(EXT)
analyze:
	$(SCANBUILD) $(SCANOPTS) $(CC) $(CCOPTS) $(SRC).c -o $(SRC)$(EXT)
install:
	-cp $(SRC)$(EXT) $(PREFIX)/bin
clean:
	rm -f minibsdiff *~ *.o
