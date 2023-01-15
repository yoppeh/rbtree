# makefile

cc = clang
ld = clang
ar = ar
aflags = rcs

debug: cflags = -D DEBUG -g -O0
debug: lflags = -g -O0

release: cflags = -O3
release: lflags = -O3

lib: cflags = -O3

objs = ascending.o delete.o deletenode.o descending.o free.o insert.o lookup.o maximum.o minimum.o new.o rotate.o
lib = librbtree.a

%.o: %.c
	$(cc) $(cflags) -c -o $@ $<

debug: test

release: test
	strip test
	strip $(lib)

test: test.o $(lib)
	$(ld) $(lflags) -o test test.o $(lib)

$(lib): $(objs)
	$(ar) $(aflags) -o $(lib) $^

%.o: rbtree.h

lib: $(lib)

clean:
	if ls *.o 1> /dev/null 2>&1 ; then rm *.o ; fi
	if [ -f test ] ; then rm test ; fi
	if [ -f $(lib) ] ; then rm $(lib) ; fi
	if [ -f randomized.txt ] ; then rm randomized.txt ; fi

