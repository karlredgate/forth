CC=gcc
CFLAGS=-g -O


INCLUDES= compiler.h
OBJS= \
	linenoise.o \
	kernel.o \
	forthcore.o \
	controlflow.o \
	dictionary.o \
	double.o \
	elf.o \
	exception.o \
	file.o \
	format.o \
	implementation.o \
	init.o \
	interpreter.o \
	primitives.o \
	stack.o \
	string.o \
	vocabulary.o \
	memory.o \
	unix.o \
	signal.o \
	main.o

default: check forth

check:
	[ `uname -m` != x86_64 ]

forth: $(OBJS)
	gcc $(OBJS) $(LIBS) -o forth

$(OBJS): $(INCLUDES)

clean:
	rm $(OBJS) forth
