#
# Makefile
#

ifeq ($(CC),)
CC = gcc
endif
CFLAGS += -I/usr/include
LDFLAGS += -L/usr/lib
ifdef debug
CFLAGS += -ggdb -D DEBUG
else
CFLAGS += -O3 -flto
LDFLAGS += -flto
endif

OBJS = rbtree.o

.PHONY: all clean

all: test

clean:
	- rm -f test
	- rm -f *.o
	- rm -f randomized.txt

test: test.o $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@
ifndef debug
	strip $@
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

