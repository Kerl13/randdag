# POSIX compliant makefile
.POSIX:
.SUFFIXES:

CC     = cc

# We enable loads of warnings here in the hope that when other people try to
# build our examples on their system and with their config, they will be free of
# warnings (and bad surprises).
# This is by no mean necessary.
CFLAGS = -Werror -Wall -Wextra -pedantic -ansi

# We tell the linker that the randdag libraries are installed in ../build since
# this is where we put them by default in development.
# Configure (or remove) this to suit your needs.
LDFLAGS = -L../build

DEPS = ../build/libdoag.a utils.c utils.h

all: doag_n.exe doag_count.exe random_doag_nm1.exe

doag_n.exe: doag_n.c $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ doag_n.c utils.c -ldoag -lgmp

doag_count.exe: doag_count.c $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ doag_count.c utils.c -ldoag -lgmp

random_doag_nm1.exe: random_doag_nm1.c $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ random_doag_nm1.c utils.c -ldoag -lgmp

clean:
	rm -rf *.exe
