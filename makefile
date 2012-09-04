ifdef ($(MINGW),YES)
EXT=.exe
else
EXT=
endif

SRC=minibsdiff

SCANBUILD?=scan-build
CLANG?=$(shell which clang)

SCANOPTS?=-analyze-headers --use-analyzer $(CLANG)
CCOPTS?=-Wall -Wextra -std=c99

all:
	$(CC) $(CCOPTS) -O3 $(SRC).c -o $(SRC)$(EXT)
analyze:
	$(SCANBUILD) $(SCANOPTS) $(CC) $(CCOPTS) minibsdiff.c -o minibsdiff
clean:
	rm -f minibsdiff *~ *.o
