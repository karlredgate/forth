
OBJS += linux.o
OBJS += posix.o
OBJS += signal.o
OBJS += main.o

include Linux-$(shell uname -m).mk
