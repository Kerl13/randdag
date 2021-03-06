# POSIX compliant makefile
.POSIX:
.SUFFIXES:

CC     = cc
CFLAGS = -Wall -Wextra -pedantic -Wmissing-prototypes -O2 -ansi
LDLIBS = -lgmp
AR     = ar
RANLIB = ranlib

# Build directory
BUILD  = build/

all: $(BUILD)libdoag.a $(BUILD)doag/doag
all: $(BUILD)libbdoag.a $(BUILD)bdoag/bdoag
all: $(BUILD)libldag.a $(BUILD)ldag/ldag

clean:
	rm -rf $(BUILD)

include lib/argtable3/argtable3.mk
include src/common/common.mk
include src/doag/doag.mk
include src/bdoag/bdoag.mk
include src/ldag/ldag.mk
