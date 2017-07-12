# Makefile
# compiler to use
CC = clang
#CC = gcc
# flags to pass compiler
CFLAGS = -ggdb3 -O3 -Qunused-arguments -Wall -Werror -pthread
#CFLAGS = -g -pthread
# name for executable
#EXE = Manager

# space-separated list of header files
HDRS = Heap.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS =

# space-separated list of source files
SRCS = Heap.c

# automatically generated list of object files
#OBJS = $(SRCS:.c=.o)

all: test1 test2 test3 test4

test1: $(SRCS) test1.c $(HDRS) Makefile
	$(CC) $(CFLAGS) test1.c $(SRCS) -o $@

test2: $(SRCS) test2.c $(HDRS) Makefile
	$(CC) $(CFLAGS) test2.c $(SRCS) -o $@

test3: $(SRCS) test3.c $(HDRS) Makefile
	$(CC) $(CFLAGS) test3.c $(SRCS) -o $@

test4: $(SRCS) test4.c $(HDRS) Makefile
	gcc -fopenmp -g test4.c $(SRCS) -o $@

# dependencies
$(SRCS): $(HDRS) Makefile

# housekeeping
clean:
	rm -f core $(EXE) *.o
