# POSIX compliant makefile
.POSIX:
.SUFFIXES:

CC     = cc
CFLAGS = -Werror -Wall -Wextra -pedantic -O2 -ansi

all: doag_n.exe

doag_n.exe:doag_n.c ../build/libdoag.a utils.c utils.h
	$(CC) $(CFLAGS) -L../build -o doag_n.exe doag_n.c utils.c -ldoag -lgmp

clean:
	rm -rf *.exe
