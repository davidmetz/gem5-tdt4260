ifdef L2_BLOCK_SIZE
CFLAGS:=-DL2_BLOCK_SIZE=$(L2_BLOCK_SIZE) $(CFLAGS)
endif

ifdef L1_BLOCK_SIZE
CFLAGS:=-DL1_BLOCK_SIZE=$(L1_BLOCK_SIZE) $(CFLAGS)
endif

ifndef GEM5_PATH
GEM5_PATH=/home/amundbk/Courses/gem5-tdt4260
endif

CC=gcc
CFLAGS:=-Werror -O2 -g $(CFLAGS) -march=x86-64
LDFLAGS:=-static
CFLAGS += -I$(GEM5_PATH)/include
LDFLAGS += -L$(GEM5_PATH)/util/m5/build/x86/out -lm5

all: test_associativity

test_associativity: test_associativity.o

clean:
	rm -f test_associativity *.o
