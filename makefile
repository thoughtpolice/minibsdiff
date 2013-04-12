ifeq ($(MINGW),YES)
EXT=.exe
else
EXT=
endif

PREFIX?=$(HOME)
SRC=minibsdiff
CCOPTS?=-Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes \
    -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align          \
    -Wwrite-strings -Wnested-externs                           \
    -pedantic -std=c99

SCANBUILD?=scan-build
SCANOPTS?=-analyze-headers --use-analyzer $(shell which clang)

all:
	$(CC) $(CCOPTS) -DNDEBUG -O2 $(SRC).c -o $(SRC)$(EXT)
debug:
	$(CC) $(CCOPTS) -DDEBUG -g -O0 $(SRC).c -o $(SRC)$(EXT)
sanitize:
	# requires clang
	$(CC) $(CCOPTS) -DDEBUG -g -O1 -pie -fPIC -fsanitize=address $(SRC).c -o $(SRC)$(EXT).asan
	$(CC) $(CCOPTS) -DDEBUG -g -O1 -pie -fPIC -fsanitize=memory $(SRC).c -o $(SRC)$(EXT).msan
	$(CC) $(CCOPTS) -DDEBUG -g -O1 -pie -fPIC -fsanitize=thread $(SRC).c -o $(SRC)$(EXT).tsan
	$(CC) $(CCOPTS) -DDEBUG -g -O1 -pie -fPIC -fsanitize=integer $(SRC).c -o $(SRC)$(EXT).isan
	$(CC) $(CCOPTS) -DDEBUG -g -O1 -pie -fPIC -fsanitize=undefined $(SRC).c -o $(SRC)$(EXT).ubsan
analyze:
	$(SCANBUILD) $(SCANOPTS) $(CC) -DDEBUG $(CCOPTS) $(SRC).c -o $(SRC)$(EXT)
install:
	-cp $(SRC)$(EXT) $(PREFIX)/bin
clean:
	rm -f minibsdiff *~ *.o *.asan *.tsan *.msan *.isan *.ubsan
