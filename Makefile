# This simple makefile is copied to the target 2.6 kernel directories by
# GNUmakefile.in.

EXTRA_CFLAGS := -Werror -Wall
obj-m := xvma.o
