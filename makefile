ifeq ($(MINGW),YES)
EXT=.exe
else
EXT=
endif

SRC=minibsdiff
CCOPTS?=-Wall -Wextra -std=c99

SCANBUILD?=scan-build
SCANOPTS?=-analyze-headers --use-analyzer $(shell which clang)

all:
	$(CC) $(CCOPTS) -O3 $(SRC).c -o $(SRC)$(EXT)
analyze:
	$(SCANBUILD) $(SCANOPTS) $(CC) $(CCOPTS) $(SRC).c -o $(SRC)$(EXT)
clean:
	rm -f minibsdiff *~ *.o
