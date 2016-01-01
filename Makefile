CC=gcc
CFLAGS=-g -O

OBJS  = kernel.o
OBJS += forthcore.o
OBJS += controlflow.o
OBJS += dictionary.o
OBJS += double.o
OBJS += exception.o
OBJS += format.o
OBJS += implementation.o
OBJS += init.o
OBJS += interpreter.o
OBJS += primitives.o
OBJS += stack.o
OBJS += string.o
OBJS += vocabulary.o
OBJS += linenoise.o
OBJS += file.o
OBJS += memory.o

include $(shell uname -s).mk

INCLUDES= compiler.h
NOBJS= \
	elf.o \
	file.o \
	memory.o \
	signal.o \
	main.o

default: forth

forth: $(OBJS)
	gcc $(OBJS) $(LIBS) -o forth

$(OBJS): $(INCLUDES)

clean:
	rm $(OBJS) forth
