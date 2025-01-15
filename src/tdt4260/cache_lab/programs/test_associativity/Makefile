ifdef L2_BLOCK_SIZE
CFLAGS:=-DL2_BLOCK_SIZE=$(L2_BLOCK_SIZE) $(CFLAGS)
endif

ifdef L1_BLOCK_SIZE
CFLAGS:=-DL1_BLOCK_SIZE=$(L1_BLOCK_SIZE) $(CFLAGS)
endif


CC=gcc
CFLAGS:=-Werror -O2 -g $(CFLAGS) -march=x86-64
LDFLAGS:=-static

all: test_associativity

test_associativity: test_associativity.o

clean:
	rm -f test_associativity *.o