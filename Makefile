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
all: $(BUILD)libbdoag.a $(BUILD)bdoag/bdoag

clean:
	rm -rf $(BUILD)

include src/common/common.mk
include src/doag/doag.mk
include src/bdoag/bdoag.mk
