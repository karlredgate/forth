CC=gcc
CFLAGS=-g -O

include $(shell uname -s).mk

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

default: forth

forth: $(OBJS)
	gcc $(OBJS) $(LIBS) -o forth

$(OBJS): $(INCLUDES)

clean:
	rm $(OBJS) forth
