# POSIX compliant makefile
.POSIX:
.SUFFIXES:

CC     = cc
CFLAGS = -Wall -Wextra -pedantic -O2
LDLIBS = -lgmp
AR     = ar
RANLIB = ranlib

# Build directory
BUILD  = build/

all: $(BUILD)libdoag.a $(BUILD)doag/doag

clean:
	rm -rf $(BUILD)

include src/doag/doag.mk
